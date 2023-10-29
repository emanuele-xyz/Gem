#include <Gem\pch.h>
#include <Gem\D3D11GraphicsDevice.h>
#include <Gem\Utils.h>

namespace Gem
{
	std::unique_ptr<GraphicsDevice> GraphicsDevice::Create(GraphicsBackend backend, WindowHandle* window)
	{
		switch (backend)
		{
		#if defined(_WIN32)
		case Gem::GraphicsBackend::D3D11: { return std::make_unique<D3D11GraphicsDevice>(static_cast<HWND>(window->GetRawHandle())); } break;
		#endif
		default: { GemUnreachable(); } break;
		}

		return nullptr;
	}

	D3D11GraphicsDevice::D3D11GraphicsDevice(HWND window_handle)
		: m_device{ D3D11::CreateDevice() }
		, m_context{ D3D11::GetContext(m_device.Get()) }
		, m_adapter{ D3D11::GetDXGIAdapter3(m_device.Get()) }
		, m_adapter_desc{ D3D11::GetAdapterDesc(m_adapter.Get()) }
		, m_adapter_video_memory{ D3D11::GetAdapterVideoMemory(m_adapter.Get()) }
		, m_swapchain{ D3D11::CreateSwapChainForHWND(m_device.Get(), m_adapter.Get(), window_handle) }
		, m_backbuffer{ D3D11::GetBackbuffer(m_swapchain.Get()) }
		, m_backbuffer_rtv{ D3D11::CreateRTV(m_device.Get(), m_backbuffer.Get()) }
	{
	}
	void D3D11GraphicsDevice::Resize(int width, int height)
	{
		GemUnused(width, height);

		m_context->OMSetRenderTargets(0, nullptr, nullptr);
		m_backbuffer_rtv.Reset(); // we must explicitly release here, or ResizeBuffers will fail!
		m_backbuffer.Reset();
		HRESULT result{ m_swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0) };
		if (FAILED(result)) GemCrash("failed to resize swapchain buffers"); // TODO: log warning but don't crash
		m_backbuffer = D3D11::GetBackbuffer(m_swapchain.Get());
		m_backbuffer_rtv = D3D11::CreateBackbufferRTV(m_device.Get(), m_swapchain.Get());
	}
	void D3D11GraphicsDevice::Present(bool vsync)
	{
		m_swapchain->Present(vsync ? 1 : 0, 0);
	}
}
