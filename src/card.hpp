#ifndef BOARD_CARD_HPP
#define BOARD_CARD_HPP
#include <map>
#include <vector>

namespace YGOpen
{

template<typename T, T def = std::is_signed<T>::value ? -1 : 0>
class Sequential
{
	std::vector<T> vec;
	typename std::vector<T>::size_type pos;
public:
	Sequential() : vec(1, def), pos(0) {}
	void AddOrNext(bool add, const T& value)
	{
		if(add)
			vec.emplace_back(value);
		pos++;
	}
	void Prev()
	{
		pos--;
	}
	const T& operator()()
	{
		return vec[pos];
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
	
	Card() = default;
	Card(const Card&) = delete;
	Card(Card&&) = default;
	Card& operator=(const Card&) = delete;
	Card& operator=(Card&&) = default;
};

} // namespace YGOpen

#endif // BOARD_CARD_HPP
