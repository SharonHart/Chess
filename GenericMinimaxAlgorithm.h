#ifndef GENERIC_MINIMAX_ALGORITHM_H
#define GENERIC_MINIMAX_ALGORITHM_H

#include "ChessCommonDefs.h"

int ChessMinimax(BOARD, GAME_MOVE_PTR, PLAYER_COLOR, int, PLAYER_COLOR,int, int, void(*)(GAME_MOVE_PTR), GAME_MOVE_PTR(*)(BOARD, PLAYER_COLOR, int), void(*)(BOARD, GAME_MOVE, BOARD) , int(*)(BOARD, PLAYER_COLOR)); // return the score of the best move of the computer




#endif
#pragma once
