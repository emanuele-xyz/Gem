#pragma once
#include <Gem\WindowHandle.h>
#include <Gem\GemAPI.h>
#include <memory>

namespace Gem
{
	enum class GEM_API GraphicsBackend
	{
		#if defined(_WIN32)
		D3D11,
		#else
		#error No graphics backend supported for this platform
		#endif
	};

	class GEM_API GraphicsDevice
	{
	public:
		static std::unique_ptr<GraphicsDevice> Create(GraphicsBackend backend, WindowHandle* window);
	public:
		virtual ~GraphicsDevice() {};
	public:
		virtual void Resize(int width, int height) = 0;
		virtual void Present(bool vsync) = 0;
	};
}
