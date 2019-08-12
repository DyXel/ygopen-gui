#include "game_instance.hpp"
#include "states/loading.hpp"

namespace YGOpen
{

bool GetDisplayIndexFromWindow(SDL_Window* window, int* displayIndex)
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

GameInstance::GameInstance() : data(*this)
{}

GameInstance::~GameInstance()
{
	state.reset();
	if(window != nullptr)
		SDL_DestroyWindow(window);
	Drawing::API::UnloadBackend();
}

int GameInstance::Init(Drawing::Backend backend)
{
	window = SDL_CreateWindow(DEFAULT_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
	                          SDL_WINDOWPOS_UNDEFINED, DEFAULT_WINDOW_WIDTH,
	                          DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_OPENGL |
	                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
	if(window == nullptr)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
		                "Unable to create SDL Window: %s", SDL_GetError());
		return -1;
	}
	if(!Drawing::API::LoadBackend(window, backend))
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
		                "Unable to load selected backend");
		return -1;
	}
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
	}
#endif // #ifndef __ANDROID__
	// TODO: move this to the API
// 	if(SDL_GL_SetSwapInterval(-1) == -1)
// 	{
// 		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
// 		            "Unable to set adaptive vsync: %s", SDL_GetError());
// 		// TODO: either make all of this a option or fallback to vsync
// 	}
	Drawing::API::UpdateDrawableSize(&data.canvasWidth, &data.canvasHeight);
	Drawing::API::Clear();
	Drawing::API::Present();
	SDL_ShowWindow(window);
	state = std::make_shared<State::Loading>(&data);
	now = then = static_cast<unsigned>(SDL_GetTicks());
	return 0;
}

bool GameInstance::IsExiting() const
{
	return exiting;
}

void GameInstance::Exit()
{
	exiting = true;
}

void GameInstance::PropagateEvent(const SDL_Event& e)
{
	const auto eType = e.type;
	if(eType == SDL_QUIT)
		exiting = true;
	
	// If the event is a window size change event update the viewport/extent
	// to match the new size, also, save the new size.
	if(eType == SDL_WINDOWEVENT &&
	   e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
	{
		Drawing::API::UpdateDrawableSize(&data.canvasWidth, &data.canvasHeight);
	}
	
	if(eType != SDL_SYSWMEVENT)
		state->OnEvent(e);
}

void GameInstance::TickOnce()
{
	if(recording)
		now += 1000u / recording;
	else
		now = static_cast<unsigned>(SDL_GetTicks());
	data.elapsed = static_cast<float>(now - then) * 0.001f;
	state->Tick();
	then = now;
}

void GameInstance::DrawOnce()
{
	state->Draw();
}

void GameInstance::SetState(std::shared_ptr<State::IState> newState)
{
	state = newState;
}


} // YGOpen
