case Core::Request::kSelectCmd:
{
const auto& selectCmd = request.select_cmd();
auto X = [&](Core::SelectionType type, const auto& container)
{
	int i = 0;
	for(const auto& cardInfo : container)
	{
		const auto place = PlaceFromPbCard(cardInfo);
		auto& card = GetCard(place);
		if(!card.action)
			card.action.reset(new GraphicCard::ActionData());
		card.action->ts[type] = i;
		cardsWithAction.emplace(place, card);
		i++;
	}
};
X(Core::SELECTION_TYPE_ACTIVABLE, selectCmd.activable_cards());
X(Core::SELECTION_TYPE_SUMMONABLE, selectCmd.summonable_cards());
X(Core::SELECTION_TYPE_SPSUMMONABLE, selectCmd.spsummonable_cards());
X(Core::SELECTION_TYPE_REPOSITIONABLE, selectCmd.repositionable_cards());
X(Core::SELECTION_TYPE_MSETABLE, selectCmd.msetable_cards());
X(Core::SELECTION_TYPE_SSETABLE, selectCmd.ssetable_cards());
X(Core::SELECTION_TYPE_CAN_ATTACK, selectCmd.can_attack_cards());
multiSelect = false;
break;
}

case Core::Request::kSelectPlaces:
{
const auto& selectPlaces = request.select_places();
zoneSelectCount = selectPlaces.count();
for(auto& pbp : selectPlaces.places())
	selectableZones.insert({pbp.con(), pbp.loc(), pbp.seq()});
break;
}
break;
}
