#pragma once
#ifndef DXENV_H
#define DXENV_H
#include "framework.h"
#pragma comment(lib,"DXGI.lib")
#pragma comment(lib,"D3D11.lib")
#pragma comment(lib,"D3DCompiler.lib")
#include <d3dcompiler.h>

class DxEnv final
{
	
public:
	ComPtr<ID3D11Device1> _device;
	ComPtr<ID3D11DeviceContext1> _context;
	ComPtr<IDXGISwapChain1> _swapChain;

	ComPtr<ID3D11Texture2D> _backBuffer;
	CD3D11_TEXTURE2D_DESC _backBufferDesc;
	FLOAT _backbufferRatio = 0.0f;
	ComPtr<ID3D11RenderTargetView> _renderTarget;

	ComPtr<ID3D11Texture2D> _stencilBuffer;
	ComPtr<ID3D11DepthStencilView> _stencilView;

	D3D11_VIEWPORT _viewPortDesc{};
public:
	DxEnv() = default;
	DxEnv(const DxEnv&) = delete;
	DxEnv& operator=(const DxEnv&) = delete;
	~DxEnv() = default;

	bool EnumAdapters(std::vector<ComPtr<IDXGIAdapter1> >& adapters);
	void PrintAdapters(std::vector<ComPtr<IDXGIAdapter1>>& adapters);
	bool Init();
	bool BuildSwapChain(HWND wnd);
	void ConnectBuffersAndViews();
};

#endif