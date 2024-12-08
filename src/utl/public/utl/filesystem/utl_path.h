// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_allocator_type.h"
#include "utl/filesystem/utl_platform.h"
#include "utl/memory/utl_allocator.h"
#include "utl/memory/utl_allocator_traits.h"
#include "utl/numeric/utl_add_sat.h"
#include "utl/string/utl_basic_string_view.h"
#include "utl/string/utl_libc.h"

__UFS_NAMESPACE_BEGIN

using path_view = __UTL basic_string_view<path_char>;

struct __UTL_ABI_PUBLIC absolute_t {
    __UTL_HIDE_FROM_ABI explicit inline constexpr absolute_t() noexcept = default;
};

struct __UTL_ABI_PUBLIC relative_t {
    __UTL_HIDE_FROM_ABI explicit inline constexpr relative_t() noexcept = default;
};

UTL_INLINE_CXX17 constexpr absolute_t absolute{};
UTL_INLINE_CXX17 constexpr relative_t relative{};

namespace path {

using size_type = unsigned short;
namespace details {

template <typename Char>
struct constants {
    using view_type = basic_string_view<Char>;
    using pointer = Char*;
    static constexpr Char slash = (Char)'/';
    static constexpr view_type dot = literal_sequence<Char, (Char)'.'>::value;
    static constexpr view_type back = literal_sequence<Char, (Char)'.', (Char)'.'>::value;
#if UTL_TARGET_MICROSOFT
    static constexpr Char back_slash = (Char)'\\';
    static constexpr Char colon = (Char)':';
    static constexpr view_type drive_delimiter_forward =
        literal_sequence<Char, (Char)':', (Char)'/'>::value;
    static constexpr view_type drive_delimiter_back =
        literal_sequence<Char, (Char)':', (Char)'\\'>::value;
    static constexpr view_type delimiters = literal_sequence<(Char)'\\', (Char)'/'>::value;
#endif
};

template <typename Char>
struct utils : constants<Char> {
    using typename constants<Char>::view_type;
    using typename constants<Char>::pointer;
    using utils<Char>::slash;
    using utils<Char>::back;
    using utils<Char>::dot;
#if UTL_TARGET_MICROSOFT
    using utils<Char>::drive_delimiter_forward;
    using utils<Char>::drive_delimiter_back;
    using utils<Char>::back_slash;
    using utils<Char>::delimiters;
    using utils<Char>::colon;
#endif

    struct split_components {
        view_type head;
        view_type tail;
    };

    __UTL_HIDE_FROM_ABI static inline split_components split_on_first_delimiter(
        view_type str) noexcept {
#if UTL_TARGET_MICROSOFT
        auto const idx = str.find_first_of(delimiters);
        return {str.substr(0, idx), remove_prefix(str, add_sat(idx, delimiters.size()))};
#else
        auto const idx = str.find(slash);
        return {str.substr(0, idx), remove_prefix(str, add_sat(idx, 1))};
#endif
    }

    class counter_base {
    public:
        __UTL_HIDE_FROM_ABI size_type inline value() const noexcept {
            return size.root + size.stem;
        }

    protected:
        __UTL_HIDE_FROM_ABI inline explicit counter(split_components c) noexcept
            : size{c.head.size(), 0} {}

        __UTL_HIDE_FROM_ABI inline bool perform_count(
            view_type component, view_type& src_tail) noexcept {
            //  if not end, include trailing slash
            size.stem += component.size() + 1;
        count_start:
            if (src_tail.empty()) {
                return true;
            }

            auto const next = [&]() {
                auto result = split_on_first_delimiter(src_tail);
                src_tail = result.tail;
                return result.head;
            }();
            if (next == dot) {
                goto count_start;
            }

            if (next == back) {
                size.stem = sub_sat(size.stem, component.size() + 1);
                return false;
            }

            if (!perform_count(next, src_tail)) {
                goto count_start;
            }

            return true;
        }

        __UTL_HIDE_FROM_ABI inline void end_count() noexcept {
            size.stem -= 1; // remove trailing slash
        }

        __UTL_HIDE_FROM_ABI inline void add_backtrack() noexcept { size.stem += back.size() + 1; }

    private:
        struct {
            size_type root;
            size_type stem;
        } size;
    };

    class writer_base {
    public:
        __UTL_HIDE_FROM_ABI inline size_type length() const noexcept { return dst - begin.root; }
        __UTL_HIDE_FROM_ABI inline pointer data() const noexcept { return begin.root; }

    protected:
        __UTL_HIDE_FROM_ABI writer_base(split_components c, Char* begin, Char* end) noexcept
            : begin{begin, begin + c.head.size()}
            , end{end}
            , dst{begin}
            , written{0} {
            if (dst + c.head.size() < end) {
                written += c.head.size();
                __UTL libc::memcpy(dst, c.head.data(), c.head.size() * sizeof(Char));
            }

            dst += c.head.size();
        }

        __UTL_HIDE_FROM_ABI ~writer_base() = default;

        __UTL_HIDE_FROM_ABI inline bool perform_copy(
            view_type component, view_type& src_tail) noexcept {
            //  if not end, include trailing slash, if end include sentinel
            write_component(component);
        copy_start:
            if (src_tail.empty()) {
                return true;
            }
            auto const next = [&]() {
                auto result = split_on_first_delimiter(src_tail);
                src_tail = result.tail;
                return result.head;
            }();
            if (next == dot) {
                goto copy_start;
            }

            if (next == back) {
                undo_component(component.size());
                return false;
            }

            if (!perform_copy(next, src_tail)) {
                goto copy_start;
            }

            return true;
        }

        __UTL_HIDE_FROM_ABI void write_backtack() noexcept { write_component(back); }

    private:
        __UTL_HIDE_FROM_ABI void write_component(view_type component) noexcept {
            if (component.empty()) {
                return;
            }

            auto const total_size = component.size() + 1; // add slash
            if (dst + total_size < end) {
                written += total_size;
                __UTL libc::memcpy(dst, component.data(), component.size() * sizeof(Char));
                *(dst + component.size()) = slash;
            }

            dst += total_size;
        }

        __UTL_HIDE_FROM_ABI void undo_component(size_t component_size) noexcept {
            auto const total_size = component_size + 1; // add slash
            written -= (length() == written) * total_size;
            dst -= total_size;
            if (dst < begin.stem) {
                dst = begin.stem;
            }
        }

        __UTL_HIDE_FROM_ABI void end_write() noexcept {
            written -= (length() == written);
            dst -= 1;
            if (dst < begin.stem) {
                dst = begin.stem;
            }

            if (written > 0) {
                begin.root[written] = 0;
            }
        }

        struct {
            pointer const root;
            pointer const stem;
        } begin;
        pointer const end;
        pointer dst;
        size_type written;
    };
};

template <typename, typename Char = path_char>
class collapse_t;

/**
 * Demo: https://godbolt.org/z/4E39193oK
 */
template <typename Char>
class collapse_t<absolute_t, Char> : utils<Char> {
    using typename utils<Char>::view_type;
    using typename utils<Char>::pointer;
    using typename utils<Char>::split_components;
    using typename utils<Char>::writer_base;
    using typename utils<Char>::counter_base;
    using utils<Char>::slash;
    using utils<Char>::back;
    using utils<Char>::dot;
    using utils<Char>::split_on_first_delimiter;
#if UTL_TARGET_MICROSOFT
    using utils<Char>::drive_delimiter_forward;
    using utils<Char>::drive_delimiter_back;
    using utils<Char>::back_slash;
    using utils<Char>::colon;
#endif

public:
    __UTL_HIDE_FROM_ABI explicit inline collapse_t(view_type full) UTL_THROWS : full(full) {}

    __UTL_HIDE_FROM_ABI inline size_t length() const noexcept {
        return counter(split_root(full)).value(); // size of path excluding sentinel
    }

    __UTL_HIDE_FROM_ABI inline size_t copy(pointer dst, size_type buffer_size) const noexcept {
        auto end = dst + buffer_size;
        return copier(split_root(full), dst, end).length();
    }

private:
    __UTL_HIDE_FROM_ABI static inline split_components split_root(view_type abs_path) UTL_THROWS {
#if UTL_TARGET_MICROSOFT
        if (abs_path.front() == slash || abs_path.front() == back_slash) {
            return {abs_path.substr(0, 1), remove_prefix(abs_path, 1)};
        }

        auto drive = abs_path.substr(0, 3);
        if (drive.ends_with(drive_delimiter_back)) {
            return {abs_path.substr(0, 3), remove_prefix(abs_path, 3)};
        }

        if (drive.ends_with(drive_delimiter_forward)) {
            return {abs_path.substr(0, 3), remove_prefix(abs_path, 3)};
        }
#else
        if (abs_path.front() == slash) UTL_LIKELY {
            return {abs_path.substr(0, 1), remove_prefix(abs_path, 1)};
        }
#endif
        UTL_THROW(utl::program_exception("Path collapse failed, Reason=[Unexpected prefix]"));
    }

    __UTL_HIDE_FROM_ABI static inline split_components collapse_head(view_type rel_path) noexcept {
        // collapses any "." or ".." at the beginning of a relative path
        while (!rel_path.empty()) {
            auto const result = split_on_first_delimiter(rel_path);
            if (result.head != dot && result.head != back) {
                return result;
            }
            rel_path = result.tail;
        }

        return {rel_path, rel_path};
    }

    class counter : private counter_base {
    public:
        __UTL_HIDE_FROM_ABI inline explicit counter(split_components c) noexcept : counter_base(c) {
            count(collapse_head(c.tail));
        }

        using counter_base::value;

    private:
        __UTL_HIDE_FROM_ABI inline void count(split_components c) noexcept {
            while (!c.head.empty() && !counter_base::perform_count(c.head, c.tail)) {
                // we've come back to the root, so we need to sanitize again
                c = collapse_head(c.tail);
            }
        }
    };

    struct copier : private writer_base {
    public:
        __UTL_HIDE_FROM_ABI inline explicit copier(
            split_components c, Char* begin, Char* end) noexcept
            : writer_base(c, begin, end) {
            copy(collapse_head(c.tail));
            writer_base::end_write();
        }

        using writer_base::data;
        using writer_base::length;

    private:
        __UTL_HIDE_FROM_ABI inline void copy(split_components c) noexcept {
            while (!c.head.empty() && !writer_base::perform_copy(c.head, c.tail)) {
                // we've come back to the root, so we need to sanitize again
                c = collapse_head(c.tail);
            }
        }
    };

    view_type full;
};

template <typename Char>
class collapse_t<relative_t, Char> : utils<Char> {
    using typename utils<Char>::view_type;
    using typename utils<Char>::pointer;
    using typename utils<Char>::split_components;
    using utils<Char>::slash;
    using utils<Char>::back;
    using utils<Char>::dot;
    using utils<Char>::split_on_first_delimiter;
#if UTL_TARGET_MICROSOFT
    using utils<Char>::drive_delimiter_forward;
    using utils<Char>::drive_delimiter_back;
    using utils<Char>::back_slash;
    using utils<Char>::delimiters;
    using utils<Char>::colon;
#endif
    static constexpr view_type backwards =
        literal_sequence<Char, (Char)'.', (Char)'.', (Char)'/'>::value;

public:
    __UTL_HIDE_FROM_ABI explicit inline collapse_t(view_type full) UTL_THROWS : full(full) {}

    __UTL_HIDE_FROM_ABI inline size_t length() const noexcept {
        return counter(split_root(full)).value(); // size of path excluding sentinel
    }

    __UTL_HIDE_FROM_ABI inline size_t copy(pointer dst, size_type buffer_size) const noexcept {
        auto end = dst + buffer_size;
        return copier(split_root(full), dst, end).length();
    }

private:
    /**
     * Split up the root component from the path
     *
     * @note On DOS-like filesystems, the result head will be "<drive>:", on Unix-like systems, the
     * result head will be empty
     */
    __UTL_HIDE_FROM_ABI static inline split_components split_root(view_type rel_path) UTL_THROWS {
        split_components output;
#if UTL_TARGET_MICROSOFT
        static constexpr view_type dot_slash = literal_sequence<Char, (Char)'.', slash>::value;
        static constexpr view_type dot_back_slash =
            literal_sequence<Char, (Char)'.', back_slash>::value;

        auto root = first(rel_path, 2);
        if (root.ends_with(colon)) {
            output.head = root;
            output.tail = remove_prefix(rel_path, 2);
        } else {
            output.head = view_type{rel_path.data(), 0};
            output.tail = rel_path;
        }

        if (!output.tail.starts_with(slash) && !output.tail.starts_with(back_slash)) UTL_LIKELY {
            return output;
        }
#else
        if (!rel_path.starts_with(slash)) {
            return {
                view_type{rel_path.data(), 0},
                rel_path
            };
        }
#endif
        UTL_THROW(utl::program_exception("Path collapse failed, Reason=[Unexpected prefix]"));
    }

    template <typename OnBack>
    static inline split_components collapse_head(view_type rel_path, OnBack&& on_back) noexcept {
        while (!rel_path.empty()) {
            auto const result = split_on_first_delimiter(rel_path);
            auto const is_dot = result.head == dot;
            auto const is_back = result.head == back;
            if (!is_dot && !is_back) {
                return result;
            }

            if (is_back) {
                on_back();
            }
            rel_path = result.tail;
        }

        return {rel_path, rel_path};
    }

    class counter : private counter_base {
    public:
        __UTL_HIDE_FROM_ABI inline explicit counter(split_components c) noexcept : counter_base(c) {
            count(collapse_head(c.tail, [this]() { counter_base::add_backtrack(); }));
        }

        using counter_base::value;

    private:
        __UTL_HIDE_FROM_ABI inline void count(split_components c) noexcept {
            while (!c.head.empty() && !counter_base::perform_count(c.head, c.tail)) {
                // we've come back to the root, so we need to sanitize again
                c = collapse_head(c.tail, [this]() { counter_base::add_backtrack(); });
            }
        }
    };

    class copier : private writer_base {
    public:
        __UTL_HIDE_FROM_ABI inline explicit copier(
            split_components c, Char* begin, Char* end) noexcept
            : writer_base(c, begin, end) {
            copy(collapse_head(c.tail, [this]() { writer_base::write_backtack(); }));
            // replace_trailing slash with sentinel
            writer_base::end_write();
        }

        using writer_base::data;
        using writer_base::length;

    private:
        __UTL_HIDE_FROM_ABI inline void copy(split_components c) noexcept {
            while (!c.head.empty() && !writer_base::perform_copy(c.head, c.tail)) {
                // we've come back to the root, so we need to sanitize again
                c = collapse_head(c.tail, [this]() { writer_base::write_backtack(); });
            }
        }
    };

    view_type full;
};
} // namespace details

template <typename Char = path_char>
__UTL_HIDE_FROM_ABI inline constexpr bool is_absolute(__UTL basic_string_view<Char> view) noexcept {
    using traits_type = details::constants<Char>;
#if UTL_TARGET_MICROSOFT
    if (view.empty()) {
        return false;
    }

    // Microsoft only supports single character drives
    auto const prefix = view.substr(0, 3);
    return prefix.front() == traits_type::slash || prefix.front() == traits_type::back_slash ||
        prefix.ends_with(traits_type::drive_delimiter_back) ||
        prefix.ends_with(traits_type::drive_delimiter_forward);
#else
    return !view.empty() && view.front() == traits_type::slash;
#endif
}

template <typename Char = path_char>
__UTL_HIDE_FROM_ABI inline constexpr bool is_relative(__UTL basic_string_view<Char> view) noexcept {
    return !is_absolute(view);
}

template <typename Char = path_char>
__UTL_HIDE_FROM_ABI inline size_type collapse(
    __UTL basic_string_view<Char> view, __UTL span<Char> dst) noexcept {
    if (is_absolute(view)) {
        return details::collapse_t<absolute_t>(view).copy(dst, dst.size());
    }

    return details::collapse_t<relative_t>(view).copy(dst, dst.size());
}

template <typename Char = path_char>
__UTL_HIDE_FROM_ABI inline size_type collapse(
    absolute_t, __UTL basic_string_view<Char> view, __UTL span<Char> dst) {
    return details::collapse_t<absolute_t>(view).copy(dst.data(), dst.size());
}

template <typename Char = path_char>
__UTL_HIDE_FROM_ABI inline size_type collapse(
    relative_t, __UTL basic_string_view<Char> view, __UTL span<Char> dst) {
    return details::collapse_t<relative_t>(view).copy(dst.data(), dst.size());
}

template <typename Char = path_char>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, PURE) inline size_type effective_length(
    __UTL basic_string_view<Char> view) noexcept {
    if (is_absolute(view)) {
        return details::collapse_t<absolute_t>(view).length();
    }

    return details::collapse_t<relative_t>(view).length();
}

template <typename Char = path_char>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, PURE) inline size_type effective_length(
    absolute_t, __UTL basic_string_view<Char> view) noexcept {
    return details::collapse_t<absolute_t>(view).length();
}

template <typename Char = path_char>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, PURE) inline size_type effective_length(
    relative_t, __UTL basic_string_view<Char> view) noexcept {
    return details::collapse_t<relative_t>(view).length();
}

template <typename Char = path_char>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, PURE) inline UTL_CONSTEXPR_CXX14 __UTL basic_string_view<Char> basename(
    __UTL basic_string_view<Char> view) const noexcept {
    using traits_type = details::constants<Char>;
#if UTL_TARGET_MICROSOFT
    view = remove_suffix(view, !view.empty() && traits_type::delimiters.contains(view.back()));
    // if no delimiter, argument will wrap around to zero, so 'view' will be returned
    return view.substr(view.find_last_of(traits_type::delimiters) + 1);
#else
    view = remove_suffix(view, view.ends_with(traits_type::slash));
    return view.substr(view.find_last_of(traits_type::slash) + 1);
#endif
}

template <typename Char = path_char>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, PURE) inline UTL_CONSTEXPR_CXX14 __UTL basic_string_view<Char> basename(
    __UTL basic_string_view<Char> path, __UTL basic_string_view<Char> suffix) const noexcept {
    using traits_type = details::constants<Char>;
    auto const name = basename(path);
    if (suffix.size() == name.size()) {
        // Quirk of the `basename` operation
        // if suffix is equal to the basename, nothing is removed
        return name;
    }

    return remove_suffix(name, name.ends_with(suffix) * suffix.size());
}

template <typename Char = path_char>
__UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 __UTL basic_string_view<Char> extension(
    __UTL basic_string_view<Char> path) const noexcept {
    auto const name = basename(path);
    return remove_prefix(name, add_sat(name.find_last_of((Char)'.'), 1));
}

template <typename Char = path_char>
__UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 __UTL basic_string_view<Char> dirname(
    __UTL basic_string_view<Char> path) const noexcept {
    using view_type = __UTL basic_string_view<Char>;
    using traits_type = details::constants<Char>;
    // if relative and result is empty return dot
    // if absolute and result is empty return root
    auto const abs = is_absolute(path);
    if (!abs && path.empty()) {
        return traits_type::dot;
    }

#if UTL_TARGET_MICROSOFT
    // absolute path cannot be empty
    auto trimmed = remove_suffix(path, traits_type::delimiters.contains(path.back()));
    if (trimmed.empty() || trimmed.ends_with(traits_type::colon)) {
        return abs ? path : traits_type::dot;
    }

    return trimmed.substr(0, trimmed.find_last_of(traits_type::delimiters));
#else
    auto trimmed = remove_suffix(path, path.ends_with(traits_type::slash));
    if (trimmed.empty()) {
        return abs ? path : traits_type::dot;
    }

    return trimmed.substr(0, trimmed.find_last_of(traits_type::delimiters));
#endif
}

// TODO
template <typename Char = path_char, UTL_CONCEPT_CXX20(convertible_to<basic_string_view<Char>>) V,
    UTL_CONCEPT_CXX20(convertible_to<basic_string_view<Char>>)... Vs UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_conjunction(
        bool_constant<(sizeof...(Vs) > 0)>, is_convertible<V, basic_string_view<Char>>,
        is_convertible<Vs, basic_string_view<Char>>...))>
UTL_CONSTRAINT_CXX20(sizeof...(Vs) > 0)
__UTL_HIDE_FROM_ABI inline size_t join(__UTL span<Char> dst, V&& root, Vs&&... args) noexcept;

template <typename Char = path_char, UTL_CONCEPT_CXX20(allocator_type) Alloc,
    UTL_CONCEPT_CXX20(convertible_to<basic_string_view<Char>>) V,
    UTL_CONCEPT_CXX20(convertible_to<basic_string_view<Char>>)... Vs UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_conjunction(
            bool_constant<(sizeof...(Vs) > 0) && sizeof(allocator_traits<Alloc>) != 0>,
            is_convertible<V, basic_string_view<Char>>,
            is_convertible<Vs, basic_string_view<Char>>...))>
UTL_CONSTRAINT_CXX20(sizeof...(Vs) > 0)
__UTL_HIDE_FROM_ABI inline basic_string<Char, Alloc> join(
    basic_string<Char, Alloc>&& root, Vs&&... args) noexcept;

template <typename Char = path_char,
    UTL_CONCEPT_CXX20(allocator_type) Alloc = __UTL allocator<Char>,
    UTL_CONCEPT_CXX20(convertible_to<basic_string_view<Char>>) V,
    UTL_CONCEPT_CXX20(convertible_to<basic_string_view<Char>>)... Vs UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_conjunction(
            bool_constant<(sizeof...(Vs) > 0)>, is_convertible<V, basic_string_view<Char>>,
            is_convertible<Vs, basic_string_view<Char>>...))>
UTL_CONSTRAINT_CXX20(sizeof...(Vs) > 0)
__UTL_HIDE_FROM_ABI inline basic_string<Char, Alloc> join(V&& root, Vs&&... args) noexcept;

} // namespace path

__UFS_NAMESPACE_END
