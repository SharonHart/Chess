#ifndef CHESS_COMMON_UTILS_H
#define CHESS_COMMON_UTILS_H

#include "CommonUtils.h"
#include "ChessCommonDefs.h"

#define VALIDATE_PIECE(piece)		assert((PIECE_TYPE_MIN <= (piece)) && ((piece) < NUM_OF_PIECE_TYPES));

#define VALIDATE_GAME_MODE(mode)		assert((0 <= (mode)) && ((mode) < GAME_MODE_NUM));
#define VALIDATE_GAME_DIFFICULTY(diff)	assert((0 <= (diff)) && ((diff) < GAME_DIFFICULTY_NUM));
#define VALIDATE_PLAYER_COLOR(col)		assert((0 <= (col)) && ((col) < PLAYER_COLOR_NUM));
#define VALIDATE_PLAYER_COLOR(col)		assert((0 <= (col)) && ((col) < PLAYER_COLOR_NUM));

#define VALIDATE_BOARD_LOCATION(loc)	\
{										\
	assert(0 <= (loc).row);				\
	assert((loc).row < BOARD_SIZE);		\
	assert(0 <= (loc).column);			\
	assert((loc).column < BOARD_SIZE);	\
}

#define VALIDATE_GAME_MOVE(move)					\
{													\
	VALIDATE_BOARD_LOCATION((move).origin);			\
	VALIDATE_BOARD_LOCATION((move).destination);	\
	VALIDATE_PIECE((move).newType);					\
}

#define DEBUG_PRINT_MOVE(move)	DEBUG_PRINT("move: <%d,%d> to <%d,%d>", (move).origin.column, (move).origin.row, (move).destination.column, (move).destination.row);

CHESS_PIECE_TYPE ChessCommonUtilsConvertPieceTypeToColor(COLORLESS_CHESS_PIECE_TYPE pieceType);
void ChessCommonUtilsPrintBoard(BOARD* pBOARD);
#endif