#ifndef CHESS_FLOW_CONTROLLOER_H
#define CHESS_FLOW_CONTROLLOER_H

#include "ChessGenericUIInterface.h"
#include "CommonUtils.h"

typedef enum
{
	FLOW_STATE_SETTINGS = 0,
	FLOW_STATE_GAME,
	FLOW_STATE_NUM,
	FLOW_STATE_GAME_END,
} FLOW_STATE;

/* PUBLIC API */
void ChessControllerInit(INTERFACE_MODE interfaceMode);
void ChessControllerRun(void);
void ChessControllerTerminate(void);

FLOW_STATE ChessControllerGetFlowState(void);
MOVE_STATUS ChessControllerStartGame(void);
MOVE_STATUS ChessControllerPerformUserMove(GAME_MOVE gameMove);
BOOL ChessControllerLoadGame(const char* filename);
BOOL ChessControllerSaveGame(const char* filename);

#endif