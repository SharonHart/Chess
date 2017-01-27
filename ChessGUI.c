// un-comment to enable
//#define DEBUG_GUI

#ifdef __linux__
#include <unistd.h>
#endif
#include "CommonUtils.h"
#include "ChessCommonUtils.h"
#include "ChessCommonDefs.h"
#include "InterfaceDefinitions.h"

#include "ChessGenericUIInterface.h"
#include "ChessGUI.h"
#include "ChessGUIProtected.h"

#include "GenericGraphicsFramework.h"
#include "ChessLogic.h"
#include "ChessFlowController.h"

#define SAVE_FILE_SLOT_1	"chessprog_saved_game_1.xml"
#define SAVE_FILE_SLOT_2	"chessprog_saved_game_2.xml"
#define SAVE_FILE_SLOT_3	"chessprog_saved_game_3.xml"
#define SAVE_FILE_SLOT_4	"chessprog_saved_game_4.xml"
#define SAVE_FILE_SLOT_5	"chessprog_saved_game_5.xml"
#define SAVE_FILE_SLOT_6	"chessprog_saved_game_6.xml"
#define SAVE_FILE_SLOT_7	"chessprog_saved_game_7.xml"

/* GLOBAL DATA */
/* "Protected" - should be used only in GUI Settings/Game */
CONTROL* g_gui_pCurrentWindowToDisplay;
CONTROL g_gui_gameBoardPanel;
CONTROL g_gui_boardButtonsControls[BOARD_SIZE][BOARD_SIZE];           /* Board Controls Array */
CONTROL g_gui_boardPieceImageLabelsControls[BOARD_SIZE][BOARD_SIZE];  /* Board Controls Array */
CONTROL g_gui_savedGamesMenuWindow;
GAME_SLOTS_OPERATION g_gui_gameSlotsOperation;

/* LOCAL DATA */
static CONTROL m_savedGamesTitleLabel;
static CONTROL m_slot1Button;
static CONTROL m_slot2Button;
static CONTROL m_slot3Button;
static CONTROL m_slot4Button;
static CONTROL m_slot5Button;
static CONTROL m_slot6Button;
static CONTROL m_slot7Button;

/* Lookup Tables Initializations */

// Column X Row
BOARD_LOCATION GuiCoordinatesToBoardPosition(GUI_BOARD_COORDINATES coordinates)
{
	BOARD_LOCATION res;
	res.column = (coordinates.x - GUI_BOARD_POSITION_A_X - GAME_BOARD_X) / GUI_BOARD_POSITION_WIDTH;
	res.row = BOARD_SIZE - 1 - ((coordinates.y - GUI_BOARD_POSITION_8_Y - GAME_BOARD_Y) / GUI_BOARD_POSITION_HEIGHT);
	return res;
}

GUI_BOARD_COORDINATES LookupBoardPositionGuiCoordinates[BOARD_SIZE][BOARD_SIZE] = 
{
	{{GUI_BOARD_POSITION_A_X, GUI_BOARD_POSITION_1_Y},
	{GUI_BOARD_POSITION_A_X, GUI_BOARD_POSITION_2_Y},
	{GUI_BOARD_POSITION_A_X, GUI_BOARD_POSITION_3_Y},
	{GUI_BOARD_POSITION_A_X, GUI_BOARD_POSITION_4_Y},
	{GUI_BOARD_POSITION_A_X, GUI_BOARD_POSITION_5_Y},
	{GUI_BOARD_POSITION_A_X, GUI_BOARD_POSITION_6_Y},
	{GUI_BOARD_POSITION_A_X, GUI_BOARD_POSITION_7_Y},
	{GUI_BOARD_POSITION_A_X, GUI_BOARD_POSITION_8_Y}},

	{{GUI_BOARD_POSITION_B_X, GUI_BOARD_POSITION_1_Y},
	{GUI_BOARD_POSITION_B_X, GUI_BOARD_POSITION_2_Y},
	{GUI_BOARD_POSITION_B_X, GUI_BOARD_POSITION_3_Y},
	{GUI_BOARD_POSITION_B_X, GUI_BOARD_POSITION_4_Y},
	{GUI_BOARD_POSITION_B_X, GUI_BOARD_POSITION_5_Y},
	{GUI_BOARD_POSITION_B_X, GUI_BOARD_POSITION_6_Y},
	{GUI_BOARD_POSITION_B_X, GUI_BOARD_POSITION_7_Y},
	{GUI_BOARD_POSITION_B_X, GUI_BOARD_POSITION_8_Y}},

	{{GUI_BOARD_POSITION_C_X, GUI_BOARD_POSITION_1_Y},
	{GUI_BOARD_POSITION_C_X, GUI_BOARD_POSITION_2_Y},
	{GUI_BOARD_POSITION_C_X, GUI_BOARD_POSITION_3_Y},
	{GUI_BOARD_POSITION_C_X, GUI_BOARD_POSITION_4_Y},
	{GUI_BOARD_POSITION_C_X, GUI_BOARD_POSITION_5_Y},
	{GUI_BOARD_POSITION_C_X, GUI_BOARD_POSITION_6_Y},
	{GUI_BOARD_POSITION_C_X, GUI_BOARD_POSITION_7_Y},
	{GUI_BOARD_POSITION_C_X, GUI_BOARD_POSITION_8_Y}},

	{{GUI_BOARD_POSITION_D_X, GUI_BOARD_POSITION_1_Y},
	{GUI_BOARD_POSITION_D_X, GUI_BOARD_POSITION_2_Y},
	{GUI_BOARD_POSITION_D_X, GUI_BOARD_POSITION_3_Y},
	{GUI_BOARD_POSITION_D_X, GUI_BOARD_POSITION_4_Y},
	{GUI_BOARD_POSITION_D_X, GUI_BOARD_POSITION_5_Y},
	{GUI_BOARD_POSITION_D_X, GUI_BOARD_POSITION_6_Y},
	{GUI_BOARD_POSITION_D_X, GUI_BOARD_POSITION_7_Y},
	{GUI_BOARD_POSITION_D_X, GUI_BOARD_POSITION_8_Y}},

	{{GUI_BOARD_POSITION_E_X, GUI_BOARD_POSITION_1_Y},
	{GUI_BOARD_POSITION_E_X, GUI_BOARD_POSITION_2_Y},
	{GUI_BOARD_POSITION_E_X, GUI_BOARD_POSITION_3_Y},
	{GUI_BOARD_POSITION_E_X, GUI_BOARD_POSITION_4_Y},
	{GUI_BOARD_POSITION_E_X, GUI_BOARD_POSITION_5_Y},
	{GUI_BOARD_POSITION_E_X, GUI_BOARD_POSITION_6_Y},
	{GUI_BOARD_POSITION_E_X, GUI_BOARD_POSITION_7_Y},
	{GUI_BOARD_POSITION_E_X, GUI_BOARD_POSITION_8_Y}},

	{{GUI_BOARD_POSITION_F_X, GUI_BOARD_POSITION_1_Y},
	{GUI_BOARD_POSITION_F_X, GUI_BOARD_POSITION_2_Y},
	{GUI_BOARD_POSITION_F_X, GUI_BOARD_POSITION_3_Y},
	{GUI_BOARD_POSITION_F_X, GUI_BOARD_POSITION_4_Y},
	{GUI_BOARD_POSITION_F_X, GUI_BOARD_POSITION_5_Y},
	{GUI_BOARD_POSITION_F_X, GUI_BOARD_POSITION_6_Y},
	{GUI_BOARD_POSITION_F_X, GUI_BOARD_POSITION_7_Y},
	{GUI_BOARD_POSITION_F_X, GUI_BOARD_POSITION_8_Y}},

	{{GUI_BOARD_POSITION_G_X, GUI_BOARD_POSITION_1_Y},
	{GUI_BOARD_POSITION_G_X, GUI_BOARD_POSITION_2_Y},
	{GUI_BOARD_POSITION_G_X, GUI_BOARD_POSITION_3_Y},
	{GUI_BOARD_POSITION_G_X, GUI_BOARD_POSITION_4_Y},
	{GUI_BOARD_POSITION_G_X, GUI_BOARD_POSITION_5_Y},
	{GUI_BOARD_POSITION_G_X, GUI_BOARD_POSITION_6_Y},
	{GUI_BOARD_POSITION_G_X, GUI_BOARD_POSITION_7_Y},
	{GUI_BOARD_POSITION_G_X, GUI_BOARD_POSITION_8_Y}},

	{{GUI_BOARD_POSITION_H_X, GUI_BOARD_POSITION_1_Y},
	{GUI_BOARD_POSITION_H_X, GUI_BOARD_POSITION_2_Y},
	{GUI_BOARD_POSITION_H_X, GUI_BOARD_POSITION_3_Y},
	{GUI_BOARD_POSITION_H_X, GUI_BOARD_POSITION_4_Y},
	{GUI_BOARD_POSITION_H_X, GUI_BOARD_POSITION_5_Y},
	{GUI_BOARD_POSITION_H_X, GUI_BOARD_POSITION_6_Y},
	{GUI_BOARD_POSITION_H_X, GUI_BOARD_POSITION_7_Y},
	{GUI_BOARD_POSITION_H_X, GUI_BOARD_POSITION_8_Y}}
};

// Attention:
// The order has to exactly match the order in CHESS_PIECE_TYPE
const char* MapPieceTypeToGuiImageFileName[] =
{
	(const char*)0, // BLANK_POSITION
	GUI_IMG_WHITE_PAWN, // WHITE_PAWN
	GUI_IMG_WHITE_BISHOP, // WHITE_BISHOP
	GUI_IMG_WHITE_KNIGHT, // WHITE_KNIGHT
	GUI_IMG_WHITE_ROOK, // WHITE_ROOK
	GUI_IMG_WHITE_QUEEN, // WHITE_QUEEN
	GUI_IMG_WHITE_KING, // WHITE_KING
	GUI_IMG_BLACK_PAWN, // BLACK_PAWN
	GUI_IMG_BLACK_BISHOP, // BLACK_BISHOP
	GUI_IMG_BLACK_KNIGHT, // BLACK_KNIGHT
	GUI_IMG_BLACK_ROOK, // BLACK_ROOK
	GUI_IMG_BLACK_QUEEN, // BLACK_QUEEN
	GUI_IMG_BLACK_KING, // BLACK_KING	
};

/********************************/
/* PRIVATE METHODS DECLARATIONS */
/********************************/
void ChessGUIInitializeUserInterface(CHESS_UI *ui);
void ChessGUITerminate(void);

/* Create Window's */

/* button call backs */
BOOL SavedGameButtonPressCallback(CONTROL* pButton);

/**************************************/
/* PUBLIC API METHODS IMPLEMENTATIONS */
/**************************************/
BOOL ChessGUIInitialize(CHESS_UI *ui)
{
	FUNCTION_DEBUG_TRACE;
	ChessGUIInitializeUserInterface(ui);
	return GenericGraphicsFrameworkInit();
}

/***********************************/
/* PRIVATE METHODS IMPLEMENTATIONS */
/***********************************/

void ChessGUIInitializeUserInterface(CHESS_UI *ui)
{
	FUNCTION_DEBUG_TRACE;
	ui->ChessUIDisplayBoard = ChessGUIDisplayBoard;
	//ui->ChessUIGetUserCmd = ChessGUIGetUserCmd;	// todo remove
	//ui->ChessUIDisplayCmdStatus = ChessGUIDisplayCmdStatus; // todo remove

	ui->ChessUIDisplayStartMenu = ChessGUIDisplayStartMenu;
	ui->ChessUIPromptForSettings = ChessGUIPromptForSettings;
	//ui->ChessUIDisplayGameMode = ChessGUIDisplayGameMode;

	ui->ChessUIStartGame = ChessGUIStartGame;
	ui->ChessUIPromptForGameCmd = ChessGUIPromptForGameCmd;
	ui->ChessUIDisplayCheck = ChessGUIDisplayCheck;
	ui->ChessUIDisplayCheckmate = ChessGUIDisplayCheckmate;
	ui->ChessUIDisplayTiedGame = ChessGUIDisplayTiedGame;
	ui->ChessUIDisplayMove = ChessGUIDisplayMove;
	ui->ChessUIDisplayComputerMove = ChessGUIDisplayComputerMove;
	ui->ChessUIDisplayMoves = ChessGUIDisplayMoves;

	ui->ChessUITerminate = ChessGUITerminate;


}

void ChessGUICreateSavedGamesMenu(const char* titleFileName)
{
	FUNCTION_DEBUG_TRACE;
	GenericGraphicsFrameworkCreateWindow(&g_gui_savedGamesMenuWindow, WINDOW_WIDTH, WINDOW_HEIGHT, GUI_IMG_GAME_BACKGROUND, SavedGameButtonPressCallback);

	GenericGraphicsFrameworkCreateLabel(&m_savedGamesTitleLabel, &g_gui_savedGamesMenuWindow,
		MENU_LARGE_LABEL_X, MENU_LARGE_LABEL_0_Y, LARGE_LABEL_WIDTH, LARGE_LABEL_HEIGHT,
		titleFileName, TRANSPARENT_WHITE);

	GenericGraphicsFrameworkCreateButton(&m_slot1Button, &g_gui_savedGamesMenuWindow,
		MENU_SMALL_LABEL_0_X, MENU_SMALL_LABEL_0_Y, SMALL_LABEL_WIDTH, SMALL_LABEL_HEIGHT, 
		GUI_IMG_SLOT1, NULL, NO_TRANSPARENCY);

	GenericGraphicsFrameworkCreateButton(&m_slot2Button, &g_gui_savedGamesMenuWindow,
		MENU_SMALL_LABEL_0_X, MENU_SMALL_LABEL_1_Y, SMALL_LABEL_WIDTH, SMALL_LABEL_HEIGHT, 
		GUI_IMG_SLOT2, NULL, NO_TRANSPARENCY);		  

	GenericGraphicsFrameworkCreateButton(&m_slot3Button, &g_gui_savedGamesMenuWindow,
		MENU_SMALL_LABEL_0_X, MENU_SMALL_LABEL_2_Y, SMALL_LABEL_WIDTH, SMALL_LABEL_HEIGHT, 
		GUI_IMG_SLOT3, NULL, NO_TRANSPARENCY);	

	GenericGraphicsFrameworkCreateButton(&m_slot4Button, &g_gui_savedGamesMenuWindow,
		MENU_SMALL_LABEL_0_X, MENU_SMALL_LABEL_3_Y, SMALL_LABEL_WIDTH, SMALL_LABEL_HEIGHT, 
		GUI_IMG_SLOT4, NULL, NO_TRANSPARENCY);	

	GenericGraphicsFrameworkCreateButton(&m_slot5Button, &g_gui_savedGamesMenuWindow,
		MENU_SMALL_LABEL_1_X, MENU_SMALL_LABEL_0_Y, SMALL_LABEL_WIDTH, SMALL_LABEL_HEIGHT, 
		GUI_IMG_SLOT5, NULL, NO_TRANSPARENCY);	

	GenericGraphicsFrameworkCreateButton(&m_slot6Button, &g_gui_savedGamesMenuWindow,
		MENU_SMALL_LABEL_1_X, MENU_SMALL_LABEL_1_Y, SMALL_LABEL_WIDTH, SMALL_LABEL_HEIGHT, 
		GUI_IMG_SLOT6, NULL, NO_TRANSPARENCY);	

	GenericGraphicsFrameworkCreateButton(&m_slot7Button, &g_gui_savedGamesMenuWindow,
		MENU_SMALL_LABEL_1_X, MENU_SMALL_LABEL_2_Y, SMALL_LABEL_WIDTH, SMALL_LABEL_HEIGHT, 
		GUI_IMG_SLOT7, NULL, NO_TRANSPARENCY);	  
}

void ChessGUICreateBoardPanel(CONTROL* pParentWindow)
{
	int row;
	int column;
	GUI_BOARD_COORDINATES currBoardGuiCoordinates = {-1, -1};

	FUNCTION_DEBUG_TRACE;

	// Panel as a parent for its children buttons & piece images
	GenericGraphicsFrameworkCreatePanel(&g_gui_gameBoardPanel, pParentWindow, 
		GAME_BOARD_X, GAME_BOARD_Y, GAME_BOARD_WIDTH, GAME_BOARD_HEIGHT, GUI_IMG_GAME_BOARD);

	// one-time create buttons
	for (row = 0; row < BOARD_SIZE; row++)
	{
		for (column = 0; column < BOARD_SIZE; column++)
		{
			currBoardGuiCoordinates = LookupBoardPositionGuiCoordinates[column][row];

			GenericGraphicsFrameworkCreateButton(&(g_gui_boardButtonsControls[column][row]), &g_gui_gameBoardPanel,
				currBoardGuiCoordinates.x, currBoardGuiCoordinates.y, GUI_BOARD_POSITION_WIDTH, GUI_BOARD_POSITION_HEIGHT, 
				NULL, GUI_IMG_BOARD_POSITION_SELECTED, TRANSPARENT_WHITE);
		}
	}
}

void ChessGUIDisableBoardButtons(void)
{
	int row;
	int column;
	FUNCTION_DEBUG_TRACE;
	for (row = 0; row < BOARD_SIZE; row++)
	{
		for (column = 0; column < BOARD_SIZE; column++)
		{
			GenericGraphicsFrameworkDisableButton(&(g_gui_boardButtonsControls[column][row]));
		}
	}
}

void ChessGUIUpdateBoardPieceLabels(BOARD* pBoard)
{
	int column;
	int row;
	CHESS_PIECE_TYPE currPieceType;
	const char* pieceImageFileName = NULL;
	GUI_BOARD_COORDINATES currBoardGuiCoordinates = {-1, -1};

	assert(pBoard);
	FUNCTION_DEBUG_TRACE;
	for (row = 0; row < BOARD_SIZE; row++)
	{
		for (column = 0; column < BOARD_SIZE; column++)
		{
			currPieceType = (*pBoard)[column][row];
#ifdef DEBUG_GUI
			VERBOSE_PRINT("board[%d][%d] = %d", column, row, currPieceType);
#endif
			VALIDATE_PIECE(currPieceType);

			// map piece type to image
			pieceImageFileName = MapPieceTypeToGuiImageFileName[currPieceType];			
			if (NULL == pieceImageFileName)
			{
				continue;
			}
			// map position to coordinates
			currBoardGuiCoordinates = LookupBoardPositionGuiCoordinates[column][row];

			GenericGraphicsFrameworkCreateLabel(&(g_gui_boardPieceImageLabelsControls[column][row]), &g_gui_gameBoardPanel, 
				currBoardGuiCoordinates.x, currBoardGuiCoordinates.y - GUI_PIECE_IMAGE_3D_Y_AXIS_ADJUSTMENT, 
				GUI_PIECE_WIDTH, GUI_PIECE_HEIGHT, pieceImageFileName, TRANSPARENT_WHITE);
		}
	}
}

void ChessGUIRespondToMoveStatusErrors(MOVE_STATUS status)
{
	CONTROL notificationLabel;
	DEBUG_PRINT("status = %d", status);
	switch (status)
	{
		// case MOVE_SUCCESSFUL:
			// ?
			//ChessGUIUpdateBoardSetup(ChessLogicGetBoardReference());
			// break;

		case INVALID_BOARD_POSITION:
			GenericGraphicsFrameworkShowNotification(
				&notificationLabel,
				g_gui_pCurrentWindowToDisplay,
				NOTIFICATION_X,
				NOTIFICATION_Y,
				NOTIFICATION_WIDTH,
				NOTIFICATION_HEIGHT,
				GUI_IMG_INVALID_BOARD_POSITION,
				true,
				GUI_NOTIFICATIONS_DELAY
				);
			break;

			
		case INVALID_PIECE:
			GenericGraphicsFrameworkShowNotification(
				&notificationLabel,
				g_gui_pCurrentWindowToDisplay,
				NOTIFICATION_X,
				NOTIFICATION_Y,
				NOTIFICATION_WIDTH,
				NOTIFICATION_HEIGHT,
				GUI_IMG_INVALID_BOARD_POSITION,
				true,
				GUI_NOTIFICATIONS_DELAY
				);
			break;
			
		case ILLEGAL_MOVE:
			GenericGraphicsFrameworkShowNotification(
				&notificationLabel,
				g_gui_pCurrentWindowToDisplay,
				NOTIFICATION_X,
				NOTIFICATION_Y,
				NOTIFICATION_WIDTH,
				NOTIFICATION_HEIGHT,
				GUI_IMG_INVALID_BOARD_POSITION,
				true,
				GUI_NOTIFICATIONS_DELAY
				);
			break;
			
		case ILLEGAL_BOARD_INITIALIZATION:
			GenericGraphicsFrameworkShowNotification(
				&notificationLabel,
				g_gui_pCurrentWindowToDisplay,
				NOTIFICATION_X,
				NOTIFICATION_Y,
				NOTIFICATION_WIDTH,
				NOTIFICATION_HEIGHT,
				GUI_IMG_ILLEGAL_BOARD_INITIALIZATION,
				true,
				GUI_NOTIFICATIONS_DELAY
				);
			break;
			
		default:
		break;
		
	} // end switch status
		
}

/***********************************/
/* PSUEDO PUBLIC METHODS IMPLEMENTATIONS */
/***********************************/
/* UI Interface Implementations */

void ChessGUIDisplayBoard(BOARD* pBoard)
{
	//FUNCTION_DEBUG_TRACE;
	DEBUG_PRINT("empty implementation");
}

void ChessGUIDisplayStartMenu(BOARD* pBoard)
{
	FUNCTION_DEBUG_TRACE;
	ChessGUISettingsDisplayStartMenu();
}

void ChessGUIPromptForSettings(void)
{
	FUNCTION_DEBUG_TRACE;
	GenericGraphicsFrameworkBlockingPollForEvents(g_gui_pCurrentWindowToDisplay);
}

void ChessGUIStartGame(void)
{
	FUNCTION_DEBUG_TRACE;
	//ChessGUICreateGameWindow();
	ChessGUIDisplayUpdatedBoard();
}

void ChessGUIDisplayUpdatedBoard()
{
	FUNCTION_DEBUG_TRACE;
	ChessGUICreateGameWindow();
	ChessGUIUpdateBoardPieceLabels(ChessLogicGetBoardReference());
	ChessGUIDisplayGameWindow();
}

void ChessGUIPromptForGameCmd(PLAYER_COLOR color)
{
	FUNCTION_DEBUG_TRACE;
	ChessGUIDisplayUpdatedBoard();
	GenericGraphicsFrameworkBlockingPollForEvents(g_gui_pCurrentWindowToDisplay);
}

void ChessGUIDisplayCheck(void)
{
	CONTROL notificationLabel;
	FUNCTION_DEBUG_TRACE;
	ChessGUIDisplayUpdatedBoard();
	GenericGraphicsFrameworkShowNotification(
		&notificationLabel,
		g_gui_pCurrentWindowToDisplay,
		NOTIFICATION_X,
		NOTIFICATION_Y,
		NOTIFICATION_WIDTH,
		NOTIFICATION_HEIGHT,
		GUI_IMG_CHECK,
		true,
		GUI_NOTIFICATIONS_DELAY
		);
}

void ChessGUIDisplayMove(GAME_MOVE gameMove)
{
	CONTROL* pOriginButtonControl;
	CONTROL* pDestinationButtonControl;
	FUNCTION_DEBUG_TRACE;
	VALIDATE_GAME_MOVE(gameMove);

	pOriginButtonControl = &(g_gui_boardButtonsControls[gameMove.origin.column][gameMove.origin.row]);
	pDestinationButtonControl = &(g_gui_boardButtonsControls[gameMove.destination.column][gameMove.destination.row]);
	GenericGraphicsFrameworkSelectButton(*pOriginButtonControl);
	GenericGraphicsFrameworkSelectButton(*pDestinationButtonControl);
	ChessGUIDisplayGameWindow();
	GenericGraphicsFrameworkUnselectButton(*pOriginButtonControl);
	GenericGraphicsFrameworkUnselectButton(*pDestinationButtonControl);	
}

void ChessGUIDisplayComputerMove(GAME_MOVE computerMove)
{
	FUNCTION_DEBUG_TRACE;	
	ChessGUIDisplayMove(computerMove);
#ifdef __linux__
	sleep(1);
#endif
}

// Not implemented in GUI
void ChessGUIDisplayMoves(GAME_MOVE_PTR movesList)
{
	DEBUG_PRINT("empty implementation");
}

void ChessGUITerminate(void)
{
	FUNCTION_DEBUG_TRACE;
#ifdef __linux__
	GenericGraphicsFrameworkTerminate();
#endif 
}

/* ON PRESS CALLBACKS */

BOOL SavedGameButtonPressCallback(CONTROL* pButton)
{
	const char* savedSlotFileName = NULL;
	FUNCTION_DEBUG_TRACE;
	if (&m_slot1Button == pButton)
	{
		DEBUG_PRINT("Slot1 Button Pressed");
		savedSlotFileName = SAVE_FILE_SLOT_1;
	}
	else if (&m_slot2Button == pButton)
	{
		DEBUG_PRINT("Slot2 Button Pressed");
		savedSlotFileName = SAVE_FILE_SLOT_2;
	}   
	else if (&m_slot3Button == pButton)
	{
		DEBUG_PRINT("Slot3 Button Pressed");
		savedSlotFileName = SAVE_FILE_SLOT_3;
	}  
	else if (&m_slot4Button == pButton)
	{
		DEBUG_PRINT("Slot4 Button Pressed");
		savedSlotFileName = SAVE_FILE_SLOT_4;
	}   
	else if (&m_slot5Button == pButton)
	{
		DEBUG_PRINT("Slot5 Button Pressed");
		savedSlotFileName = SAVE_FILE_SLOT_5;
	}
	else if (&m_slot6Button == pButton)
	{
		DEBUG_PRINT("Slot6 Button Pressed");
		savedSlotFileName = SAVE_FILE_SLOT_6;
	}
	else if (&m_slot7Button == pButton)
	{
		DEBUG_PRINT("Slot7 Button Pressed");
		savedSlotFileName = SAVE_FILE_SLOT_7;
	}
	else
	{
		DEBUG_PRINT("Button Press was out of any button's range");
	}
	switch (g_gui_gameSlotsOperation)
	{
	case LOAD_GAME_OPERATION:
		ChessControllerLoadGame(savedSlotFileName);
		ChessGUIDisplayGameSettingsMenu();
		break;

	case SAVE_GAME_OPERATION:
		ChessControllerSaveGame(savedSlotFileName);
		break;

	default:
		PRINT_ERROR("Invalid Load/Save operation");
	}
	return false;
}





