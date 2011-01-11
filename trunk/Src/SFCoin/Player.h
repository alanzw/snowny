#pragma once

#include <windows.h>
#include "GlobalVar.h"

class CPlayer
{
public:
	enum PlayerStatus{
		PS_IDLE = 0,
		PS_CLICKSTART,
		PS_STARTTING,
		PS_GAMING,
		PS_DEAD,
		PS_GAMEOVER};

	CPlayer(DWORD id=0);
	DWORD GetCoinNumber();
	void IncrementCoin();
	BOOL CoinsChanged();

	BOOL IsClickStart() const { return m_Status == PS_CLICKSTART; }
	BOOL ClickStart();

	PlayerStatus GetStatus() {return m_Status;}
	void RefreshStatus(GAMEFLOW gameFlow);

private:
	DWORD m_dCoins;
	DWORD m_OldCoins;
	DWORD m_dId;
	PlayerStatus m_Status;

	static DWORD m_UnionCoins;
	static DWORD m_OldUnionCoins;
};