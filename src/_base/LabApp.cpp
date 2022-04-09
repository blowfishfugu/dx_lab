#include "LabApp.h"
#include "resource.h"
#include "Ticker.h"

#include <filesystem>

namespace
{
	constexpr LPCTSTR szWindowClass{ _T("dx_lab_window") };
}
ATOM LabApp::MyRegisterClass()
{
	if (m_instance == NULL)
	{
		return NULL;
	}

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);

	HICON icon=LoadIcon(m_instance, MAKEINTRESOURCE(IDI_DXLAB));
	// If the icon is NULL, then use the first one found in the exe
	if (icon == NULL)
		icon = ExtractIcon(m_instance, _exeFullPath.c_str(), 0);

	wcex.style = CS_HREDRAW | CS_VREDRAW; //CS_OWNDC <- ?better
	wcex.lpfnWndProc = LabApp::AppWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_instance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIcon = icon;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	
	_registeredClass=RegisterClassEx(&wcex);

	return _registeredClass;
}

void LabApp::MyUnregisterClass()
{
	if (_registeredClass && m_instance )
	{
		UnregisterClass(szWindowClass, m_instance);
	}
}

LabApp::LabApp(LPCTSTR appTitle)
	:_appTitle{appTitle}
{
	if (m_instance == NULL)
	{
		m_instance = (HINSTANCE)GetModuleHandle(NULL);
	}

	this->_exeFullPath.resize(MAX_PATH,0);
	GetModuleFileName(NULL, _exeFullPath.data(), MAX_PATH);
	_exeFullPath.shrink_to_fit();
	const size_t slashIndex = _exeFullPath.find_last_of("\\");
	if (slashIndex != std::string::npos)
	{
		this->_exeFolder = _exeFullPath.substr(0, slashIndex);
	}
	MyRegisterClass();
}


LabApp::~LabApp()
{
	MyUnregisterClass();
}

bool LabApp::Init()
{
	if (m_instance == nullptr)
	{
		return false;
	}

	if (!dx.Init())
	{
		return false;
	}

	m_mainwnd = CreateWindow(szWindowClass, this->_appTitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, m_instance, nullptr);

	if (!m_mainwnd)
	{
		return false;
	}


	ShowWindow(m_mainwnd, SW_SHOW);
	UpdateWindow(m_mainwnd);
	
	if(!dx.BuildSwapChain(this->m_mainwnd))
	{
		return false;
	}
	
	return true;
}

//TODO: dx-init and dx-render into separate class and pass that as run-parameter
int LabApp::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	Ticker ticker(60);
	
	int f60 = 0;
	float t60 = 0;
	std::string title;
	title.resize(20, 0);
	const std::filesystem::path folder{ _exeFolder };


	std::filesystem::path pixelFile{ folder };
	pixelFile.concat(L"\\PixelShader.cso");
	
	ComPtr<ID3DBlob> pixelblob;
	ComPtr<ID3D11PixelShader> pPixelShader;
	D3DReadFileToBlob(pixelFile.c_str(), &pixelblob);
	dx._device->CreatePixelShader(pixelblob->GetBufferPointer(), pixelblob->GetBufferSize(), nullptr, &pPixelShader);

	std::filesystem::path vertexFile{ folder };
	vertexFile.concat(L"\\VertexShader.cso");

	ComPtr<ID3DBlob> vertexblob;
	ComPtr<ID3D11VertexShader> pVertexShader;
	D3DReadFileToBlob(vertexFile.c_str(), &vertexblob);
	dx._device->CreateVertexShader(vertexblob->GetBufferPointer(), vertexblob->GetBufferSize(), nullptr, &pVertexShader);


	//describing the vertex-structure
	ComPtr<ID3D11InputLayout> pInputLayout;
	
	//TODO: common vertex-components into separate header
	struct color //DXGI_FORMAT_R32G32B32_FLOAT
	{
		float r;
		float g;
		float b;
	};
	constexpr UINT colorSize = sizeof(color);
	
	struct colorU //DXGI_FORMAT_R8G8B8A8_UNORM
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};
	constexpr UINT colorUSize = sizeof(colorU);


	struct xypos //DXGI_FORMAT_R32G32_FLOAT
	{
		float x; 
		float y;
	};
	constexpr UINT positionSize = sizeof(xypos);
	
	struct vertex
	{
		xypos p;
		color c;
	};
	constexpr UINT vertexSize = sizeof(vertex);

	//TODO: template to build input_elements by component-type
	const D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		//Semantic in vertexshader:Position, 0, float2, slot,offset
		{"Position",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"Color",0,DXGI_FORMAT_R32G32B32_FLOAT,0,positionSize,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	std::vector<vertex> vertices{
		{ {0.0f,0.5f}, {1.0f,0.0f,0.0f} },
		{ {0.3f,-0.3f}, {0.0f,1.0f,0.0f} },
		{ {-0.7f,-0.7f}, {0.0f,0.0f,1.0f} },
		{ {0.8f,0.9f}, {1.0f,0.5f,0.0f} },
		{ {0.8f,-1.0f}, {0.0f,1.0f,0.5f} },
		{ {0.5f,0.0f}, {0.5f,0.0f,1.0f} }
	};
	std::vector<UINT> indices{0,1,2,3,4,5,0,5,1};

	dx._device->CreateInputLayout(inputDesc, static_cast<UINT>(std::size(inputDesc)),
		vertexblob->GetBufferPointer(), vertexblob->GetBufferSize(),
		&pInputLayout); //->IA


	ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bufferDesc{
		static_cast<UINT>(vertexSize*vertices.size()),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER,
		0u, 0u,
		vertexSize
	};

	D3D11_SUBRESOURCE_DATA bufferData = {};
	bufferData.pSysMem = vertices.data();

	dx._device->CreateBuffer(&bufferDesc, &bufferData, &pVertexBuffer);
	
	ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC indexDesc{
		static_cast<UINT>(sizeof(UINT)*indices.size()),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_INDEX_BUFFER,
		0u, 0u,
		static_cast<UINT>(sizeof(UINT))
	};
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	dx._device->CreateBuffer(&indexDesc, &indexData, &pIndexBuffer);


	struct ConstantMatrix
	{
		float matrix[4][4];
	};
	float angle = 0.5f;
	auto updateTransform = [](DxEnv& dx, float angle)
	{
		ConstantMatrix transform
		{
			dx._backbufferRatio* std::cos(angle), std::sin(angle), 0.0f, 0.0f,
			dx._backbufferRatio* -std::sin(angle), std::cos(angle), 0.0f, 0.0f,
			0.0f,0.0f,1.0f,0.0f,
			0.0f,0.0f,0.0f,1.0f
		};
		return transform;
	};
	
	D3D11_BUFFER_DESC constDesc;
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.Usage = D3D11_USAGE_DYNAMIC;
	constDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constDesc.MiscFlags = 0u;
	constDesc.ByteWidth = sizeof(ConstantMatrix);
	constDesc.StructureByteStride = 0u;

	UINT flip = 0;

	bool gotMessage = false;
	while (msg.message != WM_QUIT)
	{
		gotMessage = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);
		if (gotMessage)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			flip++;
			ticker.Tick();
			const float delta = ticker.GetDelta();
			const float elapsed = ticker.GetElapsed();
			const float frameDelta = ticker.GetFramesDelta();
			sprintf_s(title.data(), title.size(), "%.3f ms per 60", frameDelta);
			SetWindowText(m_mainwnd, title.c_str());
			
			//updateEntities 
			// TODO: a SceneMgr to add/remove current entities

			//{pausable
			//sMovement
			//sPhysics/sCollisions
			//sUpdateLifespans //->determine nexts to remove
			//}

			//sUpdateInputs //->different inputs per scene
			
			//->UpdateSubResource
			//sRenderSystem //->different renderoutput per scene
			
			float rb = std::sinf(elapsed*0.001f)*0.3f + 0.3f;
			const float rgba[4]{rb*0.5f,delta*0.001f,rb,1.0f};
			dx._context->ClearRenderTargetView(dx._renderTarget.Get(), rgba);

			
			
			//IA inputassembler //<-vertexbuffer + vertextopo + bufferlayout
			//VS vertexshader //hlsl :"Position" from IA
			//PS pixelshader //hlsl : returns "float4 color"
			//OM outputmerger //choose rendertarget
			//Draw
			if (pVertexBuffer)
			{
				constexpr const UINT strides[]{ vertexSize };
				constexpr const UINT offsets[]{ 0u };
				ID3D11Buffer* const buffers[]{ pVertexBuffer.Get() };
				static_assert(_countof(strides) == _countof(offsets));
				static_assert(_countof(strides) == _countof(buffers));

				constexpr const UINT bufferCount = static_cast<UINT>(_countof(strides));

				dx._context->IASetVertexBuffers(0u, bufferCount, buffers, strides, offsets);
				dx._context->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0u);
				
				//topology
				dx._context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				//bind vertex layout
				dx._context->IASetInputLayout(pInputLayout.Get());
				//||
				ConstantMatrix transform = updateTransform(dx, elapsed*0.001f);
				D3D11_SUBRESOURCE_DATA constData{};
				constData.pSysMem = &transform;
				
				ComPtr<ID3D11Buffer> pConstantBuffer;
				dx._device->CreateBuffer(&constDesc, &constData, &pConstantBuffer);


				dx._context->VSSetShader(pVertexShader.Get(), nullptr, 0u);
				dx._context->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
				
				//||
				dx._context->PSSetShader(pPixelShader.Get(), nullptr, 0u);
				//||
				//it can multipass on several targets.. but here only one
				dx._context->OMSetRenderTargets(1u, dx._renderTarget.GetAddressOf(), nullptr);
				//||
				//dx._context->Draw( static_cast<UINT>(vertices.size()/2), (vertices.size()/2)*(flip%2));
				dx._context->DrawIndexed(static_cast<UINT>(indices.size()), 0u, 0u);
			}
			//showframe
			dx._swapChain->Present(1, 0);
		}
	}
	this->OnLeave();
	return static_cast<int>(msg.wParam);
}

void LabApp::OnLeave() //TODO: OnLeave necessary? or RAII?
{
}

//TODO: trampoline-method to attach wndproc per window in wm_create
//TODO: capture keyboard and mouse
LRESULT LabApp::AppWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
