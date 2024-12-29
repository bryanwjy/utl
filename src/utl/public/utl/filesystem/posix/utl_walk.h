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

extern "C" int close(int);

__UFS_NAMESPACE_BEGIN

namespace details {
class __UTL_ABI_PUBLIC dir_context;

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

class __UTL_ABI_PUBLIC dir_context {
    using directory_stack = basic_string<path_char, bump_allocator<8128>>;

public:
    static directory_stack& stack() noexcept {
        thread_local directory_stack instance;
        return instance;
    }

    dir_context(zpath_view path) noexcept;
    dir_context(dir_context const&) = delete;
    dir_context& operator=(dir_context const&) = delete;
    dir_context(dir_context&&) noexcept = delete;
    dir_context& operator=(dir_context&&) noexcept = delete;
    void reset(directory const& other) noexcept {
        fd = open(other, io::read | io::directory);
        return *this;
    }

    ~dir_context() = default;

    dir_entry read() noexcept;

    dir_entry peek() const noexcept;

    void seek(size_t offset) noexcept;

    __UTL_HIDE_FROM_ABI bool complete() const noexcept { return buffer.empty(); }

    __UTL_HIDE_FROM_ABI bool valid() const noexcept { return fd.is_open(); }

    file_handle const& handle() const { return fd; }

    directory directory() const { return current; }

private:
    __UTL span<unsigned char> buffer;
    unsigned char const* next;
    file_handle fd;
};

template <typename F, typename E>
__UTL_HIDE_FROM_ABI walk_result walk_impl(zpath_view in_path, F&& func, E&& continue_on_error) {
#if UTL_ASSERTIONS_ENABLED
    thread_local bool enter_flag = true;
    [[maybe_unused]] auto const already_entered = __UTL atomic_release::exchange(&enter_flag, true);
    UTL_ASSERT(!enter_flag);
    UTL_ON_SCOPE_EXIT {
        __UTL atomic_release::signal_fence();
        enter_flag = false;
    };
#endif

    UTL_ON_SCOPE_EXIT {
        stack.clear();
    };

    dir_context context{in_path};
    if (!context.valid()) {
        return walk_result{
            __UTL unexpect, in_path, error_code{errno, __UTL system_category()}
        };
    }

    using path_type = basic_short_string<path_char, 512>;
    path_type current_dir{in_path};
context_loop:
    stack += "//";
    auto const dirlen = current_dir.size();
    while (!context.complete()) {
        auto const entry = context.read();
        auto const file_name = entry.file_name();
        if (!walk_invoke(func, current_dir, entry)) {
            // manual stop
            return walk_result{};
        }

        if (entry.file_type() == file_type::directory) {
            path::append(stack, file_name);
        }
    }

    while (stack.ends_with("//")) {
        // end of dir
        stack.resize(stack.size() - 2);
        if (stack.empty()) {
            return walk_result{};
        }

        auto const current_file = path::basename(current_dir);
        auto const prefix_size = current_file.size() + 1; // + 1 for delimiter
        current_dir.resize(current_dir.size() - prefix_size);
    }

    auto const next_file = path::basename(stack);
    path::append(current_dir, next_file);
    auto const prefix_size = next_file.size() + 1; // + 1 for delimiter
    stack.resize(stack.size() - prefix_size);
    context.reset(current_dir);
    goto context_loop;
}

} // namespace details

__UFS_NAMESPACE_END
