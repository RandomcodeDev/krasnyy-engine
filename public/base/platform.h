// Platform API

#pragma once

#ifdef CH_WIN32
#include "phnt_wrapper.h"
#endif

#include "base.h"
#include "compiler.h"
#include "dll.h"
#include "types.h"

// Initialize anything necessary to use platform functions. Terminates the process on failure.
extern BASEAPI void Plat_Init();

// Shut down platform
extern BASEAPI void Plat_Shutdown();
