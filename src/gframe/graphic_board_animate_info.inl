case Core::Information::kDraw:
{
const auto& draw = info.draw();
const auto player = draw.player();
const auto drawCount = draw.cards().size();
auto& pHand = hand[player];
auto& pDeck = deck[player];
const auto handSz = pHand.size();
const auto deckSz = pDeck.size();
if(advancing)
{
	const int range = handSz - drawCount;
	// Animate cards already in hand
	std::vector<Animation::MoveCardData> cards;
	for(int i = 0; i < range; i++)
	{
		GraphicCard& card = pHand[i];
		Place p = {player, LOCATION_HAND, i, -1};
		Animation::MoveCardData mcd =
		{
			pHand[i],
			GetHandLocXYZ(p, range),
			GetRotXYZ(p, card.pos()),
			GetLocXYZ(p),
			GetRotXYZ(p, card.pos())
		};
		cards.push_back(mcd);
	}
	ani.Push(std::make_shared<Animation::MoveCards>(cam.vp, cards));
	// Animate moved cards
	for(int i = 0; i < drawCount; i++)
	{
		const int index = range + i;
		GraphicCard& card = pHand[index];
		Place startP = {player, LOCATION_DECK, deckSz - i, -1};
		Place endP = {player, LOCATION_HAND, index, -1};
		Animation::MoveCardData mcd =
		{
			card,
			GetLocXYZ(startP),
			GetRotXYZ(startP, card.pos()),
			GetLocXYZ(endP),
			GetRotXYZ(endP, card.pos())
		};
// 		ani.Push(/*draw sound*/);
		ani.Push(std::make_shared<Animation::MoveCard>(cam.vp, mcd));
	}
}
else
{
	// Animate moved cards
	for(int i = drawCount; i > 0; i--)
	{
		GraphicCard& card = pDeck[deckSz - i];
		Place startP = {player, LOCATION_HAND, handSz + i, -1};
		Place endP = {player, LOCATION_DECK, deckSz - i, -1};
		Animation::MoveCardData mcd =
		{
			card,
			GetHandLocXYZ(startP, handSz + drawCount + 1),
			GetRotXYZ(startP, card.pos()),
			GetLocXYZ(endP),
			GetRotXYZ(endP, card.pos())
		};
		ani.Push(std::make_shared<Animation::MoveCard>(cam.vp, mcd));
	}
	// Animate cards left in hand
	std::vector<Animation::MoveCardData> cards;
	for(int i = 0; i < handSz; i++)
	{
		GraphicCard& card = pHand[i];
		Place p = {player, LOCATION_HAND, i, -1};
		Animation::MoveCardData mcd =
		{
			card,
			GetHandLocXYZ(p, handSz + drawCount),
			GetRotXYZ(p, card.pos()),
			GetLocXYZ(p),
			GetRotXYZ(p, card.pos())
		};
		cards.push_back(mcd);
	}
	ani.Push(std::make_shared<Animation::MoveCards>(cam.vp, cards));
}
break;
}
