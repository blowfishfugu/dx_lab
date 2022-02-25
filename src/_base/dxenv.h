#pragma once
#ifndef DXENV_H
#define DXENV_H
#include <wrl.h>
#include <d3d11_1.h>
#include <vector>
#pragma comment(lib,"DXGI.lib")
#pragma comment(lib,"D3D11.lib")

using namespace Microsoft::WRL;
class DxEnv
{
	
public:
	ComPtr<ID3D11Device1> _device;
	ComPtr<ID3D11DeviceContext1> _context;
public:
	bool EnumAdapters(std::vector<ComPtr<IDXGIAdapter1> >& adapters);
	void PrintAdapters(std::vector<ComPtr<IDXGIAdapter1>>& adapters);
	bool Init();

};

#endif