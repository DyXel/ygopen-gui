#include "menu.hpp"

#include <SDL_image.h>

#include <glm/gtc/matrix_transform.hpp> // glm::ortho

#include "../game_data.hpp"
#include "../sdl_utility.hpp"

#include "../drawing/primitive.hpp"
#include "../drawing/texture.hpp"
#include "../drawing/types.hpp"

namespace YGOpen
{

namespace State
{
constexpr auto BKG_DRAWING_MODE = Drawing::PDM_TRIANGLE_STRIP;
const Drawing::Color BKG_COLOR = {0.110f, 0.114f, 0.125f, 1.0f};
static const Drawing::Vertices BKG_VERTICES =
{
	// Corners
	{ -1.0f,  1.0f, 0.0f},
	{  1.0f,  1.0f, 0.0f},
	{ -1.0f, -1.0f, 0.0f},
	{  1.0f, -1.0f, 0.0f},
};
static const Drawing::Colors BKG_COLORS =
{
	BKG_COLOR, BKG_COLOR, BKG_COLOR, BKG_COLOR,
};

Menu::Menu(GameData* ptrData) :
	data(ptrData), env(data->guiFont, data->elapsed)
{
	auto texture = Drawing::API::NewTexture();
	SDL_Surface* image = IMG_Load("texture.png");
	if(image)
	{
		image = SDLU_SurfaceToRGBA32(image);
		texture->SetImage(image->w, image->h, image->pixels);
		SDL_FreeSurface(image);
	}
	else
	{
		SDL_Log("IMG_Load: %s\n", IMG_GetError());
	}
	
	bkg = Drawing::API::NewPrimitive();
	
	// Background quad
	bkg->SetDrawMode(BKG_DRAWING_MODE);
	bkg->SetVertices(BKG_VERTICES);
// 	bkg->SetColors(BKG_COLORS);
	bkg->SetTexCoords({{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}});
	bkg->SetTexture(texture);
	
	duelBtn = std::make_shared<GUI::CButton>(env);
	exitBtn = std::make_shared<GUI::CButton>(env);
	
	env.Add(duelBtn);
	env.Add(exitBtn);
	
	OnResize();
}

void Menu::OnEvent(const SDL_Event& e)
{
	if(e.type == SDL_WINDOWEVENT &&
	   e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
	{
		OnResize();
	}
	env.PropagateEvent(e);
}

void Menu::Tick()
{
	env.Tick();
}

void Menu::Draw()
{
// 	Drawing::API::Clear();
	bkg->Draw();
	env.Draw();
	Drawing::API::Present();
}

void Menu::OnResize()
{
	const int& w = data->canvasWidth;
	const int& h = data->canvasHeight;
	SDL_Rect bCanvas;
	proj = glm::ortho<float>(0.0f, w, h, 0.0f);
	if(w >= h)
	{
		bCanvas.w = w / 5;
		bCanvas.h = h / 20;
	}
	else
	{
		bCanvas.w = w * 3 / 4;
		bCanvas.h = h / 10;
	}
	
	const int bSeparation = h / 40;
	
	bCanvas.x = w / 2 - bCanvas.w / 2;
	bCanvas.y = bSeparation;
	duelBtn->Resize(proj, bCanvas);
	bCanvas.y += bSeparation + bCanvas.h;
	exitBtn->Resize(proj, bCanvas);
}

} // State

} // YGOpen
