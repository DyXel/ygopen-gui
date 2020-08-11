#ifndef YGOPEN_SERVICE_LOCATOR_HPP
#define YGOPEN_SERVICE_LOCATOR_HPP

#define USE_GL_CORE

namespace YGOpen
{

struct Service
{
#define SERVICE(klass, member) class klass; klass& member;
	SERVICE(Config, cfg)
	SERVICE(Immediate, imm)
#undef SERVICE
};

} // namespace YGOpen

#endif // YGOPEN_SERVICE_LOCATOR_HPP
