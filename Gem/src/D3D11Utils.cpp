#include <Gem\pch.h>
#include <Gem\D3D11Utils.h>
#include <Gem\Utils.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib") // for IID_ID3D11ShaderReflection

namespace Gem::D3D11
{
	wrl::ComPtr<ID3D11Device1> CreateDevice()
	{
		HRESULT result{ S_OK };

		wrl::ComPtr<ID3D11Device> base_device{};
		wrl::ComPtr<ID3D11DeviceContext> base_context{};
		D3D_FEATURE_LEVEL feature_levels[]{ D3D_FEATURE_LEVEL_11_0 };
		UINT flags{};
		#if defined(GEM_DEBUG)
		flags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif
		result = D3D11CreateDevice(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			flags,
			feature_levels,
			GemArrayCount(feature_levels),
			D3D11_SDK_VERSION,
			base_device.ReleaseAndGetAddressOf(),
			0,
			base_context.ReleaseAndGetAddressOf()
		);
		if (FAILED(result)) GemCrash("failed to create D3D11 device and context");

		wrl::ComPtr<ID3D11Device1> device{};
		result = base_device->QueryInterface(device.ReleaseAndGetAddressOf());
		if (FAILED(result)) GemCrash("failed to get D3D11.1 device");

		return device;
	}

	wrl::ComPtr<ID3D11DeviceContext1> GetContext(ID3D11Device1* device)
	{
		wrl::ComPtr<ID3D11DeviceContext1> context{};
		device->GetImmediateContext1(context.ReleaseAndGetAddressOf());
		return context;
	}

	void BreakOnErrors(ID3D11Device1* device)
	{
		wrl::ComPtr<ID3D11Debug> debug{};
		device->QueryInterface(debug.ReleaseAndGetAddressOf());
		if (debug)
		{
			wrl::ComPtr<ID3D11InfoQueue> info_queue{};
			HRESULT result{ debug->QueryInterface(info_queue.ReleaseAndGetAddressOf()) };
			if (SUCCEEDED(result))
			{
				info_queue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
				info_queue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
			}
			else
			{
				// TODO: log
				GemCrash("failed to set break on d3d11 errors");
			}
		}
	}

	wrl::ComPtr<IDXGIAdapter3> GetDXGIAdapter3(ID3D11Device1* device)
	{
		HRESULT result{ S_OK };

		wrl::ComPtr<IDXGIDevice1> dxgi_device{};
		result = device->QueryInterface(dxgi_device.ReleaseAndGetAddressOf());
		if (FAILED(result)) GemCrash("failed to get DXGI device");

		wrl::ComPtr<IDXGIAdapter> dxgi_adapter{};
		result = dxgi_device->GetAdapter(dxgi_adapter.ReleaseAndGetAddressOf());
		if (FAILED(result)) GemCrash("failed to get DXGI adapter");

		wrl::ComPtr<IDXGIAdapter3> dxgi_adapter_3{};
		result = dxgi_adapter->QueryInterface(dxgi_adapter_3.ReleaseAndGetAddressOf());
		if (FAILED(result)) GemCrash("failed to get DXGI adapter version 3");

		return dxgi_adapter_3;
	}

	std::string GetAdapterDesc(IDXGIAdapter* adapter)
	{
		DXGI_ADAPTER_DESC desc{};
		HRESULT result{ adapter->GetDesc(&desc) };
		if (FAILED(result)) GemCrash("failed to get DXGI_ADAPTER_DESC");
		auto str{ Gem::ConvertWideStringToString(desc.Description) };
		return str;
	}

	SIZE_T GetAdapterVideoMemory(IDXGIAdapter* adapter)
	{
		DXGI_ADAPTER_DESC desc{};
		HRESULT result{ adapter->GetDesc(&desc) };
		if (FAILED(result)) GemCrash("failed to get DXGI_ADAPTER_DESC");
		return desc.DedicatedVideoMemory;
	}

	wrl::ComPtr<IDXGISwapChain1> CreateSwapChainForHWND(ID3D11Device1* device, IDXGIAdapter3* adapter, HWND window_handle)
	{
		HRESULT result{ S_OK };

		wrl::ComPtr<IDXGIFactory2> dxgi_factory{};
		result = adapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(dxgi_factory.ReleaseAndGetAddressOf()));
		if (FAILED(result)) GemCrash("failed to get DXGI factory from DXGI adapter");

		DXGI_SWAP_CHAIN_DESC1 swap_chain_desc{};
		swap_chain_desc.Width = 0; // use window w
		swap_chain_desc.Height = 0; // use window h
		swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_chain_desc.Stereo = FALSE;
		swap_chain_desc.SampleDesc = { 1, 0 }; // default sampler mode
		swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc.BufferCount = 2;
		swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;
		swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swap_chain_desc.Flags = 0;

		wrl::ComPtr<IDXGISwapChain1> swap_chain{};
		result = dxgi_factory->CreateSwapChainForHwnd(
			device,
			window_handle,
			&swap_chain_desc,
			NULL,
			NULL,
			swap_chain.ReleaseAndGetAddressOf()
		);
		if (FAILED(result)) GemCrash("failed to create DXGI swap chain for window");

		return swap_chain;
	}

	wrl::ComPtr<ID3D11Texture2D> GetBackbuffer(IDXGISwapChain1* swap_chain)
	{
		wrl::ComPtr<ID3D11Texture2D> buffer{};
		HRESULT result{ swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(buffer.ReleaseAndGetAddressOf())) };
		if (FAILED(result)) GemCrash("failed to get swap chain backbuffer");
		return buffer;
	}

	wrl::ComPtr<ID3D11RenderTargetView> CreateRTV(ID3D11Device1* device, ID3D11Texture2D* texture)
	{
		wrl::ComPtr<ID3D11RenderTargetView> rtv{};
		HRESULT result{ device->CreateRenderTargetView(texture, NULL, rtv.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash("failed to create render target view");
		return rtv;
	}

	wrl::ComPtr<ID3D11RenderTargetView> CreateBackbufferRTV(ID3D11Device1* device, IDXGISwapChain1* swap_chain)
	{
		auto back_buffer{ GetBackbuffer(swap_chain) };
		auto rtv{ CreateRTV(device, back_buffer.Get()) };
		return rtv;
	}

	wrl::ComPtr<ID3D11Texture2D> GetTexture2D(ID3D11RenderTargetView* rtv)
	{
		wrl::ComPtr<ID3D11Resource> resource{};
		rtv->GetResource(resource.ReleaseAndGetAddressOf());
		wrl::ComPtr<ID3D11Texture2D> texture{};
		HRESULT result{ resource->QueryInterface(texture.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash("failed to get render target 2D texture from view");
		return texture;
	}

	wrl::ComPtr<ID3D11Texture2D> CreateDepthStencilBuffer(ID3D11Device1* device, const D3D11_TEXTURE2D_DESC* desc)
	{
		wrl::ComPtr<ID3D11Texture2D> buffer{};
		HRESULT result{ device->CreateTexture2D(desc, NULL, buffer.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash("failed to create depth stencil buffer");
		return buffer;
	}

	wrl::ComPtr<ID3D11DepthStencilView> CreateDSV(ID3D11Device1* device, ID3D11Texture2D* depth_stencil_buffer)
	{
		wrl::ComPtr<ID3D11DepthStencilView> dsv{};
		HRESULT result{ device->CreateDepthStencilView(depth_stencil_buffer, NULL, dsv.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash("failed to create depth stencil view");
		return dsv;
	}

	wrl::ComPtr<ID3D11Texture2D> CreateDepthStencilBufferFromRTV(ID3D11Device1* device, ID3D11RenderTargetView* rtv, DXGI_FORMAT format)
	{
		auto texture{ GetTexture2D(rtv) };
		D3D11_TEXTURE2D_DESC desc{};
		texture->GetDesc(&desc);
		desc.Format = format;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		return CreateDepthStencilBuffer(device, &desc);
	}

	wrl::ComPtr<ID3D11DepthStencilView> CreateDSVFromRTV(ID3D11Device1* device, ID3D11RenderTargetView* rtv)
	{
		wrl::ComPtr<ID3D11Texture2D> texture{ GetTexture2D(rtv) };
		D3D11_TEXTURE2D_DESC depth_buffer_desc{};
		texture->GetDesc(&depth_buffer_desc);
		depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		wrl::ComPtr<ID3D11Texture2D> depth_stencil_buffer{ CreateDepthStencilBuffer(device, &depth_buffer_desc) };
		wrl::ComPtr<ID3D11DepthStencilView> dsv{ CreateDSV(device, depth_stencil_buffer.Get()) };
		return dsv;
	}

	wrl::ComPtr<ID3DBlob> ReadFileToBlob(const std::string& path)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter{};
		std::wstring wide_path{ converter.from_bytes(path) };

		wrl::ComPtr<ID3DBlob> blob{};
		HRESULT result{ D3DReadFileToBlob(wide_path.c_str(), blob.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash(std::format("failed to read file '{}'", path)); // TODO: use exeptions only in exceptional scenarios

		return blob;
	}

	wrl::ComPtr<ID3DBlob> CompileFromSrc(const std::string& src, const std::string& entry, const std::string& target, const std::string& path)
	{
		wrl::ComPtr<ID3DBlob> bytecode{};
		wrl::ComPtr<ID3DBlob> errors{};
		UINT flags{ 0 };
		#if defined(GEM_DEBUG)
		flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_OPTIMIZATION_LEVEL0;
		#elif defined(GEM_RELEASE)
		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
		#endif
		HRESULT result{ D3DCompile(src.c_str(), src.size(), NULL, NULL, NULL, entry.c_str(), target.c_str(), flags, 0, bytecode.ReleaseAndGetAddressOf(), errors.ReleaseAndGetAddressOf()) };
		if (FAILED(result))
		{
			auto error_msg{ static_cast<const char*>(errors->GetBufferPointer()) };
			GemCrash(std::format("failed to compile shader '{}' - target: {} - entry: {} - msg: {}", path, target, entry, error_msg));
		}
		return bytecode;
	}

	wrl::ComPtr<ID3DBlob> CompileFromFile(const std::string& path, const std::string& entry, const std::string& target)
	{
		wrl::ComPtr<ID3DBlob> src{ ReadFileToBlob(path) };
		wrl::ComPtr<ID3DBlob> bytecode{};
		wrl::ComPtr<ID3DBlob> errors{};
		HRESULT result{ D3DCompile(src->GetBufferPointer(), src->GetBufferSize(), NULL, NULL, NULL, entry.c_str(), target.c_str(), 0, 0, bytecode.ReleaseAndGetAddressOf(), errors.ReleaseAndGetAddressOf()) };
		if (FAILED(result))
		{
			auto error_msg{ static_cast<const char*>(errors->GetBufferPointer()) };
			GemCrash(std::format("failed to compile shader '{}' - target: {} - entry: {} - msg: {}", path, target, entry, error_msg));
		}
		return bytecode;
	}

	wrl::ComPtr<ID3D11VertexShader> CreateVertexShader(ID3D11Device1* device, ID3DBlob* blob, const std::string& path)
	{
		wrl::ComPtr<ID3D11VertexShader> shader{};
		HRESULT result{ device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, shader.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash(std::format("failed to create vertex shader '{}'", path));
		return shader;
	}

	wrl::ComPtr<ID3D11PixelShader> CreatePixelShader(ID3D11Device1* device, ID3DBlob* blob, const std::string& path)
	{
		wrl::ComPtr<ID3D11PixelShader> shader{};
		HRESULT result{ device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, shader.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash(std::format("failed to create pixel shader '{}'", path));
		return shader;
	}

	std::vector<InputElementDesc> GetInputElementDescFromBytecode(ID3DBlob* vs_blob, const std::string& vs_path)
	{
		HRESULT result{ S_OK };

		// Reflect shader info
		wrl::ComPtr<ID3D11ShaderReflection> reflector{};
		result = D3DReflect(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(reflector.ReleaseAndGetAddressOf()));
		if (FAILED(result)) GemCrash(std::format("{}: failed to get D3D reflector", vs_path));

		// Get shader info
		D3D11_SHADER_DESC shader_desc{};
		result = reflector->GetDesc(&shader_desc);
		if (FAILED(result)) GemCrash(std::format("{}: failed to get D3D11 shader description from reflector", vs_path));

		// Read input layout description from shader info
		std::vector<D3D11_INPUT_ELEMENT_DESC> input_element_desc{};
		for (unsigned i = 0; i < shader_desc.InputParameters; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC param_desc{};
			reflector->GetInputParameterDesc(i, &param_desc);

			// fill out input element desc
			D3D11_INPUT_ELEMENT_DESC element_desc;
			element_desc.SemanticName = param_desc.SemanticName;
			element_desc.SemanticIndex = param_desc.SemanticIndex;
			element_desc.InputSlot = 0;
			element_desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			element_desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			element_desc.InstanceDataStepRate = 0;

			// determine DXGI format
			if (param_desc.Mask == 1)
			{
				if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) element_desc.Format = DXGI_FORMAT_R32_UINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) element_desc.Format = DXGI_FORMAT_R32_SINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) element_desc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (param_desc.Mask <= 3)
			{
				if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) element_desc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) element_desc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) element_desc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (param_desc.Mask <= 7)
			{
				if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) element_desc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) element_desc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) element_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (param_desc.Mask <= 15)
			{
				if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) element_desc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) element_desc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) element_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			// save element desc
			input_element_desc.push_back(element_desc);
		}

		std::vector<InputElementDesc> out{};
		out.reserve(input_element_desc.size());
		for (const auto& desc : input_element_desc)
		{
			InputElementDesc description{};
			description.SemanticName = desc.SemanticName;
			description.SemanticIndex = desc.SemanticIndex;
			description.Format = desc.Format;
			description.InputSlot = desc.InputSlot;
			description.AlignedByteOffset = desc.AlignedByteOffset;
			description.InputSlotClass = desc.InputSlotClass;
			description.InstanceDataStepRate = desc.InstanceDataStepRate;
			out.push_back(description);
		}

		return out;
	}

	wrl::ComPtr<ID3D11InputLayout> CreateInputLayout(ID3D11Device* device, ID3DBlob* vs_blob, const std::string& vs_path, std::span<const InputElementDesc> descriptions)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> input_element_desc{};
		input_element_desc.reserve(descriptions.size());
		for (const auto& description : descriptions)
		{
			D3D11_INPUT_ELEMENT_DESC desc{};
			desc.SemanticName = description.SemanticName.c_str();
			desc.SemanticIndex = description.SemanticIndex;
			desc.Format = description.Format;
			desc.InputSlot = description.InputSlot;
			desc.AlignedByteOffset = description.AlignedByteOffset;
			desc.InputSlotClass = description.InputSlotClass;
			desc.InstanceDataStepRate = description.InstanceDataStepRate;
			input_element_desc.push_back(desc);
		}

		wrl::ComPtr<ID3D11InputLayout> input_layout{};
		auto vs_blob_buf{ vs_blob->GetBufferPointer() };
		auto vs_blob_size{ vs_blob->GetBufferSize() };
		HRESULT result{ device->CreateInputLayout(input_element_desc.data(), static_cast<UINT>(input_element_desc.size()), vs_blob_buf, vs_blob_size, input_layout.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash(std::format("{}: failed to create input layout", vs_path));
		return input_layout;
	}

	wrl::ComPtr<ID3D11InputLayout> CreateInputLayout(ID3D11Device1* device, ID3DBlob* vertex_shader_blob, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT count)
	{
		wrl::ComPtr<ID3D11InputLayout> input_layout{};
		auto vs_blob_buf{ vertex_shader_blob->GetBufferPointer() };
		auto vs_blob_size{ vertex_shader_blob->GetBufferSize() };
		HRESULT result{ device->CreateInputLayout(input_element_desc, count, vs_blob_buf, vs_blob_size, input_layout.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash("failed to create input layout");
		return input_layout;
	}

	wrl::ComPtr<ID3D11InputLayout> GetInputLayoutFromBytecode(ID3D11Device* device, ID3DBlob* vs_blob, const std::string& vs_path)
	{
		HRESULT result{ S_OK };

		// Reflect shader info
		wrl::ComPtr<ID3D11ShaderReflection> reflector{};
		result = D3DReflect(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(reflector.ReleaseAndGetAddressOf()));
		if (FAILED(result)) GemCrash(std::format("{}: failed to get D3D reflector", vs_path));

		// Get shader info
		D3D11_SHADER_DESC shader_desc{};
		result = reflector->GetDesc(&shader_desc);
		if (FAILED(result)) GemCrash(std::format("{}: failed to get D3D11 shader description from reflector", vs_path));

		// Read input layout description from shader info
		std::vector<D3D11_INPUT_ELEMENT_DESC> input_element_desc{};
		for (unsigned i = 0; i < shader_desc.InputParameters; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC param_desc{};
			reflector->GetInputParameterDesc(i, &param_desc);

			// fill out input element desc
			D3D11_INPUT_ELEMENT_DESC element_desc;
			element_desc.SemanticName = param_desc.SemanticName;
			element_desc.SemanticIndex = param_desc.SemanticIndex;
			element_desc.InputSlot = 0;
			element_desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			element_desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			element_desc.InstanceDataStepRate = 0;

			// determine DXGI format
			if (param_desc.Mask == 1)
			{
				if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) element_desc.Format = DXGI_FORMAT_R32_UINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) element_desc.Format = DXGI_FORMAT_R32_SINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) element_desc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (param_desc.Mask <= 3)
			{
				if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) element_desc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) element_desc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) element_desc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (param_desc.Mask <= 7)
			{
				if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) element_desc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) element_desc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) element_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (param_desc.Mask <= 15)
			{
				if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) element_desc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) element_desc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) element_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			// save element desc
			input_element_desc.push_back(element_desc);
		}

		wrl::ComPtr<ID3D11InputLayout> input_layout{};
		result = device->CreateInputLayout(input_element_desc.data(), static_cast<UINT>(input_element_desc.size()), vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), input_layout.ReleaseAndGetAddressOf());
		if (FAILED(result)) GemCrash(std::format("{}: failed to create input layout", vs_path));

		return input_layout;
	}

	wrl::ComPtr<ID3D11Buffer> CreateVertexBuffer(ID3D11Device1* device, UINT size_in_bytes, D3D11_USAGE usage, UINT access, const void* data)
	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = size_in_bytes;
		desc.Usage = usage;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = access;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vertex_buffer_data{};
		vertex_buffer_data.pSysMem = data;
		vertex_buffer_data.SysMemPitch = 0;
		vertex_buffer_data.SysMemSlicePitch = 0;

		wrl::ComPtr<ID3D11Buffer> buffer{};
		HRESULT result{ device->CreateBuffer(&desc, &vertex_buffer_data, buffer.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash("failed to create vertex buffer");

		return buffer;
	}

	wrl::ComPtr<ID3D11Buffer> CreateConstantBuffer(ID3D11Device1* device, UINT size_in_bytes)
	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = size_in_bytes;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//constant_buffer_desc.MiscFlags = ;
		//constant_buffer_desc.StructureByteStride = ;

		wrl::ComPtr<ID3D11Buffer> buffer{};
		HRESULT result{ device->CreateBuffer(&desc, NULL, buffer.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash("failed to create constant buffer");

		return buffer;
	}

	wrl::ComPtr<ID3D11SamplerState> CreateSamplerState(ID3D11Device1* device)
	{
		D3D11_SAMPLER_DESC desc{};
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		//sampler_desc.MipLODBias = ;
		//sampler_desc.MaxAnisotropy = ;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 0.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		//sampler_desc.MinLOD = ;
		//sampler_desc.MaxLOD = ;

		wrl::ComPtr<ID3D11SamplerState> sampler_state{};
		HRESULT result{ device->CreateSamplerState(&desc, sampler_state.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash("failed to create sampler state");

		return sampler_state;
	}

	wrl::ComPtr<ID3D11Texture2D> CreateTexture2D(ID3D11Device1* device, UINT w, UINT h, D3D11_USAGE usage, UINT bind_flags)
	{
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = w;
		desc.Height = h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc = { 1, 0 };
		desc.Usage = usage;
		desc.BindFlags = bind_flags;
		wrl::ComPtr<ID3D11Texture2D> texture{};
		HRESULT result{ device->CreateTexture2D(&desc, nullptr, texture.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash("failed to create texture");
		return texture;
	}

	wrl::ComPtr<ID3D11Texture2D> CreateTexture(ID3D11Device1* device, const std::string& path, UINT w, UINT h, const void* data, UINT pitch)
	{
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = w;
		desc.Height = h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc = { 1, 0 };
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		//texture_desc.CPUAccessFlags = ;
		//texture_desc.MiscFlags = ;
		D3D11_SUBRESOURCE_DATA subresource_data{};
		subresource_data.pSysMem = data;
		subresource_data.SysMemPitch = pitch;
		//texture_subresource_data.SysMemSlicePitch = ;

		wrl::ComPtr<ID3D11Texture2D> texture{};
		HRESULT result{ device->CreateTexture2D(&desc, &subresource_data, texture.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash(std::format("failed to create texture '{}'", path));

		return texture;
	}

	wrl::ComPtr<ID3D11Texture2D> CreateTexture2D(ID3D11Device1* device, const D3D11_TEXTURE2D_DESC* desc, const D3D11_SUBRESOURCE_DATA* data)
	{
		wrl::ComPtr<ID3D11Texture2D> texture{};
		HRESULT result{ device->CreateTexture2D(desc, data, texture.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash("failed to create texture");
		return texture;
	}

	wrl::ComPtr<ID3D11Texture2D> CreateTexture(ID3D11Device1* device, ID3D11Texture2D* old_texture)
	{
		D3D11_TEXTURE2D_DESC desc{};
		old_texture->GetDesc(&desc);
		wrl::ComPtr<ID3D11Texture2D> new_texture{ CreateTexture2D(device, &desc, nullptr) };
		return new_texture;
	}

	wrl::ComPtr<ID3D11Texture2D> CreateTexture2DFromRTV(ID3D11Device1* device, ID3D11RenderTargetView* rtv)
	{
		auto texture{ GetTexture2D(rtv) };
		return CreateTexture(device, texture.Get());

	}

	wrl::ComPtr<ID3D11ShaderResourceView> CreateSRV(ID3D11Device1* device, ID3D11Resource* resource)
	{
		wrl::ComPtr<ID3D11ShaderResourceView> srv{};
		HRESULT result{ device->CreateShaderResourceView(resource, NULL, srv.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash("failed to create texture shader resorce view");
		return srv;
	}

	wrl::ComPtr<ID3D11Buffer> CreateBuffer(ID3D11Device1* device, const D3D11_BUFFER_DESC* desc, const D3D11_SUBRESOURCE_DATA* data)
	{
		wrl::ComPtr<ID3D11Buffer> buffer{};
		HRESULT result{ device->CreateBuffer(desc, data, buffer.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash("failed to create D3D11 buffer");
		return buffer;
	}

	wrl::ComPtr<ID3D11RasterizerState> CreateRasterizerState(ID3D11Device1* device, D3D11_FILL_MODE fill_mode, D3D11_CULL_MODE cull_mode, BOOL front_counter_clockwise)
	{
		D3D11_RASTERIZER_DESC rasterizer_desc{};
		rasterizer_desc.FillMode = fill_mode;
		rasterizer_desc.CullMode = cull_mode;
		rasterizer_desc.FrontCounterClockwise = front_counter_clockwise;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = FALSE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;

		wrl::ComPtr<ID3D11RasterizerState> rasterizer_state{};
		HRESULT result{ device->CreateRasterizerState(&rasterizer_desc, rasterizer_state.ReleaseAndGetAddressOf()) };
		if (FAILED(result)) GemCrash("failed to create rasterizer state");

		return rasterizer_state;
	}

	wrl::ComPtr<ID3D11BlendState> CreateBlendState(ID3D11Device1* device, bool alpha_blend)
	{
		wrl::ComPtr<ID3D11BlendState> blend_state{};
		if (alpha_blend)
		{
			// NOTE: https://www.youtube.com/watch?v=wVkLeaWQOlQ
			D3D11_BLEND_DESC blend_desc{};
			blend_desc.AlphaToCoverageEnable = false;
			blend_desc.IndependentBlendEnable = false; // If FALSE, only the RenderTarget[0] members are used
			blend_desc.RenderTarget[0].BlendEnable = true;
			blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			HRESULT result{ device->CreateBlendState(&blend_desc, blend_state.ReleaseAndGetAddressOf()) };
			if (FAILED(result)) GemCrash("Failed to create blend state");
		}
		return blend_state;
	}

	UINT GetTexture2DWidth(ID3D11Texture2D* texture)
	{
		D3D11_TEXTURE2D_DESC desc{};
		texture->GetDesc(&desc);
		return desc.Width;
	}

	UINT GetTexture2DHeight(ID3D11Texture2D* texture)
	{
		D3D11_TEXTURE2D_DESC desc{};
		texture->GetDesc(&desc);
		return desc.Height;
	}
}
