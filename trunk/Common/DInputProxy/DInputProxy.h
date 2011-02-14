#pragma once

// 键ID，支持与操作（主要用于DIPInputLock）
enum KeyID{
	DIP_START	= 1,		// 开始键
	DIP_A		= 1 << 1,	// 游戏中确认键：A
	DIP_B		= 1 << 2,	// 游戏中回退键：B
	DIP_UP		= 1 << 3,	// 上
	DIP_DOWN	= 1 << 4,	// 下
	DIP_LEFT	= 1 << 5,	// 左
	DIP_RIGHT	= 1 << 6,	// 右
	DIP_COIN	= 1 << 7,	// 我们需要的
	DIP_ALL		= 1 << 8	// 提供一个全键标识
};

// 玩家标识
enum PlayID{P1 = 0, P2};

struct KeyMapping
{
	PlayID	pid;			// 玩家标识
	DWORD	devid;			// 与该绑定的设备guid的前4字节
	BYTE	keyvalSTART;	// 开始键键值，如键盘A键：DIK_A，手柄2号键：1，下同
	BYTE	keyvalA;		// 确认键：A
	BYTE	keyvalB;		// 游戏中回退键：B
	BYTE	keyvalUP;		// 上
	BYTE	keyvalDOWN;		// 下
	BYTE	keyvalLEFT;		// 左
	BYTE	keyvalRIGHT;	// 右
	BYTE	keyvalCOIN;		// 我们需要的
};

typedef void (*KDPROC)(const PlayID &pid, const KeyID &kid);
struct DInputProxyConfig
{
	KeyMapping	KeyMapping[2];	// P1、P2
	KDPROC		KeyDownProc;
};

// 初始化，必做
/*
// 初始化实例
DInputProxyConfig config;
memset(&config, 0, sizeof(config));
config.KeyMapping[0].pid = P1;
config.KeyMapping[1].pid = P2;
config.KeyDownProc = KeyDownProc;

config.KeyMapping[0].devid = 0x6F1D2B61;// 键盘id
config.KeyMapping[0].keyvalA = 0x24;	// J
config.KeyMapping[0].keyvalB = 0x25;	// K
config.KeyMapping[0].keyvalCOIN = 0x04;	// 3
config.KeyMapping[0].keyvalDOWN = 0xD0;	// down
config.KeyMapping[0].keyvalLEFT = 0xCB;	// left
config.KeyMapping[0].keyvalRIGHT= 0xCD; // RIGHT
config.KeyMapping[0].keyvalSTART= 0x26; // L
config.KeyMapping[0].keyvalUP	= 0xC8; // up

config.KeyMapping[1].devid = 0x6101f3a70;// 手柄id
config.KeyMapping[1].keyvalCOIN = 0x05;	// 4
config.KeyMapping[1].keyvalA = 0x00;	// 1
config.KeyMapping[1].keyvalB = 0x01;	// 2
config.KeyMapping[1].keyvalSTART = 0x07;	// 7

InitDInputProxy(config);
*/
BOOL InitDInputProxy(const DInputProxyConfig &config);

// 模拟按键事件
// 如 1P 按键 start，DIPKeyDown(P1, DIP_START) 
void DIPInput(const PlayID &pid, const KeyID &kid);

// 锁定、解锁用户输入（模拟输入不受影响）
void DIPInputLock(const PlayID &pid, const KeyID &kid);
