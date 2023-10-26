#pragma once

#include <GemRuntime\Entry.h>

#if defined(_WIN32)
#include <Windows.h>

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	GemRuntime::Entry();
	return 0;
}
#endif
