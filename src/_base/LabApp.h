#pragma once
#ifndef LABAPP_H
#define LABAPP_H
#include "framework.h"
#include "dxenv.h"

class LabApp
{
	HWND m_mainwnd = NULL;
	HINSTANCE m_instance = NULL;
	std::string _exePath;
	std::string _appTitle;
	
	Initializers::Com comInit;
	DxEnv dx;
	ATOM MyRegisterClass(HINSTANCE hInstance);
public:
	LabApp(LPCTSTR appTitle);
	virtual bool Init();
	int Run();
	virtual void OnLeave();
	static LRESULT CALLBACK AppWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif