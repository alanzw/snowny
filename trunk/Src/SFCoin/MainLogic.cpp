#include "stdafx.h"
#include "GlobalVar.h"
#include "ContrlCenter.h"

//定义任意导出函数，为了让StreetFighter.exe加载SFCoin.dll
void ForLoad()
{

}

DWORD LogicThread(LPVOID pParam)
{
	//CPlayer players[2];
	TRACE(L"StreetFighter LogicThread\n");
	if (!InitializeLogic())
	{
		TRACE(L"StreetFighter InitializeLogic failed\n");
		return 0;
	}
	g_ContrlCenter.Init();
	g_ContrlCenter.Run();

	//UnInitializeLogic();
	return 0;
}

