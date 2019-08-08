#ifndef TEXT_SMITH_HPP
#define TEXT_SMITH_HPP
#include <string_view>
#include <SDL_rwops.h>
#include <SDL_ttf.h>

namespace YGOpen
{

class TextSmith
{
public:
	TextSmith() = default;
	~TextSmith();

	// NOTE: avoid calling this multiple times. Rebuilds font.
	bool LoadFont(SDL_RWops* fontFile, int size);
	
	void SetTextColor(const SDL_Color& c);
	void SetShadowColor(const SDL_Color& c);
	
	SDL_Surface* Text(std::string_view text) const;
	SDL_Surface* ShadowedText(std::string_view text) const;
private:
	TTF_Font* f{nullptr};
	SDL_Color tc{240, 240, 240, 240};
	SDL_Color sc{20, 20, 20, 255};
};

} // YGOpen

#endif // TEXT_SMITH_HPP
