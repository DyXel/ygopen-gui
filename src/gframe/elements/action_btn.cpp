#include "action_btn.hpp"

#include "../../drawing/primitive.hpp"
#include "../../drawing/quad.hpp"
#include "../../drawing/renderer.hpp"
#include "../../drawing/texture.hpp"

#include "../../gui/environment.hpp"

namespace YGOpen
{

namespace GUI
{

static const Drawing::Color CONTENT_COLOR = {0.298f, 0.341f, 0.373f, 0.75f};

static const Drawing::Colors CONTENT_COLORS =
{
	CONTENT_COLOR, CONTENT_COLOR,
	CONTENT_COLOR, CONTENT_COLOR,
};

void CActBtn::Resize(const glm::mat4& mat, const SDL_Rect& rect)
{
	r = rect;
	
	Drawing::ResizeQuad(content.second, r.w, r.h);
	Drawing::ResizeQuad(img.second, r.w, r.h);
	
	content.first->SetVertices(content.second);
	img.first->SetVertices(img.second);
	
	auto projModel = mat * Drawing::Trans2D(r.x, r.y);
	content.first->SetMatrix(projModel);
	img.first->SetMatrix(projModel);
}

void CActBtn::Draw()
{
	content.first->Draw();
	img.first->Draw();
}

void CActBtn::SetCallback(Callback callback)
{
	cb = std::move(callback);
}

void CActBtn::SetImage(Drawing::Texture tex)
{
	img.first->SetTexture(tex);
}

CActBtn::CActBtn(Environment& env) : IElement(env)
{
	content.first = env.renderer->NewPrimitive();
	content.first->SetDrawMode(Drawing::GetQuadDrawMode());
	content.first->SetColors(CONTENT_COLORS);
	content.second = Drawing::GetQuadVertices({}, {});
	
	img.first = env.renderer->NewPrimitive();
	img.first->SetDrawMode(Drawing::GetQuadDrawMode());
	img.first->SetTexCoords(Drawing::GetQuadTexCoords());
	img.second = Drawing::GetQuadVertices({}, {});
}

bool CActBtn::OnEvent(const SDL_Event& e)
{
	if(e.type == SDL_MOUSEMOTION)
	{
		SDL_Point p = {e.motion.x, e.motion.y};
		if(SDL_PointInRect(&p, &r) == 0u)
		{
			content.first->SetBrightness(1.0f);
			img.first->SetBrightness(1.0f);
		}
	}
	else if(e.type == SDL_MOUSEBUTTONDOWN)
	{
		SDL_Point p = {e.button.x, e.button.y};
		if(SDL_PointInRect(&p, &r) != 0u)
		{
			content.first->SetBrightness(0.5f);
			img.first->SetBrightness(0.5f);
			return pressed = true;
		}
	}
	else if(e.type == SDL_MOUSEBUTTONUP)
	{
		if(pressed)
		{
			pressed = false;
			SDL_Point p = {e.button.x, e.button.y};
			if(SDL_PointInRect(&p, &r) != 0u)
			{
				content.first->SetBrightness(1.0f);
				img.first->SetBrightness(1.0f);
				if(cb)
					cb();
				return true;
			}
		}
	}
	return false;
}

} // namespace GUI

} // namespace YGOpen
