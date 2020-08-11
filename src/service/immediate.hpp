#ifndef YGOPEN_SERVICE_IMMEDIATE_HPP
#define YGOPEN_SERVICE_IMMEDIATE_HPP
#include <ego/ego_fwd.hpp>

namespace YGOpen
{

// Information which is shared across an Instance and its states, and that
// is not worth saving (entirely calculated at runtime) or
// changes too quickly to be considered a configuration.
class Service::Immediate final
{
public:
	bool exiting; // Flag that is checked each tick to exit the program.
	float elapsed; // Time elapsed since last draw call in seconds.
	int width, height; // This instance's window width and height.
	Ego::SRenderer renderer; // This instance's window Ego renderer.
};

} // namespace YGOpen::Service

#endif // YGOPEN_SERVICE_IMMEDIATE_HPP
