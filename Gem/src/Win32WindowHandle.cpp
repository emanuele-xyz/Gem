#include <Gem\pch.h>
#include <Gem\Win32WindowHandle.h>
#include <Gem\Errors.h>
#include <Gem\Utils.h>

namespace Gem
{
	std::unique_ptr<WindowHandle> WindowHandle::Create(const std::string& title, int width, int height, bool is_windowed)
	{
		return std::make_unique<Win32WindowHandle>(title, width, height, is_windowed);
	}

	LRESULT Win32WindowClass::Procedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		LRESULT result = 0;
		if (msg == WM_CREATE)
		{
			// setup window user data and window procedure
			auto create{ reinterpret_cast<CREATESTRUCT*>(lparam) };
			auto window{ static_cast<Win32Window*>(create->lpCreateParams) };
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Win32Window::Procedure));
			result = window->Procedure(hwnd, msg, wparam, lparam);
		}
		else
		{
			result = DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return result;
	}
	Win32WindowClass::Win32WindowClass(const std::string& name)
		: m_name{ name }
	{
		/* NOTE
		* On Windows, if you go on Display Settings, you can change the display's scale. From what I have
		* understood, this changes the display's DPI.
		* This, if you do nothing, messes things up. Especially when specifying the window width and height.
		* To solve this issule we set the DPI awareness context to system aware.
		* Read here for more details: https://learn.microsoft.com/en-us/windows/win32/hidpi/dpi-awareness-context
		*/
		{
			// NOTE: this may fail if we call it multiple times
			BOOL ok{ SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE) };
			if (!ok) GemCrash("failed to set win32 process dpi awareness context");
		}

		WNDCLASSEX wc = { };
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = Procedure;
		//wc.cbClsExtra = ;
		//wc.cbWndExtra = ;
		wc.hInstance = GetModuleHandle(NULL);
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		//wc.hbrBackground = ;
		//wc.lpszMenuName = ;
		wc.lpszClassName = m_name.c_str();
		wc.hIconSm = LoadIcon(0, IDI_APPLICATION);

		ATOM result = RegisterClassEx(&wc);
		if (!result) GemCrash("failed to register win32 window class");
	}
	Win32WindowClass::~Win32WindowClass()
	{
		BOOL _ = UnregisterClassA(m_name.c_str(), GetModuleHandle(NULL));
		GemUnused(_);
	}

	LRESULT Win32Window::Procedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		auto window{ reinterpret_cast<Win32Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA)) };
		return window->OnMessage(hwnd, msg, wparam, lparam);
	}
	Win32Window::Win32Window(const std::string& class_name, const std::string& title, int width, int height, DWORD style)
		: m_handle{}
		, m_messages{}
	{
		LONG window_w{};
		LONG window_h{};
		{
			RECT rect{ 0, 0, width, height };
			BOOL ok{ AdjustWindowRect(&rect, style, FALSE) };
			if (!ok) GemCrash("failed to compute win32 window rectangle");
			window_w = rect.right - rect.left;
			window_h = rect.bottom - rect.top;
		}

		m_handle = CreateWindow(
			class_name.c_str(),
			title.c_str(),
			style | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT,
			window_w, window_h,
			0,
			0,
			GetModuleHandle(NULL),
			this
		);
		if (!m_handle) GemCrash("failed to create win32 window");
	}
	Win32Window::~Win32Window()
	{
		BOOL _ = DestroyWindow(m_handle);
		GemUnused(_);
	}
	LONG Win32Window::Width()
	{
		RECT rect{};
		BOOL result{ GetClientRect(m_handle, &rect) };
		GemAssert(result);
		return rect.right;
	}
	LONG Win32Window::Height()
	{
		RECT rect{};
		BOOL result{ GetClientRect(m_handle, &rect) };
		GemAssert(result);
		return rect.bottom;
	}
	void Win32Window::Update()
	{
		m_messages.clear();

		MSG message = {};
		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
	LRESULT Win32Window::OnMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		m_messages.push_back({ hwnd, msg, wparam, lparam });

		if (msg == WM_CLOSE)
		{
			/* NOTE
			* DefWindowProc would destroy the window.
			* We don't want that, since the user should exit the game using a menu.
			* Hence, we "catch" this message, instead of passing it to the default
			* window procedure
			*/
			return 0;
		}
		else
		{
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
	}

	Win32WindowHandle::Win32WindowHandle(const std::string& title, int width, int height, bool is_windowed)
		: m_window_class{ "GemWin32WindowClass" }
		, m_window{ m_window_class.Name(), title, width, height, is_windowed ? WS_OVERLAPPEDWINDOW : WS_POPUPWINDOW }
	{
	}
	void Win32WindowHandle::Update()
	{
		m_window.Update();

		m_events.clear();
		for (const auto& msg : m_window.GetMessages())
		{
			AddEvent(msg);
		}
	}
	void Win32WindowHandle::AddEvent(const Win32WindowMsg& msg)
	{
		switch (msg.msg)
		{
		case WM_CLOSE: { m_events.push_back(WindowEvent::Close()); } break;
		case WM_KILLFOCUS: { m_events.push_back(WindowEvent::FocusLost()); } break;
		case WM_SETFOCUS: { m_events.push_back(WindowEvent::FocusGained()); } break;
		default: { /* NOTE: skip */ } break;
		}
	}
}
