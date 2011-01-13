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
	BOOL IsDead() const {return m_Status == PS_DEAD;}
	BOOL ClickStart();

	PlayerStatus GetStatus() {return m_Status;}
	void SetStatus(CPlayer::PlayerStatus val) { m_Status = val; }
	void RefreshStatus(GAMEFLOW gameFlow);

private:
	int QueryHP();
	void ChangeStatus(PlayerStatus newStatus) 
	{
		m_Status = newStatus;
		m_StatusChangeTime = GetTickCount();
	}

	DWORD m_dCoins;
	DWORD m_OldCoins;
	DWORD m_dId;
	PlayerStatus m_Status;
	DWORD		 m_StatusChangeTime;

	DWORD		 m_NoHpTime;

	static DWORD m_UnionCoins;
	static DWORD m_OldUnionCoins;
};