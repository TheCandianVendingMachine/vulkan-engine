from git import Repo
from tempfile import TemporaryDirectory
from pathlib import Path
from typing import Sequence, Any
from contextlib import contextmanager
import shutil
import os
import subprocess

PRINT_HEADER = '#: '

BASE_DIRECTORY: Path = Path(os.environ.get('BASE_DIRECTORY', '.'))

INSTALL_DIRECTORY: Path = BASE_DIRECTORY / 'external'
EXTERNAL_HEADER_LIB: Path = INSTALL_DIRECTORY / 'include'
EXTERNAL_LIB: Path = INSTALL_DIRECTORY / 'lib'
EXTERNAL_CMAKE_LISTS: Path = INSTALL_DIRECTORY / 'CMakeLists.txt'

GIT_CACHE_NAME: str = 'tcvm-git-cache.txt'

class CMakeProject:
    def __init__(self, subdir: Path, options: dict[str, Any]):
        self.subdir = subdir
        self.options = options

    def map(self, value: Any) -> str:
        if isinstance(value, bool):
            if value:
                return 'ON'
            return 'OFF'
        return str(value)

    def to_cmake_lists(self) -> str:
        subdir = f'add_subdirectory({self.subdir.name})'
        options = ''
        for k, v in self.options.items():
            options += f'set({k} {self.map(v)} CACHE BOOL "" FORCE)\n'
        
        if len(self.options) > 0:
            return f'{options}{subdir}\n\n'
        else:
            return subdir + '\n\n'

@contextmanager
def header_library(library_name: str):
    build_path = EXTERNAL_HEADER_LIB / library_name
    if build_path.exists():
        print(f'{PRINT_HEADER}{build_path} exists, removing...')
        shutil.rmtree(build_path)
    build_path.mkdir(parents=True, exist_ok=False)
    yield build_path

@contextmanager
def binary_library(library_name: str, target: str):
    build_path = EXTERNAL_LIB / library_name / target
    if build_path.exists():
        print(f'{PRINT_HEADER}{build_path} exists, removing...')
        shutil.rmtree(build_path)
    build_path.mkdir(parents=True, exist_ok=False)
    yield build_path

class BuildSystem:
    def build(self):
        raise NotImplementedError('This method should be overridden by subclasses.')

class NoBuildSystem:
    def build(self):
        pass

class CMakePackage(BuildSystem):
    def __init__(self, *, name: str, build_path: Path, options: dict[str, str]):
        self.name = name
        self.build_path = build_path
        self.options = options
        super().__init__()

    def build(self):
        with open(EXTERNAL_CMAKE_LISTS, 'a') as f:
            f.write(CMakeProject(self.build_path, self.options).to_cmake_lists())

class CMakeBuild(CMakePackage):
    def map(self, value: Any) -> str:
        if isinstance(value, bool):
            if value:
                return 'ON'
            return 'OFF'
        return str(value)

    def build(self):
        subprocess.run([
            'cmake',
            *[f'-D{key}={self.map(value)}' for key, value in self.options.items()],
            '.',
        ], cwd=self.build_path, check=True)

        self.copy_library()
        self.copy_headers()

    def copy_library(self):
        install_path = EXTERNAL_LIB / self.name
        if install_path.exists():
            print(f'{PRINT_HEADER}Removing {install_path}')
            shutil.rmtree(install_path)
        install_path.mkdir(parents=True, exist_ok=False)

        for target in ['Debug', 'Release']:
            subprocess.run([
                'cmake',
                '--build',
                '.',
                '--config',
                target
            ], cwd=self.build_path, check=True)

            with binary_library(self.name, target) as install_dir:
                for path in (self.build_path / target).iterdir():
                    if path.suffix in ['.lib', '.dll', '.pdb']:
                        print(f'{PRINT_HEADER}Copying {path} to {install_dir / path.name}')
                        shutil.copy2(path, install_dir / path.name)

    def copy_headers(self):
        with header_library(self.name) as build_path:
            shutil.copytree(self.build_path / 'include', build_path.parent, dirs_exist_ok=True)

class Dependency:
    def install(self):
        raise NotImplementedError('This method should be overridden by subclasses.')

class GitDependency(Dependency):
    DEPENDENCY_NAME: str = 'N/A'
    IGNORE_DIRECTORIES: list[Path] = [
        Path('.git'),
        Path('.github'),
        Path('bin'),
        Path('media'),
        Path('tools'),
        Path('docs'),
        Path('doc'),
        Path('examples'),
        Path('misc'),
        Path('tests'),
        Path('test'),
        Path('tools'),
        Path('VisualC-GDK'),
        Path('VisualC'),
        Path('XCode'),
        Path('android-project'),
    ]
    IGNORE_FILES: list[str] = [
        '.gitignore',
        '.gitmodules',
        '.readthedocs.yaml',
        '.clang-format',
        '.clang-tidy',
        'Doxyfile',
        'CHANGELOG.md',
        'CONTRIBUTING.md',
        '.gitattributes',
        '.editorconfig',
        '.wikiheaders-options',
        'Android.mk',
        'BUGS.txt',
        'WhatsNew.txt',
        '.codecov.yml',
    ]
    repo_url: str
    commit_hash: str
    temp_dir: None | Path
    install_path: None | Path

    def __init__(self, repo_url: str, commit_hash: str | None = None):
        self.repo_url = repo_url
        self.commit_hash = commit_hash
        self.temp_dir = None
        self.install_path = None

    def pull(self, directory: Path):
        print(f'{PRINT_HEADER}Cloning {self.repo_url} to {directory}')
        repo = Repo.clone_from(self.repo_url, directory, no_checkout=True)
        if self.commit_hash:
            print(f'{PRINT_HEADER}Pulling commit {self.commit_hash[0:7]}')
            repo.git.checkout(self.commit_hash)
    
    def install(self):
        self.install_path = INSTALL_DIRECTORY / self.DEPENDENCY_NAME
        if (self.install_path / GIT_CACHE_NAME).exists():
            with open(self.install_path / GIT_CACHE_NAME, 'r') as f:
                existing_hash = f.read().strip()
            
            if existing_hash == self.commit_hash:
                print(f'{PRINT_HEADER}{self.install_path} is up to date, nothing to do.')
                return

        with TemporaryDirectory() as temp_dir:
            temp_dir = Path(temp_dir)
            self.temp_dir = temp_dir
            if self.install_path.exists():
                print(f'{PRINT_HEADER}{self.install_path} exists, removing...')
                shutil.rmtree(str(self.install_path))

            self.pull(self.temp_dir)

            print(f'{PRINT_HEADER}Copying from {self.temp_dir} to {self.install_path}')
            shutil.copytree(
                self.temp_dir,
                self.install_path,
                dirs_exist_ok=True,
                ignore=self.ignore
            )

            for dir in self.install_path.iterdir():
                if dir.is_dir() and not any(dir.iterdir()):
                    print(f'{PRINT_HEADER}Removing empty directory {dir}')
                    shutil.rmtree(dir)
        
        with open(self.install_path / GIT_CACHE_NAME, 'w') as f:
            f.write(self.commit_hash)
        self.temp_dir = None

    def ignore(self, directory: str, contents: Sequence[str]) -> Sequence[str]:
        if Path(directory) in [self.temp_dir / d for d in self.IGNORE_DIRECTORIES]:
            return contents
        return [f for f in contents if f in self.IGNORE_FILES]

class FastGltf(GitDependency, CMakePackage):
    DEPENDENCY_NAME: str = 'fastgltf'
    def __init__(self):
        CMakeBuild.__init__(
            self,
            name=self.DEPENDENCY_NAME,
            build_path=INSTALL_DIRECTORY / self.DEPENDENCY_NAME,
            options={
                'FASTGLTF_COMPILE_AS_CPP20': True,
                'FASTGLTF_USE_64BIT_FLOAT': True
            }
        )

        GitDependency.__init__(
            self,
            'https://github.com/spnda/fastgltf.git',
            '0d1b67a28c4950ea2deb796702006dcbe31e02b3'
        )

class FmtLib(GitDependency, CMakePackage):
    DEPENDENCY_NAME: str = 'fmtlib'
    def __init__(self):
        CMakePackage.__init__(
            self,
            name=self.DEPENDENCY_NAME,
            build_path=INSTALL_DIRECTORY / self.DEPENDENCY_NAME,
            options={
                'FMT_INSTALL': False,
                'FMT_TEST': False,
                'FMT_DOC': False
            }
        )

        GitDependency.__init__(
            self,
            'https://github.com/fmtlib/fmt.git',
            '40626af88bd7df9a5fb80be7b25ac85b122d6c21'
        )

class Imgui(GitDependency, BuildSystem):
    DEPENDENCY_NAME: str = 'imgui'
    WANTED_RUNTIMES: list[str] = [
        'imgui_impl_sdl3.h',
        'imgui_impl_sdl3.cpp',
        'imgui_impl_vulkan.h',
        'imgui_impl_vulkan.cpp',
    ]
    def __init__(self):
        super().__init__(
            'https://github.com/ocornut/imgui.git',
            '9a5d5c45f54b1301ea471622eddede70384243af'
        )

    def ignore(self, directory: str, contents: Sequence[str]) -> Sequence[str]:
        if directory.endswith('backends'):
            return [c for c in contents if c not in self.WANTED_RUNTIMES]
        return super().ignore(directory, contents)

    def build(self):
        with header_library(self.DEPENDENCY_NAME) as build_path:
            for path in self.install_path.iterdir():
                if path.name != GIT_CACHE_NAME:
                    src = self.install_path / path.name
                    dst = build_path / path.name
                    print(f'{PRINT_HEADER}Copying {src} to {dst}')
                    if path.is_dir():
                        shutil.copytree(src, dst)
                    else:
                        shutil.copy2(src, dst)

class Stb(GitDependency, BuildSystem):
    DEPENDENCY_NAME: str = 'stb'
    WANTED_LIBRARIES: list[str] = [
        'LICENSE',
        'stb_image.h',
        'stb_image_write.h',
    ]
    def __init__(self):
        super().__init__(
            'https://github.com/nothings/stb.git',
            'f58f558c120e9b32c217290b80bad1a0729fbb2c'
        )
    
    def ignore(self, directory: str, contents: Sequence[str]) -> Sequence[str]:
        return [f for f in contents if f not in self.WANTED_LIBRARIES]

    def build(self):
        with header_library(self.DEPENDENCY_NAME) as build_path:
            for library in self.WANTED_LIBRARIES:
                src = self.install_path / library
                dst = build_path / library
                print(f'{PRINT_HEADER}Copying {src} to {dst}')
                shutil.copy2(src, dst)
            
            print('Making implementation file')
            with open(build_path / 'stb_image_implementation.cpp', 'w') as f:
                f.write('#define STB_IMAGE_IMPLEMENTATION\n')
                f.write('#include "stb_image.h"\n\n')
                f.write('#define STB_IMAGE_WRITE_IMPLEMENTATION\n')
                f.write('#include "stb_image_write.h"\n')

class Volk(GitDependency, CMakePackage):
    DEPENDENCY_NAME: str = 'volk'
    def __init__(self):
        CMakePackage.__init__(
            self,
            name=self.DEPENDENCY_NAME,
            build_path=INSTALL_DIRECTORY / self.DEPENDENCY_NAME,
            options={
            }
        )

        GitDependency.__init__(
            self,
            'https://github.com/zeux/volk.git',
            '0b17a763ba5643e32da1b2152f8140461b3b7345'
        )
    
    def copy_headers(self):
        with header_library(self.DEPENDENCY_NAME) as build_path:
            shutil.copy2(self.build_path / 'volk.h', build_path / 'volk.h')

class VulkanMemoryAllocator(GitDependency, BuildSystem):
    DEPENDENCY_NAME: str = 'VulkanMemoryAllocator'
    def __init__(self):
        super().__init__(
            'https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git',
            '1d8f600fd424278486eade7ed3e877c99f0846b1'
        )

    def build(self):
        with header_library(self.DEPENDENCY_NAME) as build_path:
            shutil.copytree(self.install_path / 'include', build_path, dirs_exist_ok=True)

class SDL(GitDependency, CMakePackage):
    DEPENDENCY_NAME: str = 'SDL'
    def __init__(self):
        CMakePackage.__init__(
            self,
            name=self.DEPENDENCY_NAME,
            build_path=INSTALL_DIRECTORY / self.DEPENDENCY_NAME,
            options={
                'SDL_AVX': False,
                'SDL_AVX2': False,
                'SDL_AVX512F': False,
                'SDL_SSE': True,
                'SDL_SSE2': True,
                'SDL_SSE3': False,
                'SDL_SSE4_1': False,
                'SDL_SSE4_2': False,
                'SDL_MMX': False,
                'SDL_ALTIVEC': False,
                'SDL_ARMNEON': False,
                'SDL_LSX': False,
                'SDL_LASX': False,
                'SDL_VULKAN': True,
                'SDL_STATIC': True,
                'SDL_TEST_LIBRARY': False
            }
        )

        GitDependency.__init__(
            self,
            'https://github.com/libsdl-org/SDL.git',
            'a96677bdf6b4acb84af4ec294e5f60a4e8cbbe03'
        )

class RobinMap(GitDependency, BuildSystem):
    DEPENDENCY_NAME: str = 'robin-map'
    def __init__(self):
        super().__init__(
            'https://github.com/Tessil/robin-map.git',
            '4ec1bf19c6a96125ea22062f38c2cf5b958e448e'
        )

    def build(self):
        with header_library(self.DEPENDENCY_NAME) as build_path:
            shutil.copytree(self.install_path / 'include' / 'tsl', build_path, dirs_exist_ok=True)

class Catch2(GitDependency, NoBuildSystem):
    DEPENDENCY_NAME: str = 'Catch2'
    def __init__(self):
        super().__init__(
            'https://github.com/catchorg/Catch2.git',
            '25319fd3047c6bdcf3c0170e76fa526c77f99ca9'
        )

class Tracy(GitDependency, CMakePackage):
    DEPENDENCY_NAME: str = 'Tracy'
    def __init__(self):
        CMakePackage.__init__(
            self,
            name=self.DEPENDENCY_NAME,
            build_path=INSTALL_DIRECTORY / self.DEPENDENCY_NAME,
            options={
                'TRACY_ENABLE': True,
                'TRACY_ON_DEMAND': True
            }
        )

        GitDependency.__init__(
            self,
            'https://github.com/wolfpld/tracy.git',
            'c556831ddc6fe26d2fce01c14c97205a9dad46d5'
        )

    def copy_headers(self):
        with header_library(self.DEPENDENCY_NAME) as build_path:
            shutil.copytree(self.install_path / 'public', build_path, dirs_exist_ok=True)

DEPENDENCIES = {
    'stb': Stb(),
    'imgui': Imgui(),
    'fastgltf': FastGltf(),
    'fmtlib': FmtLib(),
    'volk': Volk(),
    'VulkanMemoryAllocator': VulkanMemoryAllocator(),
    'SDL': SDL(),
    'robin-map': RobinMap(),
    'Catch2': Catch2(),
    'Tracy': Tracy()
}

def main():
    if EXTERNAL_CMAKE_LISTS.exists():
        EXTERNAL_CMAKE_LISTS.unlink()

    for dep_name, dep in DEPENDENCIES.items():
        print(f"Installing {dep_name}...")
        dep.install()
        print(f"Building {dep_name}...")
        dep.build()

if __name__ == '__main__':
    main()