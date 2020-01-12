#ifndef GRAPHIC_BOARD_HPP
#define GRAPHIC_BOARD_HPP
#include <memory>

#include <SDL.h>

#include "../gui/element.hpp"
#include "../drawing/types.hpp"

#include "cmsg.pb.h"
#include "canswer.pb.h"

namespace YGOpen
{

using AnswerCallback = std::function<void(const Proto::CAnswer&)>;

struct GraphicBoardGfxData
{
	Drawing::Renderer renderer;
	// Textures
	Drawing::Texture player0Cover;
	Drawing::Texture player1Cover;
	Drawing::Texture unknownCard;
};

class IGraphicBoard : public GUI::IElement
{
public:
	static std::shared_ptr<IGraphicBoard> New(GUI::Environment& env, int flags);
	
	IGraphicBoard(GUI::Environment& env) : IElement(env)
	{}
	
	virtual void UpdateHitboxes() = 0;
	virtual void SetAnswerSubmitter(AnswerCallback cb) = 0;
	virtual void Resize(const SDL_Rect& parent, const SDL_Rect& rect) = 0;
	
	virtual void AddMsg(const Proto::CMsg& msg) = 0;
	virtual uint32_t GetTargetState() const = 0;
	virtual bool SetTargetState(uint32_t state) = 0;
	virtual void FillPile(uint32_t controller, uint32_t location, int num) = 0;
};

using GraphicBoard = std::shared_ptr<IGraphicBoard>;

} // namespace YGOpen

#endif // GRAPHIC_BOARD_HPP
