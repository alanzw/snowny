// DirectInput-Hook.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "apihijack.h"
#include "DirectInput-Hook.h"
#include "MyDirectInput8.h"
#include <vector>
#include <assert.h>

#ifdef _MANAGED
#pragma managed(push, off)
#endif

HINSTANCE hDLL;

// Function pointer types.
typedef HRESULT (WINAPI *DirectInput8Create_t)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf,
										LPVOID * ppvOut, LPUNKNOWN punkOuter);


// Function prototypes.
HRESULT WINAPI MyDirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut,
									LPUNKNOWN punkOuter);

// Hook structure.
enum
{
	D3DFN_DirectInput8Create = 0
};

SDLLHook DInputHook = 
{
	"DINPUT8.DLL",
	false, NULL,		// Default hook disabled, NULL function pointer.
	{
		{ "DirectInput8Create", MyDirectInput8Create},
		{ NULL, NULL }
	}
};

// Hook function.
HRESULT WINAPI MyDirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut,
									LPUNKNOWN punkOuter)
{
	// Let the world know we're working.
	MessageBeep(MB_ICONINFORMATION);

	ATLTRACE(TEXT("DirectInput-Hook: MyDirectInput8Create called.\n"));

	DirectInput8Create_t old_func = reinterpret_cast<DirectInput8Create_t>(DInputHook.Functions[D3DFN_DirectInput8Create].OrigFn);
	HRESULT hr = old_func(hinst, dwVersion, riidltf, ppvOut, punkOuter);

	if(SUCCEEDED(hr))
		*ppvOut = new MyDirectInput8(reinterpret_cast<IDirectInput8*>(*ppvOut));

	return hr;
}

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)  // When initializing....
	{
		hDLL = hModule;
		// We don't need thread notifications for what we're doing.  Thus, get
		// rid of them, thereby eliminating some of the overhead of this DLL
		DisableThreadLibraryCalls( hModule );

		HookAPICalls(&DInputHook);
		
// 		char targetProcess[512];
// 		DWORD count = 512;
// 
// 		HKEY appKey = 0;
// 		if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\DirectInput-Hook", 0, KEY_QUERY_VALUE,
// 			&appKey))
// 		{
// 			if(ERROR_SUCCESS == RegQueryValueEx(appKey, 0, 0, 0, reinterpret_cast<BYTE*>(targetProcess), &count))
// 			{
// 				char process[512];
// 				GetModuleFileName(GetModuleHandle(0), process, sizeof(process));
// 				PathStripPath(process);
// 				
// 				if(_strnicmp(targetProcess, process, 512) == 0)
// 				{
// 					HookAPICalls(&DInputHook);
// 				}
// 			}
// 
// 			RegCloseKey(appKey);
// 		}
	}

	return TRUE;
}

// This segment must be defined as SHARED in the .DEF
#pragma data_seg (".HookSection")
// Shared instance for all processes.
HHOOK hHook = NULL;	
#pragma data_seg ()

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) 
{
    return CallNextHookEx( hHook, nCode, wParam, lParam); 
}

struct DeviceInfo
{
	LPVOID object;
	KeyState keyState;
	BOOL hasSimulateKeyID;
	BYTE simulateKeyID;
};

std::vector<DeviceInfo> g_DeviceS;
KDPROC	g_KeydownProc = NULL;

void InstallHook()
{
	OutputDebugString( "DINPUTHOOK hook installed.\n" );
	hHook = SetWindowsHookEx( WH_CBT, HookProc, hDLL, 0 ); 
}

void RemoveHook()
{
	OutputDebugString( "DINPUTHOOK hook removed.\n" );
	UnhookWindowsHookEx( hHook );
}

void DIHKeyDown(BYTE id, BYTE keyID)
{
	assert(id < g_DeviceS.size());

	g_DeviceS[id].hasSimulateKeyID = TRUE;
	g_DeviceS[id].simulateKeyID = keyID;
}

void DIHSetKDProc( KDPROC kdProc )
{
	assert(kdProc != NULL);
	g_KeydownProc = kdProc;
}

void CreateDevice(LPVOID obj)
{
	DeviceInfo di = {0};
	di.object = obj;
	
	g_DeviceS.push_back(di);
}

size_t GetDeviceID(LPVOID obj)
{
	for (size_t i = 0; i < g_DeviceS.size(); i ++)
	{
		if (g_DeviceS[i].object == obj)
		{
			return i;
		}
	}

	assert(0 == 1);
	return 0;
}

DeviceInfo& GetDevive(LPVOID obj)
{
	DeviceInfo &di = g_DeviceS[GetDeviceID(obj)];

	assert(di.object != NULL);
	return di;
}

void RealityKeyDown(LPVOID ths, DWORD size, LPVOID data)
{
#ifdef _DEBUG
	if (size == sizeof(BYTE) * 256)
	{
		BYTE* keys = static_cast<BYTE*>(data);

		for (DWORD i = 0; i < size; i ++)
		{
			if (keys[i] & 0x80)
			{
				ATLTRACE(TEXT("Keyboard Reality 0x%x"), i);
			}
		}
	}
	else if (size == sizeof(DIJOYSTATE))
	{
		DIJOYSTATE *keys = static_cast<DIJOYSTATE *>(data);
		for (DWORD i = 0; i < 32; i ++)
		{
			if (keys->rgbButtons[i] & 0x80)
			{
				ATLTRACE(TEXT("JoyStick Reality 0x%x"), i);
			}
		}
	}
	else if (size == sizeof(DIJOYSTATE2))
	{
		DIJOYSTATE2 *keys = static_cast<DIJOYSTATE2 *>(data);
		for (DWORD i = 0; i < sizeof(keys->rgbButtons); i ++)
		{
			if (keys->rgbButtons[i] & 0x80)
			{
				ATLTRACE(TEXT("JoyStick2 Reality 0x%x"), i);
			}
		}
	}
#endif

	if (g_KeydownProc != NULL)
	{
		DeviceInfo &di = GetDevive(ths);
		KeyState ks = di.keyState;
		if (size == sizeof(BYTE) * 0x100)
		{
			BYTE* kb = static_cast<BYTE*>(data);
			ks.coin = (kb[DIK_F2] & 0x80 && ks.coin == 0) ? 1 : 0;
			ks.start = (kb[DIK_A] & 0x80 && ks.start == 0) ? 1 : 0;
			ks.tocontinue=(kb[DIK_ESCAPE] & 0x80 && ks.tocontinue == 0) ? 1 : 0;

			di.keyState.coin = (kb[DIK_F2] & 0x80) ? 1 : 0;
			di.keyState.start = (kb[DIK_A] & 0x80) ? 1 : 0;
			di.keyState.tocontinue = (kb[DIK_ESCAPE] & 0x80) ? 1 : 0;

			g_KeydownProc((BYTE)GetDeviceID(ths), ks);

			kb[DIK_F2] = ks.coin == 1 ? kb[DIK_F2] : 0;
			kb[DIK_A] = ks.start == 1 ? kb[DIK_A] : 0;
			kb[DIK_ESCAPE] = ks.tocontinue == 1 ? kb[DIK_ESCAPE] : 0;
		}
	}
}

void SimulateKeyDown(LPVOID ths, DWORD size, LPVOID data)
{
	DeviceInfo &di = GetDevive(ths);

	if (di.hasSimulateKeyID)
	{
		if (size == sizeof(BYTE) * 0x100)	// ¼üÅÌÄ£Äâ
		{
			BYTE* kb = static_cast<BYTE*>(data);
			kb[di.simulateKeyID] |= 0x80;
			di.hasSimulateKeyID = FALSE;
		}
		else if (size == sizeof(DIJOYSTATE)) // ÊÖ±úÄ£Äâ
		{

		}
	}
}

#ifdef _MANAGED
#pragma managed(pop)
#endif