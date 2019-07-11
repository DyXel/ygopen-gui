#include "gl_context.hpp"
#include <SDL.h>

GLES2_Context ctx;

int GLLoadContext()
{
#if SDL_VIDEO_DRIVER_UIKIT
#define __SDL_NOGETPROCADDR__
#elif SDL_VIDEO_DRIVER_ANDROID
#define __SDL_NOGETPROCADDR__
#elif SDL_VIDEO_DRIVER_PANDORA
#define __SDL_NOGETPROCADDR__
#endif

#if defined __SDL_NOGETPROCADDR__
#define SDL_PROC(ret,func,params) ctx.func=func;
#else
#define SDL_PROC(ret,func,params) \
	do \
	{ \
		ctx.func = reinterpret_cast<decltype(ctx.func)>(SDL_GL_GetProcAddress(#func)); \
		if (!ctx.func) \
		{ \
			SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, \
			                "Couldn't load GL function %s: %s", \
			                #func, SDL_GetError()); \
			return -1; \
		} \
	}while(0);
#endif /* __SDL_NOGETPROCADDR__ */

#include "gl_es2_funcs.h"
#undef SDL_PROC
	return 0;
}
