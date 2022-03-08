#include "LabApp.h"
#include "resource.h"
#include "Ticker.h"

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
		icon = ExtractIcon(m_instance, _exePath.c_str(), 0);

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

	this->_exePath.reserve(MAX_PATH);
	GetModuleFileName(NULL, _exePath.data(), MAX_PATH);

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

int LabApp::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	Ticker getTime;
	
	int f60 = 0;
	float t60 = 0;
	std::string title;
	title.resize(20, 0);

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
			float delta = getTime.delta();
			float elapsed = getTime.elapsed();
			t60 += delta;
			f60++;
			if (f60 >= 60) {
				sprintf_s(title.data(), title.size(), "%.3f ms per 60", t60);
				f60 = 0; t60 = 0;
				SetWindowText(m_mainwnd, title.c_str());
			}
			//updateEntities //TODO: a SceneMgr to add/remove current entities

			//{pausable
			//sMovement
			//sPhysics/sCollisions
			//sUpdateLifespans //->determine nexts to remove
			//}

			//sUpdateInputs //->different inputs per scene

			//sRenderSystem //->different renderoutput per scene
			
			while (delta > 1.0f)
			{
				delta /= 10.0f;
			}
			const float rgba[4]{delta,0.1f,std::sinf(elapsed*0.001f)*0.3f+0.3f,1.0f};
			dx._context->ClearRenderTargetView(dx._renderTarget.Get(), rgba);

			//showframe
			dx._swapChain->Present(1, 0);
		}
	}
	this->OnLeave();
	return static_cast<int>(msg.wParam);
}

void LabApp::OnLeave()
{
}

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
