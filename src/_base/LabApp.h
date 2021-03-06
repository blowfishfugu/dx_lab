#pragma once
#ifndef LABAPP_H
#define LABAPP_H
#include "framework.h"

class LabApp final
{
	HWND m_mainwnd = NULL;
	HINSTANCE m_instance = NULL;
	std::string _exeFullPath;
	std::string _exeFolder;
	std::string _appTitle;
	ATOM MyRegisterClass();
	ATOM _registeredClass = 0;
	void MyUnregisterClass();
	

	Initializers::Com comInit;
	DxEnv dx;
public:
	LabApp(LPCTSTR appTitle);
	LabApp() = delete;
	LabApp(const LabApp&) = delete;
	LabApp& operator=(const LabApp&) = delete;
	~LabApp();
	[[nodiscard]] virtual bool Init();
	int Run();
	
	static LRESULT CALLBACK AppWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif