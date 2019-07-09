#ifndef GL_COMMON_INCLUDE_HPP
#define GL_COMMON_INCLUDE_HPP
#include <SDL_opengles2.h>

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
