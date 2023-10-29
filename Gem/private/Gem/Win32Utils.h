#pragma once
#include <Gem\Errors.h>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl/client.h>
namespace wrl = Microsoft::WRL;

#define GemWin32CrashIfFailed(result) GemAssert(!(FAILED((result))))

namespace Gem::Win32
{
	std::string GetErrorMessage(DWORD error);
}
