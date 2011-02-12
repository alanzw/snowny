// TVCUSACore.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <atltrace.h>
#include <tchar.h>
#include <string>

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BYTE  oldCode[7] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
BYTE  newCode[7] = {0xE8, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};

struct SceneInfo
{
	enum SceneID {
		SI_STARTING = 0,
		SI_TITLE,
		SI_MAINMENU,
		SI_SELECTCHAR,
		SI_GAME,
		SI_CONTINUE,
		SI_YOUWIN, SI_YOULOSE, SI_1PWIN, SI_2PWIN,
		SI_NOUSE} id;
	DWORD		guid;
	std::string desc;
};

SceneInfo g_SceneInfo[] = 
{
	{SceneInfo::SI_STARTING, 0X5e03f666, "game starting"},
	{SceneInfo::SI_NOUSE, 0Xe3cdfb01, "capcom"},
	{SceneInfo::SI_NOUSE, 0Xda8850a5, "8ing"},
	{SceneInfo::SI_TITLE, 0X5822ab8c, "title"},
	{SceneInfo::SI_MAINMENU, 0X2926ad7f, "mainmenu"},
	{SceneInfo::SI_SELECTCHAR, 0X98e466d7, "select char 1P"},
	{SceneInfo::SI_SELECTCHAR, 0X2447f5a7, "select char 2P"},
	{SceneInfo::SI_GAME, 0X595c783b, "ready"},
	{SceneInfo::SI_CONTINUE, 0Xead263beb, "continue"},
	{SceneInfo::SI_NOUSE, 0X930a86fe, "continue 9"},
	{SceneInfo::SI_NOUSE, 0Xf6a83ecc, "continue, 8"},
	{SceneInfo::SI_NOUSE, 0X285be2ea, "continue 7"},

	{SceneInfo::SI_YOUWIN, 0X211630d3, "you win"},
	{SceneInfo::SI_YOULOSE, 0X44550830, "you lose"},
	{SceneInfo::SI_1PWIN, 0Xe68d8bcd, "1P win"},
	{SceneInfo::SI_2PWIN, 0Xdeb3bfd3, "2P win"}
};

void DealWithTextureGUID(DWORD textureGuid)
{
	//ATLTRACE(TEXT("TVCUSACore textureGuid: 0X%x"), textureGuid);
	for (int i = 0; i < _countof(g_SceneInfo); i ++)
	{
		if (g_SceneInfo[i].guid == textureGuid)
		{
			ATLTRACE("TVCUSACore -----------------------------scene desc: %s", g_SceneInfo[i].desc.c_str());
		}
	}
}

__declspec(naked) void HookFunc()
{
	static DWORD textureGuid = 0;
	__asm
	{
		mov eax, [esp + 0x38C]
		mov textureGuid, eax
	}

	DealWithTextureGUID(textureGuid);

#ifdef _DEBUG
	__asm mov eax, 1
#else
	__asm mov eax, 0
#endif
	__asm ret
}

void DoHook(HMODULE module)
{
	BYTE *codeAddr = (BYTE *)module + 0x0000E414;
	DWORD oldProtected = 0;
	VirtualProtect(codeAddr, 7, PAGE_READWRITE, &oldProtected);
	{
		memcpy(oldCode, codeAddr, 7);
		DWORD newCodeAddr = (DWORD)HookFunc;
		newCodeAddr -= ((DWORD)codeAddr + 5);
		*(DWORD *)&newCode[1] = newCodeAddr;
		memcpy(codeAddr, newCode, 7);
	}
	VirtualProtect(codeAddr, 7, oldProtected, NULL);
}

DWORD HookThread(LPARAM lParam)
{
	HMODULE specialModuleHandle = NULL;
	BOOL isHooked = FALSE;
	while (1)
	{
		specialModuleHandle = GetModuleHandle(TEXT("DX9_V200.dll"));
		if (isHooked == FALSE && specialModuleHandle != NULL)
		{
			ATLTRACE(TEXT("TVCUSACore Hook proc 0x%x"), specialModuleHandle);
			//MessageBoxA(NULL, "attach", "attach", MB_OK);
			DoHook(specialModuleHandle);
		}
		if (isHooked == TRUE && specialModuleHandle == NULL)
		{
			ATLTRACE(TEXT("TVCUSACore Hook invalid 0x%x"), specialModuleHandle);
		}
		isHooked = specialModuleHandle == NULL ? FALSE : TRUE;
		Sleep(100);
	}

	return 0;
}

void Initialize(HMODULE hModule)
{
	TCHAR path[MAX_PATH] = {0}, fname[MAX_PATH] = {0};
	DWORD nameLen = GetModuleFileName(NULL, path, MAX_PATH);
	path[nameLen] = 0;
	_tsplitpath(path, NULL, NULL, fname, NULL);
	ATLTRACE(TEXT("TVCUSACore path:%s fname:%s"), path, fname);

	if (_tcscmp(fname, TEXT("Special")) != 0)
	{
		// ��ж�أ��߳̾���޷�CloseHandle
		::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, (LPVOID)hModule, 0, NULL);
	}
	else
	{
		::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HookThread, NULL, 0, NULL);
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
