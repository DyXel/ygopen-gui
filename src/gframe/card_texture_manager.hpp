#ifndef __CARD_TEXTURE_MANAGER__
#define __CARD_TEXTURE_MANAGER__
#include <unordered_set>
#include <unordered_map>

#include "../drawing/types.hpp"

namespace YGOpen
{

class CardTextureManager
{
public:
	CardTextureManager(Drawing::Renderer renderer);
	~CardTextureManager();

	Drawing::Texture GetCardTextureByCode(uint32_t code);
private:
	Drawing::Renderer renderer;
	Drawing::Texture errorTexture;
	std::unordered_set<uint32_t> error_cards;
	std::unordered_map<uint32_t, Drawing::Texture> textures;

	Drawing::Texture GetTextureFromCode(uint32_t);
};

} // namespace YGOpen

#endif //__CARD_TEXTURE_MANAGER__
