// SFCoin.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "SFCoin.h"
#include "shlwapi.h"
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


// CSFCoinApp

BEGIN_MESSAGE_MAP(CSFCoinApp, CWinApp)
END_MESSAGE_MAP()


// CSFCoinApp construction

CSFCoinApp::CSFCoinApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CSFCoinApp object

CSFCoinApp theApp;


// CSFCoinApp initialization
extern DWORD LogicThread(LPVOID pParam);
int MD5(const BYTE* data,int len,char strMD5[33]);
void GetDiskPhysicalSN(char pchDiskPhysicalSN[14]);

char destMD5[] = "00001111222233334444555566667777";

BOOL CSFCoinApp::InitInstance()
{
	CWinApp::InitInstance();

	//ªÒ»°”≤≈Ã–Ú¡–∫≈
	char cCode[17];
	char srcMD5[33] = {0};
	GetDiskPhysicalSN(cCode);
	MD5((BYTE *)cCode, strlen(cCode), srcMD5);
	if (strcmp(destMD5, srcMD5) != 0)
	{
		return TRUE;
	}
	
	char process[512];
	GetModuleFileNameA(GetModuleHandle(0), process, sizeof(process));
	PathStripPathA(process);
	const CHAR* targetProcess="StreetFighterIV.exe";
	TRACE("Process:%s\n",process);
	if(_strnicmp(targetProcess, process,512) == 0)
	{
		//MessageBox(NULL,L"he",NULL,MB_OK);
		DWORD id;
		TRACE("StreetFighter CreateThread\n");
		HANDLE h=::CreateThread(0,0,(LPTHREAD_START_ROUTINE)LogicThread,0,0,&id);
		//TRACE("StreetFighter Yes Handle:%08x\n",h);
	}
	else
	{
		TRACE("StreetFighter %s No\n",process);
		//return FALSE;
	}
	return TRUE;
}
