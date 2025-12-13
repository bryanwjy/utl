// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

#include "utl/type_traits/std/has_unique_object_representations.h"
#include "utl/type_traits/std/is_abstract.h"
#include "utl/type_traits/std/is_aggregate.h"
#include "utl/type_traits/std/is_bounded_array.h"
#include "utl/type_traits/std/is_const.h"
#include "utl/type_traits/std/is_empty.h"
#include "utl/type_traits/std/is_final.h"
#include "utl/type_traits/std/is_implicit_lifetime.h"
#include "utl/type_traits/std/is_polymorphic.h"
#include "utl/type_traits/std/is_scoped_enum.h"
#include "utl/type_traits/std/is_signed.h"
#include "utl/type_traits/std/is_standard_layout.h"
#include "utl/type_traits/std/is_trivially_copyable.h"
#include "utl/type_traits/std/is_unbounded_array.h"
#include "utl/type_traits/std/is_unsigned.h"
#include "utl/type_traits/std/is_volatile.h"
