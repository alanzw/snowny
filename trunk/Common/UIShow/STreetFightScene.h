#pragma once
#include "fairy.h"

#define STANDBYBG 0
#define LOADINGBG 1
#define CREDIT	2
#define IPSTART	3
#define INSERTCOIN 4
#define LOADING 5

class CSTreetFightScene
{
public:
	CSTreetFightScene(HMODULE handle);
	~CSTreetFightScene(void);

	void TitileScene();


private:
	CFairy m_Fairy;
};
