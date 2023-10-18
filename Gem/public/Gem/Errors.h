#pragma once
#include <Gem\GemAPI.h>
#include <stdexcept>
#include <string_view>

#define GemCrash(msg) throw ::Gem::Crash(__FILE__, __LINE__, (msg))
#define GemUnreachable() GemCrash("unreachable code path")
#define GemAssert(p) do { if (!(p)) GemCrash("assertion failed: " #p); } while (false)

namespace Gem
{
	class Crash : public std::runtime_error
	{
	public:
		GEM_API Crash(std::string_view file, int line, std::string_view message);
	};
}
