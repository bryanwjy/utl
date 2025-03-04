// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_filesystem_fwd.h"

#if !defined(UTL_FILESYSTEM_FILE_HANDLE_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

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

template <typename... Os>
struct __UTL_PUBLIC_TEMPLATE file_flags_t;

template <UTL_CONCEPT_CXX20(derived_from<details::open::flag_t>) Flag, typename... Os>
UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_base_of(Flag, file_flags_t<Os...>))
__UTL_HIDE_FROM_ABI inline constexpr UTL_ENABLE_IF_CXX11(
    Flag const&, UTL_TRAIT_is_base_of(details::open::flag_t, Flag) &&
    UTL_TRAIT_is_base_of(Flag, file_flags_t<Os...>))
get(file_flags_t<Os...> const& flags) noexcept {
    return (FlagType const&)flags;
}

template <typename... Os>
struct __UTL_PUBLIC_TEMPLATE file_flags_t<Os...> : private Os... {
    static_assert(
        UTL_TRAIT_conjunction(is_base_of<details::open::flag_t, Os>...), "Invalid flag type");

public:
    __UTL_HIDE_FROM_ABI explicit inline constexpr file_flags_t() noexcept = default;
    __UTL_HIDE_FROM_ABI inline constexpr file_flags_t(file_flags_t const&) noexcept = default;
    __UTL_HIDE_FROM_ABI inline constexpr file_flags_t& operator=(
        file_flags_t const&) noexcept = default;

    template <typename... Args UTL_CONSTRAINT_CXX11(sizeof...(Args) == sizeof...(Os))>
    UTL_CONSTRAINT_CXX20(sizeof...(Args) == sizeof...(Os))
    __UTL_HIDE_FROM_ABI explicit inline constexpr file_flags_t(Args... args) noexcept
        : Os{args}... {}

    template <typename Arg, typename... Ps UTL_CONSTRAINT_CXX11(
        (sizeof...(Ps) + 1 == sizeof...(Os)))>
    UTL_CONSTRAINT_CXX20(sizeof...(Ps) + 1 == sizeof...(Os))
    __UTL_HIDE_FROM_ABI explicit inline constexpr file_flags_t(
        file_flags_t<Ps...> opt, Arg arg) noexcept
        : file_flags_t{((Ps const&)opt)..., arg} {
        static_assert(
            UTL_TRAIT_conjunction(is_base_of<Ps, file_flags_t<Ps...>>...), "Invalid type");
    }

    template <typename U UTL_CONSTRAINT_CXX11(!UTL_TRAIT_disjunction(
        is_same<details::open::flag_type_t<U>, details::open::flag_type_t<Os>>...))>
    UTL_CONSTRAINT_CXX20(requires(U const& u) {
        flag_base(u);
        requires !(... || same_as<details::open::flag_type_t<U>, details::open::flag_type_t<Os>>);
    })
    __UTL_HIDE_FROM_ABI inline constexpr file_flags_t<Os..., U> operator|(U u) const noexcept {
        return file_flags_t<Os..., U>{*this, u};
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

    template <UTL_CONCEPT_CXX20(derived_from<flag_t>) U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_base_of(flag_t, U) && !UTL_TRAIT_is_base_of(B, U))>
    UTL_CONSTRAINT_CXX20(!UTL_TRAIT_is_base_of(D, U))
    __UTL_HIDE_FROM_ABI inline constexpr file_flags_t<D, U> operator|(U u) const noexcept {
        return file_flags_t<D, U>{static_cast<D const&>(*this), u};
    }
};

template <typename D>
struct flag_base_t<D, void> : flag_t {
protected:
    __UTL_HIDE_FROM_ABI ~flag_base_t() noexcept = default;
    __UTL_HIDE_FROM_ABI friend inline UTL_CONSTEVAL type_identity<D> flag_base(D const&) noexcept {
        return {};
    }

    template <UTL_CONCEPT_CXX20(derived_from<flag_t>) U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_base_of(flag_t, U) && !UTL_TRAIT_is_same(D, U))>
    UTL_CONSTRAINT_CXX20(!same_as<D, U>)
    __UTL_HIDE_FROM_ABI inline constexpr file_flags_t<D, U> operator|(U u) const noexcept {
        return file_flags_t<D, U>{static_cast<D const&>(*this), u};
    }
};

struct access_t : flag_t {};
struct write_sync_t : flag_t {};
struct creation_t : flag_t {};

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
    using handle_type = basic_file_handle<io::access::read>;
};
struct write_only_t : flag_base_t<write_only_t, access_t> {
    using handle_type = basic_file_handle<io::access::write>;
};
struct read_write_t : flag_base_t<read_write_t, access_t> {
    using handle_type = basic_file_handle<io::access::read_write>;
};

struct append_t : flag_t<append_t> {};
struct exclusive_t : flag_base_t<exclusive_t> {};
struct close_on_exec_t : flag_base_t<close_on_exec_t> {};
struct trucate_t : flag_base_t<truncate_t> {};
struct no_control_tty_t : flag_base_t<no_control_tty_t> {};
struct path_only_t : flag_base_t<path_only_t> {};
struct unbuffered_t : flag_base_t<unbuffered_t> {};
struct no_follow_symlink_t : flag_base_t<no_follow_symlink_t> {};
struct non_blocking_t : flag_base_t<non_blocking_t> {};
struct file_sync_t : flag_base_t<file_sync_t, write_sync_t> {};
struct data_sync_t : flag_base_t<data_sync_t, write_sync_t> {};

template <typename T>
struct is_file_flags : false_type {};

template <typename T, typename Flags>
using flag_element_t = remove_cvref_t<decltype(get<T>(__UTL declval<Flags const&>()))>;

template <typename... Fs>
struct is_file_flags<file_flags_t<Fs...>> : true_type {};

template <typename B, typename T, typename = void>
struct has_flag : false_type {};
template <typename B, typename T>
struct has_flag<T, void_t<flag_element_t<B, T>>> : true_type {};

template <typename T>
struct is_valid_flags : is_file_flags<T> {};

#if UTL_CXX20
template <typename T>
concept valid_flags = is_file_flags<T>::value;
#endif

template <typename Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline UTL_CONSTEVAL bool ensure_valid_flags() noexcept {
    static_assert(!has_flag<trucate_t, Flags>::value || !has_flag<read_only_t, Flags>::value,
        "Only writable files may be truncated");
    static_assert(!has_flag<exclusive_t, Flags>::value || has_flag<creation_t, Flags>::value,
        "Exclusive flag only valid on file creation");
    static_assert(!has_flag<write_sync_t, Flags>::value || !has_flag<read_only_t, Flags>::value,
        "Integrity flags may only be applied on writable files");
    static_assert(!has_flag<temporary_t, Flags>::value || !has_flag<read_only_t, Flags>::value,
        "Temporary files must be writable");
    static_assert(!has_flag<path_only_t, Flags>::value ||
            ((!has_flag<access_t, Flags>::value || has_flag<read_only_t, Flags>::value) &&
                !UTL_TRAIT_disjunction(has_flag<exclusive_t, Flags>,
                    has_flag<no_control_tty_t, Flags>, has_flag<truncation_t, Flags>,
                    has_flag<unbuffered_t, Flags>, has_flag<write_sync_t, Flags>,
                    has_flag<creation_t, Flags>, has_flag<append_t, Flags>,
                    has_flag<non_blocking_t, Flags>)),
        "Flag operation disabled due to set 'path_only' flag");
    return true;
}

template <file_type Type, typename Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline UTL_CONSTEVAL bool ensure_valid_type_and_flags() noexcept {
    static_assert(ensure_valid_flags<Flags>(), "Invalid flags");
    static_assert(
        __UTL to_underlying(Type) < __UTL to_underlying(file_type::COUNT), "Invalid file type");

    static_assert(!has_flag<creation_t, Flags>::value || Type == file_type::regular_file,
        "Only regular files may be created");
    static_assert(!has_flag<trucate_t, Flags>::value || Type == file_type::regular_file,
        "Only regular files may be truncated");
    static_assert(!has_flag<no_control_tty_t, Flags>::value || Type == file_type::character_device,
        "'no_control_tty' flag only applies to character devices");
    using is_read_only =
        disjunction<negation<has_flag<access_t, Flags>>, has_flag<read_only_t, Flags>>;
    static_assert(
        Type != file_type::directory || is_read_only::value, "Directories cannot be writable");
    static_assert(Type != file_type::symlink ||
            (is_read_only::value &&
                UTL_TRAIT_disjunction(has_flag<exclusive_t, Flags>,
                    has_flag<no_control_tty_t, Flags>, has_flag<truncation_t, Flags>,
                    has_flag<unbuffered_t, Flags>, has_flag<write_sync_t, Flags>,
                    has_flag<creation_t, Flags>, has_flag<append_t, Flags>,
                    has_flag<non_blocking_t, Flags>)),
        "Invalid flags detected, symlinks always have IO disabled");

    return true;
}

template <typename Flags>
__UTL_HIDE_FROM_ABI auto get_handle_type(int) noexcept ->
    typename flag_element_t<access_t, Flags>::handle_type;

// default read only access
template <typename Flags>
__UTL_HIDE_FROM_ABI auto get_handle_type(...) noexcept -> basic_file_handle<io::access::read>;

template <typename Flags>
using handle_type = decltype(get_handle_type<Flags>(0));

} // namespace open
} // namespace details

namespace file_flags {
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::read_only_t read_only{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::write_only_t write_only{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::read_write_t read_write{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::create_factory_t create_file{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::temporary_factory_t temporary_file{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::exclusive_t exclusive{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::append_t append_mode{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::truncate_t truncate{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::path_only_t path_only{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::unbuffered_t unbuffered{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::close_on_exec_t close_on_exec{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::non_blocking_t non_blocking{};
// Specify file_type::symlink implies no_follow
// __UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::no_follow_symlink_t
// no_follow{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::file_sync_t ensure_file_sync{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::data_sync_t ensure_data_sync{};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr details::open::no_control_tty_t no_control_tty{};

} // namespace file_flags

__UFS_NAMESPACE_END
