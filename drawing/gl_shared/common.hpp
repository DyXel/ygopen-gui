#ifndef DRAWING_GL_SHARED_COMMON_HPP
#define DRAWING_GL_SHARED_COMMON_HPP
#include "../gl_include.hpp"
#include "../types.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLShared
{

enum AttrLocation
{
	ATTR_VERTICES = 0, // Vertices positions
	ATTR_COLORS, // Color array
	ATTR_TEXCOORDS, // Texture coordinates
	ATTR_INDICES, // Indices
	ATTR_COUNT // Total number of attributes
};

constexpr GLenum GLDrawModeFromPDM(const PrimitiveDrawMode& pdm)
{
	if(pdm == PDM_POINTS)
		return GL_POINTS;
	else if(pdm == PDM_LINE_STRIP)
		return GL_LINE_STRIP;
	else if(pdm == PDM_LINE_LOOP)
		return GL_LINE_LOOP;
	else if(pdm == PDM_LINES)
		return GL_LINES;
	else if(pdm == PDM_TRIANGLE_STRIP)
		return GL_TRIANGLE_STRIP;
	else if(pdm == PDM_TRIANGLE_FAN)
		return GL_TRIANGLE_FAN;
	else if(pdm == PDM_TRIANGLES)
		return GL_TRIANGLES;
	else
		return static_cast<GLenum>(0); // Should be unreachable
}

} // GLShared

} // Detail

} // Drawing

#endif // DRAWING_GL_SHARED_COMMON_HPP
