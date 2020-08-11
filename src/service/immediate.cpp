#include "immediate.hpp"

#include <SDL.h>

#include <ego/renderer.hpp>

namespace YGOpen
{

inline Ego::SRenderer ConstructEgoRenderer()
{
#ifdef USE_GL_CORE
#define MAKE_RENDERER MakeGLCoreRenderer
#else
#define MAKE_RENDERER MakeGLESRenderer
#endif // USE_GL_CORE
	return Ego::MAKE_RENDERER([](const char* procName)
	{
		return SDL_GL_GetProcAddress(procName);
	});
#undef MAKE_RENDERER
	// TODO: Check if the renderer is valid?
}

Service::Immediate::Immediate() :
	exiting(false),
	elapsed(0.0F),
	width(0),
	height(0),
	renderer(ConstructEgoRenderer())
{}

} // namespace YGOpen
