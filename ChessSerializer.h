#ifndef CHESS_SERIALIZER_H
#define CHESS_SERIALIZER_H

#include "ChessCommonDefs.h"

/* PUBLIC API */

typedef struct
{
	GAME_MODE gameMode;
	GAME_DIFFICULTY gameDifficulty;		/* optional */
	PLAYER_COLOR userColor;				/* optional */
	PLAYER_COLOR currPlayer;
	BOARD board;
} ChessSerialization;

BOOL ChessSerialize(ChessSerialization dataIn, const char* filename);
BOOL ChessDeserialize(ChessSerialization *dataOut, const char* filename);

#endif