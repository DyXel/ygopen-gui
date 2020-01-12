case Proto::CMsg::Info::kUpdateCard:
{
const auto& updateCard = info.update_card();
const auto deckTop = updateCard.deck_top();
auto prev = PlaceFromPbCard(updateCard.previous());
if(deckTop)
	SEQ(prev) = GetPile(CON(prev), LOC(prev)).size() - 1 - SEQ(prev);
auto& card = GetCard(prev);
const auto locVec = GetLocXYZ(prev);
if(IsAdvancing())
	PushAnimation<Animation::SetCardImage>(ctm, card);
Animation::MoveCardData mcd =
{
	card,
	locVec,
	GetRotXYZ(prev, card.pos((IsAdvancing()) ? -1 : 1)),
	locVec,
	GetRotXYZ(prev, card.pos())
};
PushAnimation<Animation::MoveCard>(cam.vp, mcd);
if(!IsAdvancing())
	PushAnimation<Animation::SetCardImage>(ctm, card);
break;
}

case Proto::CMsg::Info::kMoveCard:
{
const auto& moveCard = info.move_card();
const auto prev = PlaceFromPbCard(moveCard.previous());
const auto curr = PlaceFromPbCard(moveCard.current());
Animation::MoveCards::Container cards;
int handNetChange[2] = {0};
// Calculate net change of hands
if(LOC(prev) & LOCATION_HAND)
{
	if(IsAdvancing())
		handNetChange[CON(prev)]--;
	else
		handNetChange[CON(prev)]++;
}
if(LOC(curr) & LOCATION_HAND)
{
	if(IsAdvancing())
		handNetChange[CON(curr)]++;
	else
		handNetChange[CON(curr)]--;
}
auto RefreshHand = [&](const uint8_t p)
{
	uint32_t i = 0u;
	const auto& is = (IsAdvancing()) ? curr : prev;
	const auto& was = (IsAdvancing()) ? prev : curr;
	Place startP = {p, LOCATION_HAND, 0, -1};
	Place endP = {p, LOCATION_HAND, 0, -1};
	auto& hand = GetPile(p, LOCATION_HAND);
	for(auto& card : hand)
	{
		SEQ(startP) = SEQ(endP) = i;
		// Make gap if card was moved from the hand
		SEQ(startP) += LOC(was) & LOCATION_HAND && i >= SEQ(was);
		// Remove gap if card was moved to the hand
		SEQ(startP) -= LOC(is) & LOCATION_HAND && i >= SEQ(is);
		Animation::MoveCardData mcd =
		{
			card,
			GetHandLocXYZ(startP, hand.size() - handNetChange[p]),
			GetRotXYZ(startP, card.pos()),
			GetLocXYZ(endP),
			GetRotXYZ(endP, card.pos())
		};
		cards.push_back(mcd);
		i++;
	}
};
// Only run the animations if there was an actual net change
// A card should not be able to move to a different part of the hand
// except by shuffling (which actually does not move any card client side)
if(handNetChange[0] != 0)
	RefreshHand(0);
if(handNetChange[1] != 0)
	RefreshHand(1);
// Gets the right location taking into consideration hand movement
auto GetFixedLoc = [&](const Place& place) -> glm::vec3
{
	const auto phdc = handNetChange[CON(place)];
	if(LOC(place) & LOCATION_HAND && phdc != 0)
	{
		const auto& hand = GetPile(CON(place), LOCATION_HAND);
		return GetHandLocXYZ(place, hand.size() - phdc);
	}
	return GetLocXYZ(place);
};
// Animate actual moved card
auto& card = GetCard((IsAdvancing()) ? curr : prev);
if(IsAdvancing())
{
	Animation::MoveCardData mcd =
	{
		card,
		GetFixedLoc(prev),
		GetRotXYZ(prev, card.pos(-1)),
		GetLocXYZ(curr),
		GetRotXYZ(curr, card.pos())
	};
	cards.push_back(mcd);
}
else
{
	Animation::MoveCardData mcd =
	{
		card,
		GetFixedLoc(curr),
		GetRotXYZ(curr, card.pos(1)),
		GetLocXYZ(prev),
		GetRotXYZ(prev, card.pos())
	};
	cards.push_back(mcd);
}
// 	if(updateCard.core_reason() & 0x1 && IsAdvancing()) // REASON_DESTROY
// 		ani.Push(/*destroy sound*/);
PushAnimation<Animation::SetCardImage>(ctm, card);
PushAnimation<Animation::MoveCards>(cam.vp, std::move(cards));
// Update hand hitboxes if there was any net change
if(handNetChange[0] != 0)
	PushAnimation<Animation::Call>(std::bind(&CGraphicBoard::HandHitbox, this, 0));
if(handNetChange[1] != 0)
	PushAnimation<Animation::Call>(std::bind(&CGraphicBoard::HandHitbox, this, 1));
break;
}

case Proto::CMsg::Info::kAddCard:
{
const auto& addCard = info.add_card();
const auto& cardInfo = addCard.card();
const auto place = PlaceFromPbCard(cardInfo);
if(IsRealtime())
{
	auto& card = GetCard(place);
	LazyCardGraphic(card);
}
static const glm::vec3 FAR_AWAY_LOCATION = {0.0f, 0.0f, 100.0f};
static const glm::vec3 FAR_AWAY_ROTATION = {0.0f, 0.0f, 0.0f};
// TODO: handle hand animation
if(IsAdvancing())
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
	PushAnimation<Animation::SetCardImage>(ctm, card);
	PushAnimation<Animation::MoveCard>(cam.vp, mcd);
}
else
{
	auto& card = GetLimboCard(std::tuple_cat(place, std::make_tuple(State())));
	Animation::MoveCardData mcd =
	{
		card,
		GetLocXYZ(place),
		GetRotXYZ(place, card.pos()),
		FAR_AWAY_LOCATION,
		FAR_AWAY_ROTATION,
	};
	PushAnimation<Animation::MoveCard>(cam.vp, mcd);
	PushAnimation<Animation::SetCardImage>(ctm, card);
}
break;
}

case Proto::CMsg::Info::kRemoveCard:
{
const auto& removeCard = info.remove_card();
const auto place = PlaceFromPbCard(removeCard.card());
static const glm::vec3 FAR_AWAY_LOCATION = {0.0f, 0.0f, 100.0f};
static const glm::vec3 FAR_AWAY_ROTATION = {0.0f, 0.0f, 0.0f};
// TODO: handle hand animation
if(IsAdvancing())
{
	auto& card = GetLimboCard(std::tuple_cat(place, std::make_tuple(State())));
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

case Proto::CMsg::Info::kDraw:
{
const auto& draw = info.draw();
const auto player = draw.player();
const auto drawCount = draw.cards().size();
auto& pHand = GetPile(player, LOCATION_HAND);
auto& pDeck = GetPile(player, LOCATION_DECK);
const auto handSz = pHand.size();
const auto deckSz = pDeck.size();
if(IsAdvancing())
{
	const int range = handSz - drawCount;
	// Animate cards already in hand
	Animation::MoveCards::Container cards;
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
	PushAnimation<Animation::MoveCards>(cam.vp, std::move(cards));
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
		PushAnimation<Animation::SetCardImage>(ctm, card);
		PushAnimation<Animation::MoveCard>(cam.vp, mcd);
	}
	PushAnimation<Animation::Call>(std::bind(&CGraphicBoard::HandHitbox, this, player));
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
		PushAnimation<Animation::SetCardImage>(ctm, card);
	}
	// Animate cards left in hand
	Animation::MoveCards::Container cards;
	for(std::size_t i = 0; i < handSz; i++)
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
	PushAnimation<Animation::MoveCards>(cam.vp, std::move(cards));
	PushAnimation<Animation::Call>(std::bind(&CGraphicBoard::HandHitbox, this, player));
}
break;
}

case Proto::CMsg::Info::kSwapCards:
{
const auto& swapCards = info.swap_cards();
const auto card1Place = PlaceFromPbCard(swapCards.card1());
const auto card2Place = PlaceFromPbCard(swapCards.card2());
auto& card1 = GetCard(card1Place);
auto& card2 = GetCard(card2Place);
Animation::MoveCards::Container cards;
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
PushAnimation<Animation::MoveCards>(cam.vp, std::move(cards));
// TODO: update hand hitboxes if swapped cards had anything to do with hand
break;
}

case Proto::CMsg::Info::kShuffleLocation:
{
const auto& shuffleLocation = info.shuffle_location();
const auto player = shuffleLocation.player();
if(shuffleLocation.location() == LOCATION_HAND)
{
	const Place shufflePlace = {player, LOCATION_HAND, 0, -1};
	const glm::vec3 shuffleLoc = GetHandLocXYZ(shufflePlace, 1);
	const glm::vec3 shuffleRot = GetRotXYZ(shufflePlace, POS_FACEDOWN);
	Animation::MoveCards::Container cards1, cards2;
	Animation::SetCardImages::Container cardsImg;
	int i = 0;
	for(auto& card : GetPile(player, LOCATION_HAND))
	{
		const Place place = {player, LOCATION_HAND, i, -1};
		const glm::vec3 loc = GetLocXYZ(place);
		const glm::vec3 rot = GetRotXYZ(place, card.pos());
		Animation::MoveCardData mcd1 =
		{
			card,
			loc,
			rot,
			shuffleLoc,
			shuffleRot
		};
		Animation::MoveCardData mcd2 =
		{
			card,
			shuffleLoc,
			shuffleRot,
			loc,
			rot
		};
		cards1.push_back(mcd1);
		cards2.push_back(mcd2);
		cardsImg.push_back(card);
		i++;
	}
	PushAnimation<Animation::MoveCards>(cam.vp, std::move(cards1));
	PushAnimation<Animation::SetCardImages>(ctm, std::move(cardsImg));
	PushAnimation<Animation::MoveCards>(cam.vp, std::move(cards2));
}
else
{
	// TODO
}
break;
}
