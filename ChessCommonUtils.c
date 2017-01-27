#include "ChessCommonUtils.h"
#include "ChessLogic.h"

CHESS_PIECE_TYPE ChessCommonUtilsConvertPieceTypeToColor(COLORLESS_CHESS_PIECE_TYPE pieceType)
{
	PLAYER_COLOR color = ChessLogicGetNextPlayer();
	switch (color)
	{
		case PLAYER_COLOR_WHITE:
			return (CHESS_PIECE_TYPE)pieceType;
		case PLAYER_COLOR_BLACK:
			return (CHESS_PIECE_TYPE)(pieceType + NUM_OF_COLORLESS_PIECE_TYPES);
		default:
			return PIECE_TYPE_INVALID;
	}
	return PIECE_TYPE_INVALID;
}

void ChessCommonUtilsPrintBoard(BOARD* pBOARD)
{
	int row, column;
	for (row = 0; row < BOARD_SIZE; row++)
	{
		for (column = 0; column < BOARD_SIZE; column++)
		{
			//VALIDATE_PIECE(piece);
			VERBOSE_PRINT("board[%d][%d] = %d", column, row, (*pBOARD)[column][row]);
		}
	}
	
}
