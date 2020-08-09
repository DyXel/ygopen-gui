#ifndef YGOPEN_SERVICE_IMMEDIATE_HPP
#define YGOPEN_SERVICE_IMMEDIATE_HPP

namespace YGOpen::Service
{

// Information which is shared across an Instance and its states, and that
// is not worth saving (entirely calculated at runtime) or
// changes too quickly to be considered a configuration.
class Immediate
{
public:
	bool exiting; // Flag that is checked each tick to exit the program.
	float elapsed; // Time elapsed since last draw call in seconds.
	int width, height; // This instance's window width and height.
};

} // namespace YGOpen::Service

#endif // YGOPEN_SERVICE_IMMEDIATE_HPP
