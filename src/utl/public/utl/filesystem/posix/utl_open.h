// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_filesystem_fwd.h"

#if !defined(UTL_FILESYSTEM_FILE_HANDLE_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/expected/utl_expected.h"
#include "utl/filesystem/utl_path.h"

__UFS_NAMESPACE_BEGIN
namespace details {
namespace open {
struct flag_t {
protected:
    __UTL_HIDE_FROM_ABI inline constexpr flag_t() noexcept = default;
    __UTL_HIDE_FROM_ABI ~flag_t() noexcept = default;
};

__UTL_HIDE_FROM_ABI void flag_base(...) noexcept = delete;
template <typename D>
struct flag_type {
    using type = typename decltype(flag_base(__UTL declval<D const&>()))::type;
};
template <typename D>
using flag_type_t = typename flag_type<D>::type;

} // namespace open
} // namespace details

template <UTL_CONCEPT_CXX20(derived_from<details::open::flag_t>) Flag, typename... Os>
UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_base_of(Flag, open_flags_t<Os...>))
__UTL_HIDE_FROM_ABI inline constexpr UTL_ENABLE_IF_CXX11(
    Flag const&, UTL_TRAIT_is_base_of(details::open::flag_t, Flag) &&
    UTL_TRAIT_is_base_of(Flag, open_flags_t<Os...>))
get(open_flags_t<Os...> const& flags) noexcept {
    return (FlagType const&)flags;
}

template <typename... Os>
struct __UTL_PUBLIC_TEMPLATE open_flags_t<Os...> : private Os... {
    static_assert(
        UTL_TRAIT_conjunction(is_base_of<details::open::flag_t, Os>...), "Invalid flag type");

public:
    __UTL_HIDE_FROM_ABI explicit inline constexpr open_flags_t() noexcept = default;

    template <typename... Args UTL_CONSTRAINT_CXX11(sizeof...(Args) == sizeof...(Os))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr open_flags_t(Args... args) noexcept
        : Os{args}... {}

    template <typename Arg, typename... Ps UTL_CONSTRAINT_CXX11(
        (sizeof...(Ps) + 1 == sizeof...(Os)))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr open_flags_t(
        open_flags_t<Ps...> opt, Arg arg) noexcept
        : open_flags_t{((Ps const&)opt)..., arg} {
        static_assert(
            UTL_TRAIT_conjunction(is_base_of<Ps, open_flags_t<Ps...>>...), "Invalid type");
    }

    template <typename U UTL_CONSTRAINT_CXX11(!UTL_TRAIT_disjunction(
        is_same<details::open::flag_type_t<U>, details::open::flag_type_t<Os>>...))>
    __UTL_HIDE_FROM_ABI inline constexpr open_flags_t<Os..., U> operator|(U u) const noexcept {
        return open_flags_t<Os..., U>{*this, u};
    }
};

namespace details {
namespace open {

template <typename D, typename B = void>
struct flag_base_t : B {
protected:
    __UTL_HIDE_FROM_ABI ~flag_base_t() noexcept = default;
    __UTL_HIDE_FROM_ABI friend inline constexpr type_identity<B> flag_base(D const&) noexcept {
        return {};
    }

    template <typename U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_base_of(flag_t, U) && !UTL_TRAIT_is_base_of(B, U))>
    __UTL_HIDE_FROM_ABI inline constexpr open_flags_t<D, U> operator|(U u) const noexcept {
        return open_flags_t<D, U>{static_cast<D const&>(*this), u};
    }
};

struct access_t : flag_t {};
struct exclusivity_t : flag_t {};
struct control_terminal_t : flag_t {};
struct truncation_t : flag_t {};
struct io_operation_t : flag_t {};
struct buffer_behaviour_t : flag_t {};
struct write_integrity_t : flag_t {};
struct creation_t : flag_t {};
struct exec_behaviour_t : flag_t {};
struct append_mode_t : flag_t {};
struct symlink_behaviour_t : flag_t {};
struct blocking_behaviour_t : flag_t {};

struct create_t : flag_base_t<create_t, creation_t> {
    __UTL_HIDE_FROM_ABI explicit inline constexpr create_t(perms p) noexcept : mode_{p} {}

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr perms mode() const noexcept { return mode_; }

private:
    perms mode_;
};

struct temporary_t : flag_base_t<temporary_t, creation_t> {
    __UTL_HIDE_FROM_ABI explicit inline constexpr temporary_t(perms p) noexcept : mode_{p} {}

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr perms mode() const noexcept { return mode_; }

private:
    perms mode_;
};

struct create_factory_t {
    __UTL_HIDE_FROM_ABI explicit inline constexpr create_factory_t() noexcept = default;

    __UTL_HIDE_FROM_ABI inline constexpr create_t operator()(perms p) const noexcept {
        return create_t{p};
    }
};

struct temporary_factory_t {
    __UTL_HIDE_FROM_ABI explicit inline constexpr temporary_factory_t() noexcept = default;

    __UTL_HIDE_FROM_ABI inline constexpr create_t operator()(perms p) const noexcept {
        return temporary_t{p};
    }
};

struct read_only_t : flag_base_t<read_only_t, access_t> {
    using handle_type = basic_file_handle<file_access::read>;
    __UTL_HIDE_FROM_ABI static constexpr int value = O_RDONLY;
};
struct write_only_t : flag_base_t<write_only_t, access_t> {
    using handle_type = basic_file_handle<file_access::write>;
    __UTL_HIDE_FROM_ABI static constexpr int value = O_WRONLY;
};
struct read_write_t : flag_base_t<read_write_t, access_t> {
    using handle_type = basic_file_handle<file_access::read_write>;
    __UTL_HIDE_FROM_ABI static constexpr int value = O_RDWR;
};

template <bool B>
struct append_t : flag_t<append_t<B>, append_mode_t> {
    __UTL_HIDE_FROM_ABI static constexpr int value = B ? O_APPEND : 0;
};

template <bool B>
struct exclusive_t : flag_base_t<exclusive_t<B>, exclusivity_t> {
    __UTL_HIDE_FROM_ABI static constexpr int value = B ? O_EXCL : 0;
};

template <bool B>
struct close_on_exec_t : flag_base_t<close_on_exec_t<B>, exec_behaviour_t> {
    __UTL_HIDE_FROM_ABI static constexpr int value = B ? O_CLOEXEC : 0;
};

template <bool B>
struct set_control_terminal_t : flag_base_t<set_control_terminal_t<B>, control_terminal_t> {
    __UTL_HIDE_FROM_ABI static constexpr int value = B ? 0 : O_NOCTTY;
};

template <bool B>
struct trucate_t : flag_base_t<truncate_t<B>, truncation_t> {
    __UTL_HIDE_FROM_ABI static constexpr int value = B ? O_TRUNC : 0;
};

template <bool B>
struct allow_io_t : flag_base_t<allow_io_t<B>, io_operation_t> {
    __UTL_HIDE_FROM_ABI static constexpr int value = B ? 0 : O_PATH;
};

template <bool B>
struct buffered_t : flag_base_t<buffered_t<B>, buffer_behaviour_t> {
    __UTL_HIDE_FROM_ABI static constexpr int value = B ? 0 : O_DIRECT;
};

template <bool B>
struct follow_symlink_t : flag_base_t<follow_symlink_t<B>, symlink_behaviour_t> {
    __UTL_HIDE_FROM_ABI static constexpr int value = B ? 0 : O_NOFOLLOW;
};

template <bool B>
struct blocking_t : flag_base_t<non_blocking_t, blocking_behaviour_t> {
    __UTL_HIDE_FROM_ABI static constexpr int value = B ? 0 : O_NONBLOCK;
};

struct normal_write_integrity_t : flag_base_t<normal_write_integrity_t, write_integrity_t> {
    __UTL_HIDE_FROM_ABI static constexpr int value = 0;
};
struct auto_file_sync_t : flag_base_t<auto_file_sync_t, write_integrity_t> {
    __UTL_HIDE_FROM_ABI static constexpr int value = O_SYNC;
};
struct auto_data_sync_t : flag_base_t<auto_data_sync_t, write_integrity_t> {
    __UTL_HIDE_FROM_ABI static constexpr int value = O_DSYNC;
};

template <typename T>
struct is_open_flags : false_type {};

template <typename T, typename Flags>
using flag_element_t = remove_cvref_t<decltype(get<T>(__UTL declval<Flags const&>()))>;

template <typename... Fs>
struct is_open_flags<open_flags_t<Fs...>> : true_type {};

template <typename B, typename T, typename = void>
struct has_flag : false_type {};
template <typename B, typename T>
struct has_flag<T, void_t<flag_element_t<B, T>>> : true_type {};

template <typename T>
struct is_valid_flags : is_open_flags<T> {};

#if UTL_CXX20
template <typename T>
concept valid_flags = is_open_flags<T>::value;
#endif

template <typename F, UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline constexpr UTL_ENABLE_IF_CXX11(
    int, __UTL always_true<flag_element_t<F, Flags>>()) to_posix_flags(int) noexcept {
    return flag_element_t<F, Flags>::value;
}

template <typename F, UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline constexpr int to_posix_flags(...) noexcept {
    return 0;
}

template <UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline constexpr int to_posix_flags(Flags f) noexcept {
    return to_posix_flags<access_t, Flags>(0) | to_posix_flags<exclusivity_t, Flags>(0) |
        to_posix_flags<control_terminal_t, Flags>(0) | to_posix_flags<truncation_t, Flags>(0) |
        to_posix_flags<io_operation_t, Flags>(0) | to_posix_flags<buffer_behaviour_t, Flags>(0) |
        to_posix_flags<write_integrity_t, Flags>(0) | to_posix_flags<creation_t, Flags>(0) |
        to_posix_flags<exec_behaviour_t, Flags>(0) | to_posix_flags<append_mode_t, Flags>(0) |
        to_posix_flags<symlink_behaviour_t, Flags>(0);
}

template <typename Flags>
UTL_CONSTRAINT_CXX20(has_flag<creation_t, Flags>::value)
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline constexpr UTL_ENABLE_IF_CXX11(
    int, has_flag<creation_t, Flags>::value) to_posix_mode(Flags f) noexcept {
    return to_underlying(get<creation_t>(f).mode());
}

UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline constexpr int to_posix_mode(...) noexcept {
    return 0;
}

template <file_type Type>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline UTL_CONSTEVAL int file_type_flag() noexcept {
    return 0;
}

template <>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline UTL_CONSTEVAL int
file_type_flag<file_type::directory>() noexcept {
    return O_DIRECTORY;
}

template <>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline UTL_CONSTEVAL int
file_type_flag<file_type::symlink>() noexcept {
    return O_NOFOLLOW | O_PATH;
}

template <typename Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline UTL_CONSTEVAL bool ensure_valid_flags() noexcept {
    static_assert(!has_flag<trucate_t<true>, Flags>::value || !has_flag<read_only_t, Flags>::value,
        "Only writable files may be truncated");
    static_assert(!has_flag<exclusive_t<true>, Flags>::value || has_flag<creation_t, Flags>::value,
        "Exclusive flag only valid on file creation");
    static_assert(
        !has_flag<write_integrity_t, Flags>::value || !has_flag<read_only_t, Flags>::value,
        "Integrity flags may only be applied on writable files");
    static_assert(!has_flag<temporary_t, Flags>::value || !has_flag<read_only_t, Flags>::value,
        "Temporary files must be writable");
    static_assert(!has_flag<allow_io_t<false>, Flags>::value ||
            ((!has_flag<access_t, Flags>::value || has_flag<read_only_t, Flags>::value) &&
                !UTL_TRAIT_disjunction(has_flag<exclusivity_t, Flags>,
                    has_flag<control_terminal_t, Flags>, has_flag<truncation_t, Flags>,
                    has_flag<buffer_behaviour_t, Flags>, has_flag<write_integrity_t, Flags>,
                    has_flag<creation_t, Flags>, has_flag<append_mode_t, Flags>,
                    has_flag<blocking_behaviour_t, Flags>)),
        "Flag operation disabled due to set 'disabled_io' flag");
}

template <file_type Type, typename Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline UTL_CONSTEVAL bool ensure_valid_type_and_flags() noexcept {
    static_assert(ensure_valid_flags(), "Invalid flags");
    static_assert(
        __UTL to_underlying(Type) < __UTL to_underlying(file_type::COUNT), "Invalid file type");

    static_assert(!has_flag<creation_t, Flags>::value || Type == file_type::regular_file,
        "Only regular files may be created");
    static_assert(!has_flag<trucate_t<true>, Flags>::value || Type == file_type::regular_file,
        "Only regular files may be truncated");
    static_assert(!has_flag<set_control_terminal_t<false>, Flags>::value ||
            Type == file_type::character_device,
        "'no_control_tty' flag only applies to character devices");
    using is_read_only =
        disjunction<negation<has_flag<access_t, Flags>>, has_flag<read_only_t, Flags>>;
    static_assert(
        Type != file_type::directory || is_read_only::value, "Directories cannot be writable");
    static_assert(Type != file_type::symlink ||
            (is_read_only::value &&
                !UTL_TRAIT_disjunction(has_flag<exclusivity_t, Flags>,
                    has_flag<control_terminal_t, Flags>, has_flag<truncation_t, Flags>,
                    has_flag<buffer_behaviour_t, Flags>, has_flag<write_integrity_t, Flags>,
                    has_flag<creation_t, Flags>, has_flag<append_mode_t, Flags>,
                    has_flag<blocking_behaviour_t, Flags>)),
        "Invalid flags detected, symlinks always have IO disabled");

    return true;
}

template <typename Flags>
__UTL_HIDE_FROM_ABI auto get_handle_type(int) noexcept ->
    typename flag_element_t<access_t, Flags>::handle_type;
template <typename Flags>
__UTL_HIDE_FROM_ABI auto get_handle_type(...) noexcept -> basic_file_handle<file_access::read>;

template <typename Flags>
using handle_type = decltype(get_handle_type<Flags>(0));

} // namespace open
} // namespace details

namespace open_flags {
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::read_only_t read_only{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::write_only_t write_only{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::read_write_t read_write{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::create_factory_t create_file{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::temporary_factory_t temporary_file{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::exclusive_t<true> exclusive{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::append_t<true> append_mode{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::truncate_t<true> truncate{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::allow_io_t<false> disabled_io{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::buffered_t<false> unbuffered{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::close_on_exec_t<true> close_on_exec{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::blocking_t<false> non_blocking{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::follow_symlink_t<false> no_follow{};
__UTL_HIDE_FROM_ABI
    UTL_INLINE_CXX17 constexpr details::open::auto_file_sync_t ensure_file_integrity{};
__UTL_HIDE_FROM_ABI
    UTL_INLINE_CXX17 constexpr details::open::auto_data_sync_t ensure_data_integrity{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::set_control_terminal_t<false> no_control_tty{};

} // namespace open_flags

template <typename T>
using open_result_t = __UTL expected<details::open::handle_type<T>, __UTL error_code>;

template <file_type Type, UTL_CONCEPT_CXX20(details::path::container) P,
    UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD)
UTL_ENABLE_IF_CXX11(open_result_t<Flags>, details::open::is_valid_flags<Flags>::value)
open(P const& path, Flags flags) noexcept(noexcept(details::path::terminated_path(path))) {
    // if P is a known symlink, Type can be specified to override symlink behaviour,
    // otherwise, P must either be unknown or equal to Type
    static_assert(details::path::typeof_file<P>::value == file_type::unknown ||
            details::path::typeof_file<P>::value == file_type::symlink ||
            Type == details::path::typeof_file<P>::value,
        "Type mismatch");
    static_assert(details::open::ensure_valid_type_and_flags<Type, Flags>(), "Invalid input");
    return details::path::terminated_path(path).and_then([&](zpath_view tmp_path) {
        using native_handle = details::file_descriptor_t;
        details::open::handle_type<T> result = static_cast<native_handle>(::open(tmp_path.data(),
            details::open::to_posix_flags(flags) | details::open::file_type_flag<Type>(),
            details::open::to_posix_mode(flags)));

        if (result) {
            return open_result_t<Flags>{__UTL in_place, __UTL move(result)};
        }

        return open_result_t<Flags>{__UTL unexpect, errno, __UTL system_category()};
    });
}

template <UTL_CONCEPT_CXX20(details::path::known_type_container) P,
    UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD)
UTL_ENABLE_IF_CXX11(open_result_t<Flags>, details::open::is_valid_flags<Flags>::value)
open(P const& path, Flags flags) noexcept(noexcept(details::path::terminated_path(path))) {
    return open<details::path::typeof_file<P>::value>(path, flags);
}

template <UTL_CONCEPT_CXX20(details::path::container) P,
    UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD)
UTL_ENABLE_IF_CXX11(open_result_t<Flags>, details::open::is_valid_flags<Flags>::value)
open(P const& path, Flags flags) noexcept(noexcept(details::path::terminated_path(path))) {
    static_assert(details::open::ensure_valid_flags<Type, Flags>(), "Invalid input");
    return details::path::terminated_path(path).and_then([&](zpath_view tmp_path) {
        using native_handle = details::file_descriptor_t;
        details::open::handle_type<T> result = static_cast<native_handle>(::open(tmp_path.data(),
            details::open::to_posix_flags(flags), details::open::to_posix_mode(flags)));

        if (result) {
            return open_result_t<Flags>{__UTL in_place, __UTL move(result)};
        }

        return open_result_t<Flags>{__UTL unexpect, errno, __UTL system_category()};
    });
}

template <file_type Type, UTL_CONCEPT_CXX20(details::path::container) P,
    UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD)
UTL_ENABLE_IF_CXX11(open_result_t<Flags>, details::open::is_valid_flags<Flags>::value)
open_at(read_file_handle const& dir, P const& path, Flags flags) noexcept(
    noexcept(details::path::terminated_path(path))) {
    static_assert(details::path::typeof_file<P>::value == file_type::unknown ||
            details::path::typeof_file<P>::value == file_type::symlink ||
            Type == details::path::typeof_file<P>::value,
        "Type mismatch");
    static_assert(details::open::ensure_valid_type_and_flags<Type, Flags>(), "Invalid input");

    return details::path::terminated_path(path).and_then([&](zpath_view tmp_path) {
        using native_handle = details::file_descriptor_t;
        details::open::handle_type<T> result =
            static_cast<native_handle>(::openat(dir.native_handle(), tmp_path.data(),
                details::open::to_posix_flags(flags) | details::open::file_type_flag<Type>(),
                details::open::to_posix_mode(flags)));

        if (result) {
            return open_result_t<Flags>{__UTL in_place, __UTL move(result)};
        }

        return open_result_t<Flags>{__UTL unexpect, errno, __UTL system_category()};
    });
}

template <UTL_CONCEPT_CXX20(details::path::known_type_container) P,
    UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD)
UTL_ENABLE_IF_CXX11(open_result_t<Flags>, details::open::is_valid_flags<Flags>::value)
open_at(read_file_handle const& dir, P const& path, Flags flags) noexcept(
    noexcept(details::path::terminated_path(path))) {
    return open_at<details::path::typeof_file<P>::value>(dir, path, flags);
}

template <UTL_CONCEPT_CXX20(details::path::container) P,
    UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD)
UTL_ENABLE_IF_CXX11(open_result_t<Flags>, details::open::is_valid_flags<Flags>::value)
open_at(read_file_handle const& dir, P const& path, Flags flags) noexcept(
    noexcept(details::path::terminated_path(path))) {
    static_assert(details::open::ensure_valid_flags<Type, Flags>(), "Invalid input");
    return details::path::terminated_path(path).and_then([&](zpath_view tmp_path) {
        using native_handle = details::file_descriptor_t;
        details::open::handle_type<T> result =
            static_cast<native_handle>(::openat(dir.native_handle(), tmp_path.data(),
                details::open::to_posix_flags(flags), details::open::to_posix_mode(flags)));

        if (result) {
            return open_result_t<Flags>{__UTL in_place, __UTL move(result)};
        }

        return open_result_t<Flags>{__UTL unexpect, errno, __UTL system_category()};
    });
}

struct cwd_handle_t {
    __UTL_HIDE_FROM_ABI explicit inline constexpr cwd_handle_t() noexcept = default;
};

__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr cwd_handle_t cwd_handle{};

template <file_type Type, UTL_CONCEPT_CXX20(details::path::container) P,
    UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) auto open_at(cwd_handle_t, P const& path, Flags flags) noexcept(
    noexcept(open<Type>(path, flags))) -> decltype(open<Type>(path, flags)) {
    return open<Type>(path, flags);
}

template <UTL_CONCEPT_CXX20(details::path::container) P, UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) auto open_at(cwd_handle_t, P const& path, Flags flags) noexcept(
    noexcept(open(path, flags))) -> decltype(open(path, flags)) {
    return open(path, flags);
}

template <UTL_CONCEPT_CXX20(details::path::typed_container<file_type::regular_file>) P, UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) file_handle create(P const& path, perms mode) noexcept(
    noexcept(details::path::terminated_path(path))) {
    return open<file_type::regular_file>(
        path, open_flags::create_file(mode) | open_flags::read_write | open_flags::truncate);
}

template <UTL_CONCEPT_CXX20(details::path::container) P,
    UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_CONSTRAINT_CXX20(!details::path::known_type_container<P>)
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) file_handle create(P const& path, perms mode) noexcept(
    noexcept(details::path::terminated_path(path))) {
    return open<file_type::regular_file>(
        path, open_flags::create_file(mode) | open_flags::read_write | open_flags::truncate);
}

__UFS_NAMESPACE_END
