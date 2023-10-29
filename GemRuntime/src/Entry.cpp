#include <GemRuntime\Entry.h>
#include <Gem\Gem.h>

namespace GemRuntime
{
	void Entry()
	{
		//Gem::ShowErrorPopup("Hello from Gem Runtime!");

		auto window_handle{ Gem::WindowHandle::Create("Gem Editor", 1280, 720, true) };
		auto graphics_device{ Gem::GraphicsDevice::Create(Gem::GraphicsBackend::D3D11, window_handle.get()) };

		bool is_running{ true };
		while (is_running)
		{
			window_handle->Update();
			auto window_events{ window_handle->GetEvents() };
			for (const auto& window_event : window_events)
			{
				switch (window_event.type)
				{
				case Gem::WindowEventType::Close: { is_running = false; } break;
				case Gem::WindowEventType::FocusLost: { /* TODO: do something */ } break;
				case Gem::WindowEventType::FocusGained: { /* TODO: do something */ } break;
				default: {} break;
				}
			}

			graphics_device->Present(true);
		}

		// TODO: to be implemented
		// TODO: create and run runtime application
	}
}
