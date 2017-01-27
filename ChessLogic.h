#ifndef CHESS_LOGIC_H
#define CHESS_LOGIC_H

#include "ChessCommonDefs.h"

/* Initializes the board to the standard initial board */
void ChessLogicInitializeBoard(void);
/* Should be called to terminate the application (performs any necessary cleanups) */
void ChessLogicTerminate(void);

/* Settings Functionality */
void ChessLogicSetGameMode(GAME_MODE);
void ChessLogicSetDifficulty(GAME_DIFFICULTY);
void ChessLogicSetUserColor(PLAYER_COLOR);
void ChessLogicSetNextPlayer(PLAYER_COLOR);
/* resets settings to defaults */
void ChessLogicResetDefaultSettings();

/* Board Setup */
/* Board Location fields should be given as array offsets (0 to 7) */
MOVE_STATUS ChessLogicSetBoardPiece(BOARD_LOCATION, PLAYER_COLOR, CHESS_PIECE_TYPE);
/* Board Location fields should be given as array offsets (0 to 7) */
MOVE_STATUS ChessLogicRemoveBoardPiece(BOARD_LOCATION);
/* Removes all the pieces from the board */
void ChessLogicClearBoard();

/* Game Functionality */
/* can return MOVE_SUCCESSFUL, ILLEGAL_BOARD_INITIALIZATION, CHECK, CHECK_MATE, GAME_TIE */
MOVE_STATUS ChessLogicStartGame();
MOVE_STATUS ChessLogicPerformUserMove(GAME_MOVE);

/* Note: User is responsible to call ChessLogicFreeMovesList */
MOVE_STATUS ChessLogicGetMoves(BOARD_LOCATION, GAME_MOVE_PTR* outputParamHeadOfListOfMoves);

/* Note: User is responsible to call ChessLogicFreeMovesList */
void ChessLogicGetBestMoves(GAME_DIFFICULTY, GAME_MOVE_PTR* outputParamHeadOfListOfMoves);

void ChessLogicFreeMovesList(GAME_MOVE_PTR headOfMovesList);

int ChessLogicGetScore(GAME_DIFFICULTY, GAME_MOVE);
GAME_MOVE ChessLogicGetNextComputerMove();
MOVE_STATUS ChessLogicPerformNextComputerMove(GAME_MOVE);
void ChessLogicAdvanceNextPlayer();

/* Load-Save */
void ChessLogicLoadCompleteBoard(BOARD);

/* Getters */
BOARD* ChessLogicGetBoardReference();
void ChessLogicGetBoardCopy(BOARD*);
GAME_MODE ChessLogicGetGameMode();
GAME_DIFFICULTY ChessLogicGetDifficulty();
PLAYER_COLOR ChessLogicGetUserColor();
PLAYER_COLOR ChessLogicGetNextPlayer();
PLAYER_TYPE ChessLogicGetNextPlayerType();

#endif
#pragma once
