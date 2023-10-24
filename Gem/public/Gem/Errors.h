#pragma once
#include <Gem\GemAPI.h>
#include <stdexcept>
#include <string_view>

#if defined(GEM_DEBUG)
#define GemCrash(msg) __debugbreak()
#elif defined(GEM_RELEASE)
#define GemCrash(msg) throw ::Gem::Crash(__FILE__, __LINE__, __FUNCTION__, (msg))
#endif

#define GemUnreachable() GemCrash("unreachable code path")
#define GemAssert(p) do { if (!(p)) GemCrash("assertion failed: " #p); } while (false)

namespace Gem
{
	class Crash : public std::runtime_error
	{
	public:
		GEM_API Crash(std::string_view file, int line, std::string_view function, std::string_view message);
	};
}
