#include "dxenv.h"
#include "framework.h"


bool DxEnv::EnumAdapters(std::vector<ComPtr<IDXGIAdapter1>>& adapters)
{
	ComPtr<IDXGIFactory1> pFactory;
	CreateDXGIFactory1(__uuidof(IDXGIFactory1), &pFactory);
	if (!pFactory) { return adapters.size()>0; }
	
	IDXGIAdapter1* pAdapter=nullptr;
	UINT adapterAt = 0;
	while (pFactory->EnumAdapters1(adapterAt, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		adapters.emplace_back(pAdapter);
		adapterAt++;
	}
	//interesting, adapters are same, if gpu_exports.cpp is included
	return adapters.size() > 0;
}

void DxEnv::PrintAdapters(std::vector<ComPtr<IDXGIAdapter1>>& adapters)
{
	for (ComPtr<IDXGIAdapter1>& adapter : adapters)
	{
		DXGI_ADAPTER_DESC1 desc{};
		adapter->GetDesc1(&desc);

		std::wstring str;
		str.reserve(MAX_PATH);
		swprintf(str.data(), MAX_PATH, L"%s %d video:%I64d system:%I64d shared:%I64d\n",
			desc.Description, desc.DeviceId,
			desc.DedicatedVideoMemory, desc.DedicatedSystemMemory,
			desc.SharedSystemMemory);
		OutputDebugStringW(str.c_str());
	}
}

bool DxEnv::Init()
{
	std::vector<ComPtr<IDXGIAdapter1>> adapters;
	this->EnumAdapters(adapters);
	this->PrintAdapters(adapters);
	
	constexpr const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	constexpr const UINT featureLevelCount = sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL);

	ComPtr<ID3D11Device> pDevice = nullptr;
	D3D_FEATURE_LEVEL retFeatureLevel{};
	ComPtr<ID3D11DeviceContext> pImmediateContext = nullptr;
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0,
		0, featureLevels, featureLevelCount, D3D11_SDK_VERSION,
		&pDevice, &retFeatureLevel, &pImmediateContext);
	if (pDevice && pImmediateContext)
	{
		pDevice.As(&_device);
		pImmediateContext.As(&_context);
		OutputDebugStringA(std::to_string(retFeatureLevel).c_str());
		OutputDebugString(_T("\n"));
	}

	return 
		adapters.size()>0 
		&& _device!=nullptr 
		&& _context!=nullptr;
}

bool DxEnv::BuildSwapChain(HWND wnd)
{
	if (!_device) { return false; }
	ComPtr<IDXGIDevice1> pDevice;
	_device.As(&pDevice);
	if (!pDevice) { return false; }

	ComPtr<IDXGIAdapter> pAdapter;
	pDevice->GetAdapter(&pAdapter);
	if (!pAdapter) { return false; }

	ComPtr<IDXGIFactory1> pFactory;
	pAdapter->GetParent(__uuidof(pFactory), &pFactory);
	if (!pFactory) { return false; }

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	desc.Windowed = TRUE; // Sets the initial state of full-screen mode.
	desc.BufferCount = 2;
	desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SampleDesc.Count = 1;      //multisampling setting
	desc.SampleDesc.Quality = 0;    //vendor-specific flag
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.OutputWindow = wnd;
	pFactory->CreateSwapChain(_device.Get(), &desc, &_swapChain);
	if (!_swapChain) { return false; }


	return true;
}
