#include "stdafx.h"
#include "GlobalVar.h"
#include "ContrlCenter.h"

DWORD LogicThread(LPVOID pParam)
{
	//CPlayer players[2];
	MessageBox(NULL,L"he",NULL,MB_OK);
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

