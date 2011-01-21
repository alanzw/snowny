#pragma once

/*
Function: InstallHook
	Installs the hook. The name of the target process should be in
	HKEY_CURRENT_USER\Software\DInput-Hook before calling this function.

Remarks:
	The __stdcall convention is used so that the function is callable from VisualBasic.
	Even though __stdcall does some name-mangling, DInput-Hook.DEF exports the symbol
	'InstallHook' for this function, so you don't need to reference the mangled symbol.
*/
void InstallHook();

/*
Function: RemoveHook
	Uninstalls the hook - call this when you no longer want to search launched processes
	for the target process. Typically called after you've closed the target process and
	are exiting.

Remarks:
	The __stdcall convention is used so that the function is callable from VisualBasic.
	Even though __stdcall does some name-mangling, DInput-Hook.DEF exports the symbol
	'RemoveHook' for this function, so you don't need to reference the mangled symbol.
*/
void RemoveHook();

struct KeyState
{
	BYTE coin;
	BYTE start;
	BYTE up;
	BYTE down;
	BYTE left;
	BYTE right;
	BYTE ok;
};

#define IDK_START		0x1e	// 键盘-A 手柄-1号键
#define IDK_CONTINUE	0x01	// 键盘-esc 手柄-8号键
#define IDK_OK			0x1C	// 键盘-回车 手柄-8号键
#define IDK_BACKSPACE	0x0e	// 键盘-backspace 手柄-2号键
#define IDK_UP			0xC8	// 上
#define IDK_DOWN		0xd0	// 下
#define IDK_LEFT        0xCB	// 左
#define IDK_RIGHT		0xCD	// 右

// 模拟按键事件
// id:		设备标识，如1P、2P对应的设备
// keyID:	键盘DInput的键标识,如A:DIK_A
void DIHKeyDown(BYTE id, BYTE keyID);

typedef void (*KDPROC)(BYTE, KeyState&);
// 设置按键回调处理函数
void DIHSetKDProc(KDPROC kdProc);

// 锁定、解锁用户输入（模拟输入不受影响）
// 0xFF指全键盘或手柄按键，可以指定特定按键，一次只能指定一个
void DIHLockPlayerInput(BYTE id, BYTE key = 0xFF);
void DIHUnlockInput(BYTE id);
