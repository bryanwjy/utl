#pragma once

#include "utl/utl_config.h"

#if !UTL_TARGET_LINUX && !UTL_TARGET_BSD && !UTL_TARGET_APPLE
#  error "Invalid header access"
#endif

#if !defined(UTL_FILESYSTEM_WALK_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/atomic/utl_atomic.h"
#include "utl/scope/utl_scope_exit.h"
#include "utl/span/utl_span.h"

__UFS_NAMESPACE_BEGIN

namespace details {
namespace walk {

class dir_entry {
public:
    __UTL_HIDE_FROM_ABI inline dir_entry(zpath_view entry, file_type type) noexcept
        : entry_{view}
        , type_{type} {}
    __UTL_HIDE_FROM_ABI inline constexpr dir_entry(dir_entry const&) noexcept = default;
    __UTL_HIDE_FROM_ABI inline constexpr dir_entry& operator=(dir_entry const&) noexcept = default;

    zpath_view file_name() const noexcept { return file_name_; }
    file_type file_type() const noexcept { return type_; }

private:
    zpath_view file_name_;
    file_type type_;
};

template <typename>
class dir_reader;

template <>
class dir_reader<walk_attributes::follow_symlinks_t> {

    __UTL_ABI_PUBLIC dir_reader(zpath_view path) noexcept;
    __UTL_HIDE_FROM_ABI dir_reader(read_file_handle&& path) noexcept : fd(path) {}
    dir_reader(dir_reader const&) = delete;
    dir_reader& operator=(dir_reader const&) = delete;
    dir_reader(dir_reader&&) noexcept = delete;
    dir_reader& operator=(dir_reader&&) noexcept = delete;

    __UTL_HIDE_FROM_ABI ~dir_context() = default;

    UTL_ATTRIBUTE(_HIDE_FROM_ABI, NODISCARD) bool valid() const noexcept { return fd.is_open(); }

    UTL_ATTRIBUTE(_HIDE_FROM_ABI, NODISCARD) read_file_handle const& handle() const noexcept {
        return fd;
    }

    UTL_ATTRIBUTE(_ABI_PUBLIC, NODISCARD) dir_entry read() noexcept;

    UTL_ATTRIBUTE(_ABI_PUBLIC, NODISCARD) bool empty() noexcept;

    __UTL_ABI_PUBLIC void seek_to_beginning() noexcept;

    UTL_ATTRIBUTE(_ABI_PUBLIC, NODISCARD) walk_result reset(zpath_view new_path) noexcept;

private:
    __UTL span<unsigned char> buffer;
    unsigned char const* next;
    read_file_handle fd;
};

template <>
class dir_reader<walk_attributes::details::default_tag> {
    __UTL_ABI_PUBLIC dir_reader(zpath_view path) noexcept;
    dir_reader(dir_reader const&) = delete;
    dir_reader& operator=(dir_reader const&) = delete;
    dir_reader(dir_reader&&) noexcept = delete;
    dir_reader& operator=(dir_reader&&) noexcept = delete;

    __UTL_HIDE_FROM_ABI ~dir_context() = default;

    UTL_ATTRIBUTE(_HIDE_FROM_ABI, NODISCARD) bool valid() const noexcept { return fd.is_open(); }

    UTL_ATTRIBUTE(_HIDE_FROM_ABI, NODISCARD) read_file_handle const& handle() const noexcept {
        return fd;
    }

    UTL_ATTRIBUTE(_ABI_PUBLIC, NODISCARD) dir_entry read() noexcept;

    UTL_ATTRIBUTE(_ABI_PUBLIC, NODISCARD) bool empty() noexcept;

    __UTL_ABI_PUBLIC void seek_to_beginning() noexcept;

    __UTL_ABI_PUBLIC walk_result reset(zpath_view new_path) noexcept;

private:
    __UTL span<unsigned char> buffer;
    unsigned char const* next;
    read_file_handle fd;
};

extern template dir_reader<walk_attributes::follow_symlinks_t>::dir_reader(zpath_view) noexcept;
extern template void dir_reader<walk_attributes::follow_symlinks_t>::reset(zpath_view) noexcept;
extern template dir_entry dir_reader<walk_attributes::follow_symlinks_t>::read() noexcept;

extern template dir_reader<walk_attributes::ignore_symlinks_t>::dir_reader(zpath_view) noexcept;
extern template void dir_reader<walk_attributes::ignore_symlinks_t>::reset(zpath_view) noexcept;
extern template dir_entry dir_reader<walk_attributes::ignore_symlinks_t>::read() noexcept;

template <typename>
class func_t;

#ifndef UTL_FILESYSTEM_MAX_WALK_DEPTH
#  define UTL_FILESYSTEM_MAX_WALK_DEPTH 128
#endif

template <>
class func_t<walk_attributes::top_down_t> {
    using directory_stack = basic_string<path_char>;
    using delimiter_type = literal_sequence<path_char, (path_char)'/', (path_char)'/'>;
    using temp_path_type = basic_short_string<path_char, 512>;
    static constexpr unsigned int maximum_depth = UTL_FILESYSTEM_MAX_WALK_BOTTOM_UP_DEPTH;
    UTL_ATTRIBUTE(_HIDE_FROM_ABI, NODISCARD, PURE) static directory_stack& stack() noexcept {
        thread_local directory_stack instance = []() {
            // The syscalls to access the directory will likely be a lot slower than just allocating
            // this
            directory_stack str;
            try {
                str.reserve(8192);
            } catch (...) {}

            return str;
        }();
        return instance;
    }

    static void begin_directory() { stack() += delimiter_type::value; }

    static void push_stack(path_view filename) { path::append(stack(), filename); }

    static bool pop_stack(temp_path_type& current_dir) noexcept {
        while (stack().ends_with(delimiter_type::value)) {
            // end of dir
            stack().resize(stack.size() - 2);
            if (stack().empty()) {
                return false;
            }

            auto const current_file = path::basename(current_dir);
            auto const prefix_size = current_file.size() + 1; // + 1 for delimiter
            current_dir.resize(current_dir.size() - prefix_size);
        }

        auto const next_file = path::basename(stack);
        path::append(current_dir, next_file);
        auto const prefix_size = next_file.size() + 1; // + 1 for delimiter
        stack().resize(stack().size() - prefix_size);
        return true;
    }

public:
    constexpr explicit func_t() noexcept = default;
    func_t(func_t const&) = delete;
    func_t& operator=(func_t const&) = delete;
    func_t(func_t&&) noexcept = delete;
    func_t& operator=(func_t&&) noexcept = delete;

    template <typename Opt, typename F, typename E>
    walk_result operator()(views::directory dir, F&& func, E&& on_error) const {
        UTL_ON_SCOPE_EXIT {
            stack().clear();
        };
        using reader_type = dir_reader<Opt>;
        temp_path_type current_dir{dir.path()};
        reader_type reader{current_dir};
        if (!reader.valid()) {
            return on_error(walk_error{
                current_dir, error_code{errno, __UTL system_category()}
            });
        }

        unsigned int depth = 0u;
    context_loop:
        begin_directory();
        if (++depth >= maximum_depth) {
            return walk_result{__UTL unexpect, error_value::walk_limit_exceeded};
        }

        while (!reader.empty()) {
            auto const entry = reader.read();
            if (!walk_invoke(__UTL forward<F>(func), current_dir, entry)) {
                // manual stop
                return walk_result{};
            }

            if (reader_type::readable(entry)) {
                push_stack(entry.file_name());
            }
        }

        if (pop_stack(current_dir)) {
            auto status = reader.reset(current_dir);
            if (!status) {
                status = on_error(walk_error{current_dir, status.error()});
            }
            if (!status) {
                return status;
            }

            goto context_loop;
        }

        return walk_result{};
    }
};

template <>
class func_t<walk_attributes::bottom_up_t> {
    using directory_stack = basic_string<path_char>;
    using delimiter_type = literal_sequence<path_char, (path_char)'/', (path_char)'/'>;
    using temp_path_type = basic_short_string<path_char, 512>;
    static constexpr unsigned int maximum_depth = UTL_FILESYSTEM_MAX_WALK_BOTTOM_UP_DEPTH;

    template <typename F, typename E>
    static walk_result impl(
        temp_path_type& current_dir, F const& func, E const& on_error, unsigned int depth = 0) {
        if (depth >= maximum_depth) {
            return walk_result{__UTL unexpect, error_value::walk_limit_exceeded};
        }

        reader_type reader{current_dir};
        if (!reader.valid()) {
            auto result = on_error(walk_error{
                current_dir, error_code{errno, __UTL system_category()}
            });

            if (depth == 0 || !result) {
                return result;
            }
        }

        while (!reader.empty()) {
            auto const entry = reader.read();
            if (!reader_type::readable(entry)) {
                continue;
            }

            auto result =
                impl(path::append(current_dir, entry.file_name()), func, on_error, ++depth);
            if (!result) {
                return result;
            }

            auto const current_file = path::basename(current_dir);
            auto const prefix_size = current_file.size() + 1; // + 1 for delimiter
            current_dir.resize(current_dir.size() - prefix_size);
        }

        reader.seek_to_beginning();
        while (!reader.empty()) {
            auto const entry = reader.read();
            if (!walk_invoke(func, current_dir, entry)) {
                break;
            }
        }

        return walk_result{};
    }

public:
    constexpr explicit func_t() noexcept = default;
    func_t(func_t const&) = delete;
    func_t& operator=(func_t const&) = delete;
    func_t(func_t&&) noexcept = delete;
    func_t& operator=(func_t&&) noexcept = delete;

    template <typename Opt, typename F, typename E>
    walk_result operator()(views::directory dir, F&& func, E&& on_error) const {
        using reader_type = dir_reader<Opt>;
        temp_path_type current_dir{dir.path()};
        impl(current_dir, func, on_error);
    }
};

} // namespace walk
} // namespace details

template <UTL_CONCEPT_CXX20(walk_invocable) F, typename Opt UTL_CONSTRAINT_CXX11(
    details::is_walk_attributes<remove_cvref_t<Opt>>::value)>
__UTL_HIDE_FROM_ABI walk_result walk(views::directory dir, F&& func, Opt&& options) {
#if UTL_ASSERTIONS_ENABLED
    thread_local bool enter_flag = false;
    [[maybe_unused]] auto const already_entered = __UTL atomic_release::exchange(&enter_flag, true);
    UTL_ASSERT(!already_entered);
    UTL_ON_SCOPE_EXIT {
        __UTL atomic_release::signal_fence();
        enter_flag = false;
    };
#endif
    using attributes_type = remove_cvref_t<Opt>;
    using direction_type = details::walk::direction<attributes_type>;
    using strategy_type = details::walk::symlink_strategy<attributes_type>;
    static constexpr details::walk::func_t<direction_type> impl{};
    impl<strategy_type>(
        dir, __UTL forward<F>(func), get_error_handler(__UTL forward<Opt>(options)));
}

__UFS_NAMESPACE_END
