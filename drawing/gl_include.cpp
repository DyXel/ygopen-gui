#define _DONT_DEFINE_POINTERS_GL
#include "gl_include.hpp"

#ifndef USE_PROTOTYPES_GL
#define SDL_PROC(ret,func,params) ret (APIENTRY *func) params;
#include "gl_es2_funcs.h"
#include "gl_core_funcs.h"
#undef SDL_PROC
#endif
