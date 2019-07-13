#ifndef DRAWING_API_HPP
#define DRAWING_API_HPP
#include <memory>
#include <vector>

struct SDL_Window;

// Its highly unlike we are ever going to use more than 1 context (window)
// and more than 1 backend at the same time, for that reason I decided
// to design the API to be globally available.
// I am actually making some decisions to try and keep everything as sealed
// as possible, in case in the future I would like to move this Drawing
// namespace to a class (maybe a drop-in replacement for SDL_Renderer).
namespace Drawing
{

namespace Detail
{
class IPrimitive;
}
// Primitive: Draw object with no texture but vertices and colors instead
// Texture: Holds a texture. You attach these to Sprites and Models
// Sprite: 2D draw object. Offers easy way to manipulate its coords on screen
// Model: 3D draw object. You probably need to apply transformations to these
using Primitive = std::shared_ptr<class Detail::IPrimitive>;

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
Primitive NewPrimitive();

} // API

} // Drawing

#endif // DRAWING_API_HPP
