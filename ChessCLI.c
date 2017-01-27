#include <string.h>	// strtok

#include "CommonUtils.h"
#include "ChessCommonUtils.h"
#include "InterfaceDefinitions.h"
#include "ChessCLI.h"
#include "ChessFlowController.h"
#include "ChessLogic.h"
//#include "ChessSerializer.h"  // move to control?

#ifdef __linux__
#define STRTOK_PORTABLE		strtok_r
#elif defined(_WIN32)
#define STRTOK_PORTABLE		strtok_s
#endif

#define CLI_PRINT(...)	{                                   \
	fprintf(stdout, __VA_ARGS__);    \
								} 

#define CLI_COMMAND_DELIMITER	" "

#define CONVERT_COLOR_ENUM_TO_STRING(color, str)  \
{                                            \
	switch (color)                            \
	{                                         \
	case PLAYER_COLOR_WHITE:                  \
	str = CLI_STR_WHITE_PLAYER; \
	break;                                 \
	\
	case PLAYER_COLOR_BLACK:                  \
	str = CLI_STR_BLACK_PLAYER; \
	break;                                 \
	\
	default:                                  \
	PRINT_ERROR("Unexpected player color: %d", color);    \
	}                                         \
}


#define CONVERT_BOARD_POSITION_INTERFACE_REPRESENTATION_TO_INTERNAL_DATA(data, strRep) \
{                                                                    \
	data.column = (int)(strRep[1]) - BOARD_INTERFACE_FIRST_COLUMN;    \
	data.row = (int)(strRep[3]) - BOARD_INTERFACE_FIRST_ROW;          \
}  

#define VALIDATE_POSITION_STRING(str)  \
{                                      \
	if (('<' != str[0])                 \
	|| (',' != str[2])               \
	|| ('>' != str[4])               \
	)                                \
	{                                   \
	return CMD_INVALID;              \
	}                                   \
}

typedef COMMAND_STATUS (*COMMAND_HANDLER)(COMMAND);

// Attention:
// The order has to exactly match the order in CHESS_PIECE_TYPE
static const char* MapPieceTypeToCliBoardRepresentation[] =
{
	BLANK_POSITION_CLI_REPRESENTATION,
	WHITE_PAWN_CLI_REPRESENTATION,
	WHITE_BISHOP_CLI_REPRESENTATION ,
	WHITE_KNIGHT_CLI_REPRESENTATION,
	WHITE_ROOK_CLI_REPRESENTATION,
	WHITE_QUEEN_CLI_REPRESENTATION,
	WHITE_KING_CLI_REPRESENTATION,
	BLACK_PAWN_CLI_REPRESENTATION,
	BLACK_BISHOP_CLI_REPRESENTATION,
	BLACK_KNIGHT_CLI_REPRESENTATION,
	BLACK_ROOK_CLI_REPRESENTATION,
	BLACK_QUEEN_CLI_REPRESENTATION,
	BLACK_KING_CLI_REPRESENTATION
};

static const char* MapPieceTypeToCliPromotionRepresentation[] =
{
	CLI_STRING_PIECE_TYPE_BLANK,
	CLI_STRING_PIECE_TYPE_PAWN,
	CLI_STRING_PIECE_TYPE_BISHOP ,
	CLI_STRING_PIECE_TYPE_KNIGHT,
	CLI_STRING_PIECE_TYPE_ROOK,
	CLI_STRING_PIECE_TYPE_QUEEN,
	CLI_STRING_PIECE_TYPE_KING,
	CLI_STRING_PIECE_TYPE_PAWN,
	CLI_STRING_PIECE_TYPE_BISHOP,
	CLI_STRING_PIECE_TYPE_KNIGHT,
	CLI_STRING_PIECE_TYPE_ROOK,
	CLI_STRING_PIECE_TYPE_QUEEN,
	CLI_STRING_PIECE_TYPE_KING
};

#define CONVERT_MOVE_TO_STRING_REPRESENTATION(move, origCol, origRow, destCol, destRow, newPiece)   \
{																		\
	origCol = move.origin.column + BOARD_INTERFACE_FIRST_COLUMN;		\
	origRow = move.origin.row + BOARD_INTERFACE_FIRST_ROW;				\
	destCol = move.destination.column + BOARD_INTERFACE_FIRST_COLUMN;	\
	destRow = move.destination.row + BOARD_INTERFACE_FIRST_ROW;			\
	newPiece = MapPieceTypeToCliPromotionRepresentation[move.newType];	\
	}

/* LOCAL DATA */
static COMMAND_HANDLER m_cmdHandlers[FLOW_STATE_NUM][GAME_MODE_NUM][NUM_OF_COMMANDS];
static const char* m_cmdStatusMap[NUM_OF_COMMANDS][NUM_OF_COMMAND_STATUS];	// TODO REMOVE
static char m_cmdBuffer[MAX_CLI_COMMAND_LENGTH];

/* PRIVATE METHODS DECLARATIONS */
static void ChessCLIInitCmdStatusMap();
static CMD_OPCODE ChessCLIMapCmdStringToOpcode(const char* cmdString);
static CHESS_PIECE_TYPE ConvertPieceTypeStringToPieceType(const char* pieceTypeStr, PLAYER_COLOR pieceColor);
//static void ChessCLIDisplayMoveStatusErrors(MOVE_STATUS status);
static void ChessCLIInitCmdHandlers();
static void ChessCLIDisplayCmdStatus(CMD_OPCODE, COMMAND_STATUS);
static COMMAND ChessCLIGetUserCmd(void);
static void ChessCLIDisplayScore(int score);

// UT needs to be "friend", therefore not static
COMMAND ChessCLIParseUserCmd(char* pCmdBuffer);
COMMAND_STATUS ChessCLIHandleUserCmd(COMMAND cmd);

/* UI Interface Implementations */
static void ChessCLIDisplayBoard(BOARD*);

/* Settings */
static void ChessCLIDisplayStartMenu(BOARD*);
static void ChessCLIPromptForSettings(void);
static void ChessCLIDisplayGameMode(GAME_MODE gameMode);

/* Game */
static void ChessCLIStartGame(void);
static void ChessCLIPromptForGameCmd(PLAYER_COLOR);
static void ChessCLIDisplayCheck(void);
static void ChessCLIDisplayCheckmate(PLAYER_COLOR);
static void ChessCLIDisplayTiedGame(void);
static void ChessCLIDisplayMove(GAME_MOVE);
static void ChessCLIDisplayComputerMove(GAME_MOVE);
static void ChessCLIDisplayMoves(GAME_MOVE_PTR movesList);


static void ChessCLITerminate(void);

/* PUBLIC API METHODS IMPLEMENTATIONS */
void ChessCLIInitialize(CHESS_UI *ui)
{
	FUNCTION_DEBUG_TRACE;
	ui->ChessUIDisplayBoard = ChessCLIDisplayBoard;
	//ui->ChessUIGetUserCmd = ChessCLIGetUserCmd;	remove
	//ui->ChessUIDisplayCmdStatus = ChessCLIDisplayCmdStatus;	remove

	ui->ChessUIDisplayStartMenu = ChessCLIDisplayStartMenu;
	ui->ChessUIPromptForSettings = ChessCLIPromptForSettings;
	//ui->ChessUIDisplayGameMode = ChessCLIDisplayGameMode;

	ui->ChessUIStartGame = ChessCLIStartGame;
	ui->ChessUIPromptForGameCmd = ChessCLIPromptForGameCmd;
	ui->ChessUIDisplayCheck = ChessCLIDisplayCheck;
	ui->ChessUIDisplayCheckmate = ChessCLIDisplayCheckmate;
	ui->ChessUIDisplayTiedGame = ChessCLIDisplayTiedGame;
	ui->ChessUIDisplayMove = ChessCLIDisplayMove;
	ui->ChessUIDisplayComputerMove = ChessCLIDisplayComputerMove;
	ui->ChessUIDisplayMoves = ChessCLIDisplayMoves;

	ui->ChessUITerminate = ChessCLITerminate;

	ChessCLIInitCmdHandlers();
	ChessCLIInitCmdStatusMap();	
}

/* PRIVATE METHODS IMPLEMENTATIONS */

// TODO CANCEL
static void ChessCLIInitCmdStatusMap()
{
	FUNCTION_DEBUG_TRACE;
	m_cmdStatusMap[CMD_OPCODE_INVALID][CMD_INVALID] = CLI_STR_ILLEGAL_COMMAND;

	m_cmdStatusMap[SETTINGS_CMD_SET_GAME_MODE][CMD_INVALID_ARGUMENT] = CLI_STR_WRONG_GAME_MODE;
	m_cmdStatusMap[SETTINGS_CMD_SET_DIFFICULTY][CMD_INVALID] = CLI_STR_ILLEGAL_COMMAND;
	m_cmdStatusMap[SETTINGS_CMD_SET_DIFFICULTY][CMD_INVALID_ARGUMENT] = CLI_STR_WRONG_MINIMAX_DEPTH;
	m_cmdStatusMap[SETTINGS_CMD_SET_USER_COLOR][CMD_INVALID] = CLI_STR_ILLEGAL_COMMAND;
	m_cmdStatusMap[SETTINGS_CMD_SET_USER_COLOR][CMD_INVALID_ARGUMENT] = CLI_STR_WRONG_USER_COLOR;
	m_cmdStatusMap[SETTINGS_CMD_LOAD_BOARD][CMD_INVALID_ARGUMENT] = CLI_STR_MISSING_FILE_NAME_ARG;
	m_cmdStatusMap[SETTINGS_CMD_LOAD_BOARD][CMD_FAILED] = CLI_STR_WRONG_FILE_NAME;

	m_cmdStatusMap[SETTINGS_CMD_SET_NEXT_PLAYER][CMD_INVALID_ARGUMENT] = CLI_STR_WRONG_USER_COLOR;
	m_cmdStatusMap[SETTINGS_CMD_REMOVE_PIECE][CMD_INVALID_ARGUMENT] = CLI_STR_REMOVE_PIECE_INVALID_ARG;
	m_cmdStatusMap[SETTINGS_CMD_SET_PIECE][CMD_INVALID] = CLI_STR_ILLEGAL_COMMAND;
	m_cmdStatusMap[SETTINGS_CMD_SET_PIECE][CMD_INVALID_ARGUMENT] = CLI_STR_INVALID_POSITION;
	m_cmdStatusMap[SETTINGS_CMD_SET_PIECE][CMD_FAILED] = CLI_STR_INVALID_BOARD;

	m_cmdStatusMap[SETTINGS_CMD_START_GAME][CMD_FAILED] = CLI_STR_WRONG_BOARD_INITIALIZATION;

	m_cmdStatusMap[GAME_CMD_MOVE][CMD_INVALID] = CLI_STR_ILLEGAL_COMMAND;
	m_cmdStatusMap[GAME_CMD_MOVE][CMD_INVALID_ARGUMENT] = CLI_STR_INVALID_POSITION;
	m_cmdStatusMap[GAME_CMD_MOVE][CMD_FAILED] = CLI_STR_ILLEGAL_MOVE;
	m_cmdStatusMap[GAME_CMD_MOVE][CMD_IGNORED] = CLI_STR_NO_PIECE_IN_POSITION;

	m_cmdStatusMap[GAME_CMD_GET_MOVES][CMD_INVALID] = CLI_STR_ILLEGAL_COMMAND;
	m_cmdStatusMap[GAME_CMD_GET_MOVES][CMD_INVALID_ARGUMENT] = CLI_STR_INVALID_POSITION;
	m_cmdStatusMap[GAME_CMD_GET_MOVES][CMD_IGNORED] = CLI_STR_NO_PIECE_IN_POSITION;

	m_cmdStatusMap[GAME_CMD_GET_BEST_MOVES][CMD_INVALID] = CLI_STR_ILLEGAL_COMMAND;
	m_cmdStatusMap[GAME_CMD_GET_SCORE][CMD_INVALID] = CLI_STR_ILLEGAL_COMMAND;
	m_cmdStatusMap[GAME_CMD_SAVE][CMD_INVALID] = CLI_STR_ILLEGAL_COMMAND;
}

static CMD_OPCODE ChessCLIMapCmdStringToOpcode(const char* cmdString)
{
	VERBOSE_PRINT("\"%s\"", cmdString);
	/* Settings */
	if (0 == strncmp(SETTINGS_CMD_SET_GAME_MODE_CLI_STRING, cmdString, MAX_CLI_COMMAND_LENGTH))
	{
		return SETTINGS_CMD_SET_GAME_MODE;
	}
	else if (0 == strncmp(SETTINGS_CMD_SET_DIFFICULTY_CLI_STRING, cmdString, MAX_CLI_COMMAND_LENGTH))
	{
		return SETTINGS_CMD_SET_DIFFICULTY;
	}
	else if (0 == strncmp(SETTINGS_CMD_SET_USER_COLOR_CLI_STRING, cmdString, MAX_CLI_COMMAND_LENGTH))
	{
		return SETTINGS_CMD_SET_USER_COLOR;
	}
	else if (0 == strncmp(SETTINGS_CMD_LOAD_BOARD_CLI_STRING, cmdString, MAX_CLI_COMMAND_LENGTH))
	{
		return SETTINGS_CMD_LOAD_BOARD;
	}
	else if (0 == strncmp(SETTINGS_CMD_CLEAR_BOARD_CLI_STRING, cmdString, MAX_CLI_COMMAND_LENGTH))
	{
		return SETTINGS_CMD_CLEAR_BOARD;
	}
	else if (0 == strncmp(SETTINGS_CMD_SET_NEXT_PLAYER_CLI_STRING, cmdString, MAX_CLI_COMMAND_LENGTH))
	{
		return SETTINGS_CMD_SET_NEXT_PLAYER;
	}
	else if (0 == strncmp(SETTINGS_CMD_REMOVE_PIECE_CLI_STRING, cmdString, MAX_CLI_COMMAND_LENGTH))
	{
		return SETTINGS_CMD_REMOVE_PIECE;
	}
	else if (0 == strncmp(SETTINGS_CMD_SET_PIECE_CLI_STRING, cmdString, MAX_CLI_COMMAND_LENGTH))
	{
		return SETTINGS_CMD_SET_PIECE;
	}
	else if (0 == strncmp(SETTINGS_CMD_PRE_GAME_PRINT_BOARD_CLI_STRING, cmdString, MAX_CLI_COMMAND_LENGTH))
	{
		return SETTINGS_CMD_PRE_GAME_PRINT_BOARD;
	}

	else if (0 == strncmp(SETTINGS_CMD_START_GAME_CLI_STRING, cmdString, MAX_CLI_COMMAND_LENGTH))
	{
		return SETTINGS_CMD_START_GAME;
	}

	/* Game */
	else if (0 == strncmp(GAME_CMD_MOVE_CLI_STRING, cmdString, MAX_CLI_COMMAND_LENGTH))
	{
		return GAME_CMD_MOVE;
	}
	else if (0 == strncmp(GAME_CMD_GET_MOVES_CLI_STRING, cmdString, MAX_CLI_COMMAND_LENGTH))
	{
		return GAME_CMD_GET_MOVES;
	}
	else if (0 == strncmp(GAME_CMD_GET_BEST_MOVES_CLI_STRING, cmdString, MAX_CLI_COMMAND_LENGTH))
	{
		return GAME_CMD_GET_BEST_MOVES;
	}
	else if (0 == strncmp(GAME_CMD_GET_SCORE_CLI_STRING, cmdString, MAX_CLI_COMMAND_LENGTH))
	{
		return GAME_CMD_GET_SCORE;
	}
	else if (0 == strncmp(GAME_CMD_SAVE_CLI_STRING, cmdString, MAX_CLI_COMMAND_LENGTH))
	{
		return GAME_CMD_SAVE;
	}

	else if (0 == strncmp(GENERAL_CMD_QUIT_CLI_STRING, cmdString, MAX_CLI_COMMAND_LENGTH))
	{
		return GENERAL_CMD_QUIT;
	}

	else
	{
		PRINT_ERROR("Received invalid CLI command");
		return CMD_OPCODE_INVALID;
	}

}


static CHESS_PIECE_TYPE ConvertPieceTypeStringToPieceType(const char* pieceTypeStr, PLAYER_COLOR pieceColor)
{
	CHESS_PIECE_TYPE pieceType;
	int pieceTypeAdjustmentDueToColor = 0;
	if (PLAYER_COLOR_BLACK == pieceColor)
	{
		pieceTypeAdjustmentDueToColor = NUM_OF_COLORLESS_PIECE_TYPES;
	}
	if (0 == strncmp(pieceTypeStr, SETTINGS_CMD_ARG_PIECE_TYPE_KING_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		pieceType = WHITE_KING + pieceTypeAdjustmentDueToColor;
	}
	else if (0 == strncmp(pieceTypeStr, SETTINGS_CMD_ARG_PIECE_TYPE_QUEEN_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		pieceType = WHITE_QUEEN + pieceTypeAdjustmentDueToColor;
	}
	else if (0 == strncmp(pieceTypeStr, SETTINGS_CMD_ARG_PIECE_TYPE_ROOK_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		pieceType = WHITE_ROOK + pieceTypeAdjustmentDueToColor;
	}
	else if (0 == strncmp(pieceTypeStr, SETTINGS_CMD_ARG_PIECE_TYPE_KNIGHT_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		pieceType = WHITE_KNIGHT + pieceTypeAdjustmentDueToColor;
	}
	else if (0 == strncmp(pieceTypeStr, SETTINGS_CMD_ARG_PIECE_TYPE_BISHOP_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		pieceType = WHITE_BISHOP + pieceTypeAdjustmentDueToColor;
	}
	else if (0 == strncmp(pieceTypeStr, SETTINGS_CMD_ARG_PIECE_TYPE_PAWN_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		pieceType = WHITE_PAWN + pieceTypeAdjustmentDueToColor;
	}
	else
	{
		return PIECE_TYPE_INVALID;
	}
	return pieceType;
}

/* UI Interface Implementations */
static void ChessCLIDisplayBoard(BOARD* board)
{
	int row, column;
	CHESS_PIECE_TYPE piece;
	FUNCTION_DEBUG_TRACE;
	
	for (row = BOARD_SIZE - 1; row >= 0; row--)
	{
		CLI_PRINT(BOARD_DISPLAY_ROW_DIVIDER);
		CLI_PRINT("%d|", row+1);
		for (column = 0; column < BOARD_SIZE; column++)
		{
			piece = (*board)[column][row];
			VALIDATE_PIECE(piece);
			CLI_PRINT(" %s |", MapPieceTypeToCliBoardRepresentation[piece]);
		}
		CLI_PRINT("\n");
	}
	CLI_PRINT(BOARD_DISPLAY_ROW_DIVIDER);
	CLI_PRINT(BOARD_DISPLAY_ROW_HEADER);

}


static COMMAND ChessCLIGetUserCmd()
{
	COMMAND cmd;

	FUNCTION_DEBUG_TRACE;
	fgets(m_cmdBuffer, MAX_CLI_COMMAND_LENGTH, stdin);

	if (NULL == m_cmdBuffer || '\0' == m_cmdBuffer[0])
	{
		PRINT_ERROR("Failure to get command");
		cmd.opcode = CMD_OPCODE_INVALID;
		return cmd;
	}

	return ChessCLIParseUserCmd(m_cmdBuffer);
}



static void ChessCLIDisplayStartMenu(BOARD* pBoard)
{
	FUNCTION_DEBUG_TRACE;
	ChessCLIDisplayBoard(pBoard);
}

static void ChessCLIPromptForSettings()
{
	COMMAND userCmd;
	COMMAND_STATUS cmdStatus;
	FUNCTION_DEBUG_TRACE;

	CLI_PRINT(CLI_STR_ENTER_GAME_SETTINGS);
	userCmd = ChessCLIGetUserCmd();
	cmdStatus = ChessCLIHandleUserCmd(userCmd);
	ChessCLIDisplayCmdStatus(userCmd.opcode, cmdStatus);
}



static void ChessCLIDisplayGameMode(GAME_MODE gameMode)
{
	FUNCTION_DEBUG_TRACE;
	switch (gameMode)
	{
	case GAME_MODE_TWO_PLAYERS:
		CLI_PRINT(CLI_STR_TWO_PLAYERS_GAME_MODE);
		break;
	case GAME_MODE_COMPUTER_AI:
		CLI_PRINT(CLI_STR_PLAYER_VS_AI_GAME_MODE);
		break;
	default:
		PRINT_ERROR("Unknown game mode: %d", gameMode);
	}
}

/* Game */
static void ChessCLIStartGame(void)
{
	FUNCTION_DEBUG_TRACE;  
}

static void ChessCLIPromptForGameCmd(PLAYER_COLOR color)
{
	const char* playerColorStr = NULL;
	COMMAND userCmd;
	COMMAND_STATUS cmdStatus;
	FUNCTION_DEBUG_TRACE;
	CONVERT_COLOR_ENUM_TO_STRING(color, playerColorStr);
	if (playerColorStr)
	{
		CLI_PRINT(CLI_STR_PROMPT_MOVE, playerColorStr);
	}
	userCmd = ChessCLIGetUserCmd();
	cmdStatus = ChessCLIHandleUserCmd(userCmd);
	ChessCLIDisplayCmdStatus(userCmd.opcode, cmdStatus);   
}

static void ChessCLIDisplayCheck(void)
{
	FUNCTION_DEBUG_TRACE; 
	CLI_PRINT(CLI_STR_CHECK);
	//getchar();
}

static void ChessCLIDisplayCheckmate(PLAYER_COLOR color)
{
	const char* playerColorStr = NULL;
	FUNCTION_DEBUG_TRACE;
	CONVERT_COLOR_ENUM_TO_STRING(color, playerColorStr);
	if (playerColorStr)
	{
		CLI_PRINT(CLI_STR_CHECK_MATE, playerColorStr);
		getchar();
	}
}

static void ChessCLIDisplayTiedGame(void)
{
	FUNCTION_DEBUG_TRACE; 
	CLI_PRINT(CLI_STR_GAME_TIE)
	getchar();
}

static void ChessCLIDisplayMove(GAME_MOVE move)
{
	char origCol;
	char destCol;
	int origRow;
	int destRow;
	const char* newPieceTypeStr;

	FUNCTION_DEBUG_TRACE;
	CONVERT_MOVE_TO_STRING_REPRESENTATION(move, origCol, origRow, destCol, destRow, newPieceTypeStr);
	if (0 == strcmp(CLI_STRING_PIECE_TYPE_BLANK, newPieceTypeStr))
	{
		CLI_PRINT(CLI_STR_DISPLAY_MOVE_NO_PROMOTION, origCol, origRow, destCol, destRow);
	}
	else
	{
		CLI_PRINT(CLI_STR_DISPLAY_MOVE_WITH_PROMOTION, origCol, origRow, destCol, destRow, newPieceTypeStr);
	}
}

static void ChessCLIDisplayComputerMove(GAME_MOVE move)
{
	char origCol;
	char destCol;
	int origRow;
	int destRow;
	const char* newPieceTypeStr;
	FUNCTION_DEBUG_TRACE;
	CONVERT_MOVE_TO_STRING_REPRESENTATION(move, origCol, origRow, destCol, destRow, newPieceTypeStr);

	CLI_PRINT(CLI_STR_DISPLAY_COMPUTER_MOVE, origCol, origRow, destCol, destRow, newPieceTypeStr);
}

static void ChessCLIDisplayMoves(GAME_MOVE_PTR movesList)
{
	GAME_MOVE_PTR pMove;
	//assert(movesList);
	DEBUG_PRINT("movesList=%p",(void*)movesList);
	for (pMove = movesList; pMove != NULL; pMove = pMove->pNextMove)
	{
		//assert(pMove);
		if (NULL != pMove)
		{
			DEBUG_PRINT("ChessCLIDisplayMove(%p)", (void*)pMove);
			ChessCLIDisplayMove(*pMove);
		}
	}
}

static void ChessCLIDisplayScore(int score)
{
	FUNCTION_DEBUG_TRACE;  
	CLI_PRINT("%d\n", score);
}

static void ChessCLITerminate(void)
{
	FUNCTION_DEBUG_TRACE;
}

/* Command Handlers */

/* Settings */

static COMMAND_STATUS CommandHandlerInvalid(COMMAND cmd)
{
	FUNCTION_DEBUG_TRACE;
	return CMD_INVALID;
}

static COMMAND_STATUS CommandHandlerSetGame(COMMAND cmd)
{
	const char* gameModeStr;
	GAME_MODE gameMode;
	FUNCTION_DEBUG_TRACE;
	assert(SETTINGS_CMD_SET_GAME_MODE == cmd.opcode);
	if (cmd.argc != 2)
	{
		return CMD_INVALID_ARGUMENT;
	}

	gameModeStr = (cmd.argv[1]);
	if (0 == strncmp(gameModeStr, SETTINGS_CMD_ARG_GAME_MODE_TWO_PLAYERS_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		gameMode = GAME_MODE_TWO_PLAYERS;
	}
	else if (0 == strncmp(gameModeStr, SETTINGS_CMD_ARG_GAME_MODE_COMPUTER_AI_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		gameMode = GAME_MODE_COMPUTER_AI;
	}
	else
	{
		return CMD_INVALID_ARGUMENT;
	}

	if (gameMode > GAME_MODE_NUM || gameMode < GAME_MODE_TWO_PLAYERS)
	{
		return CMD_INVALID_ARGUMENT;
	}
	ChessLogicSetGameMode(gameMode);
	ChessCLIDisplayGameMode(gameMode);
	return CMD_SUCCESS;
}

static COMMAND_STATUS CommandHandlerSetDifficulty(COMMAND cmd)
{
	const char* difficultyType;
	GAME_DIFFICULTY difficultyDepth;
	FUNCTION_DEBUG_TRACE;
	assert(SETTINGS_CMD_SET_DIFFICULTY == cmd.opcode);

#ifdef _DEBUG
	if (ChessLogicGetGameMode() != GAME_MODE_COMPUTER_AI)
	{
		PRINT_ERROR("Set Difficulty in game mode != AI");
	} 
#endif

	if (cmd.argc < 2)
	{
		return CMD_INVALID_ARGUMENT;
	}

	difficultyType = (cmd.argv[1]);
	if (0 == strncmp(difficultyType, SETTINGS_CMD_ARG_DIFFICULTY_DEPTH_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		difficultyDepth = (GAME_DIFFICULTY)atoi(cmd.argv[2]);
		if ((difficultyDepth < GAME_DIFFICULTY_MIN) ||
			(difficultyDepth > GAME_DIFFICULTY_MAX))
		{
			return CMD_INVALID_ARGUMENT;
		}
	}
	else if (0 == strncmp(difficultyType, SETTINGS_CMD_ARG_DIFFICULTY_BEST_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		difficultyDepth = GAME_DIFFICULTY_BEST;
	}
	else
	{
		return CMD_INVALID_ARGUMENT;
	}

	ChessLogicSetDifficulty(difficultyDepth);
	return CMD_SUCCESS;
}


static COMMAND_STATUS CommandHandlerSetUserColor(COMMAND cmd)
{
	const char* playerColorStr;
	PLAYER_COLOR playerColor;
	FUNCTION_DEBUG_TRACE;
	assert(SETTINGS_CMD_SET_USER_COLOR == cmd.opcode);
#ifdef _DEBUG
	if (ChessLogicGetGameMode() != GAME_MODE_COMPUTER_AI)
	{
		PRINT_ERROR("Set User Color in game mode != AI");
	} 
#endif

	if (cmd.argc != 2)
	{
		return CMD_INVALID_ARGUMENT;
	}

	playerColorStr = (cmd.argv[1]);
	if (0 == strncmp(playerColorStr, SETTINGS_CMD_ARG_USER_COLOR_WHITE_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		playerColor = PLAYER_COLOR_WHITE;
	}
	else if (0 == strncmp(playerColorStr, SETTINGS_CMD_ARG_USER_COLOR_BLACK_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		playerColor = PLAYER_COLOR_BLACK;
	}
	else
	{
		return CMD_INVALID_ARGUMENT;
	}

	ChessLogicSetUserColor(playerColor);
	return CMD_SUCCESS;
}

static COMMAND_STATUS CommandHandlerLoadBoard(COMMAND cmd)
{
	const char* filename;

	FUNCTION_DEBUG_TRACE;
	assert(SETTINGS_CMD_LOAD_BOARD == cmd.opcode);
	if (cmd.argc != 2)
	{
		return CMD_INVALID_ARGUMENT;
	}
	filename = cmd.argv[1];

#ifdef _WIN32
	return CMD_INVALID;
#endif

#ifdef __linux__
	if (false == ChessControllerLoadGame(filename))
	{
		return CMD_FAILED;
	}
	return CMD_SUCCESS;
#endif
}

static COMMAND_STATUS CommandHandlerClearBoard(COMMAND cmd)
{
	FUNCTION_DEBUG_TRACE;
	assert(SETTINGS_CMD_CLEAR_BOARD == cmd.opcode);
	ChessLogicClearBoard();
	return CMD_SUCCESS;
}

static COMMAND_STATUS CommandHandlerSetNextPlayer(COMMAND cmd)
{
	const char* playerColorStr;
	PLAYER_COLOR playerColor;
	FUNCTION_DEBUG_TRACE;
	assert(SETTINGS_CMD_SET_NEXT_PLAYER == cmd.opcode);
	if (cmd.argc != 2)
	{
		return CMD_INVALID_ARGUMENT;
	}

	playerColorStr = (cmd.argv[1]);
	if (0 == strncmp(playerColorStr, SETTINGS_CMD_ARG_USER_COLOR_WHITE_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		playerColor = PLAYER_COLOR_WHITE;
	}
	else if (0 == strncmp(playerColorStr, SETTINGS_CMD_ARG_USER_COLOR_BLACK_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		playerColor = PLAYER_COLOR_BLACK;
	}
	else
	{
		return CMD_INVALID_ARGUMENT;
	}

	ChessLogicSetNextPlayer(playerColor);
	return CMD_SUCCESS;
}

static COMMAND_STATUS CommandHandlerRemovePiece(COMMAND cmd)
{
	const char* boardLocationStr;
	BOARD_LOCATION boardLocation;
	MOVE_STATUS status;
	FUNCTION_DEBUG_TRACE;
	assert(SETTINGS_CMD_REMOVE_PIECE == cmd.opcode);
	if (cmd.argc != 2)
	{
		return CMD_INVALID_ARGUMENT;
	}

	boardLocationStr = (cmd.argv[1]);

	if (('<' != boardLocationStr[0])
		|| (',' != boardLocationStr[2])
		|| ('>' != boardLocationStr[4])
		)
	{
		return CMD_INVALID_ARGUMENT;
	}
	// board position
	CONVERT_BOARD_POSITION_INTERFACE_REPRESENTATION_TO_INTERNAL_DATA(boardLocation, boardLocationStr);

	status = ChessLogicRemoveBoardPiece(boardLocation);
	if (INVALID_BOARD_POSITION == status)
	{
		CLI_PRINT(CLI_STR_INVALID_POSITION);
	}
	return CMD_SUCCESS;
}

static COMMAND_STATUS CommandHandlerSetPiece(COMMAND cmd)
{
	const char* boardLocationStr;
	BOARD_LOCATION boardLocation;
	const char* playerColorStr;
	PLAYER_COLOR playerColor;
	const char* pieceTypeStr;
	CHESS_PIECE_TYPE pieceType;

	MOVE_STATUS status;

	FUNCTION_DEBUG_TRACE;
	assert(SETTINGS_CMD_SET_PIECE == cmd.opcode);
	if (cmd.argc != 4)
	{
		return CMD_INVALID;
	}

	boardLocationStr = cmd.argv[1];
	playerColorStr = cmd.argv[2];
	pieceTypeStr = cmd.argv[3];

	VALIDATE_POSITION_STRING(boardLocationStr);

	// color
	if (0 == strncmp(playerColorStr, SETTINGS_CMD_ARG_USER_COLOR_WHITE_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		playerColor = PLAYER_COLOR_WHITE;
	}
	else if (0 == strncmp(playerColorStr, SETTINGS_CMD_ARG_USER_COLOR_BLACK_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		playerColor = PLAYER_COLOR_BLACK;
	}
	else
	{
		return CMD_INVALID;
	}

	// piece
	pieceType = ConvertPieceTypeStringToPieceType(pieceTypeStr, playerColor);

	if (pieceType >= NUM_OF_PIECE_TYPES || PIECE_TYPE_INVALID == pieceType)
	{
		return CMD_INVALID;
	}

	// board position
	CONVERT_BOARD_POSITION_INTERFACE_REPRESENTATION_TO_INTERNAL_DATA(boardLocation, boardLocationStr);

	status = ChessLogicSetBoardPiece(boardLocation, playerColor, pieceType);
	switch (status)
	{
	case MOVE_SUCCESSFUL:
		return CMD_SUCCESS;

	case INVALID_PIECE:
		return CMD_INVALID;

	case INVALID_BOARD_POSITION:
		return CMD_INVALID_ARGUMENT;

	case ILLEGAL_BOARD_INITIALIZATION:
		return CMD_FAILED;

	default:
		PRINT_ERROR("ChessLogicSetBoardPiece not expected to return %d", status);
		break;

	}
	return status;
}

static COMMAND_STATUS CommandHandlerPrintBoard(COMMAND cmd)
{
	FUNCTION_DEBUG_TRACE;
	assert(SETTINGS_CMD_PRE_GAME_PRINT_BOARD == cmd.opcode);
	ChessCLIDisplayBoard(ChessLogicGetBoardReference());
	return CMD_SUCCESS;
}

static COMMAND_STATUS CommandHandlerQuit(COMMAND cmd)
{
	FUNCTION_DEBUG_TRACE;
	assert(GENERAL_CMD_QUIT == cmd.opcode);
	ChessControllerTerminate();
	return CMD_SUCCESS;
}



static COMMAND_STATUS CommandHandlerGetScore(COMMAND cmd)
{
	GAME_DIFFICULTY difficulty;
	const char* difficultyStr;
	const char* originStr;
	const char* destinationStr;
	GAME_MOVE move;
	int score;
	FUNCTION_DEBUG_TRACE;
	assert(GAME_CMD_GET_SCORE == cmd.opcode);
	if (cmd.argc != 6)
	{
		return CMD_INVALID;
	}

	originStr = cmd.argv[3];
	destinationStr = cmd.argv[5];
	VALIDATE_POSITION_STRING(originStr);
	VALIDATE_POSITION_STRING(destinationStr);
	CONVERT_BOARD_POSITION_INTERFACE_REPRESENTATION_TO_INTERNAL_DATA(move.origin, originStr);
	CONVERT_BOARD_POSITION_INTERFACE_REPRESENTATION_TO_INTERNAL_DATA(move.destination, destinationStr);

	// assume that d is either a number between 1-4 or the value best
	difficultyStr = cmd.argv[1];
	if (0 == strncmp(difficultyStr, SETTINGS_CMD_ARG_DIFFICULTY_BEST_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		difficulty = GAME_DIFFICULTY_BEST;
	}
	else
	{
		difficulty = atoi(difficultyStr);
	}
	score = ChessLogicGetScore(difficulty, move);
	ChessCLIDisplayScore(score);

	return CMD_SUCCESS;
}

static COMMAND_STATUS CommandHandlerSaveGame(COMMAND cmd)
{

	const char* filename;
	FUNCTION_DEBUG_TRACE;
	assert(GAME_CMD_SAVE == cmd.opcode);
	if (cmd.argc != 2)
	{
		return CMD_INVALID;
	}
	filename = cmd.argv[1];
	ChessControllerSaveGame(filename);
#ifdef _WIN32
	return CMD_INVALID;
#endif

#ifdef __linux__
	return CMD_SUCCESS;
#endif      

}


static COMMAND_STATUS CommandHandlerStartGame(COMMAND cmd)
{
	MOVE_STATUS status;
	FUNCTION_DEBUG_TRACE;
	assert(SETTINGS_CMD_START_GAME == cmd.opcode);
	status = ChessControllerStartGame();
	// todo representations of failures
	switch (status)
	{
	case MOVE_SUCCESSFUL:
	case CHECK:
	case CHECK_MATE:
	case GAME_TIE:
		return CMD_SUCCESS;
		
	case ILLEGAL_BOARD_INITIALIZATION:
		return CMD_FAILED;

	default:
		return CMD_FAILED;	
	}
	return CMD_IGNORED;
}

static COMMAND_STATUS CommandHandlerGameMove(COMMAND cmd)
{
	GAME_MOVE move;
	MOVE_STATUS status;
	const char* originStr;
	const char* destinationStr;
	const char* pawnPromotionStr;
	CHESS_PIECE_TYPE pawnPromotionPieceType;

	FUNCTION_DEBUG_TRACE;
	assert(GAME_CMD_MOVE == cmd.opcode);

	if (cmd.argc < 4 || cmd.argc > 5)
	{
		return CMD_INVALID;
	}

	originStr = cmd.argv[1];
	destinationStr = cmd.argv[3];
	VALIDATE_POSITION_STRING(originStr);
	VALIDATE_POSITION_STRING(destinationStr);
	// board positions
	CONVERT_BOARD_POSITION_INTERFACE_REPRESENTATION_TO_INTERNAL_DATA(move.origin, originStr);
	CONVERT_BOARD_POSITION_INTERFACE_REPRESENTATION_TO_INTERNAL_DATA(move.destination, destinationStr);

	// pawn promotion
	if (5 == cmd.argc)
	{
		pawnPromotionStr = cmd.argv[4];	   
	}
	else
	{
		pawnPromotionStr = SETTINGS_CMD_ARG_PIECE_TYPE_DEFAULT_CLI_STRING;
	}

	pawnPromotionPieceType = ConvertPieceTypeStringToPieceType(pawnPromotionStr, ChessLogicGetNextPlayer());
	move.newType = pawnPromotionPieceType;

	status = ChessControllerPerformUserMove(move);

	// todo representations of failures
	switch (status)
	{
	case MOVE_SUCCESSFUL:
	case CHECK:
	case CHECK_MATE:
	case GAME_TIE:
		return CMD_SUCCESS;

	case INVALID_BOARD_POSITION:
		return CMD_INVALID_ARGUMENT;

	case ILLEGAL_MOVE:
		return CMD_FAILED;

	case INVALID_PIECE:
		return CMD_IGNORED;

	default:
		PRINT_ERROR("ChessLogicPerformUserMove not expected to return %d", status);
		break;

	} // end switch status

	return CMD_SUCCESS;
}


static COMMAND_STATUS CommandHandlerGetMoves(COMMAND cmd)
{
	const char* boardLocationStr;
	BOARD_LOCATION boardLocation;
	GAME_MOVE_PTR pMovesList;
	MOVE_STATUS status;

	FUNCTION_DEBUG_TRACE;
	assert(GAME_CMD_GET_MOVES == cmd.opcode);

	if (cmd.argc != 2)
	{
		return CMD_INVALID;
	}

	boardLocationStr = cmd.argv[1];
	VALIDATE_POSITION_STRING(boardLocationStr);
	CONVERT_BOARD_POSITION_INTERFACE_REPRESENTATION_TO_INTERNAL_DATA(boardLocation, boardLocationStr);
	status = ChessLogicGetMoves(boardLocation, &pMovesList);

	// replace cmd status
	//ChessCLIDisplayMoveStatusErrors(status);
	
	switch (status)
	{
	case MOVE_SUCCESSFUL:
		ChessCLIDisplayMoves(pMovesList);
		ChessLogicFreeMovesList(pMovesList);
		return CMD_SUCCESS;

	case INVALID_BOARD_POSITION:
		return CMD_INVALID_ARGUMENT;

	case INVALID_PIECE:
		return CMD_IGNORED;

	default:
		PRINT_ERROR("ChessLogicGetMoves not expected to return %d", status);
		break;
	}
	return status;
}

static COMMAND_STATUS CommandHandlerGetBestMoves(COMMAND cmd)
{
	GAME_MOVE_PTR pMovesList;
	GAME_DIFFICULTY difficulty;
	const char* difficultyStr;
	FUNCTION_DEBUG_TRACE;
	assert(GAME_CMD_GET_BEST_MOVES == cmd.opcode);
	if (cmd.argc != 2)
	{
		return CMD_INVALID;
	}
	difficultyStr = cmd.argv[1];
	if (0 == strncmp(difficultyStr, SETTINGS_CMD_ARG_DIFFICULTY_BEST_CLI_STRING, MAX_CLI_COMMAND_LENGTH))
	{
		difficulty = GAME_DIFFICULTY_BEST;
	}
	else
	{
		difficulty = atoi(difficultyStr);
	}
	ChessLogicGetBestMoves(difficulty, &pMovesList);
	ChessCLIDisplayMoves(pMovesList);
	DEBUG_PRINT("calling ChessLogicFreeMovesList(%p)...", (void*)pMovesList);
	ChessLogicFreeMovesList(pMovesList);
	return CMD_SUCCESS;
}


static void ChessCLIInitCmdHandlers()
{
	int offset;
	int NUM_OF_HANDLER_POINTERS = FLOW_STATE_NUM * GAME_MODE_NUM * NUM_OF_COMMANDS;
	COMMAND_HANDLER* addr = (COMMAND_HANDLER*)m_cmdHandlers;
	FUNCTION_DEBUG_TRACE;
	// First, init entire lut to invalid
	for (offset = 0; offset < NUM_OF_HANDLER_POINTERS; offset++)
	{
		*addr = CommandHandlerInvalid;
		addr++;
	}

	/* Set Game Mode */
	/* Valid only in settings state */
	/* Valid for any current Game Mode */
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_SET_GAME_MODE] = CommandHandlerSetGame;
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_SET_GAME_MODE] = CommandHandlerSetGame;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_SET_GAME_MODE] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_SET_GAME_MODE] = CommandHandlerInvalid;

	/* Set Difficulty */
	/* Valid only in settings state */
	/* Valid only for AI Game Mode */
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_SET_DIFFICULTY] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_SET_DIFFICULTY] = CommandHandlerSetDifficulty;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_SET_DIFFICULTY] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_SET_DIFFICULTY] = CommandHandlerInvalid;

	/* Set User Color */   
	/* Valid only in settings state */
	/* Valid only for AI Game Mode */
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_SET_USER_COLOR] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_SET_USER_COLOR] = CommandHandlerSetUserColor;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_SET_USER_COLOR] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_SET_USER_COLOR] = CommandHandlerInvalid;

	/* Load Board */
	/* Valid only in settings state */
	/* Valid for both game modes */
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_LOAD_BOARD] = CommandHandlerLoadBoard;
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_LOAD_BOARD] = CommandHandlerLoadBoard;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_LOAD_BOARD] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_LOAD_BOARD] = CommandHandlerInvalid;

	/* Clear Board */
	/* Valid only in settings state */
	/* Valid for both game modes */
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_CLEAR_BOARD] = CommandHandlerClearBoard;
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_CLEAR_BOARD] = CommandHandlerClearBoard;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_CLEAR_BOARD] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_CLEAR_BOARD] = CommandHandlerInvalid;

	/* Set Next Player */
	/* Valid only in settings state */
	/* Valid for both game modes */
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_SET_NEXT_PLAYER] = CommandHandlerSetNextPlayer;
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_SET_NEXT_PLAYER] = CommandHandlerSetNextPlayer;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_SET_NEXT_PLAYER] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_SET_NEXT_PLAYER] = CommandHandlerInvalid;

	/* Remove Piece */
	/* Valid only in settings state */
	/* Valid for both game modes */
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_REMOVE_PIECE] = CommandHandlerRemovePiece;
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_REMOVE_PIECE] = CommandHandlerRemovePiece;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_REMOVE_PIECE] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_REMOVE_PIECE] = CommandHandlerInvalid;

	/* Set Piece */
	/* Valid only in settings state */
	/* Valid for both game modes */
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_SET_PIECE] = CommandHandlerSetPiece;
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_SET_PIECE] = CommandHandlerSetPiece;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_SET_PIECE] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_SET_PIECE] = CommandHandlerInvalid;

	/* Print Board */
	/* Valid only in settings state */
	/* Valid for both game modes */
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_PRE_GAME_PRINT_BOARD] = CommandHandlerPrintBoard;
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_PRE_GAME_PRINT_BOARD] = CommandHandlerPrintBoard;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_PRE_GAME_PRINT_BOARD] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_PRE_GAME_PRINT_BOARD] = CommandHandlerInvalid;


	/* Start Game */
	/* Valid only in settings state */
	/* Valid for both game modes */
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_START_GAME] = CommandHandlerStartGame;
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_START_GAME] = CommandHandlerStartGame;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_TWO_PLAYERS][SETTINGS_CMD_START_GAME] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_COMPUTER_AI][SETTINGS_CMD_START_GAME] = CommandHandlerInvalid;

	/* Move */
	/* Valid only in game state */
	/* Valid for both game modes */
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_TWO_PLAYERS][GAME_CMD_MOVE] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_COMPUTER_AI][GAME_CMD_MOVE] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_TWO_PLAYERS][GAME_CMD_MOVE] = CommandHandlerGameMove;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_COMPUTER_AI][GAME_CMD_MOVE] = CommandHandlerGameMove;

	/* Get Moves */
	/* Valid only in game state */
	/* Valid for both game modes */
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_TWO_PLAYERS][GAME_CMD_GET_MOVES] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_COMPUTER_AI][GAME_CMD_GET_MOVES] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_TWO_PLAYERS][GAME_CMD_GET_MOVES] = CommandHandlerGetMoves;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_COMPUTER_AI][GAME_CMD_GET_MOVES] = CommandHandlerGetMoves;

	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_TWO_PLAYERS][GAME_CMD_GET_BEST_MOVES] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_COMPUTER_AI][GAME_CMD_GET_BEST_MOVES] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_TWO_PLAYERS][GAME_CMD_GET_BEST_MOVES] = CommandHandlerGetBestMoves;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_COMPUTER_AI][GAME_CMD_GET_BEST_MOVES] = CommandHandlerGetBestMoves;

	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_TWO_PLAYERS][GAME_CMD_GET_SCORE] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_COMPUTER_AI][GAME_CMD_GET_SCORE] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_TWO_PLAYERS][GAME_CMD_GET_SCORE] = CommandHandlerGetScore;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_COMPUTER_AI][GAME_CMD_GET_SCORE] = CommandHandlerGetScore;

	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_TWO_PLAYERS][GAME_CMD_SAVE] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_COMPUTER_AI][GAME_CMD_SAVE] = CommandHandlerInvalid;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_TWO_PLAYERS][GAME_CMD_SAVE] = CommandHandlerSaveGame;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_COMPUTER_AI][GAME_CMD_SAVE] = CommandHandlerSaveGame;

	/* Quit */
	/* Valid in both settings states */
	/* Valid for both game modes */
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_TWO_PLAYERS][GENERAL_CMD_QUIT] = CommandHandlerQuit;
	m_cmdHandlers[FLOW_STATE_SETTINGS][GAME_MODE_COMPUTER_AI][GENERAL_CMD_QUIT] = CommandHandlerQuit;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_TWO_PLAYERS][GENERAL_CMD_QUIT] = CommandHandlerQuit;
	m_cmdHandlers[FLOW_STATE_GAME][GAME_MODE_COMPUTER_AI][GENERAL_CMD_QUIT] = CommandHandlerQuit;
}

#if 0
static void ChessCLIDisplayMoveStatusErrors(MOVE_STATUS status)
{
	switch (status)
	{
	case INVALID_BOARD_POSITION:
		CLI_PRINT(CLI_STR_INVALID_POSITION);
		break;

	case INVALID_PIECE:
		CLI_PRINT(CLI_STR_NO_PIECE_IN_POSITION);
		break;

	case ILLEGAL_BOARD_INITIALIZATION:
		CLI_PRINT(CLI_STR_WRONG_BOARD_INITIALIZATION);
		break;
		
	default:
		break;
	}	
}
#endif
// TODO CANCEL
static void ChessCLIDisplayCmdStatus(CMD_OPCODE opcode, COMMAND_STATUS cmdStatus)
{
	const char* cmdStatusString = m_cmdStatusMap[opcode][cmdStatus];
	DEBUG_PRINT("opcode=%d, Status=%d", opcode, cmdStatus);
	if (cmdStatusString != NULL)
	{
		CLI_PRINT("%s", cmdStatusString);
	}
}

// UT needs to be "friend", therefore not static

COMMAND ChessCLIParseUserCmd(char* pCmdBuffer)
{
	COMMAND cmd;
	char * pChar = NULL;

	int argCount = 0;
	char* context = NULL;
	const char* cmdString = NULL;
	char * pLineFeed;
	cmd.opcode = CMD_OPCODE_INVALID;

	//VERBOSE_PRINT("\"%s\"", pCmdBuffer);
	
	// need to get rid of line endings
	pLineFeed = strchr(m_cmdBuffer, '\n');
	if (NULL != pLineFeed)
	{ 
		*pLineFeed = '\0';
	}
	// of all kinds (unix/win)	
	pLineFeed = strchr(m_cmdBuffer, '\r');
	if (NULL != pLineFeed)
	{ 
		*pLineFeed = '\0';
	}	
	
	VERBOSE_PRINT("\"%s\"", pCmdBuffer);
	
	pChar = STRTOK_PORTABLE(pCmdBuffer, CLI_COMMAND_DELIMITER, &context);

	while (pChar != NULL && argCount < MAX_NUM_COMMAND_ARGS)
	{
		cmd.argv[argCount++] = pChar;
		pChar = STRTOK_PORTABLE(NULL, CLI_COMMAND_DELIMITER, &context);
	}
	if (0 == argCount)
	{
		PRINT_ERROR("Failure to get command");
		return cmd;
	}
	
	cmd.argc = argCount;
	cmdString = cmd.argv[0];
	cmd.opcode = ChessCLIMapCmdStringToOpcode(cmdString);
	VERBOSE_PRINT("cmd opcode = %d", cmd.opcode);
	return cmd;
}

COMMAND_STATUS ChessCLIHandleUserCmd(COMMAND userCmd)
{
	COMMAND_HANDLER cmdHandler;
	FLOW_STATE flowState = ChessControllerGetFlowState();
	GAME_MODE gameMode = ChessLogicGetGameMode();
	assert(0 <= flowState && flowState < FLOW_STATE_NUM);
	assert(0 <= userCmd.opcode && userCmd.opcode < NUM_OF_COMMANDS);
	assert(0 <= gameMode && gameMode < GAME_MODE_NUM);
	cmdHandler = m_cmdHandlers[flowState][gameMode][userCmd.opcode];

	FUNCTION_DEBUG_TRACE;
	return cmdHandler(userCmd);
}


