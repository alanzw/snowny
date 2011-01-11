#pragma once

#include "Config.h"

typedef enum __gameflow{
	flow_unknow=0,
	flow_start,
	flow_titlemenu,
	flow_demo,
	flow_mainmenu,
	flow_setting,
	flow_selectchar,
	flow_game,
	flow_gameover,
	flow_continue
}GAMEFLOW;

extern GAMEFLOW g_GameFlow;

extern CConfig  g_Config;

BOOL InitializeLogic();
VOID UnInitializeLogic();
void GameFlowUpdate();