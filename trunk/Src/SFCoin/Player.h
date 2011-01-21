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
		PS_GAMEOVER
	};

	CPlayer(DWORD id=0);
	DWORD GetCoinNumber();
	void IncrementCoin();
	BOOL CoinsChanged();

	BOOL IsClickStart() const { return m_Status == PS_CLICKSTART; }
	BOOL IsDead() const {return m_Status == PS_DEAD;}
	BOOL IsIdle() const {return m_Status == PS_IDLE; }
	BOOL IsInGame() const {return m_Status == PS_GAMING && QueryHP() > 0;}
	BOOL ClickStart(DWORD *continueValAddr);

	PlayerStatus GetStatus() {return m_Status;}
	void SetStatus(CPlayer::PlayerStatus val) { m_Status = val; m_StatusChangeTime = GetTickCount();}
	void RefreshStatus(GAMEFLOW gameFlow);

private:
	int QueryHP() const;
	void AddCoins(int increCoins);

	DWORD m_dCoins;
	DWORD m_OldCoins;
	DWORD m_dId;
	PlayerStatus m_Status;
	DWORD		 m_StatusChangeTime;

	static DWORD m_UnionCoins;
	static DWORD m_OldUnionCoins;

	DWORD		*m_ContinueVal;
};