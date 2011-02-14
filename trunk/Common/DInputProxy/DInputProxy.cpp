#include "StdAfx.h"
#include "DInputProxy.h"
#include "MyDirectInput8.h"

struct DeviceInfo
{
	PlayID		pid;
	DWORD		devobj;				// 设备对象，与玩家一一绑定
	KeyID		emuKID;				// 即将模拟的按键
	KeyID		prevPressKID;		// 记录上次的按键状态，做重复过滤
	KeyID		lockedKey;			// 锁定的按键
};

// 设备对象与类型对
// 发现模拟器中会重复创建设备
std::map<LPVOID, DWORD> g_Dev_Obj_ID;

DeviceInfo g_DeviceTbl[2];
DInputProxyConfig g_Config;

typedef HRESULT (WINAPI *fpDirectInput8Create)(
						   HINSTANCE hinst,
						   DWORD dwVersion,
						   REFIID riidltf,
						   LPVOID * ppvOut,
						   LPUNKNOWN punkOuter
						   );
fpDirectInput8Create Real_DirectInput8Create = NULL;
HRESULT WINAPI Hook_DirectInput8Create(
						   HINSTANCE hinst,
						   DWORD dwVersion,
						   REFIID riidltf,
						   LPVOID * ppvOut,
						   LPUNKNOWN punkOuter
						   )
{
	// Let the world know we're working.
	MessageBeep(MB_ICONINFORMATION);

	ATLTRACE(TEXT("DInputProxy: DirectInput8Create called 0x%x.\n"), 
		Real_DirectInput8Create);

	HRESULT hr = 0;
	hr = Real_DirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);

	if(SUCCEEDED(hr))
		*ppvOut = new MyDirectInput8(reinterpret_cast<IDirectInput8*>(*ppvOut));

	return hr;
}

BOOL HookDirectInput8Create()
{
	HMODULE di8 = LoadLibrary(TEXT("DINPUT8.dll"));
	if (NULL == di8)
	{
		ATLTRACE("DInputProxy: di8 == NULL");
		return FALSE;
	}
	Real_DirectInput8Create = 
		(fpDirectInput8Create)GetProcAddress(di8, "DirectInput8Create");
	if (NULL == Real_DirectInput8Create)
	{
		ATLTRACE("DInputProxy: Real_DirectInput8Create == NULL");
		return FALSE;
	}

	return NO_ERROR == DetourTransactionBegin()
		&& NO_ERROR == DetourUpdateThread(GetCurrentThread())
		&& NO_ERROR == DetourAttach(&(PVOID&)Real_DirectInput8Create, Hook_DirectInput8Create)
		&& NO_ERROR == DetourTransactionCommit();
}

BOOL HookDInput()
{
	return HookDirectInput8Create();
}

BOOL InitDInputPrcxy( const DInputProxyConfig &config )
{
	// 避免用户漏掉设置
	assert(config.P1KeyMapping.pid == P1 && config.P2KeyMapping.pid == P2);
	g_Config = config;

	memset(&g_DeviceTbl, 0, sizeof(g_DeviceTbl));
	g_DeviceTbl[P1].pid = P1;
	g_DeviceTbl[P2].pid = P1;

	if (FALSE == HookDInput())
	{
		ATLTRACE("DInputProxy: FALSE == HookDInput()");
		return FALSE;
	}

	return TRUE;
}

void DIPInput( const PlayID &pid, const KeyID &kid )
{
	g_DeviceTbl[pid].emuKID = kid;
}

void DIPInputLock( const PlayID &pid, const KeyID &kid )
{
	g_DeviceTbl[pid].lockedKey = kid;
}
