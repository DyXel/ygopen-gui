#include "menu.hpp"

#include <SDL_image.h>

#include "../game_data.hpp"
#include "../sdl_utility.hpp"

#include "../drawing/primitive.hpp"
#include "../drawing/quad.hpp"
#include "../drawing/renderer.hpp"
#include "../drawing/texture.hpp"
#include "../drawing/types.hpp"

namespace YGOpen
{

namespace State
{

Menu::Menu(GameInstance& gi, GameData& data, Drawing::Renderer renderer) :
	gi(gi), data(data), renderer(renderer), env(renderer, data.guiFont, data.elapsed)
{
	bkg = renderer->NewPrimitive();
	bkgVertices = Drawing::GetQuadVertices({}, {});
	
	// Background quad
	bkg->SetTexture(data.menuBkg);
	bkg->SetDrawMode(Drawing::GetQuadDrawMode());
	bkg->SetTexCoords(Drawing::GetQuadTexCoords());
	
	// GUI setup
	duelBtn = GUI::CButton::New(env);
	duelBtn->SetText(u8"Duel"); // TODO(dyxel): move to i18n config file
	exitBtn = GUI::CButton::New(env);
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
	env.OnEvent(e);
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
	const int& w = data.canvas.w;
	const int& h = data.canvas.h;
	const int bkgW = data.menuBkg->GetWidth();
	SDL_Rect bCanvas;
	glm::mat4 proj = Drawing::Get2DProjMatrix(w, h);
	renderer->SetViewport(0, 0, w, h);
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
	
	Drawing::ResizeQuad(bkgVertices, bkgW, h);
	bkg->SetVertices(bkgVertices);
	bkg->SetMatrix(proj * Drawing::Trans2D((-bkgW / 2.0f) + (w / 2.0f), 0.0f));
	
	const int bSeparation = h / 40;
	
	bCanvas.x = w / 2 - bCanvas.w / 2;
	bCanvas.y = bSeparation;
	duelBtn->Resize(proj, bCanvas);
	bCanvas.y += bSeparation + bCanvas.h;
	exitBtn->Resize(proj, bCanvas);
}

} // namespace State

} // namespace YGOpen
