#ifndef GRAPHIC_BOARD_HPP
#define GRAPHIC_BOARD_HPP
#include <memory>

#include <SDL.h>

#include "../gui/element.hpp"
#include "../drawing/types.hpp"

#include "core_msg.pb.h"
#include "core_msg_answer.pb.h"

namespace YGOpen
{

using AnswerCallback = std::function<void(const Core::Answer&)>;

struct GraphicBoardGfxData
{
	Drawing::Renderer renderer;
	// Textures
	Drawing::Texture player0Cover;
	Drawing::Texture player1Cover;
	Drawing::Texture unknownCard;
};

ELEMENT_DECLARATION(CGraphicBoard)
{
public:
	ELEMENT_IMPLEMENT_New_METHOD(CGraphicBoard)
	CGraphicBoard(GUI::Environment& env, Drawing::Renderer renderer, int flags);
	~CGraphicBoard();
	
	void SetCameraPosition(const glm::vec3& cPos);
	void UpdateHitboxes();
	void SetAnswerCallback(AnswerCallback answerCallback);
	void Resize(const SDL_Rect& parent, const SDL_Rect& rect);
	
	// Element public overrides
	void Draw() override;
	
	// DuelBoard overrides / calls forwarding
	void AppendMsg(const Core::AnyMsg& msg);
	uint32_t GetState() const;
	uint32_t GetStatesCount() const;
	uint32_t GetTargetState() const;
	bool SetTargetState(uint32_t state);
	void FillPile(uint32_t controller, uint32_t location, int num);
private:
	class impl;
	std::unique_ptr<impl> pimpl;
	
	// Element private/protected overrides
	void Resize(const glm::mat4& mat, const SDL_Rect& rect) override;
	void Tick() override;
// 	void OnFocus(bool gained) override;
	bool OnEvent(const SDL_Event& e) override;
};

using GraphicBoard = std::shared_ptr<CGraphicBoard>;

} // namespace YGOpen

#endif // GRAPHIC_BOARD_HPP
