case Core::Information::kUpdateCard:
{
const auto& updateCard = info.update_card();
const auto& currentInfo = updateCard.current();
auto previous = PlaceFromPbCard(updateCard.previous());
if(updateCard.deck_top())
	SEQ(previous) = GetPile(previous).size() - 1 - SEQ(previous);
auto& card = GetCard(previous);
if(advancing)
{
	card.code.AddOrNext(realtime, currentInfo.code());
	card.pos.AddOrNext(realtime, currentInfo.pos());
}
else
{
	card.code.Prev();
	card.pos.Prev();
}
break;
}

case Core::Information::kMoveCard:
{
const auto& moveCard = info.move_card();
const auto& currentInfo = moveCard.current();
const auto previous = PlaceFromPbCard(moveCard.previous());
const auto current = PlaceFromPbCard(currentInfo);
if(advancing)
{
	auto& card = MoveSingle(previous, current);
	card.code.AddOrNext(realtime, currentInfo.code());
	card.pos.AddOrNext(realtime, currentInfo.pos());
}
else
{
	auto& card = GetCard(current);
	card.code.Prev();
	card.pos.Prev();
	MoveSingle(current, previous);
}
break;
}

case Core::Information::kAddCard:
{
const auto& addCard = info.add_card();
const auto& cardInfo = addCard.card();
if(advancing)
{
	auto place = PlaceFromPbCard(addCard.card());
	if(realtime && IsPile(place))
	{
		auto& pile = GetPile(place);
		C& card = *pile.emplace(pile.begin() + SEQ(place));
		card.code.AddOrNext(realtime, cardInfo.code());
		card.pos.AddOrNext(realtime, cardInfo.pos());
	}
	else if (realtime && !IsPile(place))
	{
		auto p = zoneCards.emplace(place, C{});
		C& card = (*p.first).second;
		card.code.AddOrNext(realtime, cardInfo.code());
		card.pos.AddOrNext(realtime, cardInfo.pos());
	}
	// Move out of the temporal
	else if(!realtime && IsPile(place))
	{
		auto& pile = GetPile(place);
		const auto t = std::tuple_cat(place, std::tie(state));
		C& card = *pile.emplace(pile.begin() + SEQ(place),
		                        std::move(tempCards[t]));
		tempCards.erase(t);
		card.code.AddOrNext(realtime, cardInfo.code());
		card.pos.AddOrNext(realtime, cardInfo.pos());
	}
	else // (!realtime && !IsPile(place))
	{
		const auto t = std::tuple_cat(place, std::tie(state));
		auto p = zoneCards.emplace(place, std::move(tempCards[t]));
		tempCards.erase(t);
		C& card = (*p.first).second;
		card.code.AddOrNext(realtime, cardInfo.code());
		card.pos.AddOrNext(realtime, cardInfo.pos());
	}
}
else
{
	auto place = PlaceFromPbCard(addCard.card());
	// Move into the temporal
	if(IsPile(place))
	{
		auto& pile = GetPile(place);
		auto& card = pile[SEQ(place)];
		card.code.Prev();
		card.pos.Prev();
		tempCards.emplace(std::tuple_cat(place, std::tie(state)),
		                                 std::move(card));
		pile.erase(pile.begin() + SEQ(place));
	}
	else
	{
		auto& card = zoneCards[place];
		card.code.Prev();
		card.pos.Prev();
		tempCards.emplace(std::tuple_cat(place, std::tie(state)),
		                                 std::move(card));
		zoneCards.erase(place);
	}
}
break;
}

case Core::Information::kRemoveCard:
{
const auto& removeCard = info.remove_card();
const auto place = PlaceFromPbCard(removeCard.card());
if(advancing)
{
	// Move into the temporal
	if(IsPile(place))
	{
		auto& pile = GetPile(place);
		tempCards.emplace(std::tuple_cat(place, std::tie(state)),
		                                 std::move(pile[SEQ(place)]));
		pile.erase(pile.begin() + SEQ(place));
	}
	else
	{
		tempCards.emplace(std::tuple_cat(place, std::tie(state)),
		                                 std::move(zoneCards[place]));
		zoneCards.erase(place);
	}
}
else
{
	// Move out of the temporal
	if(IsPile(place))
	{
		auto& pile = GetPile(place);
		const auto t = std::tuple_cat(place, std::tie(state));
		pile.emplace(pile.begin() + SEQ(place), std::move(tempCards[t]));
		tempCards.erase(t);
	}
	else // (!realtime && !IsPile(place))
	{
		const auto t = std::tuple_cat(place, std::tie(state));
		zoneCards.emplace(place, std::move(tempCards[t]));
		tempCards.erase(t);
	}
}
break;
}

case Core::Information::kDraw:
{
const auto& draw = info.draw();
const auto& cards = draw.cards();
const auto player = draw.player();
const auto handSz = hand[player].size();
if(advancing)
{
	std::move(deck[player].rbegin(), deck[player].rbegin() + cards.size(),
	          std::back_inserter(hand[player]));
	deck[player].resize(deck[player].size() - cards.size());
	for(int i = 0; i < cards.size(); i++)
	{
		auto& card = hand[player][handSz + i];
		card.code.AddOrNext(realtime, cards[i].code());
		card.pos.AddOrNext(realtime, cards[i].pos());
	}
}
else
{
	for(int i = 0; i < cards.size(); i++)
	{
		auto& card = hand[player][handSz - i - 1];
		card.code.Prev();
		card.pos.Prev();
	}
	std::move(hand[player].rbegin(), hand[player].rbegin() + cards.size(),
	          std::back_inserter(deck[player]));
	hand[player].resize(hand[player].size() - cards.size());
}
break;
}

case Core::Information::kSwapCards:
{
const auto& swapCards = info.swap_cards();
const auto& card1Info = swapCards.card1();
const auto& card2Info = swapCards.card2();
const auto card1Place = PlaceFromPbCard(card1Info);
const auto card2Place = PlaceFromPbCard(card2Info);
C tmp;
if(IsPile(card1Place))
{
	auto& pile = GetPile(card1Place);
	tmp = std::move(pile[SEQ(card1Place)]);
	pile.erase(pile.begin() + SEQ(card1Place));
}
else
{
	tmp = std::move(zoneCards[card1Place]);
	zoneCards.erase(card1Place);
}
MoveSingle(card2Place, card1Place);
if(IsPile(card2Place))
{
	auto& pile = GetPile(card2Place);
	pile.emplace(pile.begin() + SEQ(card2Place), std::move(tmp));
}
else
{
	zoneCards[card2Place] = std::move(tmp);
}
break;
}

case Core::Information::kShuffleLocation:
{
const auto& shuffleLocation = info.shuffle_location();
if(advancing)
{
	auto& shuffledCards = shuffleLocation.shuffled_cards();
	auto& pile = GetPile(shuffleLocation.player(), shuffleLocation.location());
	int i = 0;
	for(auto& c : pile)
	{
		if(!shuffledCards.empty())
		{
			c.code.AddOrNext(realtime, shuffledCards[i].code());
			i++;
			continue;
		}
		c.code.AddOrNext(realtime, 0);
	}
}
else
{
	auto& pile = GetPile(shuffleLocation.player(), shuffleLocation.location());
	for(auto& c : pile)
		c.code.Prev();
}
break;
}

case Core::Information::kShuffleSetCards:
{
const auto& shuffleSetCards = info.shuffle_set_cards();
const auto& previousCards = shuffleSetCards.previous_cards();
const auto& currentCards = shuffleSetCards.current_cards();
if(advancing)
{
	for(int i = 0; i < previousCards.size(); i++)
	{
		auto& card = zoneCards[PlaceFromPbCard(previousCards[i])];
		if(!currentCards.empty())
		{
			const auto& currentInfo = currentCards[i];
			card.code.AddOrNext(realtime, currentInfo.code());
			card.pos.AddOrNext(realtime, currentInfo.pos());
			continue;
		}
		card.code.AddOrNext(realtime, 0);
		card.pos.AddOrNext(realtime, previousCards[i].pos());
	}
}
else
{
	for(int i = 0; i < previousCards.size(); i++)
	{
		auto& card = zoneCards[PlaceFromPbCard(previousCards[i])];
		card.code.Prev();
		card.pos.Prev();
	}
}
break;
}

case Core::Information::kCounterChange:
{
const auto& counterChange = info.counter_change();
const Counter counter = CounterFromPbCounter(counterChange.counter());
const Place place = PlaceFromPbPlace(counterChange.place());
if(advancing)
{
	if(counterChange.type() == Core::Msg::CounterChange::COUNTER_CHANGE_TYPE_ADD)
		AddCounter(place, counter);
	else // counterChange.type() == Core::Msg::CounterChange::COUNTER_CHANGE_TYPE_REMOVE
		RemoveCounter(place, counter);
}
else
{
	if(counterChange.type() == Core::Msg::CounterChange::COUNTER_CHANGE_TYPE_ADD)
		RemoveCounter(place, counter);
	else // counterChange.type() == Core::Msg::CounterChange::COUNTER_CHANGE_TYPE_REMOVE
		AddCounter(place, counter);
}
break;
}

case Core::Information::kDisableZones:
{
const auto& disableZones = info.disable_zones();
const auto& places = disableZones.places();
if(advancing)
{
	if(!realtime)
	{
		for(auto& zone : disabledZones)
			zone.second.AddOrNext(false, false);
	}
	std::set<Place> tmpSet = [&]()
	{
		std::set<Place> s;
		for(auto& p : places)
			s.insert(PlaceFromPbPlace(p));
		return s;
	}();
	for(auto& zone : disabledZones)
		zone.second.AddOrNext(true, tmpSet.find(zone.first) != tmpSet.end());
}
else
{
	for(auto& zone : disabledZones)
		zone.second.Prev();
}
break;
}

case Core::Information::kLpChange:
{
const auto& lpChange = info.lp_change();
const auto player = lpChange.player();
const auto amount = lpChange.amount();
if(advancing)
{
	auto type = lpChange.type();
	if(type == Core::Msg::LpChange::LP_CHANGE_TYPE_DAMAGE ||
	   type == Core::Msg::LpChange::LP_CHANGE_TYPE_PAY)
	{
		auto deltaAmount = static_cast<int32_t>(playerLP[player]()) - amount;
		if(deltaAmount < 0)
			playerLP[player].AddOrNext(realtime, 0);
		else
			playerLP[player].AddOrNext(realtime, deltaAmount);
	}
	else if(type == Core::Msg::LpChange::LP_CHANGE_TYPE_RECOVER)
	{
		playerLP[player].AddOrNext(realtime, playerLP[player]() + amount);
	}
	else // (type == Core::Msg::LpChange::LP_CHANGE_TYPE_BECOME)
	{
		playerLP[player].AddOrNext(realtime, amount);
	}
}
else
{
	playerLP[player].Prev();
}
break;
}

case Core::Information::kNewTurn:
{
auto& newTurn = info.new_turn();
if(advancing)
{
	turn++;
	turnPlayer.AddOrNext(realtime, newTurn.turn_player());
}
else
{
	turnPlayer.Prev();
	turn--;
}
break;
}

case Core::Information::kNewPhase:
{
auto& newPhase = info.new_phase();
if(advancing)
	phase.AddOrNext(realtime, newPhase.phase());
else
	phase.Prev();
break;
}
