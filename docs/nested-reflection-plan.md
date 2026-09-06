# Plan: nested reflected types

Goal: extend the reflection system so runtime code can inspect both atomic values and structured reflected objects, e.g. `engine::Transform -> position_ -> x`.

## Current state

The current reflection model has:

- `Type<T>`: compile-time traits for names, string conversion, construction, casts.
- `RuntimeType`: runtime-erased type metadata for atomic values.
- `RuntimeTypeAtomic<T>`: runtime wrapper for types with `Type<T>`.
- `Member`: metadata/accessor for one reflected member.
- `RuntimeMember`: a `Member` bound to a concrete object pointer.
- `REFLECT_START`, `REFLECT_MEMBER`, `REFLECT_END`: macros that generate `Meta::static_members()` and `meta().members()`.

The recent accessor-lambda change means members no longer depend on `offsetof`, which makes nested/non-standard-layout objects much safer to support.

## Desired API

A runtime type should be able to say whether it has child members:

```cpp
std::shared_ptr<RuntimeType> type = RuntimeType::instance<engine::Transform>();

if (type->is_object()) {
    auto members = type->members(&transform);
}
```

Runtime member traversal should be possible:

```cpp
auto transform_meta = transform.meta();
auto position = transform_meta.get("position_");
auto x = position.get("x");
```

Or via explicit runtime type calls:

```cpp
auto position = transform.meta().get("position_");
auto children = position.members();
```

## Proposed model

### 1. Add runtime kind information

Add a lightweight type category:

```cpp
enum class RuntimeTypeKind {
    Atomic,
    Object,
};
```

Extend `RuntimeType`:

```cpp
virtual auto kind() const -> RuntimeTypeKind = 0;
virtual auto is_atomic() const -> bool;
virtual auto is_object() const -> bool;
```

`RuntimeTypeAtomic<T>::kind()` returns `Atomic`.

### 2. Add object runtime type wrapper

Introduce:

```cpp
template <typename T>
class RuntimeTypeObject final : public RuntimeType {
public:
    auto kind() const -> RuntimeTypeKind override { return RuntimeTypeKind::Object; }
    auto members(const void* data) const -> std::vector<RuntimeMember> override;
    auto member(const void* data, std::string_view name) const -> std::optional<RuntimeMember> override;
};
```

This wrapper assumes `T` has the reflection macro-generated metadata:

```cpp
requires requires(T& value) {
    value.meta().members();
}
```

Implementation:

```cpp
template <typename T>
auto RuntimeTypeObject<T>::members(const void* data) const -> std::vector<RuntimeMember> {
    auto& object = *static_cast<const T*>(data);
    auto static_members = T::Meta::static_members();
    std::vector<RuntimeMember> out;
    out.reserve(static_members.size());
    for (const auto& member : static_members) {
        out.emplace_back(member.into_runtime(&object));
    }
    return out;
}
```

### 3. Extend `RuntimeType` with default object APIs

Add default no-op methods to `RuntimeType`:

```cpp
virtual auto members(const void* data) const -> std::vector<RuntimeMember>;
virtual auto member(const void* data, std::string_view name) const -> std::optional<RuntimeMember>;
```

For atomic types, return empty/nullopt.

### 4. Make `RuntimeType::instance<T>()` choose atomic vs object

Currently `RuntimeType::instance<T>()` works when `Type<T>::name()` exists. Change it to select in this order:

1. If `T` has reflected object metadata, return `RuntimeTypeObject<T>`.
2. Else if `Type<T>` exists, return `RuntimeTypeAtomic<T>`.
3. Else static assert with a good unsupported-type message.

Suggested concepts:

```cpp
template <typename T>
concept HasStaticReflection = requires {
    T::Meta::name;
    T::Meta::static_members();
};

template <typename T>
concept HasAtomicReflection = requires(const T& value) {
    { Type<T>::name() } -> std::convertible_to<std::string_view>;
    { Type<T>::as_string(value) } -> std::convertible_to<std::string>;
};
```

Potential issue: reflected objects also have `Type<T>` specializations if we add them for `Transform`. If both exist, prefer object wrapper so children are visible.

### 5. Add child traversal to `RuntimeMember`

Extend `RuntimeMember`:

```cpp
auto data() const -> const void*;
auto has_members() const -> bool;
auto members() const -> std::vector<RuntimeMember>;
auto get(std::string_view name) const -> std::optional<RuntimeMember>;
```

Implementation uses `meta.type_info`:

```cpp
auto RuntimeMember::members() const -> std::vector<RuntimeMember> {
    return meta.type_info->members(data());
}
```

### 6. Keep string conversion behavior

Nested/object runtime types still need string conversion. Recommended behavior:

- Atomic: existing behavior.
- Object: if `Type<T>::as_string` exists, use it.
- Object without `Type<T>::as_string`: produce generic object string like:

```text
Transform{position_: [0, 0, 0], scale_: [1, 1, 1], rotation_: [0, 0, 0, 1]}
```

This avoids requiring every reflected object to also have a `Type<T>` specialization.

### 7. Decide whether vectors are atomic or objects

Short term: keep `linalg::VectorN<T>` as atomic. This already supports readable Transform strings.

Future option: add object-style vector runtime wrappers so editor UI can expose `x/y/z/w` individually.

Recommended staged approach:

1. Implement nested object support for macro-reflected classes only.
2. Keep vectors atomic.
3. Later add specialized `RuntimeTypeVector<T, N>` if editor-style vector expansion is needed.

## Implementation steps

### Commit 1: Add runtime type kind and default child APIs

Files:

- `include/engine/reflection/type.h`
- `engine/reflection/type.cpp`

Changes:

- Add `RuntimeTypeKind`.
- Add `kind()`, `is_atomic()`, `is_object()`.
- Add default empty `members()` / `member()` methods.
- Add `RuntimeMember::data()` helper.

Build after this commit.

### Commit 2: Add object reflection concepts and `RuntimeTypeObject<T>`

Files:

- `include/engine/reflection/type.h`

Changes:

- Add `HasStaticReflection` and `HasAtomicReflection` concepts.
- Add `RuntimeTypeObject<T>`.
- Make `RuntimeType::instance<T>()` choose object vs atomic vs unsupported.

Build after this commit.

### Commit 3: Add `RuntimeMember` child traversal

Files:

- `include/engine/reflection/type.h`
- `engine/reflection/type.cpp`

Changes:

- `RuntimeMember::has_members()`.
- `RuntimeMember::members()`.
- `RuntimeMember::get(std::string_view)`.

Build after this commit.

### Commit 4: Add generic object string fallback

Files:

- `include/engine/reflection/type.h`

Changes:

- `RuntimeTypeObject<T>::to_string` and `to_human_string` should use `Type<T>` if available.
- Otherwise, format from runtime members.

Build after this commit.

### Commit 5: Add docs/tests or sample usage

Files:

- `docs/reflection.md` or tests if desired.

Example to verify manually:

```cpp
engine::Transform transform;
auto members = transform.meta().members();
auto position = transform.meta().get("position_");
auto position_children = position.members(); // empty at first if Vector3 remains atomic
```

If vectors remain atomic, `position.members()` returns empty, but `transform` itself still exposes `position_`, `scale_`, and `rotation_`.

## Tests to add later

- `RuntimeType::instance<Transform>()->kind() == RuntimeTypeKind::Object`.
- `RuntimeType::instance<double>()->kind() == RuntimeTypeKind::Atomic`.
- `transform.meta().get("position_").to_string()` returns vector text.
- `RuntimeType::instance<Transform>()->members(&transform)` returns three members.
- Unknown child lookup returns `std::nullopt` for the new optional API.
- Unsupported non-reflected/non-atomic types fail at compile time.

## Risks / decisions

- Returning `std::vector<RuntimeMember>` from runtime APIs allocates. This is fine for editor/debug reflection. If reflection becomes hot-path, switch to fixed arrays/spans later.
- `Meta::members()` currently returns `std::array`. `RuntimeTypeObject<T>` can convert that to vector for type-erased APIs.
- If a type has both macro reflection and a `Type<T>` specialization, prefer object reflection for traversal but use `Type<T>` for string formatting.
- Avoid making vectors nested in the first pass unless the editor absolutely needs per-component controls now.
