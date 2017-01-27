#include "ChessCommonDefs.h"
#include "CommonUtils.h"
#include "GenericGraphicsFramework.h"
#include "ChessGUIProtected.h"
#include "InterfaceDefinitions.h"
#include "ChessLogic.h"
#include "ChessFlowController.h"

/* LOCAL DATA */

// TODO
// Optimization:
// Instead of statically allocated
// these can be allocated dynamically only ad-hoc
// this could help decrease the binary image memory footprint

/* Setting Menu Controls */
static CONTROL m_okButton;
static CONTROL m_cancelButton;

static CONTROL m_startMenuWindow;
static CONTROL m_mainMenuTitleLabel;
static CONTROL m_newGameButton;
static CONTROL m_loadGameButton;
static CONTROL m_quitSettingsButton;

static CONTROL m_newGameModeMenuWindow;
static CONTROL m_newGameModeMenuTitleLabel;
static CONTROL m_twoPlayerGameModeButton;
static CONTROL m_computerGameModeButton;

static CONTROL m_settingsMenuWindow;
static CONTROL m_settingsMenuTitleLabel;
static CONTROL m_boardSetupButton;
static CONTROL m_firstPlayerButton;
static CONTROL m_userColorButton;

static CONTROL m_boardSetupWindow;
static CONTROL m_boardSetupControlPanel;
static CONTROL m_pieceSelectionPanel;
static CONTROL m_clearButton;
static CONTROL m_blankPieceButton;
static CONTROL m_whiteKingButton;
static CONTROL m_whiteQueenButton;
static CONTROL m_whiteBishopButton;
static CONTROL m_whiteKnightButton;
static CONTROL m_whiteRookButton;
static CONTROL m_whitePawnButton;
static CONTROL m_blackKingButton;
static CONTROL m_blackQueenButton;
static CONTROL m_blackBishopButton;
static CONTROL m_blackKnightButton;
static CONTROL m_blackRookButton;
static CONTROL m_blackPawnButton;

static CONTROL m_firstPlayerMenuWindow;
static CONTROL m_firstPlayerMenuTitleLabel;
static CONTROL m_firstPlayerWhiteButton;
static CONTROL m_firstPlayerBlackButton;

static CONTROL m_userColorMenuWindow;
static CONTROL m_userColorMenuTitleLabel;
static CONTROL m_userColorWhiteButton;
static CONTROL m_userColorBlackButton;

static CONTROL m_difficultyMenuWindow;
static CONTROL m_difficultyMenuTitleLabel;
static CONTROL m_difficulty1Button;
static CONTROL m_difficulty2Button;
static CONTROL m_difficulty3Button;
static CONTROL m_difficulty4Button;
static CONTROL m_difficultyBestButton;


// Create Settings Menu's
void ChessGUICreateGenericSettingsMenu(CONTROL* pWindow, ON_PRESS_CALLBACK cb, 
													CONTROL* pTitle, const char* titleImageFileName,
													CONTROL** buttonsArr, 
													const char** buttonImagesArray, 
													const char** selectedButtonImagesArray, 
													int numButtons,
													BOOL includeConfirmation
													);
void ChessGUICreateStartMenu(void);

void ChessGUICreateGameModeSettingsMenu(void);
void ChessGUICreateGameSettingsMenu(void);
void ChessGUICreateTwoPlayerSettingsMenu(void);
void ChessGUICreateAISettingsMenu(void);
void ChessGUICreateBoardSetupSettingsMenu(void);
void ChessGUICreateFirstPlayerSettingsMenu(void);
void ChessGUICreateUserColorSettingsMenu(void);
void ChessGUICreateDifficultySettingsMenu(void);

/* Display Settings */
void ChessGUIDisplayGameModeSettingsMenu(void);
void ChessGUIDisplayLoadGameMenu(void);
void ChessGUIQuitGame(void);

void ChessGUIDisplayBoardSetupMenu(void);
void ChessGUIDisplayFirstPlayerSettingsMenu(void);
void ChessGUIDisplayUserColorSettingsMenu(void);
void ChessGUIDisplayDifficultySettingsMenu(void);

/* BUTTON CALL BACKS */
BOOL StartMenuButtonPressCallback(CONTROL* pButton);

BOOL GameModeMenuButtonPressCallback(CONTROL* pButton);
BOOL TwoPlayerSettingsMenuButtonPressCallback(CONTROL* pButton);
BOOL ComputerAISettingsMenuButtonPressCallback(CONTROL* pButton);
BOOL BoardSetupMenuButtonPressCallback(CONTROL* pButton);
BOOL FirstPlayerMenuButtonPressCallback(CONTROL* pButton);
BOOL UserColorMenuButtonPressCallback(CONTROL* pButton);
BOOL DifficultyMenuButtonPressCallback(CONTROL* pButton);


/* CREATE */ 

void ChessGUICreateGenericSettingsMenu(CONTROL* pWindow, ON_PRESS_CALLBACK cb, 
													CONTROL* pTitle, const char* titleImageFileName,
													CONTROL** buttonsArr, const char** buttonImagesArray, const char** selectedButtonImagesArray, int numButtons,
													BOOL includeConfirmation
													)
{
	int buttonX = MENU_LARGE_LABEL_X;
	int buttonY = MENU_LARGE_LABEL_1_Y;
	int buttonIndex;

	//FUNCTION_DEBUG_TRACE;
	assert(pWindow);
	assert(cb);
	assert(pTitle);
	assert(titleImageFileName);
	assert(buttonsArr);
	assert(buttonImagesArray);
	GenericGraphicsFrameworkCreateWindow(pWindow, WINDOW_WIDTH, WINDOW_HEIGHT, GUI_IMG_GAME_BACKGROUND, cb);
	GenericGraphicsFrameworkCreateLabel(pTitle, pWindow,
		MENU_LARGE_LABEL_X, MENU_LARGE_LABEL_0_Y, LARGE_LABEL_WIDTH, LARGE_LABEL_HEIGHT,
		titleImageFileName, TRANSPARENT_WHITE);

	for (buttonIndex = 0; buttonIndex < numButtons; buttonIndex++)
	{
		assert(buttonsArr[buttonIndex]);
		assert(buttonImagesArray[buttonIndex]);
		GenericGraphicsFrameworkCreateButton(
			buttonsArr[buttonIndex], pWindow,
			buttonX, buttonY, LARGE_LABEL_WIDTH, LARGE_LABEL_HEIGHT,
			buttonImagesArray[buttonIndex], selectedButtonImagesArray[buttonIndex], NO_TRANSPARENCY);
		buttonY += SETTINGS_LABEL_HEIGHT_WITH_SPACE;
	}
	if (false == includeConfirmation)
	{
		return;
	}
	GenericGraphicsFrameworkCreateButton(&m_okButton, pWindow,
		MENU_SMALL_LABEL_0_X, buttonY, LARGE_LABEL_WIDTH, LARGE_LABEL_HEIGHT,
		GUI_IMG_OK, NULL, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_cancelButton, pWindow,
		MENU_SMALL_LABEL_1_X, buttonY, LARGE_LABEL_WIDTH, LARGE_LABEL_HEIGHT,
		GUI_IMG_CANCEL, NULL, NO_TRANSPARENCY);
}



void ChessGUICreateStartMenu()
{
	int numButtons = 3;  // this is not a magic number
	CONTROL* buttonsArr[] = {&m_newGameButton, &m_loadGameButton, &m_quitSettingsButton};
	const char* buttonImagesArray[] = {GUI_IMG_NEW_GAME, GUI_IMG_LOAD_GAME, GUI_IMG_QUIT};
	const char* selectedButtonImagesArray[] = {NULL, NULL, NULL};
	BOOL includeConfirmation = false;

	FUNCTION_DEBUG_TRACE;

	ChessGUICreateGenericSettingsMenu(
		&m_startMenuWindow,
		StartMenuButtonPressCallback,
		&m_mainMenuTitleLabel, GUI_IMG_MAIN_MENU_TITLE,
		buttonsArr, buttonImagesArray, selectedButtonImagesArray, numButtons, includeConfirmation);
}


void ChessGUICreateGameModeSettingsMenu()
{
	int numButtons = 2;  // this is not a magic number
	CONTROL* buttonsArr[] = {&m_twoPlayerGameModeButton, &m_computerGameModeButton};
	const char* buttonImagesArray[] = {GUI_IMG_TWO_PLAYER_MODE, GUI_IMG_AI_MODE};
	const char* selectedButtonImagesArray[] = {GUI_IMG_TWO_PLAYER_MODE_SELECTED, GUI_IMG_AI_MODE_SELECTED};
	BOOL includeConfirmation = true;

	FUNCTION_DEBUG_TRACE;

	ChessGUICreateGenericSettingsMenu(
		&m_newGameModeMenuWindow,
		GameModeMenuButtonPressCallback,
		&m_newGameModeMenuTitleLabel, GUI_IMG_NEW_GAME_MODE_MENU_TITLE,
		buttonsArr, buttonImagesArray, selectedButtonImagesArray, numButtons, includeConfirmation);
}

void ChessGUICreateGameSettingsMenu(void)
{
	GAME_MODE gameMode = ChessLogicGetGameMode();
	switch (gameMode)
	{
	case GAME_MODE_TWO_PLAYERS:
		ChessGUICreateTwoPlayerSettingsMenu();
		break;

	case GAME_MODE_COMPUTER_AI:
		ChessGUICreateAISettingsMenu();
		break;

	default:
		PRINT_ERROR("Invalid Game Mode: %d", gameMode);
	}

}

void ChessGUICreateTwoPlayerSettingsMenu(void)
{
	int numButtons = 2;  // this is not a magic number
	CONTROL* buttonsArr[] = {&m_boardSetupButton, &m_firstPlayerButton};
	const char* buttonImagesArray[] = {GUI_IMG_BOARD_SETUP, GUI_IMG_FIRST_PLAYER};
	const char* selectedButtonImagesArray[] = {NULL, NULL};
	BOOL includeConfirmation = true;

	FUNCTION_DEBUG_TRACE;

	ChessGUICreateGenericSettingsMenu(
		&m_settingsMenuWindow,
		TwoPlayerSettingsMenuButtonPressCallback,
		&m_settingsMenuTitleLabel, GUI_IMG_TWO_PLAYER_MENU_TITLE,
		buttonsArr, buttonImagesArray, selectedButtonImagesArray, numButtons, includeConfirmation);
}

void ChessGUICreateAISettingsMenu(void)
{
	int numButtons = 3;  // this is not a magic number
	CONTROL* buttonsArr[] = {&m_boardSetupButton, &m_firstPlayerButton, &m_userColorButton};
	const char* buttonImagesArray[] = {GUI_IMG_BOARD_SETUP, GUI_IMG_FIRST_PLAYER, GUI_IMG_USER_COLOR};
	const char* selectedButtonImagesArray[] = {NULL, NULL, NULL};
	BOOL includeConfirmation = true;

	FUNCTION_DEBUG_TRACE;

	ChessGUICreateGenericSettingsMenu(
		&m_settingsMenuWindow,
		ComputerAISettingsMenuButtonPressCallback,
		&m_settingsMenuTitleLabel, GUI_IMG_AI_SETTINGS_MENU_TITLE,
		buttonsArr, buttonImagesArray, selectedButtonImagesArray, numButtons, includeConfirmation);
}

void ChessGUICreateBoardSetupSettingsMenu(void)
{
	FUNCTION_DEBUG_TRACE;
	GenericGraphicsFrameworkCreateWindow(&m_boardSetupWindow, WINDOW_WIDTH, WINDOW_HEIGHT, GUI_IMG_GAME_BACKGROUND, BoardSetupMenuButtonPressCallback);

	GenericGraphicsFrameworkCreatePanel(&m_boardSetupControlPanel, &m_boardSetupWindow, GAME_MENU_PANEL_X, GAME_MENU_PANEL_Y, GAME_MENU_PANEL_WIDTH, GAME_MENU_PANEL_HEIGHT, NULL);

	GenericGraphicsFrameworkCreateButton(&m_clearButton, &m_boardSetupControlPanel,
		BOARD_SETUP_LABEL_MID_X, BOARD_SETUP_LABEL_7_Y, 138, 55, 
		GUI_IMG_BOARD_SETUP_CLEAR, NULL, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_okButton, &m_boardSetupControlPanel,
		BOARD_SETUP_LABEL_0_X, BOARD_SETUP_LABEL_8_Y, BOARD_SETUP_BUTTON_WIDTH, BOARD_SETUP_BUTTON_HEIGHT, 
		GUI_IMG_BOARD_SETUP_OK, NULL, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_cancelButton, &m_boardSetupControlPanel,
		BOARD_SETUP_LABEL_1_X, BOARD_SETUP_LABEL_8_Y, BOARD_SETUP_BUTTON_WIDTH, BOARD_SETUP_BUTTON_HEIGHT, 
		GUI_IMG_BOARD_SETUP_CANCEL, NULL, NO_TRANSPARENCY);

	// Board Button Presses
	ChessGUICreateBoardPanel(&m_boardSetupWindow);
}

void ChessGUICreateFirstPlayerSettingsMenu(void)
{
	int numButtons = 2;  // this is not a magic number
	CONTROL* buttonsArr[] = {&m_firstPlayerWhiteButton, &m_firstPlayerBlackButton};
	const char* buttonImagesArray[] = {GUI_IMG_WHITE, GUI_IMG_BLACK};
	const char* selectedButtonImagesArray[] = {GUI_IMG_WHITE_SELECTED, GUI_IMG_BLACK_SELECTED};
	BOOL includeConfirmation = true;

	FUNCTION_DEBUG_TRACE;

	ChessGUICreateGenericSettingsMenu(
		&m_firstPlayerMenuWindow,
		FirstPlayerMenuButtonPressCallback,
		&m_firstPlayerMenuTitleLabel, GUI_IMG_FIRST_PLAYER_MENU_TITLE,
		buttonsArr, buttonImagesArray, selectedButtonImagesArray, numButtons, includeConfirmation);
}

void ChessGUICreateUserColorSettingsMenu(void)
{
	int numButtons = 2;  // this is not a magic number
	CONTROL* buttonsArr[] = {&m_userColorWhiteButton, &m_userColorBlackButton};
	const char* buttonImagesArray[] = {GUI_IMG_WHITE, GUI_IMG_BLACK};
	const char* selectedButtonImagesArray[] = {GUI_IMG_WHITE_SELECTED, GUI_IMG_BLACK_SELECTED};
	BOOL includeConfirmation = true;

	FUNCTION_DEBUG_TRACE;

	ChessGUICreateGenericSettingsMenu(
		&m_userColorMenuWindow,
		UserColorMenuButtonPressCallback,
		&m_userColorMenuTitleLabel, GUI_IMG_USER_COLOR_MENU_TITLE,
		buttonsArr, buttonImagesArray, selectedButtonImagesArray, numButtons, includeConfirmation);
}

void ChessGUICreateDifficultySettingsMenu(void)
{
	FUNCTION_DEBUG_TRACE;
	GenericGraphicsFrameworkCreateWindow(&m_difficultyMenuWindow, WINDOW_WIDTH, WINDOW_HEIGHT, GUI_IMG_GAME_BACKGROUND, DifficultyMenuButtonPressCallback);

	GenericGraphicsFrameworkCreateLabel(&m_difficultyMenuTitleLabel, &m_difficultyMenuWindow,
		MENU_LARGE_LABEL_X, MENU_LARGE_LABEL_0_Y, LARGE_LABEL_WIDTH, LARGE_LABEL_HEIGHT,
		GUI_IMG_DIFFICULTY_MENU_TITLE, TRANSPARENT_WHITE);

	GenericGraphicsFrameworkCreateButton(&m_difficulty1Button, &m_difficultyMenuWindow,
		MENU_SMALL_LABEL_0_X, MENU_SMALL_LABEL_0_Y, SMALL_LABEL_WIDTH, SMALL_LABEL_HEIGHT, 
		GUI_IMG_1, GUI_IMG_1_SELECTED, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_difficulty2Button, &m_difficultyMenuWindow,
		MENU_SMALL_LABEL_0_X, MENU_SMALL_LABEL_1_Y, SMALL_LABEL_WIDTH, SMALL_LABEL_HEIGHT, 
		GUI_IMG_3, GUI_IMG_3_SELECTED, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_difficulty3Button, &m_difficultyMenuWindow,
		MENU_SMALL_LABEL_1_X, MENU_SMALL_LABEL_0_Y, SMALL_LABEL_WIDTH, SMALL_LABEL_HEIGHT, 
		GUI_IMG_2, GUI_IMG_2_SELECTED, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_difficulty4Button, &m_difficultyMenuWindow,
		MENU_SMALL_LABEL_1_X, MENU_SMALL_LABEL_1_Y, SMALL_LABEL_WIDTH, SMALL_LABEL_HEIGHT, 
		GUI_IMG_4, GUI_IMG_4_SELECTED, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_difficultyBestButton, &m_difficultyMenuWindow,
		MENU_SMALL_LABEL_0_X, MENU_SMALL_LABEL_2_Y, LARGE_LABEL_WIDTH, LARGE_LABEL_HEIGHT, 
		GUI_IMG_BEST, GUI_IMG_BEST_SELECTED, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_okButton, &m_difficultyMenuWindow,
		MENU_SMALL_LABEL_0_X, MENU_SMALL_LABEL_4_Y, LARGE_LABEL_WIDTH, LARGE_LABEL_HEIGHT,
		GUI_IMG_OK, NULL, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_cancelButton, &m_difficultyMenuWindow,
		MENU_SMALL_LABEL_1_X, MENU_SMALL_LABEL_4_Y, LARGE_LABEL_WIDTH, LARGE_LABEL_HEIGHT,
		GUI_IMG_CANCEL, NULL, NO_TRANSPARENCY);	  

}



void ChessGUICreatePieceSelectionPanel(void)
{
	FUNCTION_DEBUG_TRACE;
	GenericGraphicsFrameworkCreatePanel(&m_pieceSelectionPanel, &m_boardSetupControlPanel, 0, 0, GAME_MENU_PANEL_WIDTH, GAME_MENU_PANEL_HEIGHT, NULL);

	GenericGraphicsFrameworkCreateButton(&m_blankPieceButton, &m_pieceSelectionPanel,
		BOARD_SETUP_LABEL_MID_X, BOARD_SETUP_LABEL_6_Y, 138, 58, 
		GUI_IMG_SELECT_BLANK_PIECE, NULL, NO_TRANSPARENCY); 

	GenericGraphicsFrameworkCreateButton(&m_whiteKingButton, &m_pieceSelectionPanel,
		BOARD_SETUP_LABEL_0_X, BOARD_SETUP_LABEL_0_Y, BOARD_SETUP_PIECE_BUTTON_WIDTH, BOARD_SETUP_PIECE_BUTTON_HEIGHT, 
		GUI_IMG_SELECT_WHITE_KING, NULL, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_whiteQueenButton, &m_pieceSelectionPanel,
		BOARD_SETUP_LABEL_0_X, BOARD_SETUP_LABEL_1_Y, BOARD_SETUP_PIECE_BUTTON_WIDTH, BOARD_SETUP_PIECE_BUTTON_HEIGHT, 
		GUI_IMG_SELECT_WHITE_QUEEN, NULL, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_whiteBishopButton, &m_pieceSelectionPanel,
		BOARD_SETUP_LABEL_0_X, BOARD_SETUP_LABEL_2_Y, BOARD_SETUP_PIECE_BUTTON_WIDTH, BOARD_SETUP_PIECE_BUTTON_HEIGHT, 
		GUI_IMG_SELECT_WHITE_BISHOP, NULL, NO_TRANSPARENCY);	  

	GenericGraphicsFrameworkCreateButton(&m_whiteKnightButton, &m_pieceSelectionPanel,
		BOARD_SETUP_LABEL_0_X, BOARD_SETUP_LABEL_3_Y, BOARD_SETUP_PIECE_BUTTON_WIDTH, BOARD_SETUP_PIECE_BUTTON_HEIGHT, 
		GUI_IMG_SELECT_WHITE_KNIGHT, NULL, NO_TRANSPARENCY);	  

	GenericGraphicsFrameworkCreateButton(&m_whiteRookButton, &m_pieceSelectionPanel,
		BOARD_SETUP_LABEL_0_X, BOARD_SETUP_LABEL_4_Y, BOARD_SETUP_PIECE_BUTTON_WIDTH, BOARD_SETUP_PIECE_BUTTON_HEIGHT, 
		GUI_IMG_SELECT_WHITE_ROOK, NULL, NO_TRANSPARENCY);	  

	GenericGraphicsFrameworkCreateButton(&m_whitePawnButton, &m_pieceSelectionPanel,
		BOARD_SETUP_LABEL_0_X, BOARD_SETUP_LABEL_5_Y, BOARD_SETUP_PIECE_BUTTON_WIDTH, BOARD_SETUP_PIECE_BUTTON_HEIGHT, 
		GUI_IMG_SELECT_WHITE_PAWN, NULL, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_blackKingButton, &m_pieceSelectionPanel,
		BOARD_SETUP_LABEL_1_X, BOARD_SETUP_LABEL_0_Y, BOARD_SETUP_PIECE_BUTTON_WIDTH, BOARD_SETUP_PIECE_BUTTON_HEIGHT, 
		GUI_IMG_SELECT_BLACK_KING, NULL, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_blackQueenButton, &m_pieceSelectionPanel,
		BOARD_SETUP_LABEL_1_X, BOARD_SETUP_LABEL_1_Y, BOARD_SETUP_PIECE_BUTTON_WIDTH, BOARD_SETUP_PIECE_BUTTON_HEIGHT, 
		GUI_IMG_SELECT_BLACK_QUEEN, NULL, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_blackBishopButton, &m_pieceSelectionPanel,
		BOARD_SETUP_LABEL_1_X, BOARD_SETUP_LABEL_2_Y, BOARD_SETUP_PIECE_BUTTON_WIDTH, BOARD_SETUP_PIECE_BUTTON_HEIGHT, 
		GUI_IMG_SELECT_BLACK_BISHOP, NULL, NO_TRANSPARENCY);	  

	GenericGraphicsFrameworkCreateButton(&m_blackKnightButton, &m_pieceSelectionPanel,
		BOARD_SETUP_LABEL_1_X, BOARD_SETUP_LABEL_3_Y, BOARD_SETUP_PIECE_BUTTON_WIDTH, BOARD_SETUP_PIECE_BUTTON_HEIGHT, 
		GUI_IMG_SELECT_BLACK_KNIGHT, NULL, NO_TRANSPARENCY);	  

	GenericGraphicsFrameworkCreateButton(&m_blackRookButton, &m_pieceSelectionPanel,
		BOARD_SETUP_LABEL_1_X, BOARD_SETUP_LABEL_4_Y, BOARD_SETUP_PIECE_BUTTON_WIDTH, BOARD_SETUP_PIECE_BUTTON_HEIGHT, 
		GUI_IMG_SELECT_BLACK_ROOK, NULL, NO_TRANSPARENCY);	  

	GenericGraphicsFrameworkCreateButton(&m_blackPawnButton, &m_pieceSelectionPanel,
		BOARD_SETUP_LABEL_1_X, BOARD_SETUP_LABEL_5_Y, BOARD_SETUP_PIECE_BUTTON_WIDTH, BOARD_SETUP_PIECE_BUTTON_HEIGHT, 
		GUI_IMG_SELECT_BLACK_PAWN, NULL, NO_TRANSPARENCY);  
}




/* DISPLAYS */

void ChessGUISettingsDisplayStartMenu()
{
	FUNCTION_DEBUG_TRACE;
	ChessGUICreateStartMenu();
	g_gui_pCurrentWindowToDisplay = &m_startMenuWindow;
	GenericGraphicsFrameworkDrawTree(&m_startMenuWindow);
}

void ChessGUIDisplayGameModeSettingsMenu(void)
{
	FUNCTION_DEBUG_TRACE;
	ChessGUICreateGameModeSettingsMenu();
	g_gui_pCurrentWindowToDisplay = &m_newGameModeMenuWindow;
	GenericGraphicsFrameworkDrawTree(&m_newGameModeMenuWindow);
}

void ChessGUIDisplayLoadGameMenu()
{
	FUNCTION_DEBUG_TRACE;
	ChessGUICreateSavedGamesMenu(GUI_IMG_LOAD_GAME_MENU_TITLE);
	g_gui_gameSlotsOperation = LOAD_GAME_OPERATION;
	g_gui_pCurrentWindowToDisplay = &g_gui_savedGamesMenuWindow;
	GenericGraphicsFrameworkDrawTree(&g_gui_savedGamesMenuWindow);
}



void ChessGUIDisplayGameSettingsMenu(void)
{
	FUNCTION_DEBUG_TRACE;
	ChessGUICreateGameSettingsMenu();
	g_gui_pCurrentWindowToDisplay = &m_settingsMenuWindow;
	GenericGraphicsFrameworkDrawTree(&m_settingsMenuWindow);
}

void ChessGUIDisplayBoardSetupMenu(void)
{
	FUNCTION_DEBUG_TRACE;
	ChessGUICreateBoardSetupSettingsMenu();
	ChessGUIUpdateBoardPieceLabels(ChessLogicGetBoardReference());
	g_gui_pCurrentWindowToDisplay = &m_boardSetupWindow;
	GenericGraphicsFrameworkDrawTree(&m_boardSetupWindow);
}

void ChessGUIDisplayPieceSelectionPanel(void)
{
	FUNCTION_DEBUG_TRACE;
	// Also, re-creating the entire window, so that selection panel would only be added once
	ChessGUICreateBoardSetupSettingsMenu();
	ChessGUICreatePieceSelectionPanel();
	ChessGUIUpdateBoardPieceLabels(ChessLogicGetBoardReference());
	//ChessGUIDisplayBoardSetupMenu();
}

void ChessGUIUpdateBoardSetup(BOARD* pBoard)
{
	FUNCTION_DEBUG_TRACE;
	// Hide PieceSelectionPanel by re-creating the window without the panel included
	ChessGUICreateBoardSetupSettingsMenu();
	ChessGUIUpdateBoardPieceLabels(ChessLogicGetBoardReference());
	ChessGUIDisplayBoardSetupMenu();
}

void ChessGUIDisplayFirstPlayerSettingsMenu(void)
{
	FUNCTION_DEBUG_TRACE;
	ChessGUICreateFirstPlayerSettingsMenu();
	g_gui_pCurrentWindowToDisplay = &m_firstPlayerMenuWindow;
	GenericGraphicsFrameworkDrawTree(&m_firstPlayerMenuWindow);
}

void ChessGUIDisplayUserColorSettingsMenu(void)
{
	FUNCTION_DEBUG_TRACE;
	ChessGUICreateUserColorSettingsMenu();
	g_gui_pCurrentWindowToDisplay = &m_userColorMenuWindow;
	GenericGraphicsFrameworkDrawTree(&m_userColorMenuWindow);
}

void ChessGUIDisplayDifficultySettingsMenu(void)
{
	FUNCTION_DEBUG_TRACE;
	ChessGUICreateDifficultySettingsMenu();
	g_gui_pCurrentWindowToDisplay = &m_difficultyMenuWindow;
	GenericGraphicsFrameworkDrawTree(&m_difficultyMenuWindow);
}




/* ON PRESS CALLBACKS */

BOOL StartMenuButtonPressCallback(CONTROL* pButton)
{
	FUNCTION_DEBUG_TRACE;
	if (&m_newGameButton == pButton)
	{
		DEBUG_PRINT("NewGame Button Pressed");    
		ChessGUIDisplayGameModeSettingsMenu();
	}
	else if (&m_loadGameButton == pButton)
	{
		DEBUG_PRINT("LoadGame Button Pressed");
		ChessGUIDisplayLoadGameMenu();
	}
	else if (&m_quitSettingsButton == pButton)
	{
		DEBUG_PRINT("QuitGame Button Pressed");
		ChessGUIQuitGame();
	}
	else
	{
		DEBUG_PRINT("Button Press was out of any button's range");
		return true;
	}
	// We need to stop the polling on the current window
	// So that polling can start on the next window
	return false;
}


BOOL GameModeMenuButtonPressCallback(CONTROL* pButton)
{
	static GAME_MODE gameMode;
	FUNCTION_DEBUG_TRACE;
	if (&m_twoPlayerGameModeButton == pButton)
	{
		DEBUG_PRINT("Two Player Button Pressed");
		gameMode = GAME_MODE_TWO_PLAYERS;
	}
	else if (&m_computerGameModeButton == pButton)
	{
		DEBUG_PRINT("Computer Button Pressed");
		gameMode = GAME_MODE_COMPUTER_AI;
	}
	else if (&m_okButton == pButton)
	{
		DEBUG_PRINT("OK Button Pressed");
		ChessLogicSetGameMode(gameMode);
		//ChessGUICreateGameSettingsMenu();
		ChessGUIDisplayGameSettingsMenu();
		return false;
	}
	else if (&m_cancelButton == pButton)
	{
		DEBUG_PRINT("Cancel Button Pressed");
		ChessControllerRun();
		return false;
	}
	else
	{
		DEBUG_PRINT("Button Press was out of any button's range");
	}
	return true;
}

BOOL TwoPlayerSettingsMenuButtonPressCallback(CONTROL* pButton)
{
	MOVE_STATUS status;
	FUNCTION_DEBUG_TRACE;
	if (&m_boardSetupButton == pButton)
	{
		DEBUG_PRINT("Board Setup Button Pressed");
		ChessGUIDisplayBoardSetupMenu();
		return false;
	}
	else if (&m_firstPlayerButton == pButton)
	{
		DEBUG_PRINT("First Player Button Pressed");
		ChessGUIDisplayFirstPlayerSettingsMenu();
		return false;
	}
	else if (&m_okButton == pButton)
	{
		DEBUG_PRINT("OK Button Pressed");
		status = ChessControllerStartGame();
		ChessGUIRespondToMoveStatusErrors(status);
		return false;
	}
	else if (&m_cancelButton == pButton)
	{
		DEBUG_PRINT("Cancel Button Pressed");
		ChessControllerRun();
		return false;
	}
	else
	{
		DEBUG_PRINT("Button Press was out of any button's range");
	}
	return true;
}


BOOL ComputerAISettingsMenuButtonPressCallback(CONTROL* pButton)
{
	FUNCTION_DEBUG_TRACE;
	if (&m_boardSetupButton == pButton)
	{
		DEBUG_PRINT("Board Setup Button Pressed");
		ChessGUIDisplayBoardSetupMenu();
		return false;
	}
	else if (&m_firstPlayerButton == pButton)
	{
		DEBUG_PRINT("First Player Button Pressed");
		ChessGUIDisplayFirstPlayerSettingsMenu();
		return false;
	}
	else if (&m_userColorButton == pButton)
	{
		DEBUG_PRINT("User Color Button Pressed");
		ChessGUIDisplayUserColorSettingsMenu();
		return false;
	}   
	else if (&m_okButton == pButton)
	{
		DEBUG_PRINT("OK Button Pressed");
		ChessGUIDisplayDifficultySettingsMenu();
		return false;
	}
	else if (&m_cancelButton == pButton)
	{
		DEBUG_PRINT("Cancel Button Pressed");
		ChessControllerRun();
		return false;
	}
	else
	{
		DEBUG_PRINT("Button Press was out of any button's range");
	}
	return true;
}

BOOL BoardSetupMenuButtonPressCallback(CONTROL* pButton)
{
	GUI_BOARD_COORDINATES coordinates;
	static BOARD_LOCATION boardLocation;
	MOVE_STATUS status;
	FUNCTION_DEBUG_TRACE;
	assert(pButton->pParent);

	if (&g_gui_gameBoardPanel == pButton->pParent)
	{
		coordinates.x = pButton->xPosition;
		coordinates.y = pButton->yPosition;
		boardLocation = GuiCoordinatesToBoardPosition(coordinates);
		DEBUG_PRINT("Button Press at x=%d y=%d c=%d r=%d", pButton->xPosition, pButton->yPosition, boardLocation.column, boardLocation.row);
		ChessGUIDisplayPieceSelectionPanel();
		// keep polling for piece selection
		return true;
	}

	else if (&m_pieceSelectionPanel == pButton->pParent)
	{
		if (&m_blankPieceButton == pButton)
		{
			DEBUG_PRINT("Blank Button Pressed");
			status = ChessLogicRemoveBoardPiece(boardLocation);
		}
		else if (&m_whiteKingButton == pButton)
		{
			DEBUG_PRINT("White King Button Pressed");
			status = ChessLogicSetBoardPiece(boardLocation, PLAYER_COLOR_WHITE, WHITE_KING);
		}
		else if (&m_whiteQueenButton == pButton)
		{
			DEBUG_PRINT("White Queen Button Pressed");
			status = ChessLogicSetBoardPiece(boardLocation, PLAYER_COLOR_WHITE, WHITE_QUEEN);
		}
		else if (&m_whiteBishopButton == pButton)
		{
			DEBUG_PRINT("White Bishop Button Pressed");
			status = ChessLogicSetBoardPiece(boardLocation, PLAYER_COLOR_WHITE, WHITE_BISHOP);
		}		  
		else if (&m_whiteKnightButton == pButton)
		{
			DEBUG_PRINT("White Knight Button Pressed");
			status = ChessLogicSetBoardPiece(boardLocation, PLAYER_COLOR_WHITE, WHITE_KNIGHT);
		}		
		else if (&m_whiteRookButton == pButton)
		{
			DEBUG_PRINT("White Rook Button Pressed");
			status = ChessLogicSetBoardPiece(boardLocation, PLAYER_COLOR_WHITE, WHITE_ROOK);
		}		
		else if (&m_whitePawnButton == pButton)
		{
			DEBUG_PRINT("White Pawn Button Pressed");
			status = ChessLogicSetBoardPiece(boardLocation, PLAYER_COLOR_WHITE, WHITE_PAWN);
		}

		else if (&m_blackKingButton == pButton)
		{
			DEBUG_PRINT("Black King Button Pressed");
			status = ChessLogicSetBoardPiece(boardLocation, PLAYER_COLOR_BLACK, BLACK_KING);
		}
		else if (&m_blackQueenButton == pButton)
		{
			DEBUG_PRINT("Black Queen Button Pressed");
			status = ChessLogicSetBoardPiece(boardLocation, PLAYER_COLOR_BLACK, BLACK_QUEEN);
		}
		else if (&m_blackBishopButton == pButton)
		{
			DEBUG_PRINT("Black Bishop Button Pressed");
			status = ChessLogicSetBoardPiece(boardLocation, PLAYER_COLOR_BLACK, BLACK_BISHOP);
		}		  
		else if (&m_blackKnightButton == pButton)
		{
			DEBUG_PRINT("Black Knight Button Pressed");
			status = ChessLogicSetBoardPiece(boardLocation, PLAYER_COLOR_BLACK, BLACK_KNIGHT);
		}		
		else if (&m_blackRookButton == pButton)
		{
			DEBUG_PRINT("Black Rook Button Pressed");
			status = ChessLogicSetBoardPiece(boardLocation, PLAYER_COLOR_BLACK, BLACK_ROOK);
		}		
		else if (&m_blackPawnButton == pButton)
		{
			DEBUG_PRINT("Black Pawn Button Pressed");
			status = ChessLogicSetBoardPiece(boardLocation, PLAYER_COLOR_BLACK, BLACK_PAWN);
		}	  

		if (MOVE_SUCCESSFUL == status)
		{	
			ChessGUIUpdateBoardSetup(ChessLogicGetBoardReference());
		}
		DEBUG_PRINT("set piece status = %d", status);
		ChessGUIRespondToMoveStatusErrors(status);
		// keep polling for board location
		return true;

	}	// end if piece selection

	if (&m_boardSetupControlPanel == pButton->pParent)
	{
		if (&m_clearButton == pButton)
		{
			DEBUG_PRINT("Clear Button Pressed");
			ChessLogicClearBoard();
			ChessGUIUpdateBoardSetup(ChessLogicGetBoardReference());
			// still polling, still on the current screen
			// but return false so the board would get updated
			// since we're not changing screens
			return true;
		}	   
		if (&m_okButton == pButton)
		{
			DEBUG_PRINT("OK Button Pressed");
			ChessGUIDisplayGameSettingsMenu();
			// end polling as we're exiting this screen
			return false;
		}
		if (&m_cancelButton == pButton)
		{
			DEBUG_PRINT("Cancel Button Pressed");
			ChessLogicInitializeBoard();
			ChessGUIDisplayGameSettingsMenu();
			// end polling as we're exiting this screen
			return false;
		}
	}
	else
	{
		DEBUG_PRINT("Button Press was out of any button's range");
	}

	return true;
}




BOOL FirstPlayerMenuButtonPressCallback(CONTROL* pButton)
{
	static PLAYER_COLOR playerColor;
	FUNCTION_DEBUG_TRACE;
	if (&m_firstPlayerWhiteButton == pButton)
	{
		DEBUG_PRINT("White Button Pressed");
		playerColor = PLAYER_COLOR_WHITE;
	}
	else if (&m_firstPlayerBlackButton == pButton)
	{
		DEBUG_PRINT("Black Button Pressed");
		playerColor = PLAYER_COLOR_BLACK;
	}
	else if (&m_okButton == pButton)
	{
		DEBUG_PRINT("OK Button Pressed");
		ChessLogicSetNextPlayer(playerColor);
		ChessGUIDisplayGameSettingsMenu();
		return false;
	}
	else if (&m_cancelButton == pButton)
	{
		DEBUG_PRINT("Cancel Button Pressed");
		ChessGUIDisplayGameSettingsMenu();
		return false;
	}
	else
	{
		DEBUG_PRINT("Button Press was out of any button's range");
	}
	return true;
}

BOOL UserColorMenuButtonPressCallback(CONTROL* pButton)
{
	static PLAYER_COLOR playerColor;
	FUNCTION_DEBUG_TRACE;
	if (&m_userColorWhiteButton == pButton)
	{
		DEBUG_PRINT("White Button Pressed");
		playerColor = PLAYER_COLOR_WHITE;
	}
	else if (&m_userColorBlackButton == pButton)
	{
		DEBUG_PRINT("Black Button Pressed");
		playerColor = PLAYER_COLOR_BLACK;
	}
	else if (&m_okButton == pButton)
	{
		DEBUG_PRINT("OK Button Pressed");
		ChessLogicSetUserColor(playerColor);
		ChessGUIDisplayGameSettingsMenu();
		return false;
	}
	else if (&m_cancelButton == pButton)
	{
		DEBUG_PRINT("Cancel Button Pressed");
		ChessGUIDisplayGameSettingsMenu();
		return false;
	}
	else
	{
		DEBUG_PRINT("Button Press was out of any button's range");
	}
	return true;

}

BOOL DifficultyMenuButtonPressCallback(CONTROL* pButton)
{
	MOVE_STATUS	status;
	static GAME_DIFFICULTY difficulty;
	FUNCTION_DEBUG_TRACE;
	if (&m_difficulty1Button == pButton)
	{
		DEBUG_PRINT("Difficulty 1 Button Pressed");
		difficulty = GAME_DIFFICULTY_CONSTANT_1;
	}
	else if (&m_difficulty2Button == pButton)
	{
		DEBUG_PRINT("Difficulty 2 Button Pressed");
		difficulty = GAME_DIFFICULTY_CONSTANT_2;
	}
	else if (&m_difficulty3Button == pButton)
	{
		DEBUG_PRINT("Difficulty 3 Button Pressed");
		difficulty = GAME_DIFFICULTY_CONSTANT_3;
	}
	else if (&m_difficulty4Button == pButton)
	{
		DEBUG_PRINT("Difficulty 4 Button Pressed");
		difficulty = GAME_DIFFICULTY_CONSTANT_4;
	}
	else if (&m_difficultyBestButton == pButton)
	{
		DEBUG_PRINT("Difficulty Best Button Pressed");
		difficulty = GAME_DIFFICULTY_BEST;
	}   
	else if (&m_okButton == pButton)
	{
		DEBUG_PRINT("OK Button Pressed");
		ChessLogicSetDifficulty(difficulty);
		status = ChessControllerStartGame();
		ChessGUIRespondToMoveStatusErrors(status);
		//DEBUG_PRINT("start game status = %d", status);
		return false;
	}
	else if (&m_cancelButton == pButton)
	{
		DEBUG_PRINT("Cancel Button Pressed");
		ChessGUIDisplayGameSettingsMenu();
		return false;
	}
	else
	{
		DEBUG_PRINT("Button Press was out of any button's range");
	}
	return true;

}

void ChessGUIQuitGame(void)
{
	FUNCTION_DEBUG_TRACE;
	ChessControllerTerminate();
}