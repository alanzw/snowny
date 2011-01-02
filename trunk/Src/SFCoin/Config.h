#pragma once

class CConfig
{
public:
	CConfig(void);
public:
	~CConfig(void);
	BOOL ReadConfig(LPCWSTR fileName);
	LONG CoinMode;
	LONG Difficulty;
	LONG Rounds;
	LONG TimeLimit;
	LONG UnitCoin;
};
