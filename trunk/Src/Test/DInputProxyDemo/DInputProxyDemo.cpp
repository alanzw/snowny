// DInputProxyDemo.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "..\..\..\Common\DInputProxy\DInputProxy.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

void CreateTestDlg()
{

}

void Initialize(HMODULE hModule)
{
	TCHAR path[MAX_PATH] = {0}, fname[MAX_PATH] = {0};
	DWORD nameLen = GetModuleFileName(NULL, path, MAX_PATH);
	path[nameLen] = 0;
	_tsplitpath(path, NULL, NULL, fname, NULL);
	ATLTRACE(TEXT("DInputProxyDemo path:%s fname:%s"), path, fname);

	if (_tcscmp(fname, TEXT("Special")) != 0)
	{
		// 自卸载，线程句柄无法CloseHandle
		::CreateThread(
			NULL, 
			0,
			(LPTHREAD_START_ROUTINE)FreeLibrary, 
			(LPVOID)hModule, 
			0, 
			NULL);
	}
	else
	{
		InitDInputPrcxy();
		CreateTestDlg();
	}
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Initialize(hModule);
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

