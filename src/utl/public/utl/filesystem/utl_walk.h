// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_filesystem_fwd.h"

#include "utl/filesystem/utl_platform.h"

__UFS_NAMESPACE_BEGIN

class walk_error {
public:
    __UTL_HIDE_FROM_ABI inline constexpr walk_error(zpath_view path, error_code code) noexcept
        : path_{path}
        , code_{code} {}

    [[nodiscard]] zpath_view path() const noexcept { return path_; }
    [[nodiscard]] error_code value() const noexcept { return code_; }
    __UTL_HIDE_FROM_ABI inline constexpr operator error_code() const noexcept { return code_; }

private:
    zpath_view path_;
    error_code code_;
};

using walk_result = __UTL expected<void, walk_error>;

namespace details {
template <typename F, typename E>
__UTL_HIDE_FROM_ABI walk_result walk_impl(zpath_view, F&&, E&&);
} // namespace details

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

// TODO
/**
 * ufs::ranges::directory
 * ufs::ranges::walk -> top_down only
 * follow links? Cycles?
 */

template <UTL_CONCEPT_CXX20(walk_invocable) F,
    UTL_CONCEPT_CXX20(predicate<walk_error>) E UTL_CONSTAINT_CXX11(
        UTL_TRAIT_is_walk_invocable(F) && UTL_TRAIT_is_invocable_r(bool, E))>
__UTL_HIDE_FROM_ABI inline walk_result walk(views::directory dir, F&& func, E&& continue_on_error) {
    return details::walk_impl(
        dir.path(), __UTL forward<F>(func), __UTL forward<E>(continue_on_error));
}

template <UTL_CONCEPT_CXX20(walk_invocable) F UTL_CONSTAINT_CXX11(UTL_TRAIT_is_walk_invocable(F))>
__UTL_HIDE_FROM_ABI inline walk_result walk(views::directory dir, F&& func) {
    return walk(dir, top_down, __UTL forward<F>(func), [](walk_error const&) { return true; });
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
UTL_INLINE_CXX17 file_visit_t file_visit{};

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
