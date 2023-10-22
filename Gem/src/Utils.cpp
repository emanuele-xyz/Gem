#include <Gem\pch.h>
#include <Gem\Utils.h>

namespace Gem
{
	std::string GetCurrentTimeString()
	{
		std::time_t rawtime;
		std::tm* timeinfo;
		char buffer[128];

		std::time(&rawtime);
		timeinfo = std::localtime(&rawtime);

		std::strftime(buffer, 80, "%Y_%m_%d_%H_%M_%S", timeinfo);
		std::puts(buffer);

		return buffer;
	}
}
