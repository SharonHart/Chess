#ifndef CHESS_GUI_PROTECTED_H
#define CHESS_GUI_PROTECTED_H

#include "ChessGenericUIInterface.h"
#include "GenericGraphicsFramework.h"
#include "ChessGUILayouts.h"
#include "ChessGUIResources.h"

/* "Protected Common Base" for sub modules GUISettings & GUIGame */

typedef enum
{
	LOAD_GAME_OPERATION,
	SAVE_GAME_OPERATION
} GAME_SLOTS_OPERATION;

typedef struct {
	int x;
	int y;
} GUI_BOARD_COORDINATES;

/* "Protected Data Memebers" */
extern CONTROL* g_gui_pCurrentWindowToDisplay;
extern CONTROL g_gui_gameBoardPanel;
extern GAME_SLOTS_OPERATION g_gui_gameSlotsOperation;
extern CONTROL g_gui_savedGamesMenuWindow;
extern CONTROL g_gui_boardButtonsControls[BOARD_SIZE][BOARD_SIZE];           /* Board Controls Array */

/* UI Interface Implementations */
void ChessGUIDisplayBoard(BOARD*);

/* Settings */
void ChessGUIDisplayStartMenu(BOARD*);
void ChessGUIPromptForSettings(void);

/* Game */
void ChessGUIStartGame(void);
void ChessGUIPromptForGameCmd(PLAYER_COLOR);
void ChessGUIDisplayCheck(void);
void ChessGUIDisplayCheckmate(PLAYER_COLOR);
void ChessGUIDisplayTiedGame(void);
void ChessGUIDisplayMove(GAME_MOVE);
void ChessGUIDisplayComputerMove(GAME_MOVE);
void ChessGUIDisplayMoves(GAME_MOVE_PTR movesList);

/* "Protected Methods" */

BOARD_LOCATION GuiCoordinatesToBoardPosition(GUI_BOARD_COORDINATES coordinates);

// Creates button listeners
void ChessGUICreateBoardPanel(CONTROL* pParentWindow);
void ChessGUIDisableBoardButtons(void);
// Updates the labels of pieces according to the updated board
void ChessGUIUpdateBoardPieceLabels(BOARD* pBoard);

// Slots window - Load or Save
void ChessGUICreateSavedGamesMenu(const char* titleFileName);

/* SETTINGS */
void ChessGUISettingsDisplayStartMenu(void);
void ChessGUIDisplayGameSettingsMenu(void);

/* GAME */
void ChessGUICreateGameWindow(void);
void ChessGUIDisplayGameWindow(void);
void ChessGUIDisplayMoves(GAME_MOVE_PTR movesList);
void ChessGUIRespondToMoveStatusErrors(MOVE_STATUS status);
void ChessGUIDisplayUpdatedBoard(void);
BOOL ChessGUIGameWindowOnPressCallback(CONTROL* pButton);

#endif