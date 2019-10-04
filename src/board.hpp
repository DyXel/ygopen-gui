#ifndef BOARD_HPP
#define BOARD_HPP

#include <cstdint>
#include <algorithm>
#include <map>
#include <set>
#include <tuple>
#include <array>
#include <vector>
#include <list>
#include <type_traits>

#include "enums/location.hpp"
#include "enums/position.hpp"
#include "core_data.pb.h"
#include "core_msg.pb.h"

namespace YGOpen
{

// Shorthand for a vector of cards.
template<typename C>
using Pile = std::vector<C>;

// NOTE: a negative value in overlay_sequence means DO NOT USE
using Place = std::tuple<uint32_t /*controller*/,
                         uint32_t /*location*/,
                         uint32_t /*sequence*/,
                         int32_t /*overlay_sequence*/>;
#define CON(t)  std::get<0>(t)
#define LOC(t)  std::get<1>(t)
#define SEQ(t)  std::get<2>(t)
#define OSEQ(t) std::get<3>(t)

// NOTE: Temp as in Temporal
using TempPlace = std::tuple<uint32_t /*state*/,
                             uint32_t /*controller*/,
                             uint32_t /*location*/,
                             uint32_t /*sequence*/,
                             int32_t /*overlay_sequence*/>;

using Counter = std::tuple<uint32_t /*type*/, uint32_t /*count*/>;

inline bool IsPile(const uint32_t location)
{
	return !((LOCATION_MZONE | LOCATION_SZONE | LOCATION_OVERLAY |
	       LOCATION_ONFIELD | LOCATION_FZONE | LOCATION_PZONE)
	       & location);
}

inline bool IsPile(const Place& place)
{
	return IsPile(LOC(place));
}

inline Place PlaceFromPbPlace(const Core::Data::Place& p)
{
	return {p.controller(), p.location(), p.sequence(), -1};
}

inline Place PlaceFromPbCardInfo(const Core::Data::CardInfo& cd)
{
	return {cd.controller(), cd.location(), cd.sequence(),
	        (cd.location() & LOCATION_OVERLAY) ? cd.overlay_sequence() : -1}; // TODO: handle at encoder level?
}

inline Counter CounterFromPbCounter(const Core::Data::Counter& c)
{
	return {c.type(), c.count()};
}

// TODO: change operator[] to .at() and add other meaningful exceptions

template<typename T, T def = std::is_signed<T>::value ? -1 : 0>
class Sequential
{
	std::list<T> l;
	typename std::list<T>::iterator it;
public:
	Sequential() : l(1, def), it(l.begin()) {}
	void AddOrNext(bool add, const T& v)
	{
		if(add)
			l.emplace_back(v);
		it++;
	}
	void Prev()
	{
		it--;
	}
	const T& operator()()
	{
		return *it;
	}
};

struct Card
{
	Sequential<uint32_t> pos;    // Position
	Sequential<uint32_t> code;   // Card code
	Sequential<uint32_t> alias;  // Alias card code
	Sequential<uint32_t> type;   // Type (Monster, Spell, Trap, etc..)
	Sequential<int32_t> level;   // Level
	Sequential<uint32_t> rank;   // Rank
	Sequential<uint32_t> attr;   // Attribute
	Sequential<uint32_t> race;   // Race
	Sequential<int32_t> atk;     // Attack
	Sequential<int32_t> def;     // Defense
	Sequential<int32_t> bAtk;    // Base Attack
	Sequential<int32_t> bDef;    // Base Defense
	Sequential<uint32_t> owner;  // Original owner
	Sequential<uint32_t> lscale; // Left pendulum scale
	Sequential<uint32_t> rscale; // Right pendulum scale
	Sequential<uint32_t> links;  // Link arrows
	std::map<uint32_t /*type*/, Sequential<uint32_t /*count*/>> counters;
};

template<typename C>
class DuelBoard
{
	static_assert(std::is_base_of<Card, C>::value, "C must be based off Card");
public:
	uint32_t GetTotalStates() const
	{
		return msgs.size();
	}
	
	uint32_t GetProcessedStates() const
	{
		return processedState;
	}
	
	uint32_t GetCurrentState() const
	{
		return state;
	}
	
	bool IsRealtime() const
	{
		return state == processedState;
	}
	
	// Add a message at the end of the message list.
	void AppendMsg(const Core::AnyMsg& msg)
	{
		msgs.push_back(msg);
	}
	
	// Advance game state once.
	void Forward()
	{
		if(msgs.size() == 0 || state > msgs.size() - 1)
			return;
		if((realtime = IsRealtime()))
			processedState++;
		advancing = true;
		InterpretMsg(msgs[state]);
		state++;
	}
	
	// Regress game state once.
	void Backward()
	{
		if(state == 0)
			return;
		realtime = false;
		advancing = false;
		state--;
		InterpretMsg(msgs[state]);
	}
	
	// Fill given pile with face-down cards.
	void FillPile(uint32_t controller, uint32_t location, int num)
	{
		auto& pile = GetPile(controller, location);
		pile.resize(num);
		for(auto& c : pile)
			c.pos.AddOrNext(true, POS_FACEDOWN);
	}
	
	// Set initial LP for player.
	void SetLP(uint32_t controller, uint32_t lp)
	{
		playerLP[controller].AddOrNext(true, lp);
	}
protected:
	uint32_t turn{}; // Current turn.
	std::array<Sequential<uint32_t>, 2> playerLP; // Both player LP.
	Sequential<uint32_t> turnPlayer; // Player of the current turn.
	Sequential<uint32_t> phase; // Current game phase.
	
	// Holds cards that are in "piles" (this includes hand).
	// 1) To get a pile, use GetPile.
	// 2) Moving from and onto these piles is handled by MoveSingle.
	// 3) Start of index (0) means bottom card.
	// 4) Start of index (0) means leftmost card in hand.
#define DUEL_PILES() \
	X(deck , LOCATION_DECK); \
	X(hand , LOCATION_HAND); \
	X(grave, LOCATION_GRAVE); \
	X(rmp  , LOCATION_REMOVED); \
	X(eDeck, LOCATION_EXTRA)
#define X(name, enums) std::array<Pile<C>, 2> name
	DUEL_PILES();
#undef X
	// Holds cards that are on the field (see IsPile implementation)
	// 1) Moving from and onto the field is handled by MoveSingle.
	// 2) Moving overlays is handled by MoveSingle.
	// 3) Overlay sequences are treated like piles, this means that
	// overlay sequence balancing (so there are no gaps) is performed during
	// movement of cards.
	// 4) When adding or removing cards from the field (mostly tokens), the
	// cards will be moved onto the tempCards container, along with their
	// state tag for retrieval when going backwards.
	// 5) Cards on index 5-6 mean extra monster zone monsters in mZone.
	// 6) Cards on index 5 in sZone are field zone cards.
	std::map<Place, C> zoneCards;
	
	// Holds which fields are blocked due to card effects.
	// 1) Initialized with all the zones that can be blocked. Ranges:
	// controller: (0, 1)
	// location: (LOCATION_MZONE, LOCATION_SZONE, LOCATION_PZONE)
	// sequence: for LOCATION_MZONE: (0, 1, 2, 3, 4, 5, 6)
	//           for LOCATION_SZONE: (0, 1, 2, 3, 4, 5)
	//           for LOCATION_PZONE: (0, 1)
	// NOTE: overlay_sequence is ALWAYS -1.
	std::map<Place, Sequential<bool, false>> disabledZones = []()
	{
		std::map<Place, Sequential<bool, false>> tempMap;
		std::array<int, 3> locations = {LOCATION_MZONE, LOCATION_SZONE,
		                                LOCATION_PZONE};
		for(int con = 0; con < 1; con++)
		{
			for(auto loc : locations)
			{
				int seq = 0;
				do
				{
					std::pair<Place, Sequential<bool, false>> p;
					p.first = {con, loc, seq, -1};
					tempMap.emplace(std::move(p));
					seq++;
					if(loc == LOCATION_MZONE && seq > 6)
						break;
					if(loc == LOCATION_SZONE && seq > 5)
						break;
					if(loc == LOCATION_PZONE && seq > 1)
						break;
				}while(true);
			}
		}
		return tempMap;
	}();

	// Helper functions to access above defined data.
	Pile<C>& GetPile(uint32_t controller, uint32_t location);
	Pile<C>& GetPile(const Place& place);
	C& GetCard(const Place& place);
// 	bool DoesCardHaveCounters(const C& card) const;
private:
	bool realtime{}; // Controls if a new message appends new info or forwards.
	bool advancing{}; // Are we going forward or going backward?
	uint32_t state{};
	uint32_t processedState{};
	std::vector<Core::AnyMsg> msgs;

	std::map<TempPlace, C> tempCards; // See zoneCards comments.

	// Moves a single card from one place to another. Deals with overlays
	// and counters accordingly but does not update card sequential
	// information (other than the counters).
	C& MoveSingle(const Place& from, const Place& to);
	
	// Used by HANDLE(CounterChange)
	void AddCounter(const Place& place, const Counter& counter);
	void RemoveCounter(const Place& place, const Counter& counter);

// 	void RemoveOverlayGap(const Place& place);
// 	void AddOverlayGap(const Place& place);

	// used when moving cards out of field.
	C& ClearAllCounters(C& card);

#define CRITICAL_MSGS() \
	X(UpdateCard); \
	X(AddCard); \
	X(RemoveCard); \
	X(Draw); \
	X(SwapCards); \
	X(ShuffleLocation); \
	X(ShuffleSetCards); \
	X(CounterChange); \
	X(DisableZones); \
	X(LpChange); \
	/*X(ChainAction);*/ \
	X(NewTurn); \
	X(NewPhase)
#define X(name) inline void Handle##name(const Core::Information& info)
	CRITICAL_MSGS();
#undef X
	void InterpretMsg(const Core::AnyMsg& msg)
	{
		if(msg.AnyMsg_case() != Core::AnyMsg::kInformation)
			return;
		auto& info = msg.information();
		switch(info.Information_case())
		{
#define X(name) case Core::Information::k##name: { Handle##name(info); break; }
			CRITICAL_MSGS();
#undef X
			case Core::Information::kMatchKiller:
			case Core::Information::kResult:
			case Core::Information::kHint:
			case Core::Information::kWin:
			case Core::Information::kConfirmCards:
			case Core::Information::kSummonCard:
			case Core::Information::kSelectedCards:
			case Core::Information::kOnAttack:
			case Core::Information::kCardHint:
			case Core::Information::kPlayerHint:
			{
				printf("NON-critical message (%i)\n", info.Information_case());
				return;
			}
			default:
			{
				// TODO: probably print a warning
			}
		}
		printf("Critical message (%i)\n", info.Information_case());
	}
};

template<typename C>
Pile<C>& DuelBoard<C>::GetPile(uint32_t controller, uint32_t location)
{
	switch(location)
	{
#define X(name, enums) case enums: {return name[controller]; break;}
		DUEL_PILES();
#undef X
	}
	throw std::exception();
}

template<typename C>
Pile<C>& DuelBoard<C>::GetPile(const Place& place)
{
	return GetPile(CON(place), LOC(place));
}

template<typename C>
C& DuelBoard<C>::GetCard(const Place& place)
{
	if(IsPile(place))
		return GetPile(place)[SEQ(place)];
	else
		return zoneCards[place];
}

template<typename C>
C& DuelBoard<C>::MoveSingle(const Place& from, const Place& to)
{
	// NOTE: move always move-insert for Piles
	if(from == to)
		throw std::exception();
	if(IsPile(from) && IsPile(to))
	{
		auto& fromPile = GetPile(from);
		auto& toPile = GetPile(to);
		toPile.emplace(toPile.begin() + SEQ(to),
		               std::move(fromPile[SEQ(from)]));
		fromPile.erase(fromPile.begin() + SEQ(from));
		return toPile[SEQ(to)];
	}
	// TODO: when moving overlays, overlay_sequence balancing is required
	else if(IsPile(from) && !IsPile(to))
	{
		auto& fromPile = GetPile(from);
		zoneCards[to] = std::move(fromPile[SEQ(from)]);
		fromPile.erase(fromPile.begin() + SEQ(from));
		return ClearAllCounters(zoneCards[to]);
	}
	else if(!IsPile(from) && IsPile(to))
	{
		auto& toPile = GetPile(to);
		toPile.emplace(toPile.begin() + SEQ(to), std::move(zoneCards[from]));
		zoneCards.erase(from);
		return ClearAllCounters(toPile[SEQ(to)]);
	}
	else // (!IsPile(from) && !IsPile(to))
	{
		zoneCards[to] = std::move(zoneCards[from]);
		zoneCards.erase(from);
		return zoneCards[to];
	}
}

template<typename C>
void DuelBoard<C>::AddCounter(const Place& place, const Counter& counter)
{
	C& card = GetCard(place);
	auto result = card.counters.find(std::get<0>(counter));
	if(result != card.counters.end())
	{
		const auto newAmount = result->second() + std::get<1>(counter);
		result->second.AddOrNext(realtime, newAmount);
		return;
	}
	auto& c = card.counters[std::get<0>(counter)];
	c.AddOrNext(realtime, std::get<1>(counter));
}

template<typename C>
void DuelBoard<C>::RemoveCounter(const Place& place, const Counter& counter)
{
	GetCard(place).counters[std::get<0>(counter)].Prev();
}

template<typename C>
C& DuelBoard<C>::ClearAllCounters(C& card)
{
	if(advancing)
	{
		for(auto& kv : card.counters)
			kv.second.AddOrNext(realtime, 0);
	}
	else
	{
		for(auto& kv : card.counters)
			kv.second.Prev();
	}
	return card;
}

#define HANDLE(name) \
template<typename C> \
void DuelBoard<C>::Handle##name(const Core::Information& info)

HANDLE(UpdateCard)
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
}

HANDLE(AddCard)
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
		auto p = zoneCards.emplace(place);
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
}

HANDLE(RemoveCard)
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
}

HANDLE(Draw)
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
}

HANDLE(SwapCards)
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
}

HANDLE(ShuffleLocation)
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
}

HANDLE(ShuffleSetCards)
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
}

HANDLE(CounterChange)
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
}

HANDLE(DisableZones)
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
}

HANDLE(LpChange)
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
}

HANDLE(NewTurn)
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
}

HANDLE(NewPhase)
{
auto& newPhase = info.new_phase();
if(advancing)
	phase.AddOrNext(realtime, newPhase.phase());
else
	phase.Prev();
}

#undef HANDLE

} // YGOpen

#endif // BOARD_HPP
