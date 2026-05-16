import os
import subprocess
import shutil
from pathlib import Path
import argparse

TARGET_NAME = os.environ.get('TARGET_NAME', 'shaders-compiled')
BASE_DIRECTORY: Path = Path(os.environ.get('BASE_DIRECTORY', '.'))
SHADER_DIRECTORY: Path = Path(os.environ.get('SHADER_DIRECTORY', 'shaders'))
TARGET_DIRECTORY: Path = Path(os.environ.get('TARGET_DIRECTORY', TARGET_NAME))

class CompileError(Exception):
    def __init__(self, message: str):
        super().__init__(f'Compilation failed: {message}')

class InvalidDirectory(CompileError):
    def __init__(self, path: Path):
        super().__init__(f'{path} is not a valid directory')

def glob_targets() -> list[list[Path]]:
    shader_directory = BASE_DIRECTORY / SHADER_DIRECTORY
    print(f'Finding shaders at path `{shader_directory}`')
    if not shader_directory.exists() or not shader_directory.is_dir():
        raise InvalidDirectory(shader_directory)
    
    target_shaders = []
    for subdir in shader_directory.iterdir():
        shaders = []
        print(f'Target: `{subdir.name}`')
        for shader in subdir.rglob('*.slang'):
            relpath = Path(os.path.relpath(shader, BASE_DIRECTORY))
            print(f'Found `{relpath }`')
            shaders.append(relpath)
        target_shaders.append((subdir.name, shaders))
    return target_shaders

def main():
    parser = argparse.ArgumentParser(
        prog='Shader Builder'
    )
    parser.add_argument('--level', nargs=1, default='debug', choices=('debug', 'release', 'optimised'))
    args = parser.parse_args()
    level = args.level

    additional_arguments = []
    if level == 'release':
        additional_arguments = ['-O2']
    elif level == 'optimised':
        additional_arguments = ['-g', '-O2']
    elif level == 'debug':
        additional_arguments = ['-g', '-O0']

    try:
        targets = glob_targets()
    except CompileError as e:
        print(e)
        return

    for name, target in targets:
        print(f'Processing target `{name}`')
        output_path = BASE_DIRECTORY / TARGET_DIRECTORY / name
        if output_path.exists():
            shutil.rmtree(output_path)
        os.makedirs(output_path)
            
        for shader in target:
            shader_output = output_path / f'{shader.stem}.spv'

            start = 0
            for idx, part in enumerate(output_path.parts):
                if part == shader.parts[0]:
                    start = idx
                    break
            for idx, _ in enumerate(shader.parts):
                output_idx = start + idx
                if output_idx >= len(output_path.parts) or output_path.parts[output_idx] != shader.parts[idx]:
                    shader_output = output_path / '/'.join(shader.parts[idx:])
                    break
            shader_output = shader_output.with_suffix('.spv')
            print(f'Compiling `{shader}` -> {os.path.relpath(shader_output, BASE_DIRECTORY)}')
            shader_output.parent.mkdir(exist_ok=True)
            subprocess.run(['slangc', BASE_DIRECTORY / shader, '-target', 'spirv', '-o', shader_output] + additional_arguments)
            print(shader)

if __name__ == '__main__':
    main()