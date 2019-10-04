#include "button.hpp"

#include <utility>

#include "environment.hpp"

#include "../drawing/primitive.hpp"
#include "../drawing/quad.hpp"
#include "../drawing/renderer.hpp"
#include "../drawing/texture.hpp"

namespace YGOpen
{

namespace GUI
{

// Colors for the button
static const Drawing::Color CONTENT_TOP = {0.220f, 0.247f, 0.278f, 1.0f};
static const Drawing::Color CONTENT_BOTTOM = {0.298f, 0.341f, 0.373f, 1.0f};
static const Drawing::Color LINE_LEFT = {0.286f, 0.286f, 0.286f, 1.0f};
static const Drawing::Color LINE_TOP = {0.333f, 0.333f, 0.333f, 1.0f};
static const Drawing::Color LINE_BOTTOM = {0.200f, 0.200f, 0.200f, 1.0f};
static const Drawing::Color LINE_RIGHT = {0.251f, 0.251f, 0.251f, 1.0f};

// Button's shadow
constexpr float SHADOW_SIZE = 4.0f;
static const Drawing::Colors SHADOW_COLORS =
{
	// Outermost corners
	{0.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 0.0f},
	// Innermost corners
	{0.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 0.0f, 1.0f},
};
static const Drawing::Indices SHADOW_INDICES =
{
	0, 4, 1, 5, 3, 7, 2, 6, 0, 4,
};

// Button's content
static const Drawing::Colors CONTENT_COLORS =
{
	CONTENT_TOP, CONTENT_TOP,
	CONTENT_BOTTOM, CONTENT_BOTTOM,
};

// Button's lines
static const Drawing::Colors LINES_COLORS =
{
	LINE_LEFT, LINE_LEFT,
	LINE_TOP, LINE_TOP,
	LINE_BOTTOM, LINE_BOTTOM,
	LINE_RIGHT, LINE_RIGHT,
};

CButton::CButton(Environment& env) : IElement(env)
{
	shadow.first = env.renderer->NewPrimitive();
	content.first = env.renderer->NewPrimitive();
	lines.first = env.renderer->NewPrimitive();
	text.first = env.renderer->NewPrimitive();
	
	shadow.second =
	{
		// Outermost corners
		{ -SHADOW_SIZE, -SHADOW_SIZE, 0.0f},
		{ 1.0f + SHADOW_SIZE, -SHADOW_SIZE, 0.0f},
		{ -SHADOW_SIZE, 1.0f + SHADOW_SIZE, 0.0f},
		{ 1.0f + SHADOW_SIZE, 1.0f + SHADOW_SIZE, 0.0f},
		// Innermost corners
		{ 0.0f, 0.0f, 0.0f},
		{ 1.0f, 0.0f, 0.0f},
		{ 0.0f, 1.0f, 0.0f},
		{ 1.0f, 1.0f, 0.0f},
	};
	content.second = Drawing::GetQuadVertices({}, {});
	lines.second =
	{
		// Left line
		{ 0.0f, 0.0f, 0.0f},
		{ 0.0f, 1.0f, 0.0f},
		// Top Line
		{ 0.0f, 0.0f, 0.0f},
		{ 1.0f, 0.0f, 0.0f},
		// Bottom Line
		{ -1.0f, 1.0f, 0.0f},
		{ 1.0f, 1.0f, 0.0f},
		// Right Line
		{ 1.0f, 0.0f, 0.0f},
		{ 1.0f, 1.0f, 0.0f},
	};
	text.second = Drawing::GetQuadVertices({}, {});
	
	shadow.first->SetDrawMode(Drawing::PDM_TRIANGLE_STRIP);
	shadow.first->SetColors(SHADOW_COLORS);
	
	content.first->SetDrawMode(Drawing::GetQuadDrawMode());
	content.first->SetColors(CONTENT_COLORS);
	
	lines.first->SetDrawMode(Drawing::PDM_LINES);
	lines.first->SetColors(LINES_COLORS);
	
	text.first->SetDrawMode(Drawing::GetQuadDrawMode());
	text.first->SetTexCoords(Drawing::GetQuadTexCoords());
}

void CButton::Resize(const glm::mat4& mat, const SDL_Rect& rect)
{
	r = rect;
	const auto w = static_cast<float>(rect.w);
	const auto h = static_cast<float>(rect.h);
	const auto txtWidth = strTex->GetWidth();
	const auto txtHeight = strTex->GetHeight();
	
	// Update shadow vertices
	shadow.second[1].x = (shadow.second[5].x = w) + SHADOW_SIZE;
	shadow.second[2].y = (shadow.second[6].y = h) + SHADOW_SIZE;
	shadow.second[3].x = (shadow.second[7].x = w) + SHADOW_SIZE;
	shadow.second[3].y = (shadow.second[7].y = h) + SHADOW_SIZE;
	
	// Update content vertices
	Drawing::ResizeQuad(content.second, w, h);
	
	// Update lines vertices
	lines.second[1].y = lines.second[4].y = lines.second[5].y =
	lines.second[7].y = h;
	lines.second[3].x = lines.second[5].x = lines.second[6].x =
	lines.second[7].x = w;

	// Update text vertices
	Drawing::ResizeQuad(text.second, txtWidth, txtHeight);
	
	// Set up vertices in primitives
	shadow.first->SetVertices(shadow.second);
	shadow.first->SetIndices(SHADOW_INDICES);
	content.first->SetVertices(content.second);
	lines.first->SetVertices(lines.second);
	text.first->SetVertices(text.second);
	
	// Move to right position
	const int tx = rect.x + (rect.w / 2) - (txtWidth / 2);
	const int ty = rect.y + (rect.h / 2) - (txtHeight / 2);
	auto projModel = mat * Drawing::Trans2D(rect.x, rect.y);
	auto textProjModel = mat * Drawing::Trans2D(tx, ty);

	// Set up matrices for use with the shader
	shadow.first->SetMatrix(projModel);
	content.first->SetMatrix(projModel);
	lines.first->SetMatrix(projModel);
	text.first->SetMatrix(textProjModel);
}

void CButton::Draw()
{
	shadow.first->Draw();
	content.first->Draw();
	lines.first->Draw();
	text.first->Draw();
}

void CButton::Tick()
{
	brightness -= 2.0f * env.GetTimeElapsed();
	if(brightness <= 1.0f)
	{
		brightness = 1.0f;
		env.RemoveFromTickSet(shared_from_this());
	}
	SetBrightness(brightness);
}

void CButton::OnFocus(bool gained)
{
	if(gained)
	{
		brightness = 2.0f;
		SetBrightness(brightness);
	}
	else
	{
		env.AddToTickSet(shared_from_this());
	}
}

bool CButton::OnEvent(const SDL_Event& e)
{
	if(e.type == SDL_MOUSEMOTION)
	{
		SDL_Point p = {e.motion.x, e.motion.y};
		if(SDL_PointInRect(&p, &r) != 0u)
		{
			auto ele = shared_from_this();
			env.RemoveFromTickSet(ele);
			env.Focus(ele);
			return true;
		}
		else
		{
			SetBrightness(brightness);
			pressed = false;
		}
	}
	else if(e.type == SDL_MOUSEBUTTONDOWN)
	{
		SDL_Point p = {e.button.x, e.button.y};
		if(SDL_PointInRect(&p, &r) != 0u)
		{
			SetBrightness(0.5f);
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
				SetBrightness(brightness);
				if(cb)
					cb();
				return true;
			}
		}
	}
	return false;
}

void CButton::SetCallback(Callback callback)
{
	cb = std::move(callback);
}

void CButton::SetText(std::string_view txt)
{
	// Rebuild image
	strTex = env.renderer->NewTexture();
	SDL_Surface* image = env.font.ShadowedText(txt);
	strTex->SetImage(image->w, image->h, image->pixels);
	text.first->SetTexture(strTex);
	SDL_FreeSurface(image);
}

inline void CButton::SetBrightness(float b)
{
	content.first->SetBrightness(b);
	lines.first->SetBrightness(b);
}

} // namespace GUI

} // namespace YGOpen
