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
	LPVOID		object;
	KeyState	keyState;

	BOOL		hasSimulateKeyID;
	BYTE		simulateKeyID;

	BOOL		isLocked;
	BYTE		lockedKey;
};

// 只关心手柄
std::vector<DeviceInfo> g_DeviceTbl;
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
	assert(id < g_DeviceTbl.size());

	g_DeviceTbl[id].hasSimulateKeyID = TRUE;
	g_DeviceTbl[id].simulateKeyID = keyID;
}

void DIHSetKDProc( KDPROC kdProc )
{
	assert(kdProc != NULL);
	g_KeydownProc = kdProc;
}

void DIHLockInput(BYTE id, BYTE key/*  = 0xFF*/)
{
	if (id < g_DeviceTbl.size())
	{
		g_DeviceTbl[id].isLocked = TRUE;
		g_DeviceTbl[id].lockedKey = key;
	}
}

void DIHUnlockInput(BYTE id)
{
	if (id < g_DeviceTbl.size())
	{
		g_DeviceTbl[id].isLocked = FALSE;
	}
}

// sf4设备的创建顺序是如此，而别的游戏可能不是
// 为了按键模拟能通用，且行为一致，作此排序
void CreateDevice(LPVOID obj, BOOL isKeyboard)
{
	if (isKeyboard)
	{
		DeviceInfo di = {0};
		di.object = obj;
		while (g_DeviceTbl.size() < 3)// 占位
		{
			g_DeviceTbl.push_back(di);
		}
	}
	else if (g_DeviceTbl.size() == 3)
	{
		if (g_DeviceTbl[0].object == g_DeviceTbl[2].object)
		{
			g_DeviceTbl[0].object = obj;
		}
		else if (g_DeviceTbl[1].object == g_DeviceTbl[2].object)
		{
			g_DeviceTbl[1].object = obj;
		}
	}
	else
	{
		DeviceInfo di = {0};
		di.object = obj;
		g_DeviceTbl.push_back(di);
	}
}

size_t GetDeviceID(LPVOID obj)
{
	for (size_t i = 0; i < g_DeviceTbl.size(); i ++)
	{
		if (g_DeviceTbl[i].object == obj)
		{
			return i;
		}
	}

	assert(0 == 1);
	return 0;
}

inline void UpdateKeyState( BYTE &newState, BYTE &oldState, BOOL factor) 
{
	newState = factor && oldState == 0 ? 1 : 0;
	oldState = factor ? 1 : 0;
}
inline void RealityKeyDown(LPVOID ths, DWORD size, LPVOID data)
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
		if (keys->lX != 0 || keys->lY != 0)
		{
			ATLTRACE(TEXT("DIH joystick direction:%d, %d"),
				keys->lX, keys->lY);
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
		// 键盘的投币处理
		if (size == sizeof(BYTE) * 0x100)
		{
			BYTE* kb = static_cast<BYTE*>(data);
			{
				KeyState ks = {0};
				DeviceInfo &di = g_DeviceTbl[0];

				UpdateKeyState(ks.coin, di.keyState.coin, (kb[DIK_3] & 0x80));

				// 键盘也是1P
				UpdateKeyState(ks.start, di.keyState.start, (kb[DIK_A] & 0x80));// 保留键盘A作为1P开始键
				UpdateKeyState(ks.up, di.keyState.up, (kb[DIK_UP] & 0x80));
				UpdateKeyState(ks.down, di.keyState.down, (kb[DIK_DOWN] & 0x80));
				UpdateKeyState(ks.left, di.keyState.left, (kb[DIK_LEFT] & 0x80));
				UpdateKeyState(ks.right, di.keyState.right, (kb[DIK_RIGHT] & 0x80));
				UpdateKeyState(ks.ok, di.keyState.ok, (kb[DIK_RETURN] & 0x80));

				g_KeydownProc(0, ks);
			}
			{
				KeyState ks = {0};
				DeviceInfo &di = g_DeviceTbl[1];

				UpdateKeyState(ks.coin, di.keyState.coin, (kb[DIK_4] & 0x80));
				g_KeydownProc(1, ks);
			}

			// 键盘也是1P
			if (g_DeviceTbl[0].isLocked)
			{
				if (g_DeviceTbl[0].lockedKey == 0xFF)
				{
					memset(data, 0, size);// 键盘锁定，只透几个关心的键
				}
				else if (g_DeviceTbl[0].lockedKey == IDK_START)
				{
					kb[DIK_ESCAPE] = 0;
				}
			}
		}
		else if (size == sizeof(DIJOYSTATE))
		{
			// 摇杆的开始处理
			KeyState ks = {0};
			size_t id = GetDeviceID(ths);
			DeviceInfo &di = g_DeviceTbl[id];
			DIJOYSTATE *joy = static_cast<DIJOYSTATE *>(data);

			UpdateKeyState(ks.start, di.keyState.start, joy->rgbButtons[0x7] & 0x80);
			UpdateKeyState(ks.up, di.keyState.up, joy->lY == -10000);
			UpdateKeyState(ks.down, di.keyState.down, joy->lY == 10000);
			UpdateKeyState(ks.left, di.keyState.left, joy->lX == -10000);
			UpdateKeyState(ks.right, di.keyState.right, joy->lX == 10000);
			UpdateKeyState(ks.ok, di.keyState.ok, joy->rgbButtons[0x0] & 0x80);

			g_KeydownProc((BYTE)id, ks);

			if (g_DeviceTbl[id].isLocked)
			{
				if (g_DeviceTbl[id].lockedKey == 0xFF)// 全键Lock
				{
					memset(joy->rgbButtons, 0, sizeof(joy->rgbButtons));
					joy->lX = 0;
					joy->lY = 0;
					joy->lZ = 0;
				}
				else
				{
					if (g_DeviceTbl[id].lockedKey == IDK_START)
					{
						joy->rgbButtons[0x7] = 0;
					}
				}
			}
		}
	}
}

inline void SimulateKeyDown(LPVOID ths, DWORD size, LPVOID data)
{
	DeviceInfo &di = g_DeviceTbl[GetDeviceID(ths)];

	if (di.hasSimulateKeyID)
	{
		ATLTRACE(TEXT("DIH Simulate key %x %d %d"),
			di.simulateKeyID, size, sizeof(DIJOYSTATE));
		if (size == sizeof(BYTE) * 0x100)	// 键盘模拟
		{
			BYTE* kb = static_cast<BYTE*>(data);
			kb[di.simulateKeyID] |= 0x80;
		}
		else if (size == sizeof(DIJOYSTATE)) // 手柄模拟
		{
			DIJOYSTATE *joy = static_cast<DIJOYSTATE *>(data);
			switch (di.simulateKeyID)
			{
			case IDK_START:
			case IDK_CONTINUE:
				joy->rgbButtons[0x7] |= 0x80;
				break;
			case IDK_BACKSPACE:
				joy->rgbButtons[0x1] |= 0x80;
				break;
			case IDK_OK:
				joy->rgbButtons[0x0] |= 0x80;
				break;
			case IDK_UP:
				joy->lY = -10000;
				break;
			case IDK_DOWN:
				joy->lY = 10000;
				break;
			case IDK_LEFT:
				joy->lX = -10000;
				break;
			case IDK_RIGHT:
				joy->lX = 10000;
				break;
			}
		}

		di.hasSimulateKeyID = FALSE;
	}
}

#ifdef _MANAGED
#pragma managed(pop)
#endif