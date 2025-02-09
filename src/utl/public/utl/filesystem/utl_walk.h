// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_filesystem_fwd.h"

#include "utl/filesystem/utl_platform.h"

__UFS_NAMESPACE_BEGIN

using walk_result = __UTL expected<void, error_code>;

class walk_error {
public:
    __UTL_HIDE_FROM_ABI inline constexpr walk_error(zpath_view path, error_code code) noexcept
        : path_{path}
        , code_{code} {}

    [[nodiscard]] zpath_view path() const noexcept { return path_; }
    [[nodiscard]] error_code value() const noexcept { return code_; }
    __UTL_HIDE_FROM_ABI inline constexpr operator walk_result() const noexcept {
        return walk_result{__UTL unexpect, code_};
    }

private:
    zpath_view path_;
    error_code code_;
};

__UFS_NAMESPACE_END

#define UTL_FILESYSTEM_WALK_PRIVATE_HEADER_GUARD
#if (UTL_TARGET_LINUX | UTL_TARGET_BSD | UTL_TARGET_APPLE)
#  include "utl/filesystem/posix/utl_walk.h"
#elif UTL_TARGET_MICROSOFT
#  include "utl/filesystem/winapi/utl_walk.h"
#else
#  error Invalid platform
#endif
#undef UTL_FILESYSTEM_WALK_PRIVATE_HEADER_GUARD

__UFS_NAMESPACE_BEGIN

#define UTL_TRAIT_is_walk_invocable(...) true

#if UTL_CXX20
template <typename T>
concept walk_invocable = true;
#endif

template <typename... Os>
struct __UTL_ABI_PUBLIC walk_attributes<Os...> : private Os... {
public:
    __UTL_HIDE_FROM_ABI explicit inline constexpr walk_attributes() noexcept(
        UTL_TRAIT_conjunction(is_nothrow_default_constructible<Os>...)) = default;

    template <typename... Args UTL_CONSTRAINT_CXX11(sizeof...(Args) == sizeof...(Os))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr walk_attributes(Args&&... args) noexcept(
        UTL_TRAIT_conjunction(is_nothrow_constructible<Os, Args>...))
        : Os{args}... {}

    template <typename Arg, typename... Ps UTL_CONSTRAINT_CXX11((sizeof...(Ps) > 0))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr walk_attributes(walk_attributes<Ps...>&& opt,
        Arg&& arg) noexcept(UTL_TRAIT_conjunction(is_nothrow_move_constructible<Ps>...) &&
        UTL_TRAIT_is_nothrow_constructible(Arg, Arg))
        : walk_attributes{((Ps&&)__UTL move(opt))..., __UTL forward<Arg>(arg)} {
        static_assert(
            UTL_TRAIT_conjunction(is_base<Ps, walk_attributes<Ps...>>...), "Invalid type");
    }

    template <typename Arg, typename... Ps UTL_CONSTRAINT_CXX11((sizeof...(Ps) > 0))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr walk_attributes(walk_attributes<Ps...> const& opt,
        Arg&& arg) noexcept(UTL_TRAIT_conjunction(is_nothrow_copy_constructible<Ps>...) &&
        UTL_TRAIT_is_nothrow_constructible(Arg, Arg))
        : walk_attributes{(Ps const&)opt..., __UTL forward<Arg>(arg)} {
        static_assert(
            UTL_TRAIT_conjunction(is_base<Ps, walk_attributes<Ps...>>...), "Invalid type");
    }

    template <typename U UTL_CONSTRAINT_CXX11(
        template_count<decay_t<U>, walk_attributes>::value == 0 &&
        UTL_TRAIT_is_base_of(
            details::basic_option<decay_t<U>, decay_t<U>::identifier>, decay_t<U>) &&
        UTL_TRAIT_conjunction(bool_constant<(Os::identifier != decay_t<U>::identifier)>...))>
    __UTL_HIDE_FROM_ABI inline constexpr walk_attributes<Os..., decay_t<U>>
    operator|(U&& u) const& noexcept(UTL_TRAIT_is_nothrow_constructible(
        walk_attributes<Os..., decay_t<U>>, walk_attributes const&, U)) {
        return walk_attributes<Os..., decay_t<U>>{*this, __UTL forward<U>(u)};
    }

    template <typename U UTL_CONSTRAINT_CXX11(
        template_count<decay_t<U>, walk_attributes>::value == 0 &&
        UTL_TRAIT_is_base_of(
            details::basic_option<decay_t<U>, decay_t<U>::identifier>, decay_t<U>) &&
        UTL_TRAIT_conjunction(bool_constant<(Os::identifier != decay_t<U>::identifier)>...))>
    __UTL_HIDE_FROM_ABI inline constexpr walk_attributes<Os..., decay_t<U>> operator|(
        U&& u) && noexcept(UTL_TRAIT_is_nothrow_constructible(walk_attributes<Os..., decay_t<U>>,
        walk_attributes, U)) {
        return walk_attributes<Os..., decay_t<U>>{__UTL move(*this), __UTL forward<U>(u)};
    }
};

template <typename>
struct is_walk_attributes : false_type {};
template <typename... Os>
struct is_walk_attributes<walk_attributes<Os...>> : true_type {};

namespace walk_attribute {

enum class category {
    direction,
    follow_symlinks,
    error_handler,
};

template <typename E>
class error_handler_t;
struct bottom_up_t;
struct top_down_t;

namespace details {
struct default_tag;

template <typename T>
struct category_map {
    friend constexpr category get_category_impl(category_map) noexcept;
};

template <typename T, category I>
struct basic_option {
    friend constexpr category get_category_impl(category_map<T>) noexcept { return I; }
    template <typename U>
    static constexpr category get_category() noexcept {
        return get_category_impl(category_map<U>{});
    }

    template <typename U>
    using rebind = basic_option<U, get_category<U>()>;

    __UTL_HIDE_FROM_ABI inline constexpr operator walk_attributes<T>() const& {
        return walk_attributes<T>{static_cast<T const&>(*this)};
    }

    __UTL_HIDE_FROM_ABI inline constexpr operator walk_attributes<T>() && {
        return walk_attributes<T>{static_cast<T&&>(*this)};
    }

    template <typename U UTL_CONSTRAINT_CXX11(
        I != U::identifier && UTL_TRAIT_is_base_of(rebind<U>, U))>
    __UTL_HIDE_FROM_ABI inline constexpr walk_attributes<T, U> operator|(U&& u) const& noexcept(
        UTL_TRAIT_is_nothrow_constructible(walk_attributes<T, U>, T const&, U)) {
        return walk_attributes<T, U>{static_cast<T const&>(*this), __UTL forward<U>(u)};
    }

    template <typename U UTL_CONSTRAINT_CXX11(
        I != U::identifier && UTL_TRAIT_is_base_of(rebind<U>, U))>
    __UTL_HIDE_FROM_ABI inline constexpr walk_attributes<T, U> operator|(U&& u) && noexcept(
        UTL_TRAIT_is_nothrow_constructible(walk_attributes<T, U>, T, U)) {
        return walk_attributes<T, U>{static_cast<T&&>(*this), __UTL forward<U>(u)};
    }
};

struct error_handler_factory_t;

template <typename>
struct is_error_handler : false_type {};
template <typename E>
struct is_error_handler<error_handler_t<E>> : true_type {};

struct error_handler_factory_t {
    template <typename E UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable_r(walk_result, decay_t<E>, walk_error) &&
        !is_error_handler<decay_t<E>>::value)>
    __UTL_HIDE_FROM_ABI inline constexpr error_handler_t<decay_t<E>> operator->*(
        E&& handler) const noexcept {
        return error_handler_t<decay_t<E>>{__UTL forward<E>(handler)};
    }
};
} // namespace details

template <typename E>
class error_handler_t : public details::basic_option<error_handler_t<E>, category::error_handler> {
public:
    __UTL_HIDE_FROM_ABI inline constexpr walk_result operator()(walk_error&& error) const
        noexcept(UTL_TRAIT_is_nothrow_invocable(E, walk_error)) {
        return func_(__UTL move(error));
    }

private:
    friend struct details::error_handler_factory_t;
    template <typename Arg>
    __UTL_HIDE_FROM_ABI explicit inline constexpr error_handler_t(Arg&& arg) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, Arg))
        : func_(__UTL forward<Arg>(arg)) {}

    mutable E func_;
};

template <>
class error_handler_t<details::default_tag> :
    public details::basic_option<error_handler_t<details::default_tag>, category::error_handler> {
    __UTL_HIDE_FROM_ABI inline constexpr walk_result operator(walk_error&& err) const noexcept {
        return static_cast<walk_result>(err);
    }

    __UTL_HIDE_FROM_ABI inline constexpr
    operator walk_attributes<error_handler_t<details::default_tag>, top_down_t>() const noexcept {
        return walk_attributes<error_handler_t<details::default_tag>, top_down_t>{};
    }
};

struct top_down_t : details::basic_option<top_down_t, category::direction> {
    __UTL_HIDE_FROM_ABI explicit inline constexpr top_down_t() noexcept = default;
};

struct bottom_up_t : details::basic_option<bottom_up_t, category::direction> {
    __UTL_HIDE_FROM_ABI explicit inline constexpr bottom_up_t() noexcept = default;
};

struct follow_symlinks_t : details::basic_option<follow_symlinks_t, category::follow_symlinks> {
    __UTL_HIDE_FROM_ABI explicit inline constexpr follow_symlinks_t() noexcept = default;
};

__UTL_HIDE_FROM_ABI inline constexpr top_down_t top_down{};
__UTL_HIDE_FROM_ABI inline constexpr bottom_up_t bottom_up{};
__UTL_HIDE_FROM_ABI inline constexpr follow_symlinks_t follow_symlinks{};
__UTL_HIDE_FROM_ABI inline constexpr details::error_handler_factory_t error_handler{};

} // namespace walk_attribute

// void func(views::directory dir) {
//     walk(
//         dir, [](auto const& value) { dosomething(value); },
//         walk_attribute::bottom_up | walk_attribute::follow_symlinks |
//             walk_attribute::error_handler->*[](walk_error error) { return false; });
// }

// TODO
/**
 * ufs::ranges::directory
 * ufs::ranges::walk -> top_down only
 * follow links? Cycles?
 */

template <UTL_CONCEPT_CXX20(walk_invocable) F, typename Opt UTL_CONSTRAINT_CXX11(
    details::is_walk_attributes<remove_cvref_t<Opt>>::value)>
__UTL_HIDE_FROM_ABI walk_result walk(views::directory dir, F&& func, Opt&& options);

template <UTL_CONCEPT_CXX20(walk_invocable) F, typename Opt UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_convertible(Opt, walk_attributes<Opt>))>
__UTL_HIDE_FROM_ABI inline walk_result walk(views::directory dir, F&& func, Opt&& options) {
    return details::walk_impl(dir.path(), __UTL forward<F>(func),
        static_cast<walk_attributes<Opt>>(__UTL forward<Opt>(options)));
}

template <UTL_CONCEPT_CXX20(walk_invocable) F, typename E UTL_CONSTRAINT_CXX11(
    always_true<decltype(walk_attribute::error_handler->*declval<E>())>())>
__UTL_HIDE_FROM_ABI inline walk_result walk(views::directory dir, F&& func, E&& handler) {
    return details::walk_impl(dir.path(), __UTL forward<F>(func),
        walk_attribute::error_handler->*__UTL forward<E>(handler));
}

template <UTL_CONCEPT_CXX20(walk_invocable) F, UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_walk_invocable(F))>
__UTL_HIDE_FROM_ABI inline walk_result walk(views::directory dir, F&& func) {
    return walk(dir, __UTL forward<F>(func), walk_attribute::top_down);
}

#ifdef UTL_CXX17
namespace details {
template <typename F>
class function_type;
template <typename R, typename... Args>
class function_type<R (*)(Args...)> {
    using value_type = R (*)(Args...);
    static_assert(UTL_TRAIT_is_boolean(R) || UTL_TRAIT_is_void(R), "Invalid return type");

public:
    __UTL_HIDE_FROM_ABI inline explicit function_type(value_type func) noexcept : func_{func} {}

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr auto operator()(Args&&... args) const
        noexcept(UTL_TRAIT_is_nothrow_invocable(F, Args...)) -> __UTL invoke_result_t<F, Args...> {
        return func_(__UTL forward<Args>(args)...);
    }

private:
    value_type func_;
};

template <typename F>
using visitor_base =
    conditional_t<UTL_TRAIT_is_class(F), F, details::function_type<__UTL decay_t<F>>>;

struct file_visit_t {
    __UTL_HIDE_FROM_ABI explicit inline constexpr file_visit_t() noexcept = default;
};
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 file_visit_t file_visit{};

} // namespace details
template <UTL_CONCEPT_CXX20(walk_invocable)... Fs>
class __UTL_PUBLIC_TEMPLATE file_visitor : private details::visitor_base<Fs>... {
    using Fs::operator()...;

    template <typename Arg>
    using result_type = decltype(__UTL declval<file_visitor>()(__UTL declval<Arg>()));

public:
    template <typename... Gs>
    file_visitor(Gs&&... funcs) noexcept(UTL_TRAIT_conjunction(is_nothrow_constructible<Fs, Gs>...))
        : visitor_base<Fs>{__UTL forward<Gs>(funcs)}... {}

    template <typename Arg UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_boolean_testable(result_type<Arg>))>
    bool operator()(details::file_visit_t, Arg&& arg) const {
        return static_cast<bool>(operator()(__UTL forward<Arg>(arg)));
    }

    template <typename Arg UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_void(result_type<Arg>))>
    bool operator()(details::file_visit_t, Arg&& arg) const {
        return operator()(__UTL forward<Arg>(arg)), true;
    }
};
#else  // UTL_CXX17
// TODO
#endif // UTL_CXX17

#undef UTL_TRAIT_is_walk_predicate
__UFS_NAMESPACE_END
