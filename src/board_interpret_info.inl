case Core::Information::kUpdateCard:
{
const auto& updateCard = info.update_card();
const auto& previousInfo = updateCard.previous();
const auto& currentInfo = updateCard.current();
const auto reason = updateCard.reason();
if(advancing)
{
	if(reason == Core::Msg::UpdateCard::REASON_DECK_TOP)
	{
		auto& pile = GetPile(PlaceFromPbCardInfo(previousInfo));
		auto& card = *(pile.rbegin() - previousInfo.sequence());
		card.code.AddOrNext(realtime, currentInfo.code());
	}
	else if(reason == Core::Msg::UpdateCard::REASON_MOVE)
	{
		auto previous = PlaceFromPbCardInfo(previousInfo);
		auto current = PlaceFromPbCardInfo(currentInfo);
		auto& card = MoveSingle(previous, current);
		card.code.AddOrNext(realtime, currentInfo.code());
		card.pos.AddOrNext(realtime, currentInfo.position());
	}
	else // REASON_POS_CHANGE or REASON_SET
	{
		auto& card = GetCard(PlaceFromPbCardInfo(previousInfo));
		card.code.AddOrNext(realtime, currentInfo.code());
		card.pos.AddOrNext(realtime, currentInfo.position());
	}
}
else
{
	if(reason == Core::Msg::UpdateCard::REASON_DECK_TOP)
	{
		auto& pile = GetPile(PlaceFromPbCardInfo(previousInfo));
		auto& card = *(pile.rbegin() - previousInfo.sequence());
		card.code.Prev();
	}
	else if(reason == Core::Msg::UpdateCard::REASON_MOVE)
	{
		auto previous = PlaceFromPbCardInfo(previousInfo);
		auto current = PlaceFromPbCardInfo(currentInfo);
		auto& card = GetCard(current);
		card.code.Prev();
		card.pos.Prev();
		MoveSingle(current, previous);
	}
	else // REASON_POS_CHANGE or REASON_SET
	{
		auto& card = GetCard(PlaceFromPbCardInfo(previousInfo));
		card.code.Prev();
		card.pos.Prev();
	}
}
break;
}

case Core::Information::kAddCard:
{
const auto& addCard = info.add_card();
const auto& cardInfo = addCard.card();
if(advancing)
{
	auto place = PlaceFromPbCardInfo(addCard.card());
	if(realtime && IsPile(place))
	{
		auto& pile = GetPile(place);
		C& card = *pile.emplace(pile.begin() + SEQ(place));
		card.code.AddOrNext(realtime, cardInfo.code());
		card.pos.AddOrNext(realtime, cardInfo.position());
	}
	else if (realtime && !IsPile(place))
	{
		auto p = zoneCards.emplace(place, C{});
		C& card = (*p.first).second;
		card.code.AddOrNext(realtime, cardInfo.code());
		card.pos.AddOrNext(realtime, cardInfo.position());
	}
	// Move out of the temporal
	else if(!realtime && IsPile(place))
	{
		auto& pile = GetPile(place);
		auto t = std::tuple_cat(std::tie(state), place);
		C& card = *pile.emplace(pile.begin() + SEQ(place),
		                        std::move(tempCards[t]));
		tempCards.erase(t);
		card.code.AddOrNext(realtime, cardInfo.code());
		card.pos.AddOrNext(realtime, cardInfo.position());
	}
	else // (!realtime && !IsPile(place))
	{
		auto t = std::tuple_cat(std::tie(state), place);
		auto p = zoneCards.emplace(place, std::move(tempCards[t]));
		tempCards.erase(t);
		C& card = (*p.first).second;
		card.code.AddOrNext(realtime, cardInfo.code());
		card.pos.AddOrNext(realtime, cardInfo.position());
	}
}
else
{
	auto place = PlaceFromPbCardInfo(addCard.card());
	// Move into the temporal
	if(IsPile(place))
	{
		auto& pile = GetPile(place);
		auto& card = pile[SEQ(place)];
		card.code.Prev();
		card.pos.Prev();
		tempCards.emplace(std::tuple_cat(std::tie(state), place),
		                                 std::move(card));
		pile.erase(pile.begin() + SEQ(place));
	}
	else
	{
		auto& card = zoneCards[place];
		card.code.Prev();
		card.pos.Prev();
		tempCards.emplace(std::tuple_cat(std::tie(state), place),
		                                 std::move(card));
		zoneCards.erase(place);
	}
}
break;
}

case Core::Information::kRemoveCard:
{
const auto& removeCard = info.remove_card();
if(advancing)
{
	auto place = PlaceFromPbCardInfo(removeCard.card());
	// Move into the temporal
	if(IsPile(place))
	{
		auto& pile = GetPile(place);
		tempCards.emplace(std::tuple_cat(std::tie(state), place),
		                                 std::move(pile[SEQ(place)]));
		pile.erase(pile.begin() + SEQ(place));
	}
	else
	{
		tempCards.emplace(std::tuple_cat(std::tie(state), place),
		                                 std::move(zoneCards[place]));
		zoneCards.erase(place);
	}
}
else
{
	auto place = PlaceFromPbCardInfo(removeCard.card());
	// Move out of the temporal
	if(IsPile(place))
	{
		auto& pile = GetPile(place);
		auto t = std::tuple_cat(std::tie(state), place);
		pile.emplace(pile.begin() + SEQ(place), std::move(tempCards[t]));
		tempCards.erase(t);
	}
	else // (!realtime && !IsPile(place))
	{
		auto t = std::tuple_cat(std::tie(state), place);
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
		hand[player][handSz + i].code.AddOrNext(realtime, cards[i].code());
}
else
{
	for(int i = 0; i < cards.size(); i++)
		hand[player][handSz - i - 1].code.Prev();
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
const auto card1Place = PlaceFromPbCardInfo(card1Info);
const auto card2Place = PlaceFromPbCardInfo(card2Info);
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
const auto& cardsPrevious = shuffleSetCards.cards_previous();
const auto& cardsCurrent = shuffleSetCards.cards_current();
if(advancing)
{
	for(int i = 0; i < cardsPrevious.size(); i++)
	{
		auto& c = zoneCards[PlaceFromPbCardInfo(cardsPrevious[i])];
		if(!cardsCurrent.empty())
		{
			auto& currentInfo = cardsCurrent[i];
			c.code.AddOrNext(realtime, currentInfo.code());
			c.pos.AddOrNext(realtime, currentInfo.position());
			continue;
		}
		c.code.AddOrNext(realtime, 0);
		c.pos.AddOrNext(realtime, cardsPrevious[i].position());
	}
}
else
{
	for(int i = 0; i < cardsPrevious.size(); i++)
	{
		auto& c = zoneCards[PlaceFromPbCardInfo(cardsPrevious[i])];
		c.code.Prev();
		c.pos.Prev();
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
	if(counterChange.type() == Core::Msg::CounterChange::CHANGE_ADD)
		AddCounter(place, counter);
	else // counterChange.type() == Core::Msg::CounterChange::CHANGE_REMOVE
		RemoveCounter(place, counter);
}
else
{
	if(counterChange.type() == Core::Msg::CounterChange::CHANGE_ADD)
		RemoveCounter(place, counter);
	else // counterChange.type() == Core::Msg::CounterChange::CHANGE_REMOVE
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
	if(type == Core::Msg::LpChange::CHANGE_DAMAGE ||
	   type == Core::Msg::LpChange::CHANGE_PAY)
	{
		auto deltaAmount = static_cast<int32_t>(playerLP[player]()) - amount;
		if(deltaAmount < 0)
			playerLP[player].AddOrNext(realtime, 0);
		else
			playerLP[player].AddOrNext(realtime, deltaAmount);
	}
	else if(type == Core::Msg::LpChange::CHANGE_RECOVER)
	{
		playerLP[player].AddOrNext(realtime, playerLP[player]() + amount);
	}
	else // (type == Core::Msg::LpChange::CHANGE_BECOME)
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
