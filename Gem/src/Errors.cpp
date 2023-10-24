#include <Gem\pch.h>
#include <Gem\Errors.h>

namespace Gem
{
	Crash::Crash(std::string_view file, int line, std::string_view function, std::string_view message) 
		: std::runtime_error(std::format("{}({}) crash in function {}: {}", file, line, function, message))
	{
	}
}
