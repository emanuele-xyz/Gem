#pragma once
#include <Gem\GraphicsDevice.h>
#include <Gem\Win32Utils.h>
#include <Gem\D3D11Utils.h>

namespace Gem
{
	class D3D11GraphicsDevice : public GraphicsDevice
	{
	public:
		D3D11GraphicsDevice(HWND window_handle);
	public:
		ID3D11Device1* Device() { return m_device.Get(); }
		ID3D11DeviceContext1* Context() { return m_context.Get(); }
		IDXGISwapChain1* Swapchain() { return m_swapchain.Get(); }
	public:
		void Resize(int width, int height) override;
		void Present(bool vsync) override;
	private:
		wrl::ComPtr<ID3D11Device1> m_device;
		wrl::ComPtr<ID3D11DeviceContext1> m_context;
		wrl::ComPtr<IDXGIAdapter3> m_adapter;
		std::string m_adapter_desc;
		size_t m_adapter_video_memory;
		wrl::ComPtr<IDXGISwapChain1> m_swapchain;
		wrl::ComPtr<ID3D11Texture2D> m_backbuffer;
		wrl::ComPtr<ID3D11RenderTargetView> m_backbuffer_rtv;
	};
}
