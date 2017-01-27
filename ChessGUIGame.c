#include "ChessCommonDefs.h"
#include "CommonUtils.h"
#include "ChessCommonUtils.h"
#include "GenericGraphicsFramework.h"
#include "ChessGUIProtected.h"
#include "InterfaceDefinitions.h"
#include "ChessLogic.h"
#include "ChessFlowController.h"


/* LOCAL DATA */
static CONTROL m_GameWindow;
static CONTROL m_gameMenuPanel;
static CONTROL m_bestMoveButton;
static CONTROL m_saveGameButton;
static CONTROL m_mainMenuButton;
static CONTROL m_quitGameButton;

static CONTROL m_pawnPromotionPanel;
static CONTROL m_pawnPromotionLabel;
static CONTROL m_promotionQueenButton;
static CONTROL m_promotionRookButton;
static CONTROL m_promotionBishopButton;
static CONTROL m_promotionKnightButton;

/* PRIVATE METHODS DECLARATIONS */
BOOL ChessGUIGameWindowOnPressCallback(CONTROL* pControl);
void ChessGUIDisplaySaveGameMenu();

BOOL ChessGUIPawnPromotionWindowOnPressCallback(CONTROL* pButton);

/* PUBLIC METHODS IMPLEMENTATIONS */

void ChessGUICreateGameWindow(void)
{
	FUNCTION_DEBUG_TRACE;
	GenericGraphicsFrameworkCreateWindow(&m_GameWindow, WINDOW_WIDTH, WINDOW_HEIGHT, GUI_IMG_GAME_BACKGROUND, ChessGUIGameWindowOnPressCallback);

	GenericGraphicsFrameworkCreatePanel(&m_gameMenuPanel, &m_GameWindow, GAME_MENU_PANEL_X, GAME_MENU_PANEL_Y, GAME_MENU_PANEL_WIDTH, GAME_MENU_PANEL_HEIGHT, NULL);

	GenericGraphicsFrameworkCreateButton(&m_bestMoveButton, &m_gameMenuPanel,
		GAME_MENU_LABELS_X, GAME_MENU_LABEL_0_Y, GAME_MENU_BUTTON_WIDTH, GAME_MENU_BUTTON_HEIGHT, 
		GUI_IMG_BEST_MOVE, NULL, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_saveGameButton, &m_gameMenuPanel,
		GAME_MENU_LABELS_X, GAME_MENU_LABEL_1_Y, GAME_MENU_BUTTON_WIDTH, GAME_MENU_BUTTON_HEIGHT, 
		GUI_IMG_SAVE_GAME, NULL, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_mainMenuButton, &m_gameMenuPanel,
		GAME_MENU_LABELS_X, GAME_MENU_LABEL_2_Y, GAME_MENU_BUTTON_WIDTH, GAME_MENU_BUTTON_HEIGHT, 
		GUI_IMG_MAIN_MENU, NULL, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_quitGameButton, &m_gameMenuPanel,
		GAME_MENU_LABELS_X, GAME_MENU_LABEL_3_Y, GAME_MENU_BUTTON_WIDTH, GAME_MENU_BUTTON_HEIGHT, 
		GUI_IMG_QUIT_GAME, NULL, NO_TRANSPARENCY);
	// todo: ask framework to automatically do this for every button?
	// all buttons could have the callback of their parent upon creation?
	m_quitGameButton.OnPressCallback = ChessGUIGameWindowOnPressCallback;

	// Create pawn promotion panel before the board, 
	// so that it would be displayed on top of it
	GenericGraphicsFrameworkCreatePanel(&m_pawnPromotionPanel, &m_GameWindow, 
		PAWN_PROMOTION_PANEL_X, PAWN_PROMOTION_PANEL_Y, PAWN_PROMOTION_PANEL_WIDTH, PAWN_PROMOTION_PANEL_HEIGHT, NULL);	
		
	// Board Button Presses
	ChessGUICreateBoardPanel(&m_GameWindow);
}

void ChessGUIDisplayGameWindow(void)
{
	FUNCTION_DEBUG_TRACE;
	// ??
	// Display is called at start, re-creating then, would hide pieces
	// ChessGUICreateGameWindow();
	g_gui_pCurrentWindowToDisplay = &m_GameWindow;
	GenericGraphicsFrameworkDrawTree(&m_GameWindow);
}

/* PRIVATE METHODS IMPLEMENTATIONS */

typedef enum
{
	MOVE_SELECTION_WAITING_FOR_ORIGIN = 0,
	MOVE_SELECTION_WAITING_FOR_DESTINATION,
	MOVE_SELECTION_NUM_OF_STATES,
	MOVE_SELECTION_INITIAL_STATE = MOVE_SELECTION_WAITING_FOR_ORIGIN
} BOARD_POSITIONS_SELECTION_STATE;

static BOARD_POSITIONS_SELECTION_STATE m_moveSelectionState = MOVE_SELECTION_INITIAL_STATE;
static GAME_MOVE m_gameMove;
BOOL MoveSelectionTransitionSelectOrigin(BOARD_LOCATION boardLocation);
BOOL MoveSelectionTransitionSelectDestination(BOARD_LOCATION boardLocation);
typedef BOOL (*SELECTION_STATE_MACHINE_TRANSITION_HANDLER)(BOARD_LOCATION boardLocation);
SELECTION_STATE_MACHINE_TRANSITION_HANDLER m_MoveSelectionStateMachine[MOVE_SELECTION_NUM_OF_STATES] = 
{
	MoveSelectionTransitionSelectOrigin, MoveSelectionTransitionSelectDestination
};

BOOL MoveSelectionTransitionSelectOrigin(BOARD_LOCATION boardLocation)
{
	FUNCTION_DEBUG_TRACE;
	m_gameMove.origin.row = boardLocation.row;
	m_gameMove.origin.column = boardLocation.column;
	
	m_moveSelectionState = MOVE_SELECTION_WAITING_FOR_DESTINATION;
	// keep polling for selection of next posision
	return true;
}

void ChessGUICreatePawnPromotionPanel(void)
{
	FUNCTION_DEBUG_TRACE;

		
	m_pawnPromotionPanel.OnPressCallback = ChessGUIPawnPromotionWindowOnPressCallback;

	GenericGraphicsFrameworkCreateButton(&m_promotionQueenButton, &m_pawnPromotionPanel,
		PAWN_PROMOTION_BUTTON_0_X, PAWN_PROMOTION_BUTTON_0_Y, GAME_MENU_BUTTON_WIDTH, GAME_MENU_BUTTON_HEIGHT, 
		GUI_IMG_PAWN_PROMOTION_QUEEN, NULL, NO_TRANSPARENCY);	

	GenericGraphicsFrameworkCreateButton(&m_promotionRookButton, &m_pawnPromotionPanel,
		PAWN_PROMOTION_BUTTON_1_X, PAWN_PROMOTION_BUTTON_0_Y, GAME_MENU_BUTTON_WIDTH, GAME_MENU_BUTTON_HEIGHT, 
		GUI_IMG_PAWN_PROMOTION_ROOK, NULL, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_promotionBishopButton, &m_pawnPromotionPanel,
		PAWN_PROMOTION_BUTTON_0_X, PAWN_PROMOTION_BUTTON_1_Y, GAME_MENU_BUTTON_WIDTH, GAME_MENU_BUTTON_HEIGHT, 
		GUI_IMG_PAWN_PROMOTION_BISHOP, NULL, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_promotionKnightButton, &m_pawnPromotionPanel,
		PAWN_PROMOTION_BUTTON_1_X, PAWN_PROMOTION_BUTTON_1_Y, GAME_MENU_BUTTON_WIDTH, GAME_MENU_BUTTON_HEIGHT, 
		GUI_IMG_PAWN_PROMOTION_KNIGHT, NULL, NO_TRANSPARENCY);
		
	// Background is added last, to be drawn first
	GenericGraphicsFrameworkCreateLabel(&m_pawnPromotionLabel, &m_pawnPromotionPanel, 
		0, 0, PAWN_PROMOTION_PANEL_WIDTH, PAWN_PROMOTION_PANEL_HEIGHT, 
		GUI_IMG_PAWN_PROMOTION_PANEL, NO_TRANSPARENCY);
}

void ChessGUIDisplayPawnPromotionPanel(void)
{
	FUNCTION_DEBUG_TRACE;
	ChessGUICreatePawnPromotionPanel();
	g_gui_pCurrentWindowToDisplay = &m_GameWindow;
	GenericGraphicsFrameworkDrawTree(&m_GameWindow);	
	
}

void ChessGUIPromptForPawnPromotion(void)
{
	FUNCTION_DEBUG_TRACE;
	ChessGUIDisplayPawnPromotionPanel();
	GenericGraphicsFrameworkBlockingPollForEvents(&m_pawnPromotionPanel);
}

BOOL ChessGUIPawnPromotionWindowOnPressCallback(CONTROL* pButton)
{
	static CHESS_PIECE_TYPE newTypeWithColor;
	static COLORLESS_CHESS_PIECE_TYPE newType;
	MOVE_STATUS moveStatus;	

	FUNCTION_DEBUG_TRACE;

	if (&m_promotionQueenButton == pButton)
	{
		DEBUG_PRINT("Queen Button Pressed");
		newType = PIECE_TYPE_QUEEN;
	}
	else if (&m_promotionRookButton == pButton)
	{
		DEBUG_PRINT("Rook Button Pressed");
		newType = PIECE_TYPE_ROOK;
	}
	else if (&m_promotionBishopButton == pButton)
	{
		DEBUG_PRINT("Bishop Button Pressed");
		newType = PIECE_TYPE_BISHOP;
	}
	else if (&m_promotionKnightButton == pButton)
	{
		DEBUG_PRINT("Knight Button Pressed");
		newType = PIECE_TYPE_KNIGHT;
	}
	
	newTypeWithColor = ChessCommonUtilsConvertPieceTypeToColor(newType);

	m_gameMove.newType = newTypeWithColor;
	moveStatus = ChessControllerPerformUserMove(m_gameMove);
	DEBUG_PRINT("moveStatus=%d", moveStatus);
	ChessGUIRespondToMoveStatusErrors(moveStatus);

	ChessGUIDisplayUpdatedBoard();

	// don't keep polling on this screen
	return false;
}	

BOOL MoveSelectionTransitionSelectDestination(BOARD_LOCATION boardLocation)
{
	MOVE_STATUS moveStatus;
	FUNCTION_DEBUG_TRACE;
	m_gameMove.destination.row = boardLocation.row;
	m_gameMove.destination.column = boardLocation.column;
	m_gameMove.newType = BLANK_POSITION;
	m_moveSelectionState = MOVE_SELECTION_WAITING_FOR_ORIGIN;
	moveStatus = ChessControllerPerformUserMove(m_gameMove);
	DEBUG_PRINT("moveStatus=%d", moveStatus);
	ChessGUIRespondToMoveStatusErrors(moveStatus);

	if (PAWN_PROMOTION_REQUIRED == moveStatus)
	{
		ChessGUIPromptForPawnPromotion();
	}
	else if (MOVE_SUCCESSFUL == moveStatus)
	{
		ChessGUIDisplayUpdatedBoard();		
	}
	

	// no need to keep polling
	return false;

}





void ChessGUIDisplayBestMoves(void)
{
	GAME_MOVE_PTR pMovesList;
	GAME_DIFFICULTY difficulty = GAME_DIFFICULTY_DEFAULT;
	GAME_MOVE firstMove;
	FUNCTION_DEBUG_TRACE;

	ChessLogicGetBestMoves(difficulty, &pMovesList);
	if (NULL == pMovesList)
	{
		DEBUG_PRINT("received NULL as best moves list");
		return;
	}
	firstMove = *pMovesList;
	DEBUG_PRINT_MOVE(firstMove);
	ChessGUIDisplayMove(firstMove);
	ChessLogicFreeMovesList(pMovesList);

}


BOOL ChessGUIGameWindowOnPressCallback(CONTROL* pButton)
{
	GUI_BOARD_COORDINATES coordinates;
	BOARD_LOCATION boardLocation;
	SELECTION_STATE_MACHINE_TRANSITION_HANDLER HandleBoardPositionPress;

	FUNCTION_DEBUG_TRACE;
	assert(pButton->pParent);
	if (&g_gui_gameBoardPanel == pButton->pParent)
	{
		coordinates.x = pButton->xPosition;
		coordinates.y = pButton->yPosition;
		boardLocation = GuiCoordinatesToBoardPosition(coordinates);
		DEBUG_PRINT("Button Press at x=%d y=%d c=%d r=%d", pButton->xPosition, pButton->yPosition, boardLocation.column, boardLocation.row);
		HandleBoardPositionPress = m_MoveSelectionStateMachine[m_moveSelectionState];
		return HandleBoardPositionPress(boardLocation);

	} // end if game board panel

	else if (&m_gameMenuPanel == pButton->pParent)
	{
		if (&m_bestMoveButton == pButton)
		{
			DEBUG_PRINT("Best Move Button Pressed");
			ChessGUIDisplayBestMoves();
			// keep polling on this screen
			return true;
		}

		else if (&m_saveGameButton == pButton)
		{
			DEBUG_PRINT("Save Game Button Pressed");
			ChessGUIDisplaySaveGameMenu();
			// stop polling on this screen as we're moving to another screen
			return false;
		}

		if (&m_mainMenuButton == pButton)
		{
			DEBUG_PRINT("Main Menu Button Pressed");
			ChessControllerRun();
			return false;
		}

		else if (&m_quitGameButton == pButton)
		{
			DEBUG_PRINT("Quit Game Button Pressed");
			ChessControllerTerminate();

		}
	}  // end if game menu panel

	else
	{
		DEBUG_PRINT("Button Press was out of any button's range");
	}
	return true;
}



void ChessGUIDisplaySaveGameMenu()
{
	FUNCTION_DEBUG_TRACE;
	ChessGUICreateSavedGamesMenu(GUI_IMG_SAVE_GAME_MENU_TITLE);
	g_gui_gameSlotsOperation = SAVE_GAME_OPERATION;
	g_gui_pCurrentWindowToDisplay = &g_gui_savedGamesMenuWindow;
	GenericGraphicsFrameworkDrawTree(&g_gui_savedGamesMenuWindow);
	GenericGraphicsFrameworkBlockingPollForEvents(&g_gui_savedGamesMenuWindow);   
}


void ChessGUIDisplayCheckmate(PLAYER_COLOR playerColor)
{
	CONTROL notificationLabel;
	FUNCTION_DEBUG_TRACE;
	GenericGraphicsFrameworkDisableButton(&m_bestMoveButton);
	GenericGraphicsFrameworkDisableButton(&m_saveGameButton);
	GenericGraphicsFrameworkDisableButton(&m_mainMenuButton);
	ChessGUIDisableBoardButtons();

	ChessGUIDisplayUpdatedBoard();

	GenericGraphicsFrameworkShowNotification(
		&notificationLabel,
		g_gui_pCurrentWindowToDisplay,
		NOTIFICATION_X,
		NOTIFICATION_Y,
		NOTIFICATION_WIDTH,
		NOTIFICATION_HEIGHT,
		GUI_IMG_CHECK_MATE,
		true,
		PERSISTENT_NOTIFICATION_ETERNAL_DELAY
		);

	GenericGraphicsFrameworkBlockingPollForEvents(&m_quitGameButton);
}

void ChessGUIDisplayTiedGame(void)
{
	CONTROL notificationLabel;
	FUNCTION_DEBUG_TRACE;
	GenericGraphicsFrameworkDisableButton(&m_bestMoveButton);
	GenericGraphicsFrameworkDisableButton(&m_saveGameButton);
	GenericGraphicsFrameworkDisableButton(&m_mainMenuButton);
	ChessGUIDisableBoardButtons();

	GenericGraphicsFrameworkShowNotification(
		&notificationLabel,
		g_gui_pCurrentWindowToDisplay,
		NOTIFICATION_X,
		NOTIFICATION_Y,
		NOTIFICATION_WIDTH,
		NOTIFICATION_HEIGHT,
		GUI_IMG_GAME_TIE,
		true,
		PERSISTENT_NOTIFICATION_ETERNAL_DELAY
		);
	GenericGraphicsFrameworkBlockingPollForEvents(&m_quitGameButton);
}
