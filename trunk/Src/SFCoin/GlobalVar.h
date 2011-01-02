#pragma once

typedef enum __gameflow{flow_unknow=0,flow_start=1,flow_titlemenu=2,flow_demo=3,flow_mainmenu=4,flow_setting=5,flow_selectchar=6,flow_game=7,flow_gameover=8,flow_continue=9}GAMEFLOW;

extern GAMEFLOW g_GameFlow;

BOOL InitializeLogic();
VOID UnInitializeLogic();
void GameFlowUpdate();