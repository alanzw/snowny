#include "stdafx.h"
#include "ProxyCore.h"
#include "DInputProxy.h"
struct DeviceInfo
{
	PlayID		pid;
	DWORD		devobj;				// 设备对象，与玩家一一绑定
	KeyID		emuKID;				// 即将模拟的按键
	KeyID		prevPressKID;		// 记录上次的按键状态，做重复过滤
	KeyID		lockedKey;			// 锁定的按键
};
extern std::map<LPVOID, DWORD> g_Dev_Obj_ID;
extern DeviceInfo g_DeviceTbl[2];
extern DInputProxyConfig g_Config;

void CreateDevice(LPVOID devobj, DWORD devid)
{
	ATLTRACE("DInputProxy: CreateDevice obj 0x%x id %x", devobj, devid);
	if (g_DeviceTbl[P1].devobj == NULL)
	{
		if (g_Config.P1KeyMapping.devid == devid)
		{
			g_DeviceTbl[P1].devobj = devobj;
		}
	}
	else if (g_DeviceTbl[P2].devobj == NULL)
	{
		if (g_Config.P2KeyMapping.devid == devid)
		{
			g_DeviceTbl[P2].devobj = devobj;
		}
	}
}

inline void UpdateKeyState( BYTE &newState, BYTE &oldState, BOOL factor) 
{
	newState = factor && oldState == 0 ? 1 : 0;
	oldState = factor ? 1 : 0;
}
inline void RealityKeyDown(LPVOID devobj, DWORD size, LPVOID data)
{
#ifdef _DEBUG
	if (size == sizeof(BYTE) * 256)
	{
		BYTE* keys = static_cast<BYTE*>(data);

		for (DWORD i = 0; i < size; i ++)
		{
			if (keys[i] & 0x80)
			{
				ATLTRACE(TEXT("Keyboard %x Reality 0x%x"), devobj, i);
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

	static KDPROC proc = g_Config.KeyDownProc;
	if (proc != NULL)
	{
		PlayID	pid = g_DeviceTbl[P1].devobj == devobj ? P1 :
			g_DeviceTbl[P2].devobj == devobj ? P2 : -1;
		if (pid == -1)
		{
			return;
		}

		KeyID	kid = 0;
		KeyID	prevKID = g_DeviceTbl[pid].prevPressKID;
		// 键盘处理
		if (size == sizeof(BYTE) * 0x100)
		{
			BYTE* kb = static_cast<BYTE*>(data);
			// 投币处理
			{
				static PlayID pids[] = {P1, P2};
				for (int i = 0; i < 2; i ++)
				{
					PlayID temp = pids[i];
					prevKID = g_DeviceTbl[].prevPressKID;
					if (kb[g_Config.KeyMapping[temp].keyvalCOIN] & 0x80)
					{
						if (prevKID & DIP_COIN == 0)
						{	
							proc(temp, DIP_COIN);
						}
						g_DeviceTbl[temp].prevPressKID |= DIP_COIN;
					}
					else
					{
						g_DeviceTbl[temp].prevPressKID &= DIP_COIN;
					}
				}
			}
			{
				KeyID kid;
				
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
				joy->rgbButtons[0x4] |= 0x80;
				break;
			case IDK_OK:
				joy->rgbButtons[0x3] |= 0x80;
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
