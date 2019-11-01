case Core::Information::kUpdateCard:
{
const auto& updateCard = info.update_card();
const auto& previousInfo = updateCard.previous();
const auto& currentInfo = updateCard.current();
const auto reason = updateCard.reason();
if(reason == Core::Msg::UpdateCard::REASON_DECK_TOP)
{
	auto& pile = GetPile(PlaceFromPbCardInfo(previousInfo));
	auto& card = *(pile.rbegin() - previousInfo.sequence());
	PushAnimation<Animation::SetCardImage>(card, ctm);
	// TODO
}
// REASON_MOVE or REASON_POS_CHANGE or REASON_SET
else if(advancing)
{
	const auto previous = PlaceFromPbCardInfo(previousInfo);
	const auto current = (reason == Core::Msg::UpdateCard::REASON_MOVE) ?
	                     PlaceFromPbCardInfo(currentInfo) : previous;
	auto& card = GetCard(current);
	Animation::MoveCardData mcd =
	{
		card,
		GetLocXYZ(previous),
		GetRotXYZ(previous, card.pos(-1)),
		GetLocXYZ(current),
		GetRotXYZ(current, card.pos())
	};
// 	if(updateCard.core_reason() & 0x1) // REASON_DESTROY
// 		ani.Push(/*destroy sound*/);
	PushAnimation<Animation::SetCardImage>(card, ctm);
	PushAnimation<Animation::MoveCard>(cam.vp, mcd);
}
else
{
	const auto previous = PlaceFromPbCardInfo(previousInfo);
	const auto current = (reason == Core::Msg::UpdateCard::REASON_MOVE) ?
	                     PlaceFromPbCardInfo(currentInfo) : previous;
	auto& card = GetCard(previous);
	Animation::MoveCardData mcd =
	{
		card,
		GetLocXYZ(current),
		GetRotXYZ(current, card.pos(1)),
		GetLocXYZ(previous),
		GetRotXYZ(previous, card.pos())
	};
	PushAnimation<Animation::MoveCard>(cam.vp, mcd);
	PushAnimation<Animation::SetCardImage>(card, ctm);
}
break;
}

case Core::Information::kAddCard:
{
const auto& addCard = info.add_card();
const auto& cardInfo = addCard.card();
const auto place = PlaceFromPbCardInfo(cardInfo);
if(realtime) // Set Card Data
{
	auto& card = GetCard(place);
	card.front = NewCardFrontPrim();
	card.cover = NewCardCoverPrim();
}
static const glm::vec3 FAR_AWAY_LOCATION = {0.0f, 0.0f, 100.0f};
static const glm::vec3 FAR_AWAY_ROTATION = {0.0f, 0.0f, 0.0f};
if(advancing)
{
	auto& card = GetCard(place);
	Animation::MoveCardData mcd =
	{
		card,
		FAR_AWAY_LOCATION,
		FAR_AWAY_ROTATION,
		GetLocXYZ(place),
		GetRotXYZ(place, card.pos())
	};
	PushAnimation<Animation::SetCardImage>(card, ctm);
	PushAnimation<Animation::MoveCard>(cam.vp, mcd);
}
else
{
	auto& card = tempCards[std::tuple_cat(place, std::tie(state))];
	Animation::MoveCardData mcd =
	{
		card,
		GetLocXYZ(place),
		GetRotXYZ(place, card.pos()),
		FAR_AWAY_LOCATION,
		FAR_AWAY_ROTATION,
	};
	PushAnimation<Animation::MoveCard>(cam.vp, mcd);
	PushAnimation<Animation::SetCardImage>(card, ctm);
}
break;
}

case Core::Information::kRemoveCard:
{
const auto& removeCard = info.remove_card();
const auto place = PlaceFromPbCardInfo(removeCard.card());
static const glm::vec3 FAR_AWAY_LOCATION = {0.0f, 0.0f, 100.0f};
static const glm::vec3 FAR_AWAY_ROTATION = {0.0f, 0.0f, 0.0f};
if(advancing)
{
	auto& card = tempCards[std::tuple_cat(place, std::tie(state))];
	Animation::MoveCardData mcd =
	{
		card,
		GetLocXYZ(place),
		GetRotXYZ(place, card.pos()),
		FAR_AWAY_LOCATION,
		FAR_AWAY_ROTATION,
	};
	PushAnimation<Animation::MoveCard>(cam.vp, mcd);
}
else
{
	auto& card = GetCard(place);
	Animation::MoveCardData mcd =
	{
		card,
		FAR_AWAY_LOCATION,
		FAR_AWAY_ROTATION,
		GetLocXYZ(place),
		GetRotXYZ(place, card.pos())
	};
	PushAnimation<Animation::MoveCard>(cam.vp, mcd);
}
break;
}

case Core::Information::kDraw:
{
const auto& draw = info.draw();
const auto player = draw.player();
const auto drawCount = draw.cards().size();
auto& pHand = hand[player];
auto& pDeck = deck[player];
const auto handSz = pHand.size();
const auto deckSz = pDeck.size();
auto HitboxUpdateWrapper = [&, player]()
{
	UpdateHandHitboxes(player);
};
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
	PushAnimation<Animation::MoveCards>(cam.vp, cards);
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
		PushAnimation<Animation::SetCardImage>(card, ctm);
		PushAnimation<Animation::MoveCard>(cam.vp, mcd);
	}
	PushAnimation<Animation::Call>(HitboxUpdateWrapper);
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
		PushAnimation<Animation::MoveCard>(cam.vp, mcd);
		PushAnimation<Animation::SetCardImage>(card, ctm);
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
	PushAnimation<Animation::MoveCards>(cam.vp, cards);
	PushAnimation<Animation::Call>(HitboxUpdateWrapper);
}
break;
}

case Core::Information::kSwapCards:
{
const auto& swapCards = info.swap_cards();
const auto card1Place = PlaceFromPbCardInfo(swapCards.card1());
const auto card2Place = PlaceFromPbCardInfo(swapCards.card2());
auto& card1 = GetCard(card1Place);
auto& card2 = GetCard(card2Place);
std::vector<Animation::MoveCardData> cards;
Animation::MoveCardData mcd1 =
{
	card1,
	GetLocXYZ(card1Place),
	GetRotXYZ(card1Place, card1.pos()),
	GetLocXYZ(card2Place),
	GetRotXYZ(card2Place, card1.pos())
};
Animation::MoveCardData mcd2 =
{
	card2,
	GetLocXYZ(card2Place),
	GetRotXYZ(card2Place, card2.pos()),
	GetLocXYZ(card1Place),
	GetRotXYZ(card1Place, card2.pos())
};
cards.push_back(mcd1);
cards.push_back(mcd2);
PushAnimation<Animation::MoveCards>(cam.vp, cards);
break;
}
