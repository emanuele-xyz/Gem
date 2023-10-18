#pragma once
#include <Gem\GemAPI.hpp>
#include <stdexcept>
#include <string_view>

#define GemCrash(msg) throw ::Gem::Crash((msg))
#define GemUnreachable() GemCrash("unreachable code path")
#define GemAssert(p) do { if (!(p)) GemCrash("assertion failed: " #p); } while (false)

namespace Gem
{
	class Crash : public std::runtime_error
	{
	public:
		GEM_API Crash(std::string_view);
	};
}
