#include "game_instance.hpp"

#include <stdexcept>
#include <utility>

#include <ego/renderer.hpp>

#define USE_GL_CORE

namespace YGOpen
{

static bool GetDisplayIndexFromWindow(SDL_Window* window, int& displayIndex)
{
	// NOTE: not checking number of displays because its assumed a display
	// exist if window creation succeeded.
	displayIndex = SDL_GetWindowDisplayIndex(window);
	if(displayIndex < 0)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
		            "Unable to get display for current window: %s",
		            SDL_GetError());
		return false;
	}
	return true;
}

GameInstance::GameInstance() :
	svc({cfg, imm})
{
	ConstructWindowAndGLCtx();
	ConstructEgoRenderer();
	[this]() // LoadCustomConfig
	{
		// TODO
	};
	[this]() // ApplyCustomConfig
	{
		// TODO
		// Sets window to 80% size of the display it is currently in.
#ifndef __ANDROID__
		auto SetWindowSize = [&]() -> bool
		{
			int displayIndex;
			SDL_Rect r;
			if(!GetDisplayIndexFromWindow(sdlWindow, displayIndex))
				return false;
			if(SDL_GetDisplayUsableBounds(displayIndex, &r) < 0)
			{
				SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
							"Unable to get display usable bounds: %s",
							SDL_GetError());
				return false;
			}
			int& width = svc.imm.width;
			int& height = svc.imm.height;
			width  = (r.w - r.x) * 4 / 5;
			height = (r.h - r.y) * 4 / 5;
			SDL_Log("Setting window size to (%i, %i)", width, height);
			SDL_SetWindowSize(sdlWindow, width, height);
			return true;
		};
		if(!SetWindowSize())
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Unable to set window size.");
#endif // #ifndef __ANDROID__
	}();
	// Its better to show the window as soon as possible to let the user
	// know that the program is doing something.
	SDL_GL_SwapWindow(sdlWindow); // Clears up artifacts.
	SDL_ShowWindow(sdlWindow);
	[this]() // ReloadAssets
	{
		// TODO
	}();
	now = then = static_cast<unsigned>(SDL_GetTicks());
}

GameInstance::~GameInstance()
{
	SDL_GL_DeleteContext(sdlGLCtx);
	SDL_DestroyWindow(sdlWindow);
}

void GameInstance::Run()
{
	SDL_Event e;
	while(!svc.imm.exiting)
	{
		while(SDL_PollEvent(&e) != 0)
			OnEvent(e);
		Tick();
		svc.imm.renderer->DrawAllScenes();
		SDL_GL_SwapWindow(sdlWindow);
	}
}

void GameInstance::ConstructWindowAndGLCtx()
{
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
#ifdef USE_GL_CORE
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// Don't use deprecated functionality. Required on MacOS (apparently).
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif // USE_GL_CORE
	// Create window.
	sdlWindow = SDL_CreateWindow("YGOpen",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
	if(sdlWindow == nullptr)
	{
		std::string errStr("Unable to create SDL Window: ");
		errStr += SDL_GetError();
		throw std::runtime_error(errStr);
	}
	// Create GL context.
	sdlGLCtx = SDL_GL_CreateContext(sdlWindow);
	if(sdlGLCtx == nullptr)
	{
		std::string errStr("Unable to create GL Context: ");
		errStr += SDL_GetError();
		SDL_DestroyWindow(sdlWindow);
		throw std::runtime_error(errStr);
	}
	// Log GL info.
	{
		// Look mom! No headers!
		using GLGetStringType = unsigned const char*(*)(unsigned int);
		constexpr unsigned int GL_VENDOR = 0x1F00;
		constexpr unsigned int GL_RENDERER = 0x1F01;
		constexpr unsigned int GL_VERSION = 0x1F02;
		constexpr unsigned int GL_SHADING_LANGUAGE_VERSION = 0x8B8C;
		constexpr const char* FORMAT_STR = "OpenGL: %s (0x%X): %s";
		auto glGetString = reinterpret_cast<GLGetStringType>(SDL_GL_GetProcAddress("glGetString"));
#define SDL_LOG_GL(name) SDL_Log(FORMAT_STR, #name, name, glGetString(name))
		SDL_LOG_GL(GL_VENDOR);
		SDL_LOG_GL(GL_RENDERER);
		SDL_LOG_GL(GL_VERSION);
		SDL_LOG_GL(GL_SHADING_LANGUAGE_VERSION);
#undef SDL_LOG_GL
	}
}

void GameInstance::ConstructEgoRenderer()
{
#ifdef USE_GL_CORE
#define MAKE_RENDERER MakeGLCoreRenderer
#else
#define MAKE_RENDERER MakeGLESRenderer
#endif // USE_GL_CORE
	svc.imm.renderer = Ego::MAKE_RENDERER([](const char* procName)
	{
		return SDL_GL_GetProcAddress(procName);
	});
#undef MAKE_RENDERER
	// FIXME: Check if the renderer is valid?
}

void GameInstance::OnEvent(const SDL_Event& e)
{
	if(e.type == SDL_QUIT)
		svc.imm.exiting = true;
	
	// Save new window size.
	// Scene owners are responsible for resizing their scene's viewport.
	if(e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		SDL_GL_GetDrawableSize(sdlWindow, &svc.imm.width, &svc.imm.height);
}

void GameInstance::Tick()
{
	now = static_cast<unsigned>(SDL_GetTicks());
	svc.imm.elapsed = static_cast<float>(now - then) * 0.001f;
	then = now;
}

} // namespace YGOpen
