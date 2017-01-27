#ifndef CHESS_GENERIC_UI_INTERFACE_H
#define CHESS_GENERIC_UI_INTERFACE_H

#include "ChessCommonDefs.h"
#include "ChessCommands.h"

/* Function Pointer typedefs */
/* General */
typedef void (*DISPLAY_BOARD_FUNC_P)(BOARD*);
typedef void (*TERMINATE_FUNC_P)(void);

/* Settings */
typedef void (*DISPLAY_START_MENU_FUNC_P)(BOARD*);
typedef void (*PROMPT_FOR_SETTINGS_FUNC_P)(void);

/* Game */
typedef void (*START_GAME_FUNC_P)(void);
typedef void (*PROMPT_FOR_GAME_CMD_FUNC_P)(PLAYER_COLOR);
typedef void (*DISPLAY_CHECK_FUNC_P)(void);
typedef void (*DISPLAY_CHECKMATE_FUNC_P)(PLAYER_COLOR);
typedef void (*DISPLAY_TIED_GAME_FUNC_P)(void);
typedef void (*DISPLAY_MOVE_FUNC_P)(GAME_MOVE);
typedef void (*DISPLAY_COMPUTER_MOVE_FUNC_P)(GAME_MOVE);
typedef void (*DISPLAY_MOVES_FUNC_P)(GAME_MOVE_PTR movesList);

typedef struct
{
	/* General */
	DISPLAY_BOARD_FUNC_P		   	ChessUIDisplayBoard;	

	/* Settings */
	DISPLAY_START_MENU_FUNC_P		ChessUIDisplayStartMenu;
	PROMPT_FOR_SETTINGS_FUNC_P		ChessUIPromptForSettings;	

	/* Game */
	START_GAME_FUNC_P				ChessUIStartGame;
	PROMPT_FOR_GAME_CMD_FUNC_P		ChessUIPromptForGameCmd;
	DISPLAY_CHECK_FUNC_P			ChessUIDisplayCheck;
	DISPLAY_CHECKMATE_FUNC_P		ChessUIDisplayCheckmate;
	DISPLAY_TIED_GAME_FUNC_P		ChessUIDisplayTiedGame;
	DISPLAY_MOVE_FUNC_P				ChessUIDisplayMove;
	DISPLAY_COMPUTER_MOVE_FUNC_P	ChessUIDisplayComputerMove;
	DISPLAY_MOVES_FUNC_P		   	ChessUIDisplayMoves;

	TERMINATE_FUNC_P		   		ChessUITerminate;
} CHESS_UI;

#endif