#ifndef YGOPEN_SERVICE_LOCATOR_HPP
#define YGOPEN_SERVICE_LOCATOR_HPP

namespace YGOpen
{

namespace Service
{

class Config;
class Immediate;

} // namespace Service

struct ServiceLocator
{
	Service::Config& cfg;
	Service::Immediate& imm;
};

} // namespace YGOpen

#endif // YGOPEN_SERVICE_LOCATOR_HPP
