#pragma once
#include <Gem\GemAPI.h>
#include <memory>
#include <span>

namespace Gem
{
	enum class GEM_API WindowEventType
	{
		Close,
		FocusLost,
		FocusGained,
	};

	struct GEM_API WindowEvent
	{
		static WindowEvent Close() { return { .type = WindowEventType::Close }; }
		static WindowEvent FocusLost() { return { .type = WindowEventType::FocusLost }; }
		static WindowEvent FocusGained() { return { .type = WindowEventType::FocusGained }; }

		WindowEventType type;
	};

	class GEM_API WindowHandle
	{
	public:
		static std::unique_ptr<WindowHandle> Create(const std::string& title, int width, int height, bool is_windowed);
	public:
		virtual ~WindowHandle() {}
	public:
		virtual void Update() = 0;
		virtual std::span<const WindowEvent> GetEvents() = 0;
	};
}
