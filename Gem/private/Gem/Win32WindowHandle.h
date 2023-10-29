#pragma once
#include <Gem\WindowHandle.h>
#include <Gem\Win32Utils.h>
#include <string>
#include <vector>

namespace Gem
{
	class Win32WindowClass
	{
	public:
		static LRESULT CALLBACK Procedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	public:
		Win32WindowClass(const std::string& name);
		~Win32WindowClass();
	public:
		const std::string& Name() { return m_name; }
	private:
		std::string m_name;
	};

	struct Win32WindowMsg
	{
		HWND hwnd;
		UINT msg;
		WPARAM wparam;
		LPARAM lparam;
	};

	class Win32Window
	{
	public:
		static LRESULT CALLBACK Procedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	public:
		Win32Window(const std::string& class_name, const std::string& title, int width, int height, DWORD style);
		~Win32Window();
	public:
		HWND Handle() { return m_handle; }
		LONG Width();
		LONG Height();
		std::span<const Win32WindowMsg> GetMessages() const { return m_messages; }
	public:
		void Update();
	private:
		LRESULT OnMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	private:
		HWND m_handle;
		std::vector<Win32WindowMsg> m_messages;
	};

	class Win32WindowHandle : public WindowHandle
	{
	public:
		Win32WindowHandle(const std::string& title, int width, int height, bool is_windowed);
	public:
		void Update() override;
		std::span<const WindowEvent> GetEvents() override { return m_events; };
		void* GetRawHandle() override { return m_window.Handle(); }
	private:
		void AddEvent(const Win32WindowMsg& msg);
	private:
		Win32WindowClass m_window_class;
		Win32Window m_window;
		std::vector<WindowEvent> m_events;
	};
}
