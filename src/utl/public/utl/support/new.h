/* Copyright 2023-2025 Bryan Wong */
#pragma once

#include "utl/config.h"

#ifdef UTL_MODULES
#  error "Invalid compile command"
#endif
#include "utl/support/new/exceptions.h"
#include "utl/support/new/global_new_delete.h"
#include "utl/support/new/placement_new.h"
#include "utl/support/new/runtime.h"
#include "utl/support/new/types.h"
