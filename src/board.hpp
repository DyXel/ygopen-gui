#ifndef BOARD_HPP
#define BOARD_HPP

#include <cstdint>
#include <algorithm>
#include <map>
#include <set>
#include <tuple>
#include <array>
#include <vector>
#include <type_traits>

#include "card.hpp"

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

template<typename C>
class DuelBoard
{
	static_assert(std::is_base_of<Card, C>::value, "C must be based off Card");
public:
	// Add a message at the end of the message list.
	void AppendMsg(const Core::AnyMsg& msg)
	{
		msgs.push_back(msg);
	}
	
	// Advance game state once.
	bool Forward()
	{
		if(msgs.size() == 0 || state > msgs.size() - 1)
			return false;
		if((realtime = (state == processedState)))
			processedState++;
		advancing = true;
		InterpretMsg(msgs[state]);
		state++;
		return true;
	}
	
	// Regress game state once.
	bool Backward()
	{
		if(state == 0)
			return false;
		realtime = false;
		advancing = false;
		state--;
		InterpretMsg(msgs[state]);
		return true;
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
	bool realtime{}; // Controls if a new message appends new info or forwards.
	bool advancing{}; // Are we going forward or going backward?
	uint32_t state{};
	uint32_t processedState{};
	std::vector<Core::AnyMsg> msgs;
	
	uint32_t turn{}; // Current turn.
	std::array<Sequential<uint32_t>, 2> playerLP; // Both player LP.
	Sequential<uint32_t> turnPlayer; // Player of the current turn.
	Sequential<uint32_t> phase; // Current game phase.

	std::map<TempPlace, C> tempCards; // See zoneCards comments.
	
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

	void InterpretMsg(const Core::AnyMsg& msg)
	{
		if(msg.AnyMsg_case() != Core::AnyMsg::kInformation)
			return;
		auto& info = msg.information();
		switch(info.Information_case())
		{
			// Critical messages, must be handled or the board is invalid.
#include "board_interpret_info.inl"
			// Non-critical messages, not handled by this class.
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
				return;
			}
			default:
			{
				return;
			}
		}
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

} // YGOpen

#endif // BOARD_HPP
