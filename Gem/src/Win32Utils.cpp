#include <Gem\pch.h>
#include <Gem\Win32Utils.h>

namespace Gem::Win32
{
	std::string GetErrorMessage(DWORD error)
	{
		if (error == ERROR_SUCCESS)
		{
			return std::string();
		}
		else
		{
			LPSTR buffer = nullptr;
			DWORD size = FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				error,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPSTR)&buffer,
				0,
				NULL
			);
			std::string message(buffer, size);
			LocalFree(buffer);
			return message;
		}
	}
}