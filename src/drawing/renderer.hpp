#ifndef DRAWING_RENDERER_HPP
#define DRAWING_RENDERER_HPP
#include "types.hpp"

namespace Drawing
{

enum VSyncState
{
	VSYNC_STATE_DISABLED,
	VSYNC_STATE_ENABLED,
	VSYNC_STATE_ADAPTIVE,
};

namespace Detail
{

class IRenderer
{
public:
	virtual void Clear() = 0;
	virtual void Present() = 0;
	virtual void SetViewport(int x, int y, int width, int height) = 0;
	
	virtual Drawing::Primitive NewPrimitive() = 0;
	virtual Drawing::Texture NewTexture() = 0;
	
	virtual bool SetVSync([[maybe_unused]] VSyncState state) {return false;}
};

} // Detail

} // Drawing

#endif // DRAWING_RENDERER_HPP
