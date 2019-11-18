#ifndef GRAPHIC_BOARD_CONSTANTS_HPP
#define GRAPHIC_BOARD_CONSTANTS_HPP
#include "../drawing/types.hpp"

#include "enums/duel_mode.hpp"
#include "enums/location.hpp"
#include "enums/phase.hpp"

namespace YGOpen
{

using LitePlace = std::tuple<uint32_t /*controller*/,
                             uint32_t /*location*/,
                             uint32_t /*sequence*/>;

const float CARD_THICKNESS = 0.0040f;
const glm::vec3 OVERLAY_OFFSET = {0.01f, -0.01f, -0.0001f};
const glm::vec3 UP = {0.0f, 0.0f, 1.0f};

static const Drawing::Vertices ZONE_HITBOX_VERTICES =
{
	{ -0.2f,  0.2f, 0.0f},
	{  0.2f,  0.2f, 0.0f},
	{  0.2f, -0.2f, 0.0f},
	{ -0.2f, -0.2f, 0.0f},
	{ -0.2f,  0.2f, 0.0f},
};

static const Drawing::Vertices CARD_HITBOX_VERTICES =
{
	{ -0.1233f,  0.18f, 0.0f},
	{  0.1233f,  0.18f, 0.0f},
	{  0.1233f, -0.18f, 0.0f},
	{ -0.1233f, -0.18f, 0.0f},
	{ -0.1233f,  0.18f, 0.0f},
};

#if defined(DEBUG_MOUSE_POS)
static const Drawing::Vertices MOUSE_POS_VERTICES =
{
	{ -0.01f,  0.01f, 0.0f},
	{  0.01f,  0.01f, 0.0f},
	{  0.01f, -0.01f, 0.0f},
	{ -0.01f, -0.01f, 0.0f},
};
#endif // defined(DEBUG_MOUSE_POS)

static const Drawing::Vertices ZONE_VERTICES =
{
	{ -0.2f,  0.2f, 0.0f},
	{ -0.2f, -0.2f, 0.0f},
	{  0.2f,  0.2f, 0.0f},
	{  0.2f, -0.2f, 0.0f},
};

static const Drawing::Vertices CARD_VERTICES =
{
	{ -0.1233f,  0.18f, 0.0f},
	{ -0.1233f, -0.18f, 0.0f},
	{  0.1233f,  0.18f, 0.0f},
	{  0.1233f, -0.18f, 0.0f},
};

static const Drawing::Vertices CARD_COVER_VERTICES =
{
	{ -0.1233f,  0.18f, 0.0f},
	{  0.1233f,  0.18f, 0.0f},
	{ -0.1233f, -0.18f, 0.0f},
	{  0.1233f, -0.18f, 0.0f},
};

static const std::map<LitePlace, glm::vec3> BASE_LOCATIONS =
{
	// Monster Zones
	{{0, LOCATION_MZONE  , 0}, {-0.8f, -0.4f, 0.0f}},
	{{0, LOCATION_MZONE  , 1}, {-0.4f, -0.4f, 0.0f}},
	{{0, LOCATION_MZONE  , 2}, { 0.0f, -0.4f, 0.0f}},
	{{0, LOCATION_MZONE  , 3}, { 0.4f, -0.4f, 0.0f}},
	{{0, LOCATION_MZONE  , 4}, { 0.8f, -0.4f, 0.0f}},
	// Spell & Trap Zones
	{{0, LOCATION_SZONE  , 0}, {-0.8f, -0.8f, 0.0f}},
	{{0, LOCATION_SZONE  , 1}, {-0.4f, -0.8f, 0.0f}},
	{{0, LOCATION_SZONE  , 2}, { 0.0f, -0.8f, 0.0f}},
	{{0, LOCATION_SZONE  , 3}, { 0.4f, -0.8f, 0.0f}},
	{{0, LOCATION_SZONE  , 4}, { 0.8f, -0.8f, 0.0f}},
	{{0, LOCATION_SZONE  , 5}, {-1.2f, -0.2f, 0.0f}}, // Field Zone
	// Pendulum Zones
	{{0, LOCATION_PZONE  , 0}, {-1.2f, -0.6f, 0.0f}},
	{{0, LOCATION_PZONE  , 1}, { 1.2f, -0.6f, 0.0f}},
	// Piles
	{{0, LOCATION_DECK   , 0}, { 1.2f, -1.0f, 0.0f}},
	{{0, LOCATION_EXTRA  , 0}, {-1.2f, -1.0f, 0.0f}},
	{{0, LOCATION_GRAVE  , 0}, { 1.2f, -0.2f, 0.0f}},
	{{0, LOCATION_REMOVED, 0}, { 1.6f, -0.2f, 0.0f}},
	{{0, LOCATION_HAND   , 0}, { 0.0f, -1.2f, 0.2f}},
};

static const std::map<LitePlace, glm::vec3> EMZ_LOCATIONS =
{
	// Extra Monster Zones
	{{0, LOCATION_MZONE  , 5}, {-0.4f,  0.0f, 0.0f}},
	{{0, LOCATION_MZONE  , 6}, { 0.4f,  0.0f, 0.0f}},
};

} // namespace YGOpen

#endif // GRAPHIC_BOARD_CONSTANTS_HPP
