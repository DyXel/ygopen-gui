#ifndef COMMON_DATA_HPP
#define COMMON_DATA_HPP
#include <memory>
#include "game_instance.hpp"
#include "configs.hpp"

namespace YGOpen
{
// Data which is shared across the same GameInstance and its states,
// Which should include: live configuration, OpenGL data (shaders, meshes, etc),
// synchronization primitives.. Basically everything that should be shared
// between states and the main GameInstance.
struct CommonData
{
	YGOpen::GameInstance* gi; // Owning game instance
	// Sync stuff
	
	// OpenGL stuff
	
	
	// Miscellaneous stuff
	std::unique_ptr<Configs> cfgs;
	bool isRecording{false};
	bool isPowerSaving{false};
};

} // YGOpen

#endif // COMMON_DATA_HPP
