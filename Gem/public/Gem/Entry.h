#pragma once

extern void GemEntry();

#if defined(_WIN32)
#include <Windows.h>

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	GemEntry();
	return 0;
}
#endif
