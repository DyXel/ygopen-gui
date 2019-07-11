#ifndef GL_CONTEXT_HPP
#define GL_CONTEXT_HPP
#include "gl_common_include.h"

// Code taken and modified from testgles2.c in SDL2 source code.
// TODO: proper credit
struct GLES2_Context
{
#define SDL_PROC(ret,func,params) ret (APIENTRY *func) params;
#include "gl_es2_funcs.h"
#undef SDL_PROC
};

extern GLES2_Context ctx;

// NOTE: Call after SDL_GL_CreateContext
int GLLoadContext();

#endif // GL_CONTEXT_HPP
