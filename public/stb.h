// To ensure all non-static stb headers work correctly

#pragma once

#include "base/base.h"
#include "base/basicstr.h"

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#define STBDS_NO_SHORT_NAMES
#define STBDS_REALLOC(ctx, ptr, size) Base_Realloc((ptr), (size))
#define STBDS_FREE(ctx, ptr)          Base_Free(ptr)
#define STBDS_PUBLICDEF BASEAPI
#include "stb/stb_ds.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif
