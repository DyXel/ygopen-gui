#ifndef DRAWING_API_HPP
#define DRAWING_API_HPP
#include <memory>

struct SDL_Window;

// Primitive: Draw object with no texture but vertices and colors instead
// Texture: Holds a texture. You attach these to Sprites and Models
// Sprite: 2D draw object. Offers easy way to manipulate its coords on screen
// Model: 3D draw object. You probably need to apply transformations to these
#define DRAW_OBJECTS() \
	X(Primitive); \
	X(Texture); \
	X(Sprite); \
	X(Model);

// Its highly unlike we are ever going to use more than 1 context (window)
// and more than 1 backend at the same time, for that reason i decided
// to design the API to be globally available.
namespace Drawing
{

namespace Detail
{
#define X(o) class I##o
DRAW_OBJECTS()
#undef X
} // Detail

#define X(o) using o = std::shared_ptr<Detail::I##o>
DRAW_OBJECTS()
#undef X

// Available Backends
enum Backend
{
	NOT_LOADED = 0,  // Used to check if any backend has been loaded yet
	OPENGL_CORE, // OpenGL 4.1+
	OPENGL_ES    // OpenGL ES 2.0+
};

namespace API
{

bool PreloadBackend(Backend backend);
bool LoadBackend(SDL_Window* window, Backend backend);
void UnloadBackend();

// The following functions are safe to call only after a backend has been loaded
void Clear();
void Present();
void UpdateDrawableSize(int* w, int* h);

// Functions to get new drawing objects
#define X(o) o New##o()
DRAW_OBJECTS()
#undef X

} // API

} // Drawing

#endif // DRAWING_API_HPP
