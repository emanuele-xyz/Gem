#pragma once
#include <Gem\Win32Utils.h>
#include <dxgi1_4.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>

namespace Gem::D3D11
{
	struct InputElementDesc
	{
		std::string SemanticName;
		UINT SemanticIndex;
		DXGI_FORMAT Format;
		UINT InputSlot;
		UINT AlignedByteOffset;
		D3D11_INPUT_CLASSIFICATION InputSlotClass;
		UINT InstanceDataStepRate;
	};

	// TODO: remove functions with silly parameters
	wrl::ComPtr<ID3D11Device1> CreateDevice();
	wrl::ComPtr<ID3D11DeviceContext1> GetContext(ID3D11Device1* device);
	void BreakOnErrors(ID3D11Device1* device);
	wrl::ComPtr<IDXGIAdapter3> GetDXGIAdapter3(ID3D11Device1* device);
	std::string GetAdapterDesc(IDXGIAdapter* adapter);
	SIZE_T GetAdapterVideoMemory(IDXGIAdapter* adapter);
	wrl::ComPtr<IDXGISwapChain1> CreateSwapChainForHWND(ID3D11Device1* device, IDXGIAdapter3* adapter, HWND window_handle);
	wrl::ComPtr<ID3D11Texture2D> GetBackbuffer(IDXGISwapChain1* swap_chain);
	wrl::ComPtr<ID3D11RenderTargetView> CreateRTV(ID3D11Device1* device, ID3D11Texture2D* texture);
	wrl::ComPtr<ID3D11RenderTargetView> CreateBackbufferRTV(ID3D11Device1* device, IDXGISwapChain1* swap_chain);
	wrl::ComPtr<ID3D11Texture2D> GetTexture2D(ID3D11RenderTargetView* rtv);
	wrl::ComPtr<ID3D11Texture2D> CreateDepthStencilBuffer(ID3D11Device1* device, const D3D11_TEXTURE2D_DESC* desc);
	wrl::ComPtr<ID3D11DepthStencilView> CreateDSV(ID3D11Device1* device, ID3D11Texture2D* depth_stencil_buffer);
	wrl::ComPtr<ID3D11Texture2D> CreateDepthStencilBufferFromRTV(ID3D11Device1* device, ID3D11RenderTargetView* rtv, DXGI_FORMAT format);
	wrl::ComPtr<ID3D11DepthStencilView> CreateDSVFromRTV(ID3D11Device1* device, ID3D11RenderTargetView* rtv);
	wrl::ComPtr<ID3DBlob> ReadFileToBlob(const std::string& path); // TODO: return option
	wrl::ComPtr<ID3DBlob> CompileFromSrc(const std::string& src, const std::string& entry, const std::string& target, const std::string& path);
	wrl::ComPtr<ID3DBlob> CompileFromFile(const std::string& path, const std::string& entry, const std::string& target);
	wrl::ComPtr<ID3D11VertexShader> CreateVertexShader(ID3D11Device1* device, ID3DBlob* blob, const std::string& path);
	wrl::ComPtr<ID3D11PixelShader> CreatePixelShader(ID3D11Device1* device, ID3DBlob* blob, const std::string& path);
	std::vector<InputElementDesc> GetInputElementDescFromBytecode(ID3DBlob* vs_blob, const std::string& vs_path);
	wrl::ComPtr<ID3D11InputLayout> CreateInputLayout(ID3D11Device* device, ID3DBlob* vs_blob, const std::string& vs_path, std::span<const InputElementDesc> desc);
	wrl::ComPtr<ID3D11InputLayout> CreateInputLayout(ID3D11Device1* device, ID3DBlob* vertex_shader_blob, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT count);
	wrl::ComPtr<ID3D11InputLayout> GetInputLayoutFromBytecode(ID3D11Device* device, ID3DBlob* vs_blob, const std::string& vs_path);
	wrl::ComPtr<ID3D11Buffer> CreateVertexBuffer(ID3D11Device1* device, UINT size_in_bytes, D3D11_USAGE usage, UINT access, const void* data);
	wrl::ComPtr<ID3D11Buffer> CreateConstantBuffer(ID3D11Device1* device, UINT size_in_bytes);
	wrl::ComPtr<ID3D11SamplerState> CreateSamplerState(ID3D11Device1* device);
	wrl::ComPtr<ID3D11Texture2D> CreateTexture2D(ID3D11Device1* device, UINT w, UINT h, D3D11_USAGE usage, UINT bind_flags);
	wrl::ComPtr<ID3D11Texture2D> CreateTexture(ID3D11Device1* device, const std::string& path, UINT w, UINT h, const void* data, UINT pitch);
	wrl::ComPtr<ID3D11Texture2D> CreateTexture2D(ID3D11Device1* device, const D3D11_TEXTURE2D_DESC* desc, const D3D11_SUBRESOURCE_DATA* data);
	wrl::ComPtr<ID3D11Texture2D> CreateTexture(ID3D11Device1* device, ID3D11Texture2D* old_texture);
	wrl::ComPtr<ID3D11Texture2D> CreateTexture2DFromRTV(ID3D11Device1* device, ID3D11RenderTargetView* rtv);
	wrl::ComPtr<ID3D11ShaderResourceView> CreateSRV(ID3D11Device1* device, ID3D11Resource* resource);
	wrl::ComPtr<ID3D11Buffer> CreateBuffer(ID3D11Device1* device, const D3D11_BUFFER_DESC* desc, const D3D11_SUBRESOURCE_DATA* data);
	wrl::ComPtr<ID3D11RasterizerState> CreateRasterizerState(ID3D11Device1* device, D3D11_FILL_MODE fill_mode, D3D11_CULL_MODE cull_mode, BOOL front_counter_clockwise);
	wrl::ComPtr<ID3D11BlendState> CreateBlendState(ID3D11Device1* device, bool alpha_blend);
	UINT GetTexture2DWidth(ID3D11Texture2D* texture);
	UINT GetTexture2DHeight(ID3D11Texture2D* texture);
}
