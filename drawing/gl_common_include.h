#ifndef GL_COMMON_INCLUDE_HPP
#define GL_COMMON_INCLUDE_HPP
#include <SDL_platform.h>

#if defined(__ANDROID__) || defined(__IPHONEOS__)
#include <SDL_opengles2.h>
#define GL_USED_MAJOR_VERSION 2
#define GL_USED_MINOR_VERSION 0
#define GL_USED_PROFILE SDL_GL_CONTEXT_PROFILE_ES
#else
#define GL_GLEXT_PROTOTYPES
#include <SDL_opengl.h>
#include <SDL_opengl_glext.h>
#define GL_USED_MAJOR_VERSION 4
#define GL_USED_MINOR_VERSION 1
#define GL_USED_PROFILE SDL_GL_CONTEXT_PROFILE_CORE
#endif

#if defined(DEBUGGING)
#include <SDL_log.h>
#define GL_CHECK(x) \
	do \
	{ \
	x; \
	{ \
		GLenum glError = glGetError(); \
		if(glError != GL_NO_ERROR) \
		{ \
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, \
			             "glGetError() == 0x%.4x at %s:%i\n", \
			             glError, __FILE__, __LINE__); \
		} \
	} \
	}while(0)
#else
#define GL_CHECK(x) x
#endif

#define GLM_FORCE_CXX17
#define GLM_FORCE_INLINE
#define GLM_FORCE_SIZE_T_LENGTH
#define GLM_FORCE_INTRINSICS

namespace OGL
{

enum GLAttrLoc
{
	VERTPOS = 0, // Vertice positions
	COLOR, // Color array
	TEXCOORD, // Texture coordinates
	ATTR_COUNT // Total number of attributes
};

} // OGL

#endif // GL_COMMON_INCLUDE_HPP
