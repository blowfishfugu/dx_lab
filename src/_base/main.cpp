#include "framework.h"
#include "ComInitializer.h"
#include "main.h"
#include "LabApp.h"

//#include <gpu_exports.cpp> //implicit by _common-Reference
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	
	LabApp app(_T("base window title"));

	if (!app.Init())
	{
		return FALSE;
	}

	return app.Run();
}






