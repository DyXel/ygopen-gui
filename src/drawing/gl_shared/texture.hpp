#ifndef DRAWING_GL_SHARED_TEXTURE_HPP
#define DRAWING_GL_SHARED_TEXTURE_HPP
#include "../gl_include.hpp"
#include "../texture.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLShared
{

class Texture : public ITexture
{
public:
	Texture();
	virtual ~Texture();
	
	void SetImage(int width, int height, void* data) override;
	
	void Bind();
private:
	GLuint ref{};
};

} // GLShared

} // Detail

} // Drawing

#endif // DRAWING_GL_SHARED_TEXTURE_HPP
