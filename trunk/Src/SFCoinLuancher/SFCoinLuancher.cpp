// SFCoinLuancher.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SFCoinLuancher.h"
#include "SFCoinLuancherDlg.h"
#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSFCoinLuancherApp

BEGIN_MESSAGE_MAP(CSFCoinLuancherApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSFCoinLuancherApp construction

CSFCoinLuancherApp::CSFCoinLuancherApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CSFCoinLuancherApp object

CSFCoinLuancherApp theApp;


// CSFCoinLuancherApp initialization
BOOL CopyFolder(LPCTSTR lpszFromPath,LPCTSTR lpszToPath)
{
	SHFILEOPSTRUCT FileOp; 
	ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));

	FileOp.fFlags = FOF_NOCONFIRMATION ; 
	FileOp.hNameMappings = NULL; 
	FileOp.hwnd = NULL; 
	FileOp.lpszProgressTitle = NULL; 
	FileOp.pFrom = lpszFromPath; 
	FileOp.pTo = lpszToPath; 
	FileOp.wFunc = FO_COPY; 

	return SHFileOperation(&FileOp) == 0;
}

BOOL DeleteFolder(LPCTSTR lpszPath) 
{ 
	SHFILEOPSTRUCT FileOp; 
	ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));

	FileOp.fFlags = FOF_NOCONFIRMATION; 
	FileOp.hNameMappings = NULL; 
	FileOp.hwnd = NULL; 
	FileOp.lpszProgressTitle = NULL; 
	FileOp.pFrom = lpszPath; 
	FileOp.pTo = NULL; 
	FileOp.wFunc = FO_DELETE; 

	return SHFileOperation(&FileOp) == 0;
}

BOOL CSFCoinLuancherApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

// 	bool ret = DeleteFolder(L"C:\\Documents and Settings\\Administrator\\My Documents\\CAPCOM\\STREETFIGHTERIV\0\0");
// 	ret =CopyFolder(L".\\SF4Con\\STREETFIGHTERIV\0\0",L"C:\\Documents and Settings\\Administrator\\My Documents\\CAPCOM");
// 
// 
// 	ret =DeleteFolder(L"C:\\Documents and Settings\\Administrator\\Local Settings\\Application Data\\Microsoft\\XLive\0\0");
// 	ret =CopyFolder(L".\\SF4Con\\XLive\0\0",L"C:\\Documents and Settings\\Administrator\\Local Settings\\Application Data\\Microsoft");


	// 关闭全屏
	TCHAR gameConfigFileName[] = TEXT("C:\\Documents and Settings\\Administrator\\Local Settings\\Application Data\\CAPCOM\\STREETFIGHTERIV\\config.ini");
	WritePrivateProfileString(TEXT("DISPLAY"), TEXT("FullScreen"), TEXT("OFF"), gameConfigFileName);
	CString resolution;
	resolution.Format(TEXT("%dx%d"), GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN));
	WritePrivateProfileString(TEXT("DISPLAY"), TEXT("Resolution"), (LPCTSTR)resolution, gameConfigFileName);


	HKEY appKey = 0;
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows"),
		0, KEY_SET_VALUE, &appKey))
	{
		char dll[MAX_PATH] = {0};
		GetModuleFileNameA(GetModuleHandle(NULL), dll, MAX_PATH);
		PathRemoveFileSpecA(dll);
 		strcat_s(dll, MAX_PATH, "\\SFCoin.dll");
 
 		if (ERROR_SUCCESS == RegSetValueExA(appKey, "AppInit_Dlls", 0, REG_SZ, (BYTE *)dll, strlen(dll) + 1))
		{
			STARTUPINFO si = {0};
			si.cb = sizeof(si);
			PROCESS_INFORMATION pi = {0};
			CreateProcessW(TEXT("SF4Launcher.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
			HWND targetWnd = NULL;
			while (targetWnd == NULL && WaitForSingleObject(pi.hProcess, 0) == WAIT_TIMEOUT)
			{
				Sleep(100);
				targetWnd = FindWindow(NULL, TEXT("《街头霸王IV》"));
			}
			while (targetWnd != NULL && ::IsWindow(targetWnd))
			{
				SendMessage(targetWnd, WM_COMMAND, 0x1, 0);
				Sleep(100);
			}

			Sleep(5000);

			dll[0] = 0;

			RegSetValueExA(appKey, "AppInit_Dlls", 0, REG_SZ, (BYTE *)dll, strlen(dll) + 1);
			RegCloseKey(appKey);
 		}
	}

//	CSFCoinLuancherDlg dlg;
// 	m_pMainWnd = &dlg;
// 	INT_PTR nResponse = dlg.DoModal();
// 	if (nResponse == IDOK)
// 	{
// 		// TODO: Place code here to handle when the dialog is
// 		//  dismissed with OK
// 	}
// 	else if (nResponse == IDCANCEL)
// 	{
// 		// TODO: Place code here to handle when the dialog is
// 		//  dismissed with Cancel
// 	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
