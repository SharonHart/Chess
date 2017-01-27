#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// #define DEBUG_LOGIC
#ifdef DEBUG_LOGIC
#define DEBUG_LOGIC_PRINT(...)	{				    \
	fprintf(stderr, "%s: ", __FUNCTION__);		\
	fprintf(stderr, __VA_ARGS__);		      	\
	fprintf(stderr, "\n");		            	\
	}
#else
#define DEBUG_LOGIC_PRINT(...)		;	
#endif
	
#include "ChessCommonDefs.h"
#include "ChessLogic.h"
#include "GenericMinimaxAlgorithm.h"
#include "CommonUtils.h"
#include "ChessCommonUtils.h"

#define MAX_MOVES_PER_PIECE 27

/* GLOBAL DATA */

/* LOCAL DATA */
#ifndef _DEBUG
/* static game settings */
static GAME_MODE gameMode = GAME_MODE_DEFAULT;
static GAME_DIFFICULTY gameDifficulty = GAME_DIFFICULTY_DEFAULT;
static PLAYER_COLOR userColor = PLAYER_COLOR_DEFAULT;

/* dynamic game data */
static BOARD board;
static PLAYER_COLOR currPlayer = PLAYER_COLOR_DEFAULT;
static GAME_MOVE_PTR userMoves;
static GAME_MOVE_PTR otherMoves;

#else	// for "friends"
/* static game settings */	
GAME_MODE gameMode = GAME_MODE_DEFAULT;
GAME_DIFFICULTY gameDifficulty = GAME_DIFFICULTY_DEFAULT;
PLAYER_COLOR userColor = PLAYER_COLOR_DEFAULT;

/* dynamic game data */
BOARD board;
PLAYER_COLOR currPlayer = PLAYER_COLOR_DEFAULT;
GAME_MOVE_PTR userMoves;
GAME_MOVE_PTR otherMoves;
#endif

/* PRIVATE METHODS DECLARATIONS */

int ChessLogicValidPlace(int, int); // checks that the position is valid, returns 1 if this is valid place
void ChessLogicCountPieces(BOARD, int *); // updates an array of pieces
int ChessLogicSetMorePieces(); // returns 1 if the board set caused problem
MOVE_STATUS ChessLogicCorrectColor(GAME_MOVE); // checks that the piece has the correct color
MOVE_STATUS ChessLogicLegalMove(GAME_MOVE); // checks if the move is legal, return 1 if its a legal move 
PLAYER_COLOR ChessLogicCheckColor(BOARD, int, int); // return the color of the piece located in that place
GAME_MOVE_PTR ChessInternalGetAllMoves(BOARD, PLAYER_COLOR,int);
void ChessLogicFreeMovesList(GAME_MOVE_PTR); // free a specefic list
MOVE_STATUS ChessLogicGetMovesPiece(BOARD, BOARD_LOCATION , GAME_MOVE_PTR*, PLAYER_COLOR,int); // return moves for piece, intarnal 
GAME_MOVE_PTR ChessLogicGetMovesPawn(BOARD, int, int, PLAYER_COLOR); // get moves for one pawn
GAME_MOVE_PTR ChessLogicGetMovesRook(BOARD, int, int, PLAYER_COLOR); // get moves for one rook
GAME_MOVE_PTR ChessLogicGetMovesKnight(BOARD, int, int, PLAYER_COLOR); // get moves for one knight
GAME_MOVE_PTR ChessLogicGetMovesBishop(BOARD, int, int, PLAYER_COLOR); // get moves for one bishop
GAME_MOVE_PTR ChessLogicGetMovesQueen(BOARD, int, int, PLAYER_COLOR); // get moves for one queen
GAME_MOVE_PTR ChessLogicGetMovesKing(BOARD, int, int, PLAYER_COLOR); // get moves for one king
void ChessLogicCreateBoardAfterMove(BOARD, GAME_MOVE, BOARD); // creates a temp board after a move 
GAME_MOVE_PTR ChessLogicCreateMove(int, int, int, int, GAME_MOVE_PTR, CHESS_PIECE_TYPE); // allocates and creates a move node
BOARD_LOCATION ChessLogicFindKing(BOARD, PLAYER_COLOR); // find the king
int ChessLogicIsCheck(BOARD, PLAYER_COLOR); // returns 1 if its a CHECK
int ChessLogicIsMate(BOARD, PLAYER_COLOR); //returns 1 if player color is under checkmate 		
int ChessLogicBoardScore(BOARD, PLAYER_COLOR);  // returns the score of the board


/* PUBLIC API METHODS IMPLEMENTATIONS */
void ChessLogicInitializeBoard() {
	int i,j;	
	FUNCTION_DEBUG_TRACE;
	for (j = 0; j < BOARD_SIZE; j++) { // pions
		board[j][1] = WHITE_PAWN;
		board[j][BOARD_SIZE - 2] = BLACK_PAWN;
	}
	//rooks
	board[0][0] = WHITE_ROOK;
	board[BOARD_SIZE - 1][0] = WHITE_ROOK;
	board[0][BOARD_SIZE - 1] = BLACK_ROOK;
	board[BOARD_SIZE - 1][BOARD_SIZE - 1] = BLACK_ROOK;
	//knighs
	board[1][0] = WHITE_KNIGHT;
	board[BOARD_SIZE - 2][0] = WHITE_KNIGHT;
	board[1][BOARD_SIZE - 1] = BLACK_KNIGHT;
	board[BOARD_SIZE - 2][BOARD_SIZE - 1] = BLACK_KNIGHT;
	//bishops
	board[2][0] = WHITE_BISHOP;
	board[5][0] = WHITE_BISHOP;
	board[2][BOARD_SIZE - 1] = BLACK_BISHOP;
	board[BOARD_SIZE - 3][BOARD_SIZE - 1] = BLACK_BISHOP;
	//queens
	board[3][0] = WHITE_QUEEN;
	board[3][BOARD_SIZE - 1] = BLACK_QUEEN;
	//kings
	board[4][0] = WHITE_KING;
	board[4][BOARD_SIZE - 1] = BLACK_KING;
	// rest of the board
	for (i = 0; i < BOARD_SIZE; i++)
		for (j = 2; j < BOARD_SIZE - 2; j++)
			board[i][j] = BLANK_POSITION;
}

void ChessLogicTerminate() {
	ChessLogicFreeMovesList(userMoves);
	ChessLogicFreeMovesList(otherMoves);
}
/* Settings Functionality */
void ChessLogicSetGameMode(GAME_MODE mode) {
	VALIDATE_GAME_MODE(mode);
	gameMode = mode;
}
void ChessLogicSetDifficulty(GAME_DIFFICULTY diff) {
	VALIDATE_GAME_DIFFICULTY(diff);
	gameDifficulty = diff;
}
void ChessLogicSetUserColor(PLAYER_COLOR color) {
	VALIDATE_PLAYER_COLOR(color);
	userColor = color;
}
void ChessLogicSetNextPlayer(PLAYER_COLOR color) {
	VALIDATE_PLAYER_COLOR(color);
	currPlayer = color;
}
void ChessLogicAdvanceNextPlayer() {
	if (currPlayer == PLAYER_COLOR_WHITE)
		currPlayer = PLAYER_COLOR_BLACK;
	else
		currPlayer = PLAYER_COLOR_WHITE;
}

void ChessLogicFreeMovesList(GAME_MOVE_PTR lst) {
	GAME_MOVE_PTR temp = NULL;
	VERBOSE_PRINT("list=%p", (void*)lst);
	while (lst)
	{
		temp = lst;
		lst = lst->pNextMove;
		free(temp);
	}
	VERBOSE_PRINT("last node freed=%p", (void*)temp);
#ifdef DEBUG_LOGIC	
	VERBOSE_PRINT("list=%p", (void*)lst);
#endif	
	temp = NULL;
}

MOVE_STATUS ChessLogicSetBoardPiece(BOARD_LOCATION place, PLAYER_COLOR color, CHESS_PIECE_TYPE type) {
	int x = place.column;
	int y = place.row;
	CHESS_PIECE_TYPE originType;
	DEBUG_PRINT("col=%d, row=%d, color=%d, type=%d", x, y, color, type);
	if (!ChessLogicValidPlace(x, y)) {
		DEBUG_PRINT("INVALID_BOARD_POSITION");
		return INVALID_BOARD_POSITION;
	}
	if ((y == 7 && type == WHITE_PAWN) || (y == 0 && type == BLACK_PAWN)) { // set in the last row not allowed 
		DEBUG_PRINT("returning INVALID_BOARD_POSITION");
		return INVALID_BOARD_POSITION;
	}
	if (!(((color == PLAYER_COLOR_WHITE) && (type == WHITE_BISHOP || type == WHITE_KING || type == WHITE_KNIGHT || type == WHITE_PAWN || type == WHITE_QUEEN || type == WHITE_ROOK)) ||
		((color == PLAYER_COLOR_BLACK) && (type == BLACK_BISHOP || type == BLACK_KING || type == BLACK_KNIGHT || type == BLACK_PAWN || type == BLACK_QUEEN || type == BLACK_ROOK)))) {
		DEBUG_PRINT("returning INVALID_PIECE");
		return INVALID_PIECE;
	}
	originType = board[x][y];
	board[x][y] = type;  // update the board temporarly
	if (ChessLogicSetMorePieces()) {
		board[x][y] = originType; // return the board to its origin state
		DEBUG_PRINT("returning ILLEGAL_BOARD_INITIALIZATION");
		return ILLEGAL_BOARD_INITIALIZATION;
	}
	DEBUG_PRINT("returning MOVE_SUCCESSFUL");
	return MOVE_SUCCESSFUL; // if evertythink was ok, success
}

MOVE_STATUS ChessLogicRemoveBoardPiece(BOARD_LOCATION place) {
	int x = place.column;
	int y = place.row;
	if (ChessLogicValidPlace(x, y)) {
		board[x][y] = BLANK_POSITION;
		DEBUG_PRINT("returning MOVE_SUCCESSFUL");
		return MOVE_SUCCESSFUL;
	}
	else {
		DEBUG_PRINT("returning INVALID_BOARD_POSITION");
		return INVALID_BOARD_POSITION;
	}
}

void ChessLogicClearBoard() {
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++)
		for (j = 0; j < BOARD_SIZE; j++)
			board[i][j] = BLANK_POSITION;
}

void ChessLogicResetDefaultSettings() { ///restars all the settings, insert their default value
	ChessLogicInitializeBoard();
	gameMode = GAME_MODE_DEFAULT;
	gameDifficulty = GAME_DIFFICULTY_DEFAULT;
	userColor = PLAYER_COLOR_DEFAULT;
	currPlayer = PLAYER_COLOR_DEFAULT;
}

/* Game Functionality */

MOVE_STATUS ChessLogicStartGame() {
	PLAYER_COLOR oppositeColor = PLAYER_COLOR_WHITE;	
	GAME_MOVE_PTR tempMoves = NULL;	
	BOARD_LOCATION whiteKingPlace = ChessLogicFindKing(board, PLAYER_COLOR_WHITE);
	BOARD_LOCATION blackKingPlace = ChessLogicFindKing(board, PLAYER_COLOR_BLACK);
	if ((whiteKingPlace.column == -1) || (blackKingPlace.column == -1)) {
		DEBUG_PRINT("returning ILLEGAL_BOARD_INITIALIZATION");
		return ILLEGAL_BOARD_INITIALIZATION;
	}
	if (userColor == PLAYER_COLOR_WHITE)
		oppositeColor = PLAYER_COLOR_BLACK;
	userMoves = ChessInternalGetAllMoves(board, userColor,1);
	otherMoves = ChessInternalGetAllMoves(board, oppositeColor,1);

	// check if the board started with check or tie, altough it was written in the forum that it wont be tested. 	
	if (ChessLogicIsCheck(board, currPlayer)) {
		DEBUG_PRINT("returning CHECK");
		return CHECK;
	}
	if (currPlayer == PLAYER_COLOR_WHITE) {
		tempMoves = ChessInternalGetAllMoves(board, PLAYER_COLOR_WHITE, 1);
	}
	else {
		tempMoves = ChessInternalGetAllMoves(board, PLAYER_COLOR_BLACK, 1);
	}	
	if (tempMoves == NULL) {	// the first player is under tie
		DEBUG_PRINT("returning GAME_TIE");
		return GAME_TIE;
	}
	ChessLogicFreeMovesList(tempMoves);	
	VERBOSE_PRINT("returning MOVE_SUCCESSFUL");
	return MOVE_SUCCESSFUL;
}

MOVE_STATUS ChessLogicPerformUserMove(GAME_MOVE move) {
	PLAYER_COLOR oppositeColor = PLAYER_COLOR_BLACK;
	int moveFound = 0, checkFlag = 0;	
	GAME_MOVE_PTR copyMoves;
	assert(0 <= move.newType && move.newType <= NUM_OF_PIECE_TYPES);
	DEBUG_PRINT("<%d,%d> --> <%d,%d> newType=%d", move.origin.column, move.origin.row, move.destination.column, move.destination.row, move.newType);
	// checks if start and end of moves are valid positions on the board
	if (ChessLogicLegalMove(move) != MOVE_SUCCESSFUL) {
		DEBUG_PRINT("returning INVALID_BOARD_POSITION");
		return INVALID_BOARD_POSITION; // message 1	
	}
	// checks if the position contains the correct color	
	if (ChessLogicCorrectColor(move) != MOVE_SUCCESSFUL) {
		DEBUG_PRINT("returning INVALID_PIECE");
		return INVALID_PIECE; // message 2	
	}
	if (currPlayer == userColor)
		copyMoves = userMoves; // copy the list in order not to miss the outputParamHeadOfListOfMoves move of the user
	else
		copyMoves = otherMoves; // copy the list in order not to miss the outputParamHeadOfListOfMoves move of the other player  or computer	
	while (copyMoves != NULL && moveFound == 0) { // checks that the move is in get_move list
		if (copyMoves->origin.column == move.origin.column && copyMoves->origin.row == move.origin.row && copyMoves->destination.column == move.destination.column && copyMoves->destination.row == move.destination.row)
			moveFound = 1;
		else
			copyMoves = (GAME_MOVE_PTR)(copyMoves->pNextMove);
	}
	if (moveFound == 0) {
		DEBUG_PRINT("returning ILLEGAL_MOVE");
		return ILLEGAL_MOVE;	
	}
	// perform the actual move
	if (board[move.origin.column][move.origin.row] == WHITE_PAWN && move.destination.row == BOARD_SIZE - 1) { //default promotion
		if (move.newType == BLANK_POSITION) {
			DEBUG_PRINT("returning PAWN_PROMOTION_REQUIRED");			
			return PAWN_PROMOTION_REQUIRED;
		}
		else // it is a promotion, but the newType is not blank
		{
			board[move.destination.column][move.destination.row] = move.newType;
			board[move.origin.column][move.origin.row] = BLANK_POSITION;
		}
	}
	else if (board[move.origin.column][move.origin.row] == BLACK_PAWN && move.destination.row == 0) {//default promotion
		if (move.newType == BLANK_POSITION) {
			DEBUG_PRINT("returning PAWN_PROMOTION_REQUIRED");			
			return PAWN_PROMOTION_REQUIRED;
		}
		else {  // it is a promotion, but the newType is not blank
			board[move.destination.column][move.destination.row] = move.newType;
			board[move.origin.column][move.origin.row] = BLANK_POSITION;
		}
	}
	else // it is not a promotion, do regular move. 
	{
		board[move.destination.column][move.destination.row] = board[move.origin.column][move.origin.row];
		board[move.origin.column][move.origin.row] = BLANK_POSITION;
	}	

	if (currPlayer == PLAYER_COLOR_BLACK)
		oppositeColor = PLAYER_COLOR_WHITE;

	if (ChessLogicIsCheck(board, oppositeColor))
		checkFlag = 1;

	if (ChessLogicIsMate(board, oppositeColor) && checkFlag){ // if there is no check, its tie 
		DEBUG_PRINT("returning CHECK_MATE");		
		return CHECK_MATE;
	}
	
	// free the last turn lists
	ChessLogicFreeMovesList(userMoves);
	ChessLogicFreeMovesList(otherMoves);

	// create new lists
	if (currPlayer == userColor) {
		userMoves = ChessInternalGetAllMoves(board, currPlayer,1);
		otherMoves = ChessInternalGetAllMoves(board, oppositeColor,1);
		if ((otherMoves == NULL) && (checkFlag == 0)) {
			//	ChessLogicFreeMovesList(userMoves);
			DEBUG_PRINT("returning GAME_TIE");		
			return GAME_TIE;
		}			
	}
	else {
		otherMoves = ChessInternalGetAllMoves(board, currPlayer,1);
		userMoves = ChessInternalGetAllMoves(board, oppositeColor,1);
		if ((userMoves == NULL) && (checkFlag == 0)) {
			//ChessLogicFreeMovesList(otherMoves);
			DEBUG_PRINT("returning GAME_TIE");		
			return GAME_TIE;
		}			
	}
	if (checkFlag == 1) {
		DEBUG_PRINT("returning CHECK");		
		return CHECK;
	}	
	DEBUG_PRINT("returning MOVE_SUCCESSFUL");		
	return MOVE_SUCCESSFUL;
}

MOVE_STATUS ChessLogicGetMoves(BOARD_LOCATION place, GAME_MOVE_PTR* outputParamHeadOfListOfMoves) {
	PLAYER_COLOR boardColor = ChessLogicCheckColor(board, place.column, place.row);
	// checks if start and end of moves are valid positiona on the board
	if (!ChessLogicValidPlace(place.column, place.row)) {
		DEBUG_PRINT("returning INVALID_BOARD_POSITION");
		return INVALID_BOARD_POSITION; // message 1		
	}
	if (currPlayer != boardColor) {
		DEBUG_PRINT("returning INVALID_PIECE");
		return INVALID_PIECE; // message 2
	}
	
	return ChessLogicGetMovesPiece(board, place, outputParamHeadOfListOfMoves, ChessLogicCheckColor(board, place.column, place.row),1);
}

MOVE_STATUS ChessLogicGetMovesPiece(BOARD board, BOARD_LOCATION place, GAME_MOVE_PTR* outputParamHeadOfListOfMoves, PLAYER_COLOR color,int filter) {	
	int x = place.column;
	int y = place.row;
	GAME_MOVE_PTR first = NULL;
	BOARD tempBoard;
	GAME_MOVE_PTR dummyMoves = NULL;
	GAME_MOVE_PTR currMove = NULL;
	GAME_MOVE_PTR prevMove = NULL;

	CHESS_PIECE_TYPE type = board[x][y];
	PLAYER_COLOR boardColor = ChessLogicCheckColor(board, x, y);

	// checks if start and end of moves are valid positiona on the board
	if (!ChessLogicValidPlace(x, y)) {
		DEBUG_PRINT("returning INVALID_BOARD_POSITION");
		return INVALID_BOARD_POSITION; // message 1		
	}
	// checks if the position contains the correct color	
	if (boardColor != color) {
		DEBUG_PRINT("returning INVALID_PIECE");
		return INVALID_PIECE; // message 2
	}
	
	if (color == PLAYER_COLOR_BLANK) {
		DEBUG_PRINT("returning INVALID_PIECE");
		return INVALID_PIECE; // message 2
	}

	// get moves of the right type_piece
	if (type == WHITE_PAWN || type == BLACK_PAWN)
		*outputParamHeadOfListOfMoves = ChessLogicGetMovesPawn(board, x, y, color);
	else if (type == WHITE_ROOK || type == BLACK_ROOK)
		*outputParamHeadOfListOfMoves = ChessLogicGetMovesRook(board, x, y, color);
	else if (type == WHITE_KNIGHT || type == BLACK_KNIGHT)
		*outputParamHeadOfListOfMoves = ChessLogicGetMovesKnight(board, x, y, color);
	else if (type == WHITE_BISHOP || type == BLACK_BISHOP)
		*outputParamHeadOfListOfMoves = ChessLogicGetMovesBishop(board, x, y, color);
	else if (type == WHITE_QUEEN || type == BLACK_QUEEN)
		*outputParamHeadOfListOfMoves = ChessLogicGetMovesQueen(board, x, y, color);
	else if (type == WHITE_KING || type == BLACK_KING)
		*outputParamHeadOfListOfMoves = ChessLogicGetMovesKing(board, x, y, color);
	first = *outputParamHeadOfListOfMoves;
	if (first == NULL) {
		DEBUG_LOGIC_PRINT("returning MOVE_SUCCESSFUL");
		return MOVE_SUCCESSFUL;
	}
	//if threatened, remove non-canceling threat moves
	if ((filter) && (ChessLogicIsCheck(board, color))) { //if color is under check
		ChessLogicCreateBoardAfterMove(board, *first, tempBoard);
		while (ChessLogicIsCheck(tempBoard, color)) {
			dummyMoves = first->pNextMove;
			free(first);
			first = dummyMoves;
			if (first == NULL) {
				*outputParamHeadOfListOfMoves = first;
				DEBUG_LOGIC_PRINT("returning MOVE_SUCCESSFUL");
				return MOVE_SUCCESSFUL;  // there are no possible moves , but is everything is ok
			}
			ChessLogicCreateBoardAfterMove(board, *first, tempBoard);
		}
		currMove = first->pNextMove;
		prevMove = first;
		while (currMove != NULL) {
			ChessLogicCreateBoardAfterMove(board, *currMove, tempBoard);
			if (!(ChessLogicIsCheck(tempBoard, color))) {
				prevMove->pNextMove = currMove;
				prevMove = prevMove->pNextMove;
				currMove = currMove->pNextMove;
			}
			else {
				dummyMoves = currMove->pNextMove;
				free(currMove);
				currMove = dummyMoves;
			}
		}
		prevMove->pNextMove = NULL;
	}
	// if not under check,  remove all the moves that doing them will result in check. 
	if (filter) {		
		ChessLogicCreateBoardAfterMove(board, *first, tempBoard);
		while (ChessLogicIsCheck(tempBoard, color)) {
			dummyMoves = first->pNextMove;
			free(first);
			first = dummyMoves;
			if (first == NULL) {
				*outputParamHeadOfListOfMoves = first;
				DEBUG_LOGIC_PRINT("returning MOVE_SUCCESSFUL");
				return MOVE_SUCCESSFUL;  // there are no possible moves , but is everything is ok
			}
			ChessLogicCreateBoardAfterMove(board, *first, tempBoard);
		}
		currMove = first->pNextMove;
		prevMove = first;
		while (currMove != NULL) {
			ChessLogicCreateBoardAfterMove(board, *currMove, tempBoard);
			if (!(ChessLogicIsCheck(tempBoard, color))) {
				prevMove->pNextMove = currMove;
				prevMove = prevMove->pNextMove;
				currMove = currMove->pNextMove;
			}
			else
			{
				dummyMoves = currMove->pNextMove;
				free(currMove);
				currMove = dummyMoves;
			}
		}
		prevMove->pNextMove = NULL;
	}

	*outputParamHeadOfListOfMoves = first;

	if (*outputParamHeadOfListOfMoves != NULL) { // there are moves! 
		DEBUG_LOGIC_PRINT("returning MOVE_SUCCESSFUL");
		return MOVE_SUCCESSFUL;
	}
	DEBUG_LOGIC_PRINT("returning MOVE_SUCCESSFUL");
	return MOVE_SUCCESSFUL;  // there are no possible moves , but is everything is ok
}

int convertDepthToInt(GAME_DIFFICULTY minimaxDpeth) {
	int total = 0;
	int countPieces[12] = { 0 };
	if (minimaxDpeth == GAME_DIFFICULTY_CONSTANT_1)
		return 1;
	if (minimaxDpeth == GAME_DIFFICULTY_CONSTANT_2)
		return 2;
	if (minimaxDpeth == GAME_DIFFICULTY_CONSTANT_3)
		return 3;
	if (minimaxDpeth == GAME_DIFFICULTY_CONSTANT_4)
		return 4;
	if (minimaxDpeth == GAME_DIFFICULTY_BEST) {
		ChessLogicCountPieces(board, countPieces);
		total += countPieces[0] * 3 + countPieces[1] * 14 + countPieces[2] * 8 + countPieces[3] * 14 + countPieces[4] * 27 + countPieces[5] * 8
			+ countPieces[6] * 3 + countPieces[7] * 14 + countPieces[8] * 8 + countPieces[9] * 14 + countPieces[10] * 27 + countPieces[11] * 8;
		return (int)(floor((log10(1000000)) / (log10(total))));
	}
	return -1;
}

void ChessLogicGetBestMoves(GAME_DIFFICULTY minimaxDpeth, GAME_MOVE_PTR* moves) {
	GAME_MOVE_PTR first;
	GAME_MOVE_PTR currMove = NULL;
	GAME_MOVE_PTR prevMove = NULL;	
	GAME_MOVE_PTR dummy = NULL;
	PLAYER_COLOR oppositeColor = PLAYER_COLOR_WHITE;
	PLAYER_COLOR oppositeToUserColor = PLAYER_COLOR_WHITE;
	int score = 0;	
	int maximum = -50000;	
	DEBUG_PRINT("difficulty=%d", minimaxDpeth);
	assert(moves);
	if (userColor == PLAYER_COLOR_WHITE)
		oppositeToUserColor = PLAYER_COLOR_BLACK;
	if (currPlayer == userColor) 
		first = ChessInternalGetAllMoves(board,userColor,1);
	else 
		first = ChessInternalGetAllMoves(board, oppositeToUserColor, 1);
	if (currPlayer == PLAYER_COLOR_WHITE)
		oppositeColor = PLAYER_COLOR_BLACK;	

	currMove = first;
	prevMove = first;
	while (currMove)
	{

		score = ChessMinimax(board, currMove, currPlayer, convertDepthToInt(minimaxDpeth), oppositeColor, -50000, 50000, ChessLogicFreeMovesList, ChessInternalGetAllMoves, ChessLogicCreateBoardAfterMove, ChessLogicBoardScore);
		if (score > maximum)
		{ // we change the first pointer of the moves_list
			if (first != currMove) {
				ChessLogicFreeMovesList(first);
			}
			first = currMove;
			prevMove = currMove;
			currMove = currMove->pNextMove;
			maximum = score;
			first->pNextMove = NULL; 
		}
		else if (score == maximum) {
			prevMove->pNextMove = currMove;
			prevMove = prevMove->pNextMove;
			currMove = currMove->pNextMove;
			prevMove->pNextMove = NULL;
		}
		else {
			dummy = currMove->pNextMove;
			free(currMove);
			currMove = dummy;
		}		
	}
	if(prevMove!=NULL)
		prevMove->pNextMove = NULL;
	*moves = first;
	DEBUG_PRINT("list=%p", (void*)first);
}

int ChessLogicGetScore(GAME_DIFFICULTY minimaxDepth, GAME_MOVE move) {
	PLAYER_COLOR oppositeColor = PLAYER_COLOR_WHITE;	
	if (currPlayer == PLAYER_COLOR_WHITE)
		oppositeColor = PLAYER_COLOR_BLACK;
	return ChessMinimax(board, &move, currPlayer, convertDepthToInt(minimaxDepth), oppositeColor, -50000, 50000, ChessLogicFreeMovesList, ChessInternalGetAllMoves, ChessLogicCreateBoardAfterMove, ChessLogicBoardScore);
}


GAME_MOVE ChessLogicGetNextComputerMove() {	
	GAME_MOVE_PTR first;
	GAME_MOVE_PTR bestMove= NULL;
	GAME_MOVE resultMove;
	PLAYER_COLOR oppositeColor = PLAYER_COLOR_WHITE;
	int res = 0, maximum = -50000;	
	first = otherMoves;

	if (userColor == PLAYER_COLOR_WHITE)
		oppositeColor = PLAYER_COLOR_BLACK;

	while (first)
	{
		res = ChessMinimax(board, first, oppositeColor, convertDepthToInt(gameDifficulty), userColor, -50000, 50000, ChessLogicFreeMovesList, ChessInternalGetAllMoves, ChessLogicCreateBoardAfterMove, ChessLogicBoardScore);
		if (res > maximum)
		{
			bestMove = first;
			maximum = res;			
		}
		first = first->pNextMove;
	}
	resultMove.origin.column = bestMove->origin.column;
	resultMove.origin.row = bestMove->origin.row;
	resultMove.destination.column = bestMove->destination.column;
	resultMove.destination.row = bestMove->destination.row;
	resultMove.pNextMove = bestMove->pNextMove;
	resultMove.newType =bestMove-> newType;

	return resultMove;	

}

MOVE_STATUS ChessLogicPerformNextComputerMove(GAME_MOVE move) {
	DEBUG_PRINT("<%d,%d> --> <%d,%d>", move.origin.column, move.origin.row, move.destination.column, move.destination.row);
	return ChessLogicPerformUserMove(move);
}

/* Load-Save */
GAME_MODE ChessLogicGetGameMode() {
	return gameMode;
}
GAME_DIFFICULTY ChessLogicGetDifficulty() {
	return gameDifficulty;
}
PLAYER_COLOR ChessLogicGetUserColor() {
	return userColor;
}
PLAYER_COLOR ChessLogicGetNextPlayer() {
	return currPlayer;
}

PLAYER_TYPE ChessLogicGetNextPlayerType() {
	if (currPlayer == userColor)
	{
		return PLAYER_TYPE_HUMAN;
	}
	else
	{
		return PLAYER_TYPE_COMPUTER_AI;
	}
}
BOARD* ChessLogicGetBoardReference() {
	return &board;
}

void ChessLogicGetBoardCopy(BOARD* pBoard) {
	int row, col;
	for (col = 0; col < BOARD_SIZE; col++) {
		for (row = 0; row < BOARD_SIZE; row++) {
			(*pBoard)[col][row] = board[col][row];
		}
	}
}

void ChessLogicLoadCompleteBoard(BOARD loadBoard) {
	int i, j;
	CHESS_PIECE_TYPE currPiece;
	for (i = 0; i < BOARD_SIZE; i++)
		for (j = 0; j < BOARD_SIZE; j++) {
			currPiece = loadBoard[i][j];
			VALIDATE_PIECE(currPiece);
			board[i][j] = currPiece;
		}
}



/* PRIVATE METHODS IMPLEMENTATIONS */

int ChessLogicValidPlace(int x, int y) {
	if ((x >= 0) && (x <= BOARD_SIZE - 1) && (y >= 0) && (y <= BOARD_SIZE - 1))
		return 1;
	return 0;
}


/* returns an arrays which represents the amount of pieces as bellow:
0 = white_pawn, 1 = white_rook, 2 = white_knight, 3 = white_bishop, 4 = white_queen, 5 = white_king
6 = black_pawn, 7 = black_rook , 8 = black_knight, 9 = black_bishop, 10 = black_queen, 11 = black_king  */
void ChessLogicCountPieces(BOARD board, int * countPieces) {
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++)
		for (j = 0; j < BOARD_SIZE; j++) { // count all pieces
			if (board[i][j] == WHITE_PAWN)
				countPieces[0] += 1;
			else if (board[i][j] == BLACK_PAWN)
				countPieces[6] += 1;
			else if (board[i][j] == WHITE_ROOK)
				countPieces[1] += 1;
			else if (board[i][j] == BLACK_ROOK)
				countPieces[7] += 1;
			else if (board[i][j] == WHITE_KNIGHT)
				countPieces[2] += 1;
			else if (board[i][j] == BLACK_KNIGHT)
				countPieces[8] += 1;
			else if (board[i][j] == WHITE_BISHOP)
				countPieces[3] += 1;
			else if (board[i][j] == BLACK_BISHOP)
				countPieces[9] += 1;
			else if (board[i][j] == WHITE_QUEEN)
				countPieces[4] += 1;
			else if (board[i][j] == BLACK_QUEEN)
				countPieces[10] += 1;
			else if (board[i][j] == WHITE_KING)
				countPieces[5] += 1;
			else if (board[i][j] == BLACK_KING)
				countPieces[11] += 1;
		}
}

int ChessLogicSetMorePieces() {
	int countPieces[12] = { 0 };
	ChessLogicCountPieces(board, countPieces);
	if (countPieces[5] > 1 || countPieces[11] > 1 || countPieces[4] > 1 || countPieces[10] > 1 || countPieces[3] > 2 || countPieces[9] > 2 || countPieces[2] > 2 || countPieces[8] > 2 || countPieces[1] > 2 || countPieces[7] > 2 || countPieces[0] > 8 || countPieces[6] > 8)
		return 1;
	return 0;
}

MOVE_STATUS ChessLogicCorrectColor(GAME_MOVE move) {
	int x = move.origin.column;
	int y = move.origin.row;
	PLAYER_COLOR board_piece_color = ChessLogicCheckColor(board, x, y);
	if (board_piece_color != currPlayer)
		return INVALID_PIECE;
	return MOVE_SUCCESSFUL;
}

MOVE_STATUS ChessLogicLegalMove(GAME_MOVE move) {
	if ((!(ChessLogicValidPlace(move.origin.column, move.origin.row))) || (!(ChessLogicValidPlace(move.destination.column, move.destination.row))))
		return INVALID_BOARD_POSITION;
	return MOVE_SUCCESSFUL;
}

PLAYER_COLOR ChessLogicCheckColor(BOARD board, int x, int y) {
	PLAYER_COLOR board_piece_color = PLAYER_COLOR_BLANK;
	CHESS_PIECE_TYPE type = board[x][y];	
	if (type == WHITE_PAWN || type == WHITE_ROOK || type == WHITE_KNIGHT || type == WHITE_BISHOP || type == WHITE_QUEEN || type == WHITE_KING)
		board_piece_color = PLAYER_COLOR_WHITE;
	if (type == BLACK_PAWN || type == BLACK_ROOK || type == BLACK_KNIGHT || type == BLACK_BISHOP || type == BLACK_QUEEN || type == BLACK_KING)
		board_piece_color = PLAYER_COLOR_BLACK;
	return board_piece_color;
}


GAME_MOVE_PTR ChessLogicGetMovesPawn(BOARD board, int x, int y, PLAYER_COLOR color) {
	GAME_MOVE_PTR first = NULL;
	GAME_MOVE_PTR currMove = NULL;
	GAME_MOVE_PTR newMove = NULL;
	PLAYER_COLOR oppositeColor = PLAYER_COLOR_WHITE;
	if (color == PLAYER_COLOR_WHITE) {
		oppositeColor = PLAYER_COLOR_BLACK;
		if (ChessLogicValidPlace(x, y + 1) && board[x][y + 1] == BLANK_POSITION) { // one step forward
			if (y + 1 != BOARD_SIZE - 1){ 
				newMove = ChessLogicCreateMove(x, y, x, y + 1, NULL, BLANK_POSITION);
				currMove = newMove;
				first = currMove;
			}
			else{ //white promotion
				newMove = ChessLogicCreateMove(x, y, x, y + 1, NULL, WHITE_QUEEN);
				currMove = newMove;
				first = currMove;
				newMove = ChessLogicCreateMove(x, y, x, y + 1, NULL, WHITE_BISHOP);
				currMove->pNextMove = newMove;
				currMove = currMove->pNextMove;
				newMove = ChessLogicCreateMove(x, y, x, y + 1, NULL, WHITE_KNIGHT);
				currMove->pNextMove = newMove;
				currMove = currMove->pNextMove;
				newMove = ChessLogicCreateMove(x, y, x, y + 1, NULL, WHITE_ROOK);
				currMove->pNextMove = newMove;
				currMove = currMove->pNextMove;
			}
		}
		// one step diagonal right, eat
		if (ChessLogicValidPlace(x + 1, y + 1) && ChessLogicCheckColor(board, x + 1, y + 1) == oppositeColor) {
			if (y + 1 != BOARD_SIZE - 1) {
				newMove = ChessLogicCreateMove(x, y, x + 1, y + 1, NULL, BLANK_POSITION);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
				if (first == NULL)
					first = currMove;
			}
			else { //white promotion
				newMove = ChessLogicCreateMove(x, y, x+1, y + 1, NULL, WHITE_QUEEN);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
				if (first == NULL)
					first = currMove;
				newMove = ChessLogicCreateMove(x, y, x+1, y + 1, NULL, WHITE_BISHOP);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
				newMove = ChessLogicCreateMove(x, y, x + 1, y + 1, NULL, WHITE_KNIGHT);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
				newMove = ChessLogicCreateMove(x, y, x + 1, y + 1, NULL, WHITE_ROOK);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
			}
		}
		// one step diagonal left, eat
		if (ChessLogicValidPlace(x - 1, y + 1) && ChessLogicCheckColor(board, x - 1, y + 1) == oppositeColor) {
			if (y + 1 != BOARD_SIZE - 1) {
				newMove = ChessLogicCreateMove(x, y, x - 1, y + 1, NULL, BLANK_POSITION);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
				if (first == NULL)
					first = currMove;
			}
			else { //white promotion
				newMove = ChessLogicCreateMove(x, y, x - 1, y + 1, NULL, WHITE_QUEEN);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
				if (first == NULL)
					first = currMove;
				newMove = ChessLogicCreateMove(x, y, x - 1, y + 1, NULL, WHITE_BISHOP);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
				newMove = ChessLogicCreateMove(x, y, x - 1, y + 1, NULL, WHITE_KNIGHT);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
				newMove = ChessLogicCreateMove(x, y, x - 1, y + 1, NULL, WHITE_ROOK);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
			}

		}
	}
	else // color is black
	{
		if (ChessLogicValidPlace(x, y - 1) && board[x][y - 1] == BLANK_POSITION) { // one step backward (forward)
			if (y  - 1 != 0){
				newMove = ChessLogicCreateMove(x, y, x, y - 1, NULL, BLANK_POSITION);
				currMove = newMove;
				first = currMove;
			}
			else{ //black promotion
				newMove = ChessLogicCreateMove(x, y, x, y - 1, NULL, BLACK_QUEEN);
				currMove = newMove;
				first = currMove;
				newMove = ChessLogicCreateMove(x, y, x, y - 1, NULL, BLACK_BISHOP);
				currMove->pNextMove = newMove;
				currMove = currMove->pNextMove;
				newMove = ChessLogicCreateMove(x, y, x, y - 1, NULL, BLACK_KNIGHT);
				currMove->pNextMove = newMove;
				currMove = currMove->pNextMove;
				newMove = ChessLogicCreateMove(x, y, x, y - 1, NULL, BLACK_ROOK);
				currMove->pNextMove = newMove;
				currMove = currMove->pNextMove;

			}
		
		}
		// one step diagonal right, eat
		if (ChessLogicValidPlace(x - 1, y - 1) && ChessLogicCheckColor(board, x - 1, y - 1) == oppositeColor) {
			if (y - 1 != 0) {
				newMove = ChessLogicCreateMove(x, y, x - 1, y - 1, NULL, BLANK_POSITION);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
				if (first == NULL)
					first = currMove;
			}
			else { //black promotion
				newMove = ChessLogicCreateMove(x, y, x - 1, y - 1, NULL, BLACK_QUEEN);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
				if (first == NULL)
					first = currMove;
				newMove = ChessLogicCreateMove(x, y, x - 1, y- 1, NULL, BLACK_BISHOP);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
				newMove = ChessLogicCreateMove(x, y, x - 1, y - 1, NULL, BLACK_KNIGHT);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
				newMove = ChessLogicCreateMove(x, y, x - 1, y - 1, NULL, BLACK_ROOK);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
			}
		}
		// one step diagonal left, eat
		if (ChessLogicValidPlace(x + 1, y - 1) && ChessLogicCheckColor(board, x + 1, y - 1) == oppositeColor) {
			if (y - 1 != 0) {
				newMove = ChessLogicCreateMove(x, y, x + 1, y - 1, NULL, BLANK_POSITION);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
				if (first == NULL)
					first = currMove;
			}
			else { //black promotion
				newMove = ChessLogicCreateMove(x, y, x + 1, y - 1, NULL, BLACK_QUEEN);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
				if (first == NULL)
					first = currMove;
				newMove = ChessLogicCreateMove(x, y, x + 1, y - 1, NULL, BLACK_BISHOP);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
				newMove = ChessLogicCreateMove(x, y, x + 1, y - 1, NULL, BLACK_KNIGHT);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
				newMove = ChessLogicCreateMove(x, y, x + 1, y - 1, NULL, BLACK_ROOK);
				if (currMove != NULL)
					currMove->pNextMove = newMove;
				currMove = newMove;
			}
		}
	}
	if (first != NULL)
		if (first->pNextMove != NULL)
			currMove->pNextMove = NULL;
	return first;
}

GAME_MOVE_PTR ChessLogicGetMovesRook(BOARD board, int x, int y, PLAYER_COLOR color) {
	GAME_MOVE_PTR first = NULL;
	GAME_MOVE_PTR currMove = NULL;
	GAME_MOVE_PTR newMove = NULL;
	int startx = x, starty= y;	
	PLAYER_COLOR oppositeColor = PLAYER_COLOR_WHITE;
	if (color == PLAYER_COLOR_WHITE)
		oppositeColor = PLAYER_COLOR_BLACK;

	//up 
	while (ChessLogicValidPlace(x, y + 1) && board[x][y + 1] == BLANK_POSITION) {
		newMove = ChessLogicCreateMove(startx, starty, x, y + 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
		y += 1;
	}
	if (ChessLogicValidPlace(x, y + 1) && ChessLogicCheckColor(board, x, y + 1) == oppositeColor) { // it isnt BLANK_POSITION, try to eat
		newMove = ChessLogicCreateMove(startx, starty, x, y + 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	//left
	x = startx;
	y = starty;
	while (ChessLogicValidPlace(x - 1, y) && board[x - 1][y] == BLANK_POSITION) {
		newMove = ChessLogicCreateMove(startx, starty, x - 1, y, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
		x -= 1;
	}
	if (ChessLogicValidPlace(x - 1, y) && ChessLogicCheckColor(board, x - 1, y) == oppositeColor) { // it isnt BLANK_POSITION, try to eat
		newMove = ChessLogicCreateMove(startx, starty, x - 1, y, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	x = startx;
	y = starty;
	//down
	while (ChessLogicValidPlace(x, y - 1) && board[x][y - 1] == BLANK_POSITION) {
		newMove = ChessLogicCreateMove(startx, starty, x, y - 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
		y -= 1;
	}
	if (ChessLogicValidPlace(x, y - 1) && ChessLogicCheckColor(board, x, y - 1) == oppositeColor) { // it isnt BLANK_POSITION, try to eat
		newMove = ChessLogicCreateMove(startx, starty, x, y - 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	x = startx;
	y = starty;
	//right
	while (ChessLogicValidPlace(x + 1, y) && board[x + 1][y] == BLANK_POSITION) {
		newMove = ChessLogicCreateMove(startx, starty, x + 1, y, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
		x += 1;
	}
	if (ChessLogicValidPlace(x + 1, y) && ChessLogicCheckColor(board, x + 1, y) == oppositeColor) { // it isnt BLANK_POSITION, try to eat
		newMove = ChessLogicCreateMove(startx, starty, x + 1, y, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	if (first != NULL)
		if (first->pNextMove != NULL)
			currMove->pNextMove = NULL;
	return first;
}

GAME_MOVE_PTR ChessLogicGetMovesKnight(BOARD board, int x, int y, PLAYER_COLOR color) {
	GAME_MOVE_PTR first = NULL;
	GAME_MOVE_PTR currMove = NULL;
	GAME_MOVE_PTR newMove = NULL;
	PLAYER_COLOR oppositeColor = PLAYER_COLOR_WHITE;
	if (color == PLAYER_COLOR_WHITE)
		oppositeColor = PLAYER_COLOR_BLACK;
	// up left
	if (ChessLogicValidPlace(x - 1, y + 2) && (board[x - 1][y + 2] == BLANK_POSITION || ChessLogicCheckColor(board, x - 1, y + 2) == oppositeColor)) {
		newMove = ChessLogicCreateMove(x, y, x - 1, y + 2, NULL, BLANK_POSITION);
		currMove = newMove;
		first = currMove;
	}
	// up right
	if (ChessLogicValidPlace(x + 1, y + 2) && (board[x + 1][y + 2] == BLANK_POSITION || ChessLogicCheckColor(board, x + 1, y + 2) == oppositeColor)) {
		newMove = ChessLogicCreateMove(x, y, x + 1, y + 2, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	// down left
	if (ChessLogicValidPlace(x - 1, y - 2) && (board[x - 1][y - 2] == BLANK_POSITION || ChessLogicCheckColor(board, x - 1, y - 2) == oppositeColor)) {
		newMove = ChessLogicCreateMove(x, y, x - 1, y - 2, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	// down right
	if (ChessLogicValidPlace(x + 1, y - 2) && (board[x + 1][y - 2] == BLANK_POSITION || ChessLogicCheckColor(board, x + 1, y - 2) == oppositeColor)) {
		newMove = ChessLogicCreateMove(x, y, x + 1, y - 2, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	// left up
	if (ChessLogicValidPlace(x - 2, y + 1) && (board[x - 2][y + 1] == BLANK_POSITION || ChessLogicCheckColor(board, x - 2, y + 1) == oppositeColor)) {
		newMove = ChessLogicCreateMove(x, y, x - 2, y + 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	// right up
	if (ChessLogicValidPlace(x + 2, y + 1) && (board[x + 2][y + 1] == BLANK_POSITION || ChessLogicCheckColor(board, x + 2, y + 1) == oppositeColor)) {
		newMove = ChessLogicCreateMove(x, y, x + 2, y + 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	// left down
	if (ChessLogicValidPlace(x - 2, y - 1) && (board[x - 2][y - 1] == BLANK_POSITION || ChessLogicCheckColor(board, x - 2, y - 1) == oppositeColor)) {
		newMove = ChessLogicCreateMove(x, y, x - 2, y - 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	// right down
	if (ChessLogicValidPlace(x + 2, y - 1) && (board[x + 2][y - 1] == BLANK_POSITION || ChessLogicCheckColor(board, x + 2, y - 1) == oppositeColor)) {
		newMove = ChessLogicCreateMove(x, y, x + 2, y - 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	if (first != NULL)
		if (first->pNextMove != NULL)
			currMove->pNextMove = NULL;
	return first;
}


GAME_MOVE_PTR ChessLogicGetMovesBishop(BOARD board, int x, int y, PLAYER_COLOR color) {
	GAME_MOVE_PTR first = NULL;
	GAME_MOVE_PTR currMove = NULL;
	GAME_MOVE_PTR newMove = NULL;
	PLAYER_COLOR oppositeColor = PLAYER_COLOR_WHITE;
	int startx = x, starty = y;	
	if (color == PLAYER_COLOR_WHITE)
		oppositeColor = PLAYER_COLOR_BLACK;

	//up right
	while (ChessLogicValidPlace(x + 1, y + 1) && board[x + 1][y + 1] == BLANK_POSITION) {
		newMove = ChessLogicCreateMove(startx, starty, x + 1, y + 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
		x += 1;
		y += 1;
	}
	if (ChessLogicValidPlace(x + 1, y + 1) && ChessLogicCheckColor(board, x + 1, y + 1) == oppositeColor) { // it isnt BLANK_POSITION, try to eat
		newMove = ChessLogicCreateMove(startx, starty, x + 1, y + 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	//up left
	x = startx;
	y = starty;
	while (ChessLogicValidPlace(x - 1, y + 1) && board[x - 1][y + 1] == BLANK_POSITION) {
		newMove = ChessLogicCreateMove(startx, starty, x - 1, y + 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
		x -= 1;
		y += 1;
	}
	if (ChessLogicValidPlace(x - 1, y + 1) && ChessLogicCheckColor(board, x - 1, y + 1) == oppositeColor) { // it isnt BLANK_POSITION, try to eat
		newMove = ChessLogicCreateMove(startx, starty, x - 1, y + 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	x = startx;
	y = starty;
	//down left
	while (ChessLogicValidPlace(x - 1, y - 1) && board[x - 1][y - 1] == BLANK_POSITION) {
		newMove = ChessLogicCreateMove(startx, starty, x - 1, y - 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
		x -= 1;
		y -= 1;
	}
	if (ChessLogicValidPlace(x - 1, y - 1) && ChessLogicCheckColor(board, x - 1, y - 1) == oppositeColor) { // it isnt BLANK_POSITION, try to eat
		newMove = ChessLogicCreateMove(startx, starty, x - 1, y - 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	x = startx;
	y = starty;
	//down right
	while (ChessLogicValidPlace(x + 1, y - 1) && board[x + 1][y - 1] == BLANK_POSITION) {
		newMove = ChessLogicCreateMove(startx, starty, x + 1, y - 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
		x += 1;
		y -= 1;
	}
	if (ChessLogicValidPlace(x + 1, y - 1) && ChessLogicCheckColor(board, x + 1, y - 1) == oppositeColor) { // it isnt BLANK_POSITION, try to eat
		newMove = ChessLogicCreateMove(startx, starty, x + 1, y - 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	if (first != NULL)
		if (first->pNextMove != NULL)
			currMove->pNextMove = NULL;
	return first;
}

GAME_MOVE_PTR ChessLogicGetMovesQueen(BOARD board, int x, int y, PLAYER_COLOR color) {
	GAME_MOVE_PTR first = NULL;
	GAME_MOVE_PTR currMove = NULL;
	first = ChessLogicGetMovesBishop(board, x, y, color);
	currMove = first;
	if (currMove == NULL) {
		first=ChessLogicGetMovesRook(board, x, y, color);
		return first;
	}
	while (currMove->pNextMove != NULL)
		currMove = currMove->pNextMove;
	currMove->pNextMove = ChessLogicGetMovesRook(board, x, y, color);
	return first;
}

GAME_MOVE_PTR ChessLogicGetMovesKing(BOARD board, int x, int y, PLAYER_COLOR color) {
	GAME_MOVE_PTR first = NULL;
	GAME_MOVE_PTR currMove = NULL;
	GAME_MOVE_PTR newMove = NULL;
	PLAYER_COLOR oppositeColor = PLAYER_COLOR_WHITE;
	if (color == PLAYER_COLOR_WHITE)
		oppositeColor = PLAYER_COLOR_BLACK;
	if (ChessLogicValidPlace(x, y + 1) && (board[x][y + 1] == BLANK_POSITION || ChessLogicCheckColor(board, x, y + 1) == oppositeColor)) {
		newMove = ChessLogicCreateMove(x, y, x, y + 1, NULL, BLANK_POSITION);
		currMove = newMove;
		first = currMove;
	}
	// one step left
	if (ChessLogicValidPlace(x - 1, y) && (board[x - 1][y] == BLANK_POSITION || ChessLogicCheckColor(board, x - 1, y) == oppositeColor)) {
		newMove = ChessLogicCreateMove(x, y, x - 1, y, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	// one step down
	if (ChessLogicValidPlace(x, y - 1) && (board[x][y - 1] == BLANK_POSITION || ChessLogicCheckColor(board, x, y - 1) == oppositeColor)) {
		newMove = ChessLogicCreateMove(x, y, x, y - 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	// one step right
	if (ChessLogicValidPlace(x + 1, y) && (board[x + 1][y] == BLANK_POSITION || ChessLogicCheckColor(board, x + 1, y) == oppositeColor)) {
		newMove = ChessLogicCreateMove(x, y, x + 1, y, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	// one step diagonal right up
	if (ChessLogicValidPlace(x + 1, y + 1) && (board[x + 1][y + 1] == BLANK_POSITION || ChessLogicCheckColor(board, x + 1, y + 1) == oppositeColor)) {
		newMove = ChessLogicCreateMove(x, y, x + 1, y + 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	// one step diagonal left up
	if (ChessLogicValidPlace(x - 1, y + 1) && (board[x - 1][y + 1] == BLANK_POSITION || ChessLogicCheckColor(board, x - 1, y + 1) == oppositeColor)) {
		newMove = ChessLogicCreateMove(x, y, x - 1, y + 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	// one step diagonal right down
	if (ChessLogicValidPlace(x + 1, y - 1) && (board[x + 1][y - 1] == BLANK_POSITION || ChessLogicCheckColor(board, x + 1, y - 1) == oppositeColor)) {
		newMove = ChessLogicCreateMove(x, y, x + 1, y - 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	// one step diagonal left down
	if (ChessLogicValidPlace(x - 1, y - 1) && (board[x - 1][y - 1] == BLANK_POSITION || ChessLogicCheckColor(board, x - 1, y - 1) == oppositeColor)) {
		newMove = ChessLogicCreateMove(x, y, x - 1, y - 1, NULL, BLANK_POSITION);
		if (currMove != NULL)
			currMove->pNextMove = newMove;
		currMove = newMove;
		if (first == NULL)
			first = currMove;
	}
	if (first != NULL)
		if (first->pNextMove != NULL)
			currMove->pNextMove = NULL;
	return first;
}

//filter is for filtering non-check moves or not
GAME_MOVE_PTR ChessInternalGetAllMoves(BOARD board, PLAYER_COLOR color, int filter) {
	BOARD_LOCATION m;
	GAME_MOVE_PTR first = NULL;
	GAME_MOVE_PTR currMove = NULL;
	GAME_MOVE_PTR newMoves = NULL;
	//calculate all possible moves
	int x, y;
	for (x = 0; x < BOARD_SIZE; x++) {
		for (y = 0; y < BOARD_SIZE; y++) {
			if (ChessLogicCheckColor(board, x, y) == color) {
				m.column = x;
				m.row = y;
				ChessLogicGetMovesPiece(board, m, &newMoves, color, filter);
				if (first == NULL)
					first = newMoves;
				if (currMove != NULL)
					currMove->pNextMove = newMoves;
				else
					currMove = newMoves;
				while (currMove != NULL && currMove->pNextMove != NULL)
					currMove = currMove->pNextMove;
			}
		}
	}
	return first;

}



GAME_MOVE_PTR ChessLogicCreateMove(int originx, int originy, int destx, int desty, GAME_MOVE_PTR pNextMove, CHESS_PIECE_TYPE newType) {
	GAME_MOVE_PTR newMove = NULL;
	newMove = (GAME_MOVE_PTR)malloc(sizeof(GAME_MOVE));
	if (newMove == NULL) {
		ChessLogicTerminate();
		exit(0);
	}
	newMove->origin.column = originx;
	newMove->origin.row = originy;
	newMove->destination.column = destx;
	newMove->destination.row = desty;
	newMove->pNextMove = pNextMove;
	newMove->newType = newType;
	return newMove;
}




BOARD_LOCATION ChessLogicFindKing(BOARD board, PLAYER_COLOR color) {
	int x, y;
	CHESS_PIECE_TYPE type = WHITE_KING;
	BOARD_LOCATION kingLocation;
	kingLocation.column = -1;
	kingLocation.row = -1;	
	if (color == PLAYER_COLOR_BLACK)
		type = BLACK_KING;
	for (x = 0; x < BOARD_SIZE; x++) {
		for (y = 0; y < BOARD_SIZE; y++) {
			if (board[x][y] == type) {
				kingLocation.column = x;
				kingLocation.row = y;
			}
		}
	}
	return kingLocation;
}

//returns true if player color is under check 
int ChessLogicIsCheck(BOARD board, PLAYER_COLOR color) {
	BOARD_LOCATION kingLoc;
	int xKingLoc;
	int yKingLoc;
	int x;
	int y;	
	CHESS_PIECE_TYPE oppRook = WHITE_ROOK;
	CHESS_PIECE_TYPE oppKnight = WHITE_KNIGHT;
	CHESS_PIECE_TYPE oppBishop = WHITE_BISHOP;
	CHESS_PIECE_TYPE oppQueen = WHITE_QUEEN;
	CHESS_PIECE_TYPE oppPawn = WHITE_PAWN;
	CHESS_PIECE_TYPE oppKing = WHITE_KING;	
	if (color == PLAYER_COLOR_WHITE) {
		oppRook = BLACK_ROOK;
		oppKnight = BLACK_KNIGHT;
		oppBishop = BLACK_BISHOP;
		oppQueen = BLACK_QUEEN;
		oppPawn = BLACK_PAWN;
		oppKing = BLACK_KING;			
	}
	kingLoc = ChessLogicFindKing(board, color);
	xKingLoc = kingLoc.column;
	yKingLoc = kingLoc.row;
	

	//check by rook or queen:
	x = xKingLoc + 1;
	y = yKingLoc;
	while (ChessLogicValidPlace(x, y) && (!(ChessLogicCheckColor(board, x, y) == color)) && (!(board[x][y] == oppPawn || board[x][y] == oppKnight || board[x][y] == oppBishop || board[x][y] == oppKing))) {
		if (board[x][y] == oppRook || board[x][y] == oppQueen)
			return 1;
		x++;
	}
	x = xKingLoc - 1;
	y = yKingLoc;
	while (ChessLogicValidPlace(x, y) && (!(ChessLogicCheckColor(board, x, y) == color)) && (!(board[x][y] == oppPawn || board[x][y] == oppKnight || board[x][y] == oppBishop || board[x][y] == oppKing))) {
		if (board[x][y] == oppRook || board[x][y] == oppQueen)
			return 1;
		x--;
	}
	x = xKingLoc;
	y = yKingLoc - 1;
	while (ChessLogicValidPlace(x, y) && (!(ChessLogicCheckColor(board, x, y) == color)) && (!(board[x][y] == oppPawn || board[x][y] == oppKnight || board[x][y] == oppBishop || board[x][y] == oppKing))) {
		if (board[x][y] == oppRook || board[x][y] == oppQueen)
			return 1;
		y--;
	}
	x = xKingLoc;
	y = yKingLoc + 1;
	while (ChessLogicValidPlace(x, y) && (!(ChessLogicCheckColor(board, x, y) == color)) && (!(board[x][y] == oppPawn || board[x][y] == oppKnight || board[x][y] == oppBishop || board[x][y] == oppKing))) {
		if (board[x][y] == oppRook || board[x][y] == oppQueen)
			return 1;
		y++;
	}

	//check by knight
	if ((ChessLogicValidPlace(xKingLoc - 1, yKingLoc + 2) && board[xKingLoc - 1][yKingLoc + 2] == oppKnight)
		|| (ChessLogicValidPlace(xKingLoc + 1, yKingLoc + 2) && board[xKingLoc + 1][yKingLoc + 2] == oppKnight)
		|| (ChessLogicValidPlace(xKingLoc + 1, yKingLoc - 2) && board[xKingLoc + 1][yKingLoc - 2] == oppKnight)
		|| (ChessLogicValidPlace(xKingLoc - 1, yKingLoc - 2) && board[xKingLoc - 1][yKingLoc - 2] == oppKnight))
		return 1;
	if ((ChessLogicValidPlace(xKingLoc - 2, yKingLoc + 1) && board[xKingLoc - 2][yKingLoc + 1] == oppKnight)
		|| (ChessLogicValidPlace(xKingLoc + 2, yKingLoc + 1) && board[xKingLoc + 2][yKingLoc + 1] == oppKnight)
		|| (ChessLogicValidPlace(xKingLoc - 2, yKingLoc - 1) && board[xKingLoc - 2][yKingLoc - 1] == oppKnight)
		|| (ChessLogicValidPlace(xKingLoc + 2, yKingLoc - 1) && board[xKingLoc + 2][yKingLoc - 1] == oppKnight))
		return 1;


	//check by bishop or queen
	x = xKingLoc + 1;
	y = yKingLoc + 1;
	while (ChessLogicValidPlace(x, y) && (!(ChessLogicCheckColor(board, x, y) == color)) && (!(board[x][y] == oppPawn || board[x][y] == oppKnight || board[x][y] == oppRook || board[x][y] == oppKing))) {
		if (board[x][y] == oppBishop || board[x][y] == oppQueen)
			return 1;
		x++;
		y++;
	}
	x = xKingLoc + 1;
	y = yKingLoc - 1;
	while (ChessLogicValidPlace(x, y) && (!(ChessLogicCheckColor(board, x, y) == color)) && (!(board[x][y] == oppPawn || board[x][y] == oppKnight || board[x][y] == oppRook || board[x][y] == oppKing))) {
		if (board[x][y] == oppBishop || board[x][y] == oppQueen)
			return 1;
		x++;
		y--;
	}
	x = xKingLoc - 1;
	y = yKingLoc - 1;
	while (ChessLogicValidPlace(x, y) && (!(ChessLogicCheckColor(board, x, y) == color)) && (!(board[x][y] == oppPawn || board[x][y] == oppKnight || board[x][y] == oppRook || board[x][y] == oppKing))) {
		if (board[x][y] == oppBishop || board[x][y] == oppQueen)
			return 1;
		x--;
		y--;
	}
	x = xKingLoc - 1;
	y = yKingLoc + 1;
	while (ChessLogicValidPlace(x, y) && (!(ChessLogicCheckColor(board, x, y) == color)) && (!(board[x][y] == oppPawn || board[x][y] == oppKnight || board[x][y] == oppRook || board[x][y] == oppKing))) {
		if (board[x][y] == oppBishop || board[x][y] == oppQueen)
			return 1;
		x--;
		y++;
	}

	//check by pawn
	if (oppPawn == BLACK_PAWN && ((ChessLogicValidPlace(xKingLoc - 1, yKingLoc + 1) && board[xKingLoc - 1][yKingLoc + 1] == BLACK_PAWN) || (ChessLogicValidPlace(xKingLoc + 1, yKingLoc + 1) && board[xKingLoc + 1][yKingLoc + 1] == BLACK_PAWN)))
		return 1;
	if (oppPawn == WHITE_PAWN && ((ChessLogicValidPlace(xKingLoc - 1, yKingLoc - 1) && board[xKingLoc - 1][yKingLoc - 1] == WHITE_PAWN) || (ChessLogicValidPlace(xKingLoc + 1, yKingLoc - 1) && board[xKingLoc + 1][yKingLoc - 1] == WHITE_PAWN)))
		return 1;

	//check by king
	if ((ChessLogicValidPlace(xKingLoc - 1, yKingLoc - 1) && board[xKingLoc - 1][yKingLoc - 1] == oppKing)
		|| (ChessLogicValidPlace(xKingLoc + 1, yKingLoc - 1) && board[xKingLoc + 1][yKingLoc - 1] == oppKing)
		|| (ChessLogicValidPlace(xKingLoc + 1, yKingLoc + 1) && board[xKingLoc + 1][yKingLoc + 1] == oppKing)
		|| (ChessLogicValidPlace(xKingLoc - 1, yKingLoc + 1) && board[xKingLoc - 1][yKingLoc + 1] == oppKing))
		return 1;
	if ((ChessLogicValidPlace(xKingLoc - 1, yKingLoc) && board[xKingLoc - 1][yKingLoc] == oppKing)
		|| (ChessLogicValidPlace(xKingLoc + 1, yKingLoc) && board[xKingLoc + 1][yKingLoc] == oppKing)
		|| (ChessLogicValidPlace(xKingLoc, yKingLoc + 1) && board[xKingLoc][yKingLoc + 1] == oppKing)
		|| (ChessLogicValidPlace(xKingLoc, yKingLoc - 1) && board[xKingLoc][yKingLoc - 1] == oppKing))
		return 1;

	return 0;
}

//returns true if player color is under checkmate 
int ChessLogicIsMate(BOARD board, PLAYER_COLOR color) {
	BOARD_LOCATION kingLocation;
	PLAYER_COLOR oppositeColor = PLAYER_COLOR_BLACK;
	BOARD tempBoard;
	int kingEaten;
	GAME_MOVE_PTR playerMoves, counterMoves, dummyplayerMoves, dummycounterMoves; // how can COLOR respond	
	if (color == PLAYER_COLOR_BLACK)
		oppositeColor = PLAYER_COLOR_WHITE;	
	playerMoves = ChessInternalGetAllMoves(board, color,1);		
	dummyplayerMoves = playerMoves;
	while (playerMoves != NULL) {
		kingEaten = 0;
		ChessLogicCreateBoardAfterMove(board, *playerMoves, tempBoard);
		counterMoves = ChessInternalGetAllMoves(tempBoard, oppositeColor,0); // *** need to do free malloc ** //
		dummycounterMoves = counterMoves;
		kingLocation = ChessLogicFindKing(tempBoard, color);
		while (counterMoves != NULL) {
			if (counterMoves->destination.column == kingLocation.column && counterMoves->destination.row == kingLocation.row) {
				kingEaten = 1;
				break;
			}			
			counterMoves = counterMoves->pNextMove;
		}
		ChessLogicFreeMovesList(dummycounterMoves);		
		if (!kingEaten) {				
			ChessLogicFreeMovesList(dummyplayerMoves);
			return 0;
		}
		playerMoves = playerMoves->pNextMove;
	}
	ChessLogicFreeMovesList(dummyplayerMoves);	
	return 1;
}

void ChessLogicCreateBoardAfterMove(BOARD board, GAME_MOVE move, BOARD newBoard) {
	int x, y;
	for (x = 0; x < BOARD_SIZE; x++) {
		for (y = 0; y < BOARD_SIZE; y++) {
			newBoard[x][y] = board[x][y];
		}
	}

	// perform the actual move
		// promotion
	if (board[move.origin.column][move.origin.row] == WHITE_PAWN && move.destination.row == BOARD_SIZE - 1) { //default promotion
		if (move.newType == BLANK_POSITION) {
			move.newType = WHITE_QUEEN;
			
		}
		newBoard[move.destination.column][move.destination.row] = move.newType;
	}
	else if (board[move.origin.column][move.origin.row] == BLACK_PAWN && move.destination.row == 0) { //default promotion
		if (move.newType == BLANK_POSITION) {
			move.newType = BLACK_QUEEN;
			
		}
		newBoard[move.destination.column][move.destination.row] = move.newType;
	}
		// not a promotion
	else
	{
		newBoard[move.destination.column][move.destination.row] = board[move.origin.column][move.origin.row];
	}
	newBoard[move.origin.column][move.origin.row] = BLANK_POSITION;


	/*
	if (board[move.origin.column][move.origin.row] == WHITE_PAWN && move.newType == BLANK_POSITION && move.destination.row == BOARD_SIZE - 1) //default promotion correction
		move.newType = WHITE_QUEEN;
	if (board[move.origin.column][move.origin.row] == BLACK_PAWN && move.newType == BLANK_POSITION &&  move.destination.row == 0) //default promotion correction
		move.newType = BLACK_QUEEN;
	if (move.newType == BLANK_POSITION)
		newBoard[move.destination.column][move.destination.row] = board[move.origin.column][move.origin.row];
	else
		newBoard[move.destination.column][move.destination.row] = move.newType;
	newBoard[move.origin.column][move.origin.row] = BLANK_POSITION;
}
*/
}


/* Mate score: 50000,-50000   Tie score: 25000,-25000 */
int ChessLogicBoardScore(BOARD board, PLAYER_COLOR color) {
	int count_black = 0, count_white = 0, result = 0, flagCheckColor = 0, flagCheckOpposite = 0;
	int i, j;
	GAME_MOVE_PTR colorMoves = NULL;
	GAME_MOVE_PTR oppositeMoves = NULL;
	PLAYER_COLOR oppositeColor = PLAYER_COLOR_WHITE;
	if (color == PLAYER_COLOR_WHITE)
		oppositeColor = PLAYER_COLOR_BLACK;

	// checks mate
	if (ChessLogicIsMate(board, color)) // if color is under mate
		return -50000;
	if (ChessLogicIsMate(board, oppositeColor))  // the opponent is under mate, color wins
		return 50000;
	//check
	if (ChessLogicIsCheck(board, color))
		flagCheckColor = 1;
	if (ChessLogicIsCheck(board, oppositeColor))
		flagCheckOpposite = 1;
	colorMoves = ChessInternalGetAllMoves(board, color,1);
	oppositeMoves = ChessInternalGetAllMoves(board, oppositeColor,1);
	// tie
	if (flagCheckColor == 0 && colorMoves == NULL) {
		//ChessLogicFreeMovesList(oppositeMoves);
		return -250000;
	}
	if (flagCheckOpposite == 0 && oppositeMoves == NULL) {
		//	ChessLogicFreeMovesList(colorMoves);
		return 25000;
	}

	ChessLogicFreeMovesList(colorMoves);
	ChessLogicFreeMovesList(oppositeMoves);


	for (i = 0; i < BOARD_SIZE; i++)
		for (j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == WHITE_PAWN)
				count_white += 1;
			else if (board[i][j] == WHITE_KNIGHT)
				count_white += 3;
			else if (board[i][j] == WHITE_BISHOP)
				count_white += 3;
			else if (board[i][j] == WHITE_ROOK)
				count_white += 5;
			else if (board[i][j] == WHITE_QUEEN)
				count_white += 9;
			else if (board[i][j] == WHITE_KING)
				count_white += 400;
			else if (board[i][j] == BLACK_PAWN)
				count_black += 1;
			else if (board[i][j] == BLACK_KNIGHT)
				count_black += 3;
			else if (board[i][j] == BLACK_BISHOP)
				count_black += 3;
			else if (board[i][j] == BLACK_ROOK)
				count_black += 5;
			else if (board[i][j] == BLACK_QUEEN)
				count_black += 9;
			else if (board[i][j] == BLACK_KING)
				count_black += 400;
		}
		result = count_white - count_black;
		if (color == PLAYER_COLOR_BLACK)
			result = result * -1;

		return result;
}
