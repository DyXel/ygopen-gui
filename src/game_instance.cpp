#include "game_instance.hpp"

#include "drawing/renderer.hpp"
#include "states/loading.hpp"
#include <utility>

namespace YGOpen
{

static bool GetDisplayIndexFromWindow(SDL_Window* window, int* displayIndex)
{
	// NOTE: not checking number of displays because its assumed a display
	// exist if window creation succeeded.
	*displayIndex = SDL_GetWindowDisplayIndex(window);
	if(*displayIndex < 0)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
		            "Unable to get display for current window: %s",
		            SDL_GetError());
		return false;
	}
	return true;
}

GameInstance::GameInstance(const Drawing::Backend backend) :
	SDLWindow(backend)
{
	auto SetDPI = [this]() -> bool
	{
		int displayIndex;
		if(!GetDisplayIndexFromWindow(window, &displayIndex))
			return false;
		if(SDL_GetDisplayDPI(displayIndex, &data.dpi, nullptr, nullptr) < 0)
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
			            "Unable to get display DPI: %s",
			            SDL_GetError());
			return false;
		}
		return true;
	};
	if(!SetDPI())
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
		            "Unable to set display DPI. Using default.");
		data.dpi = DEFAULT_DPI;
	}
	SDL_Log("Current DPI: %.2f", static_cast<double>(data.dpi));
#ifndef __ANDROID__
	// Sets window to 80% size of the display it is currently in.
	auto SetWindowSize = [this]() -> bool
	{
		int displayIndex;
		SDL_Rect r;
		if(!GetDisplayIndexFromWindow(window, &displayIndex))
			return false;
		if(SDL_GetDisplayUsableBounds(displayIndex, &r) < 0)
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
			            "Unable to get display usable bounds: %s",
			            SDL_GetError());
			return false;
		}
		int fWidth  = (r.w - r.x) * 4 / 5;
		int fHeight = (r.h - r.y) * 4 / 5;
		SDL_Log("Setting window size to (%i, %i)", fWidth, fHeight);
		SDL_SetWindowSize(window, fWidth, fHeight);
		return true;
	};
	if(!SetWindowSize())
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
		            "Unable to set window size. Using default.");
		SDL_SetWindowSize(window, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
	}
#endif // #ifndef __ANDROID__
	// Clear renderer before showing window (avoids transparent window)
	renderer->UpdateExtent(&data.canvasWidth, &data.canvasHeight);
	renderer->Clear();
	renderer->Present();
	// Set window title before showing window
	SDL_SetWindowTitle(window, DEFAULT_WINDOW_TITLE);
	SDL_ShowWindow(window);
	state = std::make_shared<State::Loading>(*this, data, renderer);
	now = then = static_cast<unsigned>(SDL_GetTicks());
}

GameInstance::~GameInstance()
{}

void GameInstance::Run()
{
	SDL_Event e;
	while(!exiting)
	{
		while(SDL_PollEvent(&e) != 0)
			OnEvent(e);
		Tick();
		Draw();
	}
}

void GameInstance::Exit()
{
	exiting = true;
}

void GameInstance::OnEvent(const SDL_Event& e)
{
	const auto eType = e.type;
	if(eType == SDL_QUIT)
		exiting = true;
	
	// If the event is a window size change event update the viewport/extent
	// to match the new size, also, save the new size.
	if(eType == SDL_WINDOWEVENT &&
	   e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
	{
		renderer->UpdateExtent(&data.canvasWidth, &data.canvasHeight);
	}
	
	if(eType != SDL_SYSWMEVENT)
		state->OnEvent(e);
}

void GameInstance::Tick()
{
	if(recording != 0u)
		now += 1000u / recording;
	else
		now = static_cast<unsigned>(SDL_GetTicks());
	data.elapsed = static_cast<float>(now - then) * 0.001f;
	state->Tick();
	then = now;
}

void GameInstance::Draw() const
{
	state->Draw();
}

void GameInstance::SetState(std::shared_ptr<State::IState> newState)
{
	state = std::move(newState);
}


} // namespace YGOpen
