#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>

namespace Gem::Win32
{
	std::string GetErrorMessage(DWORD error);
}
