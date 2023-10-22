#include <Gem\pch.h>
#include <Gem\Platform.h>
#include <Gem\Win32.h>

namespace Gem
{
	void ShowErrorPopup(const char* message) noexcept
	{
		auto ok{ MessageBox(NULL, message, "Error", MB_OK) };
		if (!ok)
		{
			// TODO: maybe log?
		}
	}
}
