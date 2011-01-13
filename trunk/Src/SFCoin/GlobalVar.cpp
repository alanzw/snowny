#include "stdafx.h"
#include <Windows.h>
#include "GlobalVar.h"
#include "detours.h"

CString g_strTitle=L"Title\\title_tm.emz";
CString g_strMainmenu=L"mainmenu.emz";
CString g_strCSelect=L"cselect.ems.emz";
CString g_strStarVs=L"Game\\Menu\\ENG\\vs.ems.emz";
CString g_strDemo=L"Game\\Logo\\capcom";
GAMEFLOW g_GameFlow=flow_start;
CRITICAL_SECTION g_sc_gameflow;
CConfig  g_Config;

HANDLE (WINAPI* Real_CreateFileW)(
								  LPCWSTR lpFileName,
								  DWORD dwDesiredAccess,
								  DWORD dwShareMode,
								  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
								  DWORD dwCreationDisposition,
								  DWORD dwFlagsAndAttributes,
								  HANDLE hTemplateFile
								  )=CreateFileW;

HANDLE WINAPI Hook_CreateFileW(
							   LPCWSTR lpFileName,
							   DWORD dwDesiredAccess,
							   DWORD dwShareMode,
							   LPSECURITY_ATTRIBUTES lpSecurityAttributes,
							   DWORD dwCreationDisposition,
							   DWORD dwFlagsAndAttributes,
							   HANDLE hTemplateFile
							   )
{
	HANDLE h=Real_CreateFileW(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
	CString strFileName=lpFileName;
	if(strFileName.Find(g_strTitle,0)!=-1)
	{
		EnterCriticalSection(&g_sc_gameflow);
		g_GameFlow=flow_titlemenu;
		LeaveCriticalSection(&g_sc_gameflow);
		TRACE(L"StreetFighter ========================CreateFileW:%s",strFileName);
		TRACE(L"StreetFighter g_GameFlow: %d\n",g_GameFlow);
	}
	else if(strFileName.Find(g_strMainmenu,0)!=-1)
	{
		EnterCriticalSection(&g_sc_gameflow);
		g_GameFlow=flow_mainmenu;
		LeaveCriticalSection(&g_sc_gameflow);
		TRACE(L"StreetFighter ========================CreateFileW:%s",strFileName);
		TRACE(L"StreetFighter g_GameFlow: %d\n",g_GameFlow);
	}
	else if(strFileName.Find(g_strCSelect,0)!=-1)
	{
		EnterCriticalSection(&g_sc_gameflow);
		g_GameFlow=flow_selectchar;
		LeaveCriticalSection(&g_sc_gameflow);
		TRACE(L"StreetFighter ========================CreateFileW:%s",strFileName);
		TRACE(L"StreetFighter g_GameFlow: %d\n",g_GameFlow);
	}
	else if(strFileName.Find(g_strStarVs,0)!=-1)
	{
		EnterCriticalSection(&g_sc_gameflow);
		g_GameFlow=flow_game;
		LeaveCriticalSection(&g_sc_gameflow);
		TRACE(L"StreetFighter ========================CreateFileW:%s",strFileName);
		TRACE(L"StreetFighter g_GameFlow: %d\n",g_GameFlow);
	}
	else if(strFileName.Find(g_strDemo,0)!=-1)
	{
		EnterCriticalSection(&g_sc_gameflow);
		g_GameFlow=flow_demo;
		LeaveCriticalSection(&g_sc_gameflow);
		TRACE(L"StreetFighter ========================CreateFileW:%s",strFileName);
		TRACE(L"StreetFighter g_GameFlow: %d\n",g_GameFlow);
	}
	//TRACE(L"StreetFighter CreateFile:%s",strFileName);
	return h;
}
void GameFlowUpdate()
{
	PVOID pCunterTime=(PVOID)0x00a44ffc;
	try
	{
		if(g_GameFlow == flow_game && *(DWORD*)pCunterTime != 0)
		{
			EnterCriticalSection(&g_sc_gameflow);
			g_GameFlow=flow_continue;
			LeaveCriticalSection(&g_sc_gameflow);
			TRACE(L"StreetFighter g_GameFlow: %d\n",g_GameFlow);
		}
		else if (g_GameFlow != flow_continue) // 游戏在再次进入continue状态时才重置，帮助游戏清零，cxb
		{
			*(DWORD*)pCunterTime = 0;
			//TRACE(L"StreetFighter reset continue time: %d\n", *(DWORD*)pCunterTime);
		}

		static DWORD lastTime = GetTickCount();
		if (GetTickCount() - lastTime > 2000)
		{
			TRACE(L"SF4 refresh player status");
			DWORD getGlobalDataAddr = 0x0043A670,
				getPlayerDataAddr = 0x00439F80,
				globalDataAddr = 0,
				player0Addr = 0, player1Addr = 0;
			int player0HP = -1, player1HP = -1;
			
			__asm
			{
				pushad
				mov eax, getGlobalDataAddr
				call eax
				mov globalDataAddr, eax

				push 0
				mov ecx, globalDataAddr
				mov edx, getPlayerDataAddr
				call edx
				test eax, eax
 				jz jmp2
				mov player0Addr, eax
 				movsx edi, word ptr [eax + 51F6h]
 				mov player0HP, edi

				push 1
				mov ecx, globalDataAddr
 				mov edx, getPlayerDataAddr
 				call edx
   				test eax, eax
   				jz jmp2
 				mov player1Addr, eax
 				movsx edi, word ptr [eax + 51F6h]
				mov player1HP, edi
jmp2:
				popad
			}
			lastTime = GetTickCount();

			TRACE(L"SF4 refresh player status end");
			TRACE(TEXT("GlobalDataAddr 0x%x player0 0x%x %d player1 0x%x %d"), globalDataAddr, player0Addr, player0HP, player1Addr, player1HP);
		}
	}
	catch (CException* e)
	{
		e=e;
		TRACE(L"StreetFighter can't access 0x%08x\n",pCunterTime);
	}

}

BOOL InitializeLogic()
{
	//initilize
	TRACE(L"StreetFighter InitializeLogic %x\n", Real_CreateFileW);
	InitializeCriticalSection(&g_sc_gameflow);
	DWORD lRet;
	//DetourRestoreAfterWith();
	lRet=DetourTransactionBegin();
	lRet=DetourUpdateThread(GetCurrentThread());
	lRet = DetourAttach(&(PVOID&)Real_CreateFileW,Hook_CreateFileW);
	if(NO_ERROR == lRet)
	{
		TRACE(L"StreetFighter hook success\n");//g_DebugLog.WriteLog("%s:%x:%x attached",g_ApiNodes[i].cApiName,g_ApiNodes[i].dRealAddr,g_ApiNodes[i].dMineAddr);
	}
	else
	{
		DWORD error=GetLastError();
		TRACE(L"StreetFighter hook fail:%d\n",error);
		return FALSE;
	}
	lRet=DetourTransactionCommit();
	if(NO_ERROR == lRet)
	{
		TRACE(L"StreetFighter hook success %x\n", Real_CreateFileW);//g_DebugLog.WriteLog("%s:%x:%x attached",g_ApiNodes[i].cApiName,g_ApiNodes[i].dRealAddr,g_ApiNodes[i].dMineAddr);
	}
	else
	{
		DWORD error=GetLastError();
		TRACE(L"StreetFighter hook fail:%d\n",error);
		return FALSE;
	}
	return TRUE;
}
VOID UnInitializeLogic()
{
	//uninitilize
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)Real_CreateFileW,Hook_CreateFileW);
	DetourTransactionCommit();
	DeleteCriticalSection(&g_sc_gameflow);
}