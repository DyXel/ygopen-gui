case Core::Request::kSelectCmd:
{
const auto& selectCmd = request.select_cmd();
auto X = [&](Core::CardSelectionType type, const auto& container)
{
	int i = 0;
	for(const auto& cardInfo : container)
	{
		const auto place = PlaceFromPbCardInfo(cardInfo);
		auto& card = GetCard(place);
		if(!card.action)
			card.action.reset(new GraphicCard::ActionData());
		card.action->ts[type] = i;
		cardsWithAction.emplace(place, card);
		i++;
	}
};
X(Core::CSELECT_W_EFFECT, selectCmd.cards_w_effect());
X(Core::CSELECT_CAN_ATTACK, selectCmd.cards_can_attack());
X(Core::CSELECT_SUMMONABLE, selectCmd.cards_summonable());
X(Core::CSELECT_SPSUMMONABLE, selectCmd.cards_spsummonable());
X(Core::CSELECT_REPOSITIONABLE, selectCmd.cards_repositionable());
X(Core::CSELECT_MSETABLE, selectCmd.cards_msetable());
X(Core::CSELECT_SSETABLE, selectCmd.cards_ssetable());
multiSelect = false;
break;
}

case Core::Request::kSelectPlaces:
{
const auto& selectPlaces = request.select_places();
zoneSelectCount = selectPlaces.min();
for(auto& pbp : selectPlaces.places())
	selectableZones.insert({pbp.controller(), pbp.location(), pbp.sequence()});
break;
}
break;
}
