// un-comment to enable
//#define DEBUG_SERIALIZER

#ifdef __linux__ 
#include <string.h>	// strncmp

#include "GenericXMLInterface.h"
#include "ChessSerializer.h"
#include "CommonUtils.h"
#include "ChessCommonUtils.h"

#define MAX_ELEMENT_STRING_LENGTH			10
#define ROW_ELEMENT_NAME_LENGTH				5
#define SERIALIZED_GAME_TITLE				"game"
#define SERIALIZED_NEXT_TURN_TITLE			"next_turn"
#define SERIALIZED_GAME_MODE_TITLE			"game_mode"
#define SERIALIZED_DIFFICULTY_TITLE			"difficulty"
#define SERIALIZED_USER_COLOR_TITLE			"user_color"
#define SERIALIZED_BOARD_TITLE				"board"
#define SERIALIZED_ROW_TITLE_PREFIX			"row_"
#define ROW_PREFIX_LENGTH					4

#define SERIALIZED_COLOR_WHITE				"White"
#define SERIALIZED_COLOR_BLACK				"Black"
#define SERIALIZED_GAME_MODE_TWO_PLAYERS	"1"
#define SERIALIZED_GAME_MODE_COMPUTER_AI	"2"

#define SERIALIZED_DIFFICULTY_CONSTANT_1	"1"
#define SERIALIZED_DIFFICULTY_CONSTANT_2	"2"
#define SERIALIZED_DIFFICULTY_CONSTANT_3	"3"
#define SERIALIZED_DIFFICULTY_CONSTANT_4	"4"
#define SERIALIZED_DIFFICULTY_BEST			"best"

#define BLANK_POSITION_SERIALIZED	'_'
#define WHITE_PAWN_SERIALIZED		'm'
#define WHITE_BISHOP_SERIALIZED 	'b'
#define WHITE_KNIGHT_SERIALIZED		'n'
#define WHITE_ROOK_SERIALIZED		'r'
#define WHITE_QUEEN_SERIALIZED		'q'
#define WHITE_KING_SERIALIZED		'k'
#define BLACK_PAWN_SERIALIZED		'M'
#define BLACK_BISHOP_SERIALIZED		'B'
#define BLACK_KNIGHT_SERIALIZED		'N'
#define BLACK_ROOK_SERIALIZED		'R'
#define BLACK_QUEEN_SERIALIZED		'Q'
#define BLACK_KING_SERIALIZED		'K'

/* LOCAL DATA */
static const char* SerializeGameMode[GAME_MODE_NUM] = {SERIALIZED_GAME_MODE_TWO_PLAYERS, SERIALIZED_GAME_MODE_COMPUTER_AI};
static const char* SerializeNextTurn[PLAYER_COLOR_NUM] = {SERIALIZED_COLOR_WHITE, SERIALIZED_COLOR_BLACK};
static const char* SerializeUserColor[PLAYER_COLOR_NUM] = {SERIALIZED_COLOR_WHITE, SERIALIZED_COLOR_BLACK};
static const char* SerializeDifficulty[GAME_DIFFICULTY_NUM] = {SERIALIZED_DIFFICULTY_BEST, SERIALIZED_DIFFICULTY_CONSTANT_1, SERIALIZED_DIFFICULTY_CONSTANT_2, SERIALIZED_DIFFICULTY_CONSTANT_3, SERIALIZED_DIFFICULTY_CONSTANT_4};

static char SerializePiece[] =
{
	BLANK_POSITION_SERIALIZED, 
	WHITE_PAWN_SERIALIZED, 
	WHITE_BISHOP_SERIALIZED , 
	WHITE_KNIGHT_SERIALIZED, 
	WHITE_ROOK_SERIALIZED, 
	WHITE_QUEEN_SERIALIZED, 
	WHITE_KING_SERIALIZED, 
	BLACK_PAWN_SERIALIZED, 
	BLACK_BISHOP_SERIALIZED, 
	BLACK_KNIGHT_SERIALIZED, 
	BLACK_ROOK_SERIALIZED, 
	BLACK_QUEEN_SERIALIZED, 
	BLACK_KING_SERIALIZED	
};

static const char* SerializeRowTitle[BOARD_SIZE] = 
{
	"row_1",
	"row_2",
	"row_3",
	"row_4",
	"row_5",
	"row_6",
	"row_7",
	"row_8"
};

/* PRIVATE METHODS DECLARATIONS */
static CHESS_PIECE_TYPE DeserializePiece(char serializedPieceChar);
static void ParseRow(ChessSerialization *dataOut, XML_ELEMENT element);
static void ParseElement(ChessSerialization *dataOut, XML_ELEMENT element);
static void InitializeSerializationDefaults(ChessSerialization *dataOut);

/* PUBLIC API IMPLEMENTATION */
BOOL ChessSerialize(ChessSerialization dataIn, const char* filename)
{
	xmlTextWriterPtr writer;
	CHESS_BOOL res;

	DEBUG_PRINT("%s", filename);
	// DEBUG_PRINT("gameMode=%d", dataIn.gameMode);
	// DEBUG_PRINT("currPlayer=%d", dataIn.currPlayer);
	// DEBUG_PRINT("gameDifficulty=%d", dataIn.gameDifficulty);
	// DEBUG_PRINT("userColor=%d", dataIn.userColor);
	res = XMLStartDocumentWriter(filename, &writer);

	if (CHESS_FALSE == res)
	{
		PRINT_ERROR("failed serialization of: %s", filename);
		XMLEndDocumentWrite(writer);
		XMLTerminate();		
		return false;
	}
	assert(writer);
	// StartElement Game
	XMLStartElement(writer, SERIALIZED_GAME_TITLE);

	XMLWriteFormatElementContent(writer, SERIALIZED_NEXT_TURN_TITLE, SerializeNextTurn[dataIn.currPlayer]);
	XMLWriteFormatElementContent(writer, SERIALIZED_GAME_MODE_TITLE, SerializeGameMode[dataIn.gameMode]);
	XMLWriteFormatElementContent(writer, SERIALIZED_DIFFICULTY_TITLE, SerializeDifficulty[dataIn.gameDifficulty]);
	XMLWriteFormatElementContent(writer, SERIALIZED_USER_COLOR_TITLE, SerializeUserColor[dataIn.userColor]);
	// SerializeBoard
	XMLStartElement(writer, SERIALIZED_BOARD_TITLE);
	int rawRowIndex;
	int rawColumnIndex;
	char rowSerializationBuffer[BOARD_SIZE + 1];
	CHESS_PIECE_TYPE currPiece;
	char pieceChar;
	for (rawRowIndex = BOARD_SIZE - 1; rawRowIndex >= 0; rawRowIndex--)
	{
		for (rawColumnIndex = 0; rawColumnIndex < BOARD_SIZE; rawColumnIndex++)
		{
			currPiece = dataIn.board[rawColumnIndex][rawRowIndex];
			VALIDATE_PIECE(currPiece);
			pieceChar = SerializePiece[currPiece];
			rowSerializationBuffer[rawColumnIndex] = pieceChar;
		}
		rowSerializationBuffer[BOARD_SIZE] = '\0';
		XMLWriteFormatElementContent(writer, SerializeRowTitle[rawRowIndex], rowSerializationBuffer);
	}

	// EndElement Board
	XMLEndElement(writer);

	// EndElement Game
	XMLEndElement(writer);
	XMLEndDocumentWrite(writer);
	XMLTerminate();
	return true;
}

BOOL ChessDeserialize(ChessSerialization *dataOut, const char* filename)
{
	CHESS_BOOL res;
	CHESS_BOOL hasNext;
	XML_ELEMENT element;
	xmlTextReaderPtr reader = NULL;

	DEBUG_PRINT("%s", filename);
	InitializeSerializationDefaults(dataOut);
	res = XMLStartDocumentReader(filename, &reader);
	if (CHESS_FALSE == res || NULL == reader)
	{
		PRINT_ERROR("failed deserialization of: %s", filename);
		XMLEndDocumentRead(reader);
		XMLTerminate();		
		return false;
	}
	assert(reader);
	hasNext = XMLReadElement(reader, &element);
	while (hasNext)
	{
#ifdef DEBUG_SERIALIZER	
		VERBOSE_PRINT("element: %s, %s", element.name, element.value);
#endif		
		ParseElement(dataOut, element);
		XMLFreeElement(&element);
		hasNext = XMLReadElement(reader, &element);
	} // end while
#ifdef _DEBUG
	VERBOSE_PRINT("gameMode=%d, gameDifficulty=%d, userColor=%d, currPlayer=%d", 
	dataOut->gameMode, dataOut->gameDifficulty, dataOut->userColor, dataOut->currPlayer);
	ChessCommonUtilsPrintBoard(&(dataOut->board));
#endif	
	XMLEndDocumentRead(reader);
	XMLTerminate();
	return true;	
}



/* PRIVATE METHODS IMPLEMENTATIONS */

static CHESS_PIECE_TYPE DeserializePiece(char serializedPieceChar)
{
	//FUNCTION_DEBUG_TRACE;
	switch (serializedPieceChar)
	{
	case BLANK_POSITION_SERIALIZED:
		return BLANK_POSITION;

	case WHITE_PAWN_SERIALIZED:
		return WHITE_PAWN;

	case WHITE_BISHOP_SERIALIZED:
		return WHITE_BISHOP;

	case WHITE_KNIGHT_SERIALIZED:
		return WHITE_KNIGHT;

	case WHITE_ROOK_SERIALIZED:
		return WHITE_ROOK;

	case WHITE_QUEEN_SERIALIZED:
		return WHITE_QUEEN;

	case WHITE_KING_SERIALIZED:
		return WHITE_KING;

	case BLACK_PAWN_SERIALIZED:
		return BLACK_PAWN;

	case BLACK_BISHOP_SERIALIZED:
		return BLACK_BISHOP;

	case BLACK_KNIGHT_SERIALIZED:
		return BLACK_KNIGHT;

	case BLACK_ROOK_SERIALIZED:
		return BLACK_ROOK;

	case BLACK_QUEEN_SERIALIZED:
		return BLACK_QUEEN;

	case BLACK_KING_SERIALIZED:
		return BLACK_KING;		

	default:
		return BLANK_POSITION;
	}
	return BLANK_POSITION;
}

static void ParseRow(ChessSerialization *dataOut, XML_ELEMENT element)
{
	//FUNCTION_DEBUG_TRACE;
	char pSerializedRowIndex = element.name[4];
	int rawRowIndex = pSerializedRowIndex - '1';
	int rawColumnIndex;
	CHESS_PIECE_TYPE piece;
#ifdef DEBUG_SERIALIZER	
	VERBOSE_PRINT("\nrow='%c' ==> row=%d\n---------", pSerializedRowIndex, rawRowIndex);
#endif
	for (rawColumnIndex = 0; rawColumnIndex < BOARD_SIZE; rawColumnIndex++)
	{
		piece = DeserializePiece(element.value[rawColumnIndex]);
		//VERBOSE_PRINT("piece=%d", piece);
		VALIDATE_PIECE(piece);
		dataOut->board[rawColumnIndex][rawRowIndex] = piece;
	}
}

static void ParseElement(ChessSerialization *dataOut, XML_ELEMENT element)
{
	//FUNCTION_DEBUG_TRACE;
	assert(NULL != element.name);
	assert(dataOut);
	VERBOSE_PRINT("name=%s, value=%s", element.name, element.value);
	if (0 == strncmp(element.name, SERIALIZED_ROW_TITLE_PREFIX, ROW_PREFIX_LENGTH))
	{
		ParseRow(dataOut, element);
	}

	else if (0 == strncmp(element.name, SERIALIZED_NEXT_TURN_TITLE, MAX_ELEMENT_STRING_LENGTH))
	{
		if (0 == strncmp(element.value, SERIALIZED_COLOR_WHITE, MAX_ELEMENT_STRING_LENGTH))
		{
			dataOut->currPlayer = PLAYER_COLOR_WHITE;
		}			
		else if (0 == strncmp(element.value, SERIALIZED_COLOR_BLACK, MAX_ELEMENT_STRING_LENGTH))
		{
			dataOut->currPlayer = PLAYER_COLOR_BLACK;
		}
	}
	else if (0 == strncmp(element.name, SERIALIZED_GAME_MODE_TITLE, MAX_ELEMENT_STRING_LENGTH))
	{
		if (0 == strncmp(element.value, SERIALIZED_GAME_MODE_TWO_PLAYERS, MAX_ELEMENT_STRING_LENGTH))
		{
			dataOut->gameMode = GAME_MODE_TWO_PLAYERS;
		}			
		else if (0 == strncmp(element.value, SERIALIZED_GAME_MODE_COMPUTER_AI, MAX_ELEMENT_STRING_LENGTH))
		{
			dataOut->gameMode = GAME_MODE_COMPUTER_AI;
		}
	}
	else if (0 == strncmp(element.name, SERIALIZED_DIFFICULTY_TITLE, MAX_ELEMENT_STRING_LENGTH))
	{
		if (NULL == element.value)
		{
			dataOut->gameDifficulty = GAME_DIFFICULTY_DEFAULT;
		}
		else if (0 == strncmp(element.value, SERIALIZED_DIFFICULTY_CONSTANT_1, MAX_ELEMENT_STRING_LENGTH))
		{
			dataOut->gameDifficulty = GAME_DIFFICULTY_CONSTANT_1;
		}			
		else if (0 == strncmp(element.value, SERIALIZED_DIFFICULTY_CONSTANT_2, MAX_ELEMENT_STRING_LENGTH))
		{
			dataOut->gameDifficulty = GAME_DIFFICULTY_CONSTANT_2;
		}
		else if (0 == strncmp(element.value, SERIALIZED_DIFFICULTY_CONSTANT_3, MAX_ELEMENT_STRING_LENGTH))
		{
			dataOut->gameDifficulty = GAME_DIFFICULTY_CONSTANT_3;
		}
		else if (0 == strncmp(element.value, SERIALIZED_DIFFICULTY_CONSTANT_4, MAX_ELEMENT_STRING_LENGTH))
		{
			dataOut->gameDifficulty = GAME_DIFFICULTY_CONSTANT_4;
		}
		else if (0 == strncmp(element.value, SERIALIZED_DIFFICULTY_BEST, MAX_ELEMENT_STRING_LENGTH))
		{
			dataOut->gameDifficulty = GAME_DIFFICULTY_BEST;
		}		
	}
	else if (0 == strncmp(element.name, SERIALIZED_USER_COLOR_TITLE, MAX_ELEMENT_STRING_LENGTH))
	{
		if (NULL == element.value)
		{
			dataOut->userColor = PLAYER_COLOR_DEFAULT;
		}		
		else if (0 == strncmp(element.value, SERIALIZED_COLOR_WHITE, MAX_ELEMENT_STRING_LENGTH))
		{
			dataOut->userColor = PLAYER_COLOR_WHITE;
		}			
		else if (0 == strncmp(element.value, SERIALIZED_COLOR_BLACK, MAX_ELEMENT_STRING_LENGTH))
		{
			dataOut->userColor = PLAYER_COLOR_BLACK;
		}
	}
}	// ParseElement


static void InitializeSerializationDefaults(ChessSerialization *dataOut)
{
	dataOut->gameMode = GAME_MODE_DEFAULT;
	dataOut->gameDifficulty = GAME_DIFFICULTY_DEFAULT;
	dataOut->userColor = PLAYER_COLOR_DEFAULT;
	dataOut->currPlayer = PLAYER_COLOR_DEFAULT;
}

#endif // LINUX