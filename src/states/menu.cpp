#include "menu.hpp"

#include <SDL_image.h>

#include <glm/gtc/matrix_transform.hpp> // glm::ortho

#include "../game_data.hpp"
#include "../sdl_utility.hpp"

#include "../drawing/primitive.hpp"
#include "../drawing/renderer.hpp"
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

Menu::Menu(const Drawing::Renderer& renderer, GameData& data) :
	renderer(renderer), data(data), env(renderer, data.guiFont,
	data.elapsed)
{
	bkg = renderer->NewPrimitive();
	
	// Background quad
	bkg->SetDrawMode(BKG_DRAWING_MODE);
	bkg->SetVertices(BKG_VERTICES);
	bkg->SetTexCoords({{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}});
	bkg->SetTexture(data.menuBkg);
	
	// GUI setup
	duelBtn = std::make_shared<GUI::CButton>(env);
	duelBtn->SetText(u8"Duel"); // TODO(dyxel): move to i18n config file
	exitBtn = std::make_shared<GUI::CButton>(env);
	exitBtn->SetText(u8"Exit"); // TODO(dyxel): move to i18n config file
	
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
	renderer->Clear();
	bkg->Draw();
	env.Draw();
	renderer->Present();
}

void Menu::OnResize()
{
	const int& w = data.canvasWidth;
	const int& h = data.canvasHeight;
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

} // namespace State

} // namespace YGOpen
