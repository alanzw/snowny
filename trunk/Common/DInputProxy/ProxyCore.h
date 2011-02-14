#pragma once

#include "DInputProxy.h"

struct DeviceInfo
{
	LPVOID		devobj;				// 设备对象，与玩家一一绑定
	KeyID		emuKID;				// 即将模拟的按键
	KeyID		prevPressKID;		// 记录上次的按键状态，做重复过滤
	KeyID		lockedKID;			// 锁定的按键
};

extern DeviceInfo g_DeviceTbl[2];
extern DInputProxyConfig g_ProxyConfig;

void RealityKeyDown(LPVOID devobj, DWORD size, LPVOID data);
void SimulateKeyDown(LPVOID devobj, DWORD size, LPVOID data);
void CreateDevice(LPVOID devobj, DWORD devid);