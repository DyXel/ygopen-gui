#include "quad.hpp"

#include <glm/gtc/matrix_transform.hpp> // glm::ortho

namespace Drawing
{

Matrix Get2DProjMatrix(int w, int h)
{
	return glm::ortho<float>(0.0f, w, h, 0.0f);
}

Matrix Trans2D(int x, int y)
{
	return glm::translate<float>(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
}

Vertices GetQuadVertices(OptFloat w, OptFloat h)
{
	static const Vertices DEFAULT_VERTICES =
	{
		{ 0.0f, 0.0f, 0.0f}, // top-left corner
		{ 0.0f, 1.0f, 0.0f}, // bottom-left corner
		{ 1.0f, 0.0f, 0.0f}, // top-right corner
		{ 1.0f, 1.0f, 0.0f}, // bottom-right corner
	};
	Vertices vertices = DEFAULT_VERTICES;
	ResizeQuad(vertices, w, h);
	return vertices;
}

const TexCoords GetQuadTexCoords()
{
	static const TexCoords DEFAULT_TEXCOORDS =
	{
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
	};
	return DEFAULT_TEXCOORDS;
}

PrimitiveDrawMode GetQuadDrawMode()
{
	return Drawing::PDM_TRIANGLE_STRIP;
}

void ResizeQuad(Vertices& vertices, OptFloat w, OptFloat h)
{
	if(w)
		vertices[2].x = vertices[3].x = *w;
	if(h)
		vertices[1].y = vertices[3].y = *h;
}

} // Drawing
