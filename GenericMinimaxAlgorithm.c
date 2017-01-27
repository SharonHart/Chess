#include "GenericMinimaxAlgorithm.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 

/* PUBLIC API METHODS IMPLEMENTATIONS */
int ChessMinimax(BOARD tempBoard, GAME_MOVE_PTR move, PLAYER_COLOR color, int minimaxDepth, PLAYER_COLOR maximizingPlayer, int alpha, int beta, void (*free_specific_list)(GAME_MOVE_PTR), 
					  GAME_MOVE_PTR (*GetAllMoves)(BOARD, PLAYER_COLOR,int), void (*BoardAfterMove)(BOARD, GAME_MOVE, BOARD), int(*BoardScore)(BOARD, PLAYER_COLOR)) {	
						  int bestScore, tempScore, finalScore;
						  BOARD newBoard;
						  GAME_MOVE_PTR moves, first;		
						  PLAYER_COLOR oppossiteColor = PLAYER_COLOR_WHITE;		
						  if (maximizingPlayer == PLAYER_COLOR_WHITE)
							  oppossiteColor = PLAYER_COLOR_BLACK;

						  BoardAfterMove(tempBoard, *move, newBoard);
						  finalScore = BoardScore(newBoard, color);

						  if (minimaxDepth == 1)
							  return finalScore;

						  if (finalScore == 50000 || finalScore == -50000 || finalScore == 25000 || finalScore == -25000)
							  return finalScore;

						  moves = GetAllMoves(newBoard, maximizingPlayer,1); // moves of the other player
						  first = moves;

						  if (maximizingPlayer == color) { // color is max
							  bestScore = -50000; // maximum
							  while (moves != NULL)
							  {

								  tempScore = ChessMinimax(newBoard, moves, color, minimaxDepth - 1, oppossiteColor, alpha, beta, free_specific_list, GetAllMoves, BoardAfterMove, BoardScore);
								  if (tempScore > bestScore)
									  bestScore = tempScore;
								  if (bestScore > alpha)
									  alpha = bestScore;				
								  if (beta < alpha)
									  break;
								  moves = moves->pNextMove;
							  }
						  }
						  else {
							  bestScore = 50000; 
							  while (moves != NULL) // minimum
							  {
								  tempScore = ChessMinimax(newBoard, moves, color, minimaxDepth - 1, oppossiteColor, alpha, beta, free_specific_list, GetAllMoves,BoardAfterMove, BoardScore);
								  if (tempScore < bestScore)			
									  bestScore = tempScore;	
								  if (bestScore < beta)
									  beta = bestScore;					
								  if (beta < alpha)
									  break;
								  moves = moves->pNextMove;
							  }
						  }
						  free_specific_list(first);
						  return bestScore;
}




