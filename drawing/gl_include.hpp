// This code is a huge compilation of stuff from SDL2 source code
// and my additions to be able to load OpenGL at runtime entirely or
// static link functions directly by defining USE_PROTOTYPES_GL.
// A normal user should be able to replace this header with stuff like
// GLEW and load GL functions on Drawing::API::LoadBackend() at api.cpp.
#ifndef GL_INCLUDE_HPP
#define GL_INCLUDE_HPP
#include <SDL_config.h>

#include "gl_include_defines.h"

// The following system-specific code was taken from SDL2 source code.
// Specifically: include/SDL_opengl.h

/**********************************************************************
 * Begin system-specific stuff.
 */

#if defined(_WIN32) && !defined(__WIN32__) && !defined(__CYGWIN__)
#define __WIN32__
#endif

#if defined(__WIN32__) && !defined(__CYGWIN__)
#  if (defined(_MSC_VER) || defined(__MINGW32__)) && defined(BUILD_GL32) /* tag specify we're building mesa as a DLL */
#    define GLAPI __declspec(dllexport)
#  elif (defined(_MSC_VER) || defined(__MINGW32__)) && defined(_DLL) /* tag specifying we're building for DLL runtime support */
#    define GLAPI __declspec(dllimport)
#  else /* for use with static link lib build of Win32 edition only */
#    define GLAPI extern
#  endif /* _STATIC_MESA support */
#  if defined(__MINGW32__) && defined(GL_NO_STDCALL) || defined(UNDER_CE)  /* The generated DLLs by MingW with STDCALL are not compatible with the ones done by Microsoft's compilers */
#    define GLAPIENTRY 
#  else
#    define GLAPIENTRY __stdcall
#  endif
#elif defined(__CYGWIN__) && defined(USE_OPENGL32) /* use native windows opengl32 */
#  define GLAPI extern
#  define GLAPIENTRY __stdcall
#elif defined(__OS2__) || defined(__EMX__) /* native os/2 opengl */
#  define GLAPI extern
#  define GLAPIENTRY _System
#  define APIENTRY _System
#  if defined(__GNUC__) && !defined(_System)
#    define _System
#  endif
#elif (defined(__GNUC__) && __GNUC__ >= 4) || (defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590))
#  define GLAPI __attribute__((visibility("default")))
#  define GLAPIENTRY
#endif /* WIN32 && !CYGWIN */

/*
 * WINDOWS: Include windows.h here to define APIENTRY.
 * It is also useful when applications include this file by
 * including only glut.h, since glut.h depends on windows.h.
 * Applications needing to include windows.h with parms other
 * than "WIN32_LEAN_AND_MEAN" may include windows.h before
 * glut.h or gl.h.
 */
#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#ifndef NOMINMAX   /* don't define min() and max(). */
#define NOMINMAX
#endif
#include <windows.h>
#endif

#ifndef GLAPI
#define GLAPI extern
#endif

#ifndef GLAPIENTRY
#define GLAPIENTRY
#endif

#ifndef APIENTRY
#define APIENTRY GLAPIENTRY
#endif

/* "P" suffix to be used for a pointer to a function */
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef GLAPIENTRYP
#define GLAPIENTRYP GLAPIENTRY *
#endif

#if defined(PRAGMA_EXPORT_SUPPORTED)
#pragma export on
#endif

/*
 * End system-specific stuff.
 **********************************************************************/

#ifdef USE_PROTOTYPES_GL
#define SDL_PROC(ret, func, params) GLAPI ret APIENTRY func params;
#include "gl_es2_funcs.h"
#include "gl_core_funcs.h"
#undef SDL_PROC
#else
#ifndef _DONT_DEFINE_POINTERS_GL
// NOTE: actual functions located at gl_include.cpp
#define SDL_PROC(ret,func,params) extern ret (APIENTRY *func) params;
#include "gl_es2_funcs.h"
#include "gl_core_funcs.h"
#undef SDL_PROC
#endif // _DONT_DEFINE_POINTERS_GL
#endif // USE_PROTOTYPES_GL

#endif // GL_INCLUDE_HPP
