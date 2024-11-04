// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_permissions.h"
#include "utl/filesystem/utl_platform.h"
#include "utl/memory/utl_allocator.h"
#include "utl/string/utl_basic_string_view.h"
#include "utl/string/utl_is_string_char.h"
#include "utl/string/utl_libc.h"
#include "utl/tempus/utl_libc.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"
#include "utl/utility/utl_compressed_pair.h"

__UFS_NAMESPACE_BEGIN

enum class file_type : unsigned char {
    block_device,
    character_device,
    directory,
    fifo,
    link,
    regular,
    socket,
    other,
    invalid,
    COUNT = invalid
};

template <file_type, typename Alloc>
class basic_explicit_file;

template <typename Alloc = __UTL allocator<path_char>>
class UTL_PUBLIC_TEMPLATE basic_file {
    using allocator_type = Alloc;
    using alloc_traits = __UTL allocator_traits<allocator_type>;
    using pointer = typename alloc_traits::pointer;
    template <file_type Type>
    using explicit_file = basic_explicit_file<Type, allocator_type>;
    using view_type = basic_string_view<path_char>;

public:
    __UTL_HIDE_FROM_ABI inline constexpr basic_file() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(allocator_type)) = default;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file(basic_file const& other)
        : data_{other.metadata_ref(), details::compressed_pair::value_initialize} {
        copy_path(other);
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file& operator=(basic_file const& other) {
        if (this != __UTL addressof(other)) {
            destroy();
            metadata_ref() = other.metadata_ref();
            alloc_traits::assign(alloc_ref(), other.alloc_ref());
            copy_path(other);
        }

        return *this;
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file& operator=(basic_file&& other) noexcept(
        (alloc_traits::propagate_on_container_move_assignment::value &&
            alloc_traits::is_always_equal::value) ||
        !utl::with_exceptions) {
        using can_move_t =
            conjunction<typename alloc_traits::propagate_on_container_move_assignment,
                typename alloc_traits::is_always_equal>;

        move_assign(other, can_move_t());
        return *this;
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file(
        basic_file const& other, allocator_type const& a)
        : data_{other.metadata_ref(), a} {
        copy_path(other);
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file(basic_file&& other) noexcept
        : data_{__UTL exchange(other.metadata_ref(), details::file_data{}),
              details::compressed_pair::value_initialize}
        , path_(__UTL exchange(other.path_, decltype(other.path_){})) {}

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file(
        basic_file&& other, allocator_type const& a) noexcept(alloc_traits::is_always_equal::value)
        : data_{details::compressed_pair::default_initialize, a} {
        static constexpr alloc_traits::is_always_equal overload{};
        on_move_construct_with_alloc(other, overload);
    }

    __UTL_HIDE_FROM_ABI
    explicit inline UTL_CONSTEXPR_CXX14 basic_file(allocator_type const& alloc) noexcept
        : data_{details::compressed_pair::value_initialize, alloc} {}

    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_file(
        view_type view, allocator_type const& a = allocator_type{}) UTL_THROWS
        : data_{view, a} {
        initialize_path(view);
    }

    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_file(
        path_char const* path, allocator_type const& a = allocator_type{}) UTL_THROWS
        : basic_file(view_type(path), a) {}

#if UTL_SUPPORTS_CHAR8_T
    template <UTL_CONCEPT_CXX20(string_char) Char UTL_CONSTRAINT_CXX11(
        sizeof(Char) == sizeof(path_char))>
    UTL_CONSTRAINT_CXX20(sizeof(Char) == sizeof(path_char))
    __UTL_HIDE_FROM_ABI explicit inline basic_file(
        Char const* bytes, allocator_type const& a = allocator_type{}) UTL_THROWS
        : basic_file(view_type(reinterpret_cast<path_char*>(bytes)), a) {}
#endif

    __UTL_HIDE_FROM_ABI inline constexpr view_type path() const noexcept {
        return basic_string_view<path_char>{path_str(), path_size()};
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 view_type basename() const noexcept {
        auto path_view = path();
        return path_view.substr(path_view.find_last_of(u8'/') + 1);
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 view_type basename(
        view_type suffix_to_remove) const noexcept {
        auto name = basename();
        if (suffix_to_remove.size() == name.size()) {
            return name;
        }

        return remove_suffix(name, name.ends_with(suffix_to_remove) * suffix_to_remove.size());
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 view_type extension() const noexcept {
        auto name = basename();
        return remove_prefix(name, add_sat(name.find_last_of(u8'.'), 1));
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_string_view<path_char> dirname() const noexcept {
        auto path_view = path();
        return path_view.substr(0, path_view.find_last_of(u8'/'));
    }
    __UTL_HIDE_FROM_ABI inline constexpr bool exists() const noexcept {
        return metadata().exists > 0;
    }
    __UTL_HIDE_FROM_ABI inline constexpr size_t size() const noexcept {
        return metadata().file_size;
    }
    __UTL_HIDE_FROM_ABI inline constexpr size_t hard_links() const noexcept {
        return metadata().hard_links;
    }
    __UTL_HIDE_FROM_ABI inline constexpr perms permissions() const noexcept {
        return metadata().perimissions;
    }
    __UTL_HIDE_FROM_ABI inline constexpr file_type type() const noexcept { return metadata().type; }

    __UTL_HIDE_FROM_ABI inline void refresh_metadata() UTL_THROWS { populate(); }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX20 ~basic_file() noexcept { destroy(); }

private:
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void destroy() noexcept {
        if (path().size() >= inline_size) {
            alloc_traits::deallocate(alloc_ref(), path_.dynamic, metadata_ref().path_size);
            path_.dynamic = nullptr;
            metadata_ref() = file_metadata{};
        }
    }

    __UTL_HIDE_FROM_ABI inline constexpr size_t path_size() const noexcept {
        return metadata_ref().aux16;
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void set_path_size(uint16_t val) noexcept {
        metadata_ref().aux16 = val;
    }

    __UTL_HIDE_FROM_ABI inline constexpr path_char const* path_str() const noexcept {
        return path_size() < inline_size ? path_.stack : path_.dynamic;
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void move_assign(basic_file& other, false_type)
        UTL_THROWS {
        *this = other;
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void move_assign(basic_file& other, true_type) noexcept {
        destroy();
        metadata_ref() = other.metadata_ref();
        alloc_traits::assign(alloc_ref(), __UTL move(other.alloc_ref()));
        path_ = __UTL exchange(other.path_, decltype(other.path_){});
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void on_move_construct_with_alloc(
        basic_file& other, false_type) UTL_THROWS {
        if (other.path_size() >= inline_size &&
            !alloc_traits::equals(alloc_ref(), other.alloc_ref())) {
            metadata_ref() = other.metadata_ref();
            copy_path(other);
        } else {
            static constexpr false_type equals{};
            on_move_construct_with_alloc(other, pos, count, equals);
        }
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void on_move_construct_with_alloc(
        basic_file& other, true_type) noexcept {
        static constexpr details::file_data default_data{};
        metadata_ref() = __UTL exchange(other.metadata_ref(), default_data);
        if (other.path_size() == 0) {
            path_.dynamic = nullptr;
            return;
        }

        if (other.path_size() < inline_size) {
            ::new (__UTL addressof(path_.stack)) decltype(path_.stack);
            __UTL libc::memcpy(path_.stack, other.path_str(), other.path_size() + 1);
        } else {
            path_.dynamic = __UTL exchange(other.path_.dynamic, nullptr);
        }
    }

    __UTL_HIDE_FROM_ABI inline constexpr bool populated() const noexcept {
        return metadata_ref().valid();
    }

    __UTL_HIDE_FROM_ABI void populate() const;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void copy_path(basic_file const& other) {
        if (other.path_size() == 0) {
            path_.dynamic = nullptr;
            return;
        }

        if (other.path_size() < inline_size) {
            ::new (__UTL addressof(path_.stack)) decltype(path_.stack);
            __UTL libc::memcpy(path_.stack, other.path_str(), other.path_size() + 1);
            return;
        }

        auto ptr = alloc_traits::allocate(alloc_ref(), other.path_size() + 1);
        __UTL_MEMCPY(__UTL to_address(ptr), other.path_str(), other.path_size() + 1);
        __UTL to_address(ptr)[path.size()] = 0;
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void initialize_path(view_type path) {
        if (path.empty()) {
            return;
        }

        auto const size = paths::effective_length(path) + 1;

        if (size < inline_size) {
            ::new (path_.stack) decltype(path_.stack);
            paths::collapse(path, __UTL span<path_char>(path_.stack));
        } else {
            auto ptr = alloc_traits::allocate(alloc, size);
            paths::collapse(path, __UTL span<path_char>(__UTL to_address(ptr), size));
            path_.dynamic = std::move(ptr);
        }
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 file_metadata const& metadata() const noexcept {
        if (!populated()) {
            populate();
        }
        return metadata_ref();
    }

    __UTL_HIDE_FROM_ABI inline constexpr file_metadata const& metadata_ref() const noexcept {
        return data_.first();
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 file_metadata& metadata_ref() noexcept {
        return data_.first();
    }

    __UTL_HIDE_FROM_ABI inline constexpr allocator_type const& alloc_ref() const noexcept {
        return data_.second();
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 allocator_type& alloc_ref() noexcept {
        return data_.second();
    }

    using data_type = compressed_pair<file_metadata, allocator_type>;
    mutable data_type data_;
    static constexpr size_t inline_size =
        sizeof(file_metadata) == sizeof(data_type) ? 16 : sizeof(pointer);
    union {
        pointer dynamic;
        path_char stack[inline_size];
    } path_;
};

__UFS_NAMESPACE_END
