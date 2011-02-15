#include "stdafx.h"
#include "ProxyCore.h"
#include "DInputProxy.h"

DeviceInfo g_DeviceTbl[2];
DInputProxyConfig g_ProxyConfig;

void CreateDevice(LPVOID devobj, DWORD devid)
{
	ATLTRACE("DInputProxy: CreateDevice obj 0x%x id %x", devobj, devid);

	for (int i = 0; i < 2; i ++)
	{
		PlayID pid  = (PlayID)i;
		if (g_DeviceTbl[pid].devobj == NULL)
		{
			if (g_ProxyConfig.KeyMapping[pid].devid == devid)
			{
				g_DeviceTbl[pid].devobj = devobj;
			}
			break;
		}
	}
}

inline bool UpdateKeyState(KeyID &newState, KeyID &oldState, KeyID kid, BOOL factor)
{
	if (factor)
	{
		if ((oldState & kid) == 0)
		{
			(int &)oldState |= (int)kid;
			(int &)newState |= (int)kid;
			return true;
		}
		else
		{
			(int &)newState &= (int)~kid;
		}
	}
	else
	{
		(int &)oldState &= (int)~kid;
	}

	return false;
}

void RealityKeyDown(LPVOID devobj, DWORD size, LPVOID data)
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

	static KDPROC proc = g_ProxyConfig.KeyDownProc;
	static BYTE P1CoinKV = g_ProxyConfig.KeyMapping[P1].keyvalCOIN;
	static BYTE P2CoinKV = g_ProxyConfig.KeyMapping[P2].keyvalCOIN;
	if (proc != NULL)
	{
		KeyID	kid = (KeyID)0;
		// 投币处理
		if (size == sizeof(BYTE) * 0x100)
		{
			BYTE* kb = static_cast<BYTE*>(data);
			if (UpdateKeyState(kid, g_DeviceTbl[P1].prevPressKID, DIP_COIN, kb[P1CoinKV] & 0x80))
			{
				proc(P1, DIP_COIN);
			}

			if (UpdateKeyState(kid, g_DeviceTbl[P2].prevPressKID, DIP_COIN, kb[P2CoinKV] & 0x80))
			{
				proc(P2, DIP_COIN);
			}
		}

		PlayID	pid = g_DeviceTbl[P1].devobj == devobj ? P1 :
			g_DeviceTbl[P2].devobj == devobj ? P2 : (PlayID)-1;
		if (pid == -1)
		{
			return;
		}
		KeyMapping &km = g_ProxyConfig.KeyMapping[pid];
		DeviceInfo &di = g_DeviceTbl[pid];
		// 键盘处理
		if (size == sizeof(BYTE) * 0x100)
		{
			BYTE* kb = static_cast<BYTE*>(data);
			{

				if (UpdateKeyState(kid, di.prevPressKID, DIP_START, kb[km.keyvalSTART] & 0x80) ||
					UpdateKeyState(kid, di.prevPressKID, DIP_A, kb[km.keyvalA] & 0x80) ||
					UpdateKeyState(kid, di.prevPressKID, DIP_B, kb[km.keyvalB] & 0x80) ||
					UpdateKeyState(kid, di.prevPressKID, DIP_UP, kb[km.keyvalUP] & 0x80) ||
					UpdateKeyState(kid, di.prevPressKID, DIP_DOWN, kb[km.keyvalDOWN] & 0x80) ||
					UpdateKeyState(kid, di.prevPressKID, DIP_LEFT, kb[km.keyvalLEFT] & 0x80) ||
					UpdateKeyState(kid, di.prevPressKID, DIP_RIGHT, kb[km.keyvalRIGHT] & 0x80))
				{
					proc(pid, kid);
				}
			}
			
			if (di.lockedKID != 0)
			{
				if (di.lockedKID == DIP_ALL)
				{
					memset(data, 0, size);
				}
				else
				{
					if (di.lockedKID & DIP_START)	kb[km.keyvalSTART]	= 0;
					if (di.lockedKID & DIP_A)		kb[km.keyvalA]		= 0;
					if (di.lockedKID & DIP_B)		kb[km.keyvalB]		= 0;
					if (di.lockedKID & DIP_UP)		kb[km.keyvalUP]		= 0;
					if (di.lockedKID & DIP_DOWN)	kb[km.keyvalDOWN]	= 0;
					if (di.lockedKID & DIP_LEFT)	kb[km.keyvalLEFT]	= 0;
					if (di.lockedKID & DIP_RIGHT)	kb[km.keyvalDOWN]	= 0;
				}
			}
		}
		else if (size == sizeof(DIJOYSTATE))
		{
			// 摇杆的开始处理
			DIJOYSTATE *joy = static_cast<DIJOYSTATE *>(data);

			if (UpdateKeyState(kid, di.prevPressKID, DIP_START, joy->rgbButtons[km.keyvalSTART] & 0x80) ||
				UpdateKeyState(kid, di.prevPressKID, DIP_A, joy->rgbButtons[km.keyvalA] & 0x80) ||
				UpdateKeyState(kid, di.prevPressKID, DIP_B, joy->rgbButtons[km.keyvalB] & 0x80) ||
				UpdateKeyState(kid, di.prevPressKID, DIP_UP, joy->lY == -10000) ||
				UpdateKeyState(kid, di.prevPressKID, DIP_DOWN, joy->lY == 10000) ||
				UpdateKeyState(kid, di.prevPressKID, DIP_LEFT, joy->lX == -10000) ||
				UpdateKeyState(kid, di.prevPressKID, DIP_RIGHT, joy->lX == 10000))
			{
				proc(pid, kid);
			}

			if (di.lockedKID != 0)
			{
				if (di.lockedKID == DIP_ALL)
				{
					memset(joy->rgbButtons, 0, sizeof(joy->rgbButtons));
					joy->lX = 0;
					joy->lY = 0;
					joy->lZ = 0;
				}
				else
				{
					if (di.lockedKID & DIP_START)	joy->rgbButtons[km.keyvalSTART]	= 0;
					if (di.lockedKID & DIP_A)		joy->rgbButtons[km.keyvalA]		= 0;
					if (di.lockedKID & DIP_B)		joy->rgbButtons[km.keyvalB]		= 0;
					if (di.lockedKID & DIP_UP)		joy->lY = 0;
					if (di.lockedKID & DIP_DOWN)	joy->lY = 0;
					if (di.lockedKID & DIP_LEFT)	joy->lY = 0;
					if (di.lockedKID & DIP_RIGHT)	joy->lY = 0;
				}
			}
		}
	}
}

void SimulateKeyDown(LPVOID devobj, DWORD size, LPVOID data)
{
	PlayID	pid = g_DeviceTbl[P1].devobj == devobj ? P1 :
		g_DeviceTbl[P2].devobj == devobj ? P2 : (PlayID)-1;
	if (pid == -1)
	{
		return;
	}
	KeyMapping &km = g_ProxyConfig.KeyMapping[pid];
	DeviceInfo &di = g_DeviceTbl[pid];

	if (di.emuKID != 0)
	{
		ATLTRACE(TEXT("DIH Simulate key %x %d %d"),	di.emuKID, size, sizeof(DIJOYSTATE));
		if (size == sizeof(BYTE) * 0x100)	// 键盘模拟
		{
			BYTE* kb = static_cast<BYTE*>(data);
			if (di.emuKID & DIP_START)		kb[km.keyvalSTART]	|= 0x80;
			else if (di.emuKID & DIP_A)		kb[km.keyvalA]		|= 0x80;
			else if (di.emuKID & DIP_B)		kb[km.keyvalB]		|= 0x80;
			else if (di.emuKID & DIP_UP)	kb[km.keyvalUP]		|= 0x80;
			else if (di.emuKID & DIP_DOWN)	kb[km.keyvalDOWN]	|= 0x80;
			else if (di.emuKID & DIP_LEFT)	kb[km.keyvalLEFT]	|= 0x80;
			else if (di.emuKID & DIP_RIGHT)	kb[km.keyvalRIGHT]	|= 0x80;
		}
		else if (size == sizeof(DIJOYSTATE)) // 手柄模拟
		{
			DIJOYSTATE *joy = static_cast<DIJOYSTATE *>(data);
			
			if (di.emuKID & DIP_START)		joy->rgbButtons[km.keyvalSTART] |= 0x80;
			else if (di.emuKID & DIP_A)		joy->rgbButtons[km.keyvalA]		|= 0x80;
			else if (di.emuKID & DIP_B)		joy->rgbButtons[km.keyvalB]		|= 0x80;
			else if (di.emuKID & DIP_UP)	joy->lY = -10000;
			else if (di.emuKID & DIP_DOWN)	joy->lY =  10000;
			else if (di.emuKID & DIP_LEFT)	joy->lX = -10000;
			else if (di.emuKID & DIP_RIGHT)	joy->lX =  10000;
		}

		di.emuKID = (KeyID)0;
	}
}
