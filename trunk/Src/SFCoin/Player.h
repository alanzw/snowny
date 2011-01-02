#pragma once

#include <windows.h>

class CPlayer
{
public:
	CPlayer(DWORD id=0);
	DWORD GetCoinNumber(){return m_dCoins;}
	VOID SetCoinNumber(DWORD number){m_dCoins=number;}
private:
	DWORD m_dCoins;
	DWORD m_dId;
};