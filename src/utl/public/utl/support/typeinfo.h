/* Copyright 2023-2025 Bryan Wong */
#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  if UTL_ABI_MICROSOFT
#    include <vcruntime_typeinfo.h>
#  else
#    include <cxxabi.h>
#  endif // UTL_ABI_MICROSOFT
#endif   // !UTL_MODULES

UTL_DEFAULT_NAMESPACE_BEGIN
#if UTL_LIBUTLCXXRT

UTL_NAMESPACE_BEGIN(std)
class __UTL_ABI_PUBLIC type_info {
public:
    /**
     * Virtual destructor.  This class must have one virtual function to
     * ensure that it has a vtable.
     */
    virtual ~type_info();
    bool operator==(type_info const&) const;
    bool operator!=(type_info const&) const;
    bool before(type_info const&) const;
    char const* name() const;
    type_info();

private:
    type_info(type_info const& rhs);
    type_info& operator=(type_info const& rhs);

    char const* __type_name;
};
UTL_NAMESPACE_END(std)

#elif UTL_ABI_MICROSOFT

UTL_EXPORT using ::type_info;

#  if UTL_WITH_EXCEPTIONS
UTL_EXPORT using ::std::bad_cast;
UTL_EXPORT using ::std::bad_typeid;
#  else  // UTL_WITH_EXCEPTIONS
// TODO define bad_cast
// TODO define bad_typeid
#  endif // UTL_WITH_EXCEPTIONS

#else

UTL_EXPORT using ::std::bad_cast;
UTL_EXPORT using ::std::bad_typeid;
UTL_EXPORT using ::std::type_info;

#endif // !UTL_LIBUTLCXXRT
UTL_DEFAULT_NAMESPACE_END
