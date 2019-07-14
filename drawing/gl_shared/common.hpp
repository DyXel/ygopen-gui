#ifndef DRAWING_GL_SHARED_COMMON_HPP
#define DRAWING_GL_SHARED_COMMON_HPP

namespace Drawing
{

namespace Detail
{

enum AttrLocation
{
	ATTR_VERTICES = 0, // Vertices positions
	ATTR_COLORS, // Color array
	ATTR_INDICES, // Indices
	ATTR_TEXCOORDS, // Texture coordinates
	ATTR_COUNT // Total number of attributes
};

} // Detail

} // Drawing

#endif // DRAWING_GL_SHARED_COMMON_HPP
