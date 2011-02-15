// DInputProxyDemo.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "DInputProxyDemo.h"
#include "DemoDlg.h"
#include "..\..\..\Common\DInputProxy\DInputProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//


// CDInputProxyDemoApp

BEGIN_MESSAGE_MAP(CDInputProxyDemoApp, CWinApp)
END_MESSAGE_MAP()


// CDInputProxyDemoApp construction

CDInputProxyDemoApp::CDInputProxyDemoApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDInputProxyDemoApp object

CDInputProxyDemoApp theApp;

void KeyDownProc(const PlayID &pid, const KeyID &kid)
{
	TRACE("DInputProxyDemo player %d, key:%s", pid,
		kid & DIP_START ? "Start" :
		kid & DIP_A ? "A" :
		kid & DIP_B ? "B" :
		kid & DIP_UP ? "Up" :
		kid & DIP_DOWN ? "Down" :
		kid & DIP_LEFT ? "Left" :
		kid & DIP_RIGHT ? "Right" :
		kid & DIP_COIN ? "Coin" : "unknown");
	if (pid == P1)
	{
		static bool coined = false;
		
		// 投币后只锁定start
		if (kid == DIP_COIN)
		{
			coined = true;
		}
		// 已投币，start键通过转发生效
		else if (kid == DIP_START && coined)
		{
			DIPInput(P1, DIP_START);
			TRACE("DInputProxyDemo emu start");

			DIPInputLock(P1, DIP_START);
			TRACE("DInputProxyDemo unlock keys without start");
			coined = false;
		}
		// 未投币，全键锁定
		else if (kid == DIP_START && !coined)
		{
			DIPInputLock(P1, DIP_ALL);
			TRACE("DInputProxyDemo lock all");
		}
	}
}

void InitDInputProxy()
{
	DInputProxyConfig config;
	memset(&config, 0, sizeof(config));
	config.KeyMapping[0].pid = P1;
	config.KeyMapping[1].pid = P2;
	config.KeyDownProc = KeyDownProc;

	config.KeyMapping[0].devid = 0x6F1D2B61;// 键盘id
	config.KeyMapping[0].keyvalA = 0x24;	// J
	config.KeyMapping[0].keyvalB = 0x25;	// K
	config.KeyMapping[0].keyvalCOIN = 0x04;	// 3
	config.KeyMapping[0].keyvalDOWN = 0xD0;	// down
	config.KeyMapping[0].keyvalLEFT = 0xCB;	// left
	config.KeyMapping[0].keyvalRIGHT= 0xCD; // RIGHT
	config.KeyMapping[0].keyvalSTART= 0x26; // L
	config.KeyMapping[0].keyvalUP	= 0xC8; // up

	config.KeyMapping[1].devid = 0x6101f3a70;// 手柄id
	config.KeyMapping[1].keyvalCOIN = 0x05;	// 4
	config.KeyMapping[1].keyvalA = 0x00;	// 1
	config.KeyMapping[1].keyvalB = 0x01;	// 2
	config.KeyMapping[1].keyvalSTART = 0x07;	// 7

	InitDInputProxy(config);

	DIPInputLock(P1, DIP_ALL);
}

bool Initialize(HMODULE hModule)
{
	TCHAR path[MAX_PATH] = {0}, fname[MAX_PATH] = {0};
	DWORD nameLen = GetModuleFileName(NULL, path, MAX_PATH);
	path[nameLen] = 0;
	_tsplitpath(path, NULL, NULL, fname, NULL);
	ATLTRACE(TEXT("DInputProxyDemo path:%s fname:%s"), path, fname);

	if (_tcscmp(fname, TEXT("Special")) != 0)
	{
		// 自卸载，线程句柄无法CloseHandle
		::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, (LPVOID)hModule, 0, NULL);
		return false;
	}
	else
	{
		// TODO:
		InitDInputProxy();

		return true;
	}
}

// CDInputProxyDemoApp initialization

BOOL CDInputProxyDemoApp::InitInstance()
{
	CWinApp::InitInstance();
	TRACE("CDInputProxyDemoApp::InitInstance()");

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	Initialize(AfxGetInstanceHandle());

	return TRUE;
}

int CDInputProxyDemoApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

	TRACE("CDInputProxyDemoApp::ExitInstance()");

	return CWinApp::ExitInstance();
}
