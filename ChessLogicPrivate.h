#ifndef CHESS_LOGIC_PRIVATE_H
#define CHESS_LOGIC_PRIVATE_H

extern GAME_MODE gameMode;
extern GAME_DIFFICULTY gameDifficulty;
extern PLAYER_COLOR userColor;

/* dynamic game data */
extern BOARD board;
extern PLAYER_COLOR currPlayer;
extern GAME_MOVE_PTR userMoves;
extern GAME_MOVE_PTR otherMoves;

#endif
#pragma once
