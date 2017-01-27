#include <string.h>	// strncmp

#include "InterfaceDefinitions.h"
#include "ChessFlowController.h"
#include "ChessGenericUIInterface.h"
#include "ChessGUI.h"
#include "ChessCLI.h"
#include "ChessLogic.h"
#include "ChessSerializer.h"
#include "CommonUtils.h"

/* LOCAL DATA */
static CHESS_UI m_chessUI;
static FLOW_STATE m_flowState;
void (*m_fHandleGameTurn)(void);

static CHESS_BOOL m_hasHumanMoved = CHESS_FALSE;

/* PRIVATE METHODS DECLARATIONS */
static void ChessControllerHandle2PlayerGameTurn(void);
static void ChessControllerHandleAIGameTurnHumanFirst(void);
static void ChessControllerHandleAIGameTurnComputerFirst(void);
static void ChessControllerPerformComputerTurn(void);
static void ChessControllerHandleUserTurn(void);
static void ChessControllerHandleGenericUserPostTurnConditions(MOVE_STATUS moveStatus);

/* PUBLIC API METHODS IMPLEMENTATIONS */

void ChessControllerInit(INTERFACE_MODE interfaceMode)
{
	BOOL result;
	FUNCTION_DEBUG_TRACE;

	switch (interfaceMode)
	{
	case INTERFACE_MODE_GRAPHICAL:
		result = ChessGUIInitialize(&m_chessUI);
		if (!result)
		{
			CRITICAL_ERROR("Failed to initialize GUI\n");
		}
		break;

	case INTERFACE_MODE_CONSOLE:
		ChessCLIInitialize(&m_chessUI);
		break;

	default:
		CRITICAL_ERROR("Invalid UI Mode: %d\n", interfaceMode);
		ChessControllerTerminate();
	}
	
	m_fHandleGameTurn = NULL;
}

void ChessControllerRun()
{
	FUNCTION_DEBUG_TRACE;
	ChessLogicResetDefaultSettings();
	ChessLogicInitializeBoard();
	m_chessUI.ChessUIDisplayStartMenu(ChessLogicGetBoardReference());
	
	m_flowState = FLOW_STATE_SETTINGS;
	VERBOSE_PRINT("FLOW_STATE_SETTINGS");
	
	while (FLOW_STATE_SETTINGS == m_flowState)
	{
		m_chessUI.ChessUIPromptForSettings();
	}

	assert(NULL != m_fHandleGameTurn);
	
	
	while (FLOW_STATE_GAME == m_flowState)
	{
		m_fHandleGameTurn();
	}

	ChessControllerTerminate();
}

void ChessControllerTerminate(void)
{
	FUNCTION_DEBUG_TRACE;
	m_chessUI.ChessUITerminate();
	ChessLogicTerminate();
	exit(0);
}

FLOW_STATE ChessControllerGetFlowState(void)
{
	return m_flowState;
}

MOVE_STATUS ChessControllerStartGame(void)
{
	MOVE_STATUS status;
	FUNCTION_DEBUG_TRACE;
	status = ChessLogicStartGame();
	DEBUG_PRINT("status=%d", status);

	if ((MOVE_SUCCESSFUL == status) 
		|| (CHECK == status) 
		|| (GAME_TIE == status) 
		|| (CHECK_MATE == status))
	{
		m_chessUI.ChessUIStartGame();
	}
	
	ChessControllerHandleGenericUserPostTurnConditions(status);
	
	switch (ChessLogicGetGameMode())
	{
	case GAME_MODE_TWO_PLAYERS:
		m_fHandleGameTurn = ChessControllerHandle2PlayerGameTurn;
		break;

	case GAME_MODE_COMPUTER_AI:
		switch (ChessLogicGetNextPlayerType())
		{
		case PLAYER_TYPE_HUMAN:
			m_fHandleGameTurn = ChessControllerHandleAIGameTurnHumanFirst;
			break;

		case PLAYER_TYPE_COMPUTER_AI:
			m_fHandleGameTurn = ChessControllerHandleAIGameTurnComputerFirst;
			break;

		default:
			// should absolutely not get here
			PRINT_ERROR("Invalid Player Type");
			assert(false);

		} // end next player switch
		break;

	default:
		// should absolutely not get here
		PRINT_ERROR("Invalid Game Mode");
		assert(false);
	} // end game mode switch
	return status;
}

MOVE_STATUS ChessControllerPerformUserMove(GAME_MOVE gameMove)
{
	MOVE_STATUS status;
	FUNCTION_DEBUG_TRACE;
	status = ChessLogicPerformUserMove(gameMove);

	switch (status)
	{
	case MOVE_SUCCESSFUL:
	case CHECK:
		// is this safe? or should it occur after post?
		ChessLogicAdvanceNextPlayer();
	
	case CHECK_MATE:
	case GAME_TIE:
		m_hasHumanMoved = CHESS_TRUE;
		m_chessUI.ChessUIDisplayBoard(ChessLogicGetBoardReference());
		ChessControllerHandleGenericUserPostTurnConditions(status);
		break;

	default:
		// not really interested in the rest of the cases here
		break;

	} // end switch status

	return status;
}

BOOL ChessControllerSaveGame(const char* filename)
{
	ChessSerialization dataIn;
	FUNCTION_DEBUG_TRACE;   
	assert(filename);
	dataIn.gameMode = ChessLogicGetGameMode();
	dataIn.gameDifficulty = ChessLogicGetGameMode();
	dataIn.userColor = ChessLogicGetGameMode();
	dataIn.currPlayer = ChessLogicGetGameMode();
	ChessLogicGetBoardCopy(&(dataIn.board));
#ifdef __linux__
	return ChessSerialize(dataIn, filename);
#endif  
}

BOOL ChessControllerLoadGame(const char* filename)
{
#ifdef __linux__
	BOOL res;
	FUNCTION_DEBUG_TRACE;
	assert(filename);

	ChessSerialization dataOut;
	res = ChessDeserialize(&dataOut, filename);
	if (false == res)
	{
		return false;
	}
	ChessLogicSetGameMode(dataOut.gameMode);
	ChessLogicSetDifficulty(dataOut.gameDifficulty);
	ChessLogicSetUserColor(dataOut.userColor);
	ChessLogicSetNextPlayer(dataOut.currPlayer);
	ChessLogicLoadCompleteBoard(dataOut.board);
	m_chessUI.ChessUIDisplayBoard(ChessLogicGetBoardReference());	
	return true;
#endif	   
}

/* PRIVATE METHODS IMPLEMENTATIONS */

static void ChessControllerHandle2PlayerGameTurn(void)
{
	FUNCTION_DEBUG_TRACE;
	ChessControllerHandleUserTurn();

}

static void ChessControllerHandleAIGameTurnHumanFirst(void)
{
	FUNCTION_DEBUG_TRACE;
	ChessControllerHandleUserTurn();
	ChessControllerPerformComputerTurn();
}

static void ChessControllerHandleAIGameTurnComputerFirst(void)
{
	FUNCTION_DEBUG_TRACE;
	ChessControllerPerformComputerTurn();
	ChessControllerHandleUserTurn();
}

static void ChessControllerHandleUserTurn(void)
{
	FUNCTION_DEBUG_TRACE;
	m_hasHumanMoved = CHESS_FALSE;
	while (CHESS_FALSE == m_hasHumanMoved)
	{
		m_chessUI.ChessUIPromptForGameCmd(ChessLogicGetNextPlayer());
	}
}

static void ChessControllerHandleGenericUserPostTurnConditions(MOVE_STATUS moveStatus)
{
	
	if (CHECK_MATE == moveStatus || GAME_TIE == moveStatus)
	{
		VERBOSE_PRINT("FLOW_STATE_GAME_END");
		m_flowState = FLOW_STATE_GAME_END;
		if (CHECK_MATE == moveStatus)
		{
			m_chessUI.ChessUIDisplayCheckmate(ChessLogicGetNextPlayer());
		}
		else if (GAME_TIE == moveStatus)
		{
			m_chessUI.ChessUIDisplayTiedGame();
		}
		// UI should be blocking
		// When UI releases the blocking...
		ChessControllerTerminate();
	}

	if (CHECK == moveStatus)
	{
		VERBOSE_PRINT("FLOW_STATE_GAME");
		m_flowState = FLOW_STATE_GAME;		
		m_chessUI.ChessUIDisplayCheck();
	}
	if (MOVE_SUCCESSFUL == moveStatus)
	{
		VERBOSE_PRINT("FLOW_STATE_GAME");
		m_flowState = FLOW_STATE_GAME;
	}
}

static void ChessControllerPerformComputerTurn(void)
{
	MOVE_STATUS moveStatus;
	GAME_MOVE computerMove;
	FUNCTION_DEBUG_TRACE;
	computerMove = ChessLogicGetNextComputerMove();
	moveStatus = ChessLogicPerformNextComputerMove(computerMove);
	m_chessUI.ChessUIDisplayComputerMove(computerMove);
	m_chessUI.ChessUIDisplayBoard(ChessLogicGetBoardReference());
	ChessControllerHandleGenericUserPostTurnConditions(moveStatus);
	ChessLogicAdvanceNextPlayer();
}

