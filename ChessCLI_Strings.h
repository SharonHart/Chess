#ifndef CHESS_CLI_STRINGS_H
#define CHESS_CLI_STRINGS_H

/* Command Line Output Strings */
#define CLI_STR_ENTER_GAME_SETTINGS		"Enter game settings:\n" 
#define CLI_STR_ILLEGAL_COMMAND			"Illegal command, please try again\n"
#define CLI_STR_INVALID_POSITION		"Invalid position on the board\n"

/* Command Line Interface Commands */ 
#define GENERAL_CMD_QUIT_CLI_STRING	                       "quit"

#define SETTINGS_CMD_SET_GAME_MODE_CLI_STRING              "game_mode"
#define SETTINGS_CMD_ARG_GAME_MODE_TWO_PLAYERS_CLI_STRING  "1"
#define SETTINGS_CMD_ARG_GAME_MODE_COMPUTER_AI_CLI_STRING  "2"
#define CLI_STR_WRONG_GAME_MODE                            "Wrong game mode\n"
#define CLI_STR_TWO_PLAYERS_GAME_MODE                      "Running game in 2 players mode\n"
#define CLI_STR_PLAYER_VS_AI_GAME_MODE                     "Running game in player vs. AI mode\n"

#define SETTINGS_CMD_SET_DIFFICULTY_CLI_STRING	      "difficulty"   /* only AI Mode */
#define SETTINGS_CMD_ARG_DIFFICULTY_DEPTH_CLI_STRING  "depth"
#define SETTINGS_CMD_ARG_DIFFICULTY_BEST_CLI_STRING   "best"
#define CLI_STR_WRONG_MINIMAX_DEPTH                   "Wrong value for minimax depth. The value should be between 1 and 4\n"

#define SETTINGS_CMD_SET_USER_COLOR_CLI_STRING	      "user_color"   /* only AI Mode */
#define SETTINGS_CMD_ARG_USER_COLOR_WHITE_CLI_STRING  "white"
#define SETTINGS_CMD_ARG_USER_COLOR_BLACK_CLI_STRING  "black"
#define CLI_STR_WRONG_USER_COLOR                      "Wrong value for user color. The color should be white or black\n"

#define SETTINGS_CMD_LOAD_BOARD_CLI_STRING	         "load"
#define CLI_STR_MISSING_FILE_NAME_ARG                 "Invalid argument for file name to load. Load receives a full or a relative file path\n"
#define CLI_STR_WRONG_FILE_NAME                       "Wrong file name\n"

#define SETTINGS_CMD_CLEAR_BOARD_CLI_STRING	         "clear"
#define SETTINGS_CMD_SET_NEXT_PLAYER_CLI_STRING	      "next_player"

#define SETTINGS_CMD_REMOVE_PIECE_CLI_STRING	"rm"
#define CLI_STR_REMOVE_PIECE_INVALID_ARG		"Invalid argument. Usage: \"rm <x,y>\", x represents the column and y represents the row.\n" \
												"x should be [a-h], y should be [1-8]\n" 

#define SETTINGS_CMD_SET_PIECE_CLI_STRING		"set"
#define CLI_STR_SET_PIECE_INVALID_USAGE			"Invalid argument. Usage: \"set <x,y> a b\", this command places a piece of color a and type b in <x,y> position.\n" 
#define CLI_STR_INVALID_BOARD					"Setting this piece creates an invalid board\n"  

#define SETTINGS_CMD_ARG_PIECE_TYPE_DEFAULT_CLI_STRING	"queen"
#define SETTINGS_CMD_ARG_PIECE_TYPE_KING_CLI_STRING		"king"
#define SETTINGS_CMD_ARG_PIECE_TYPE_QUEEN_CLI_STRING	"queen"
#define SETTINGS_CMD_ARG_PIECE_TYPE_ROOK_CLI_STRING		"rook"
#define SETTINGS_CMD_ARG_PIECE_TYPE_KNIGHT_CLI_STRING	"knight"
#define SETTINGS_CMD_ARG_PIECE_TYPE_BISHOP_CLI_STRING	"bishop"
#define SETTINGS_CMD_ARG_PIECE_TYPE_PAWN_CLI_STRING		"pawn"

#define SETTINGS_CMD_PRE_GAME_PRINT_BOARD_CLI_STRING	"print"

#define SETTINGS_CMD_START_GAME_CLI_STRING			"start"
#define CLI_STR_WRONG_BOARD_INITIALIZATION			"Wrong board initialization\n"

#define GAME_CMD_MOVE_CLI_STRING				"move"
#define CLI_STR_MOVE_USAGE						"usage: \"move <x,y> to <i,j> [optional_promotion_piece_type]\"\n"
#define CLI_STR_ILLEGAL_MOVE					"Illegal move\n"
#define CLI_STR_NO_PIECE_IN_POSITION			"The specified position does not contain your piece\n"   

#define GAME_CMD_GET_MOVES_CLI_STRING            	"get_moves"
#define GAME_CMD_GET_BEST_MOVES_CLI_STRING        	"get_best_moves"
#define GAME_CMD_GET_SCORE_CLI_STRING            	"get_score"
#define GAME_CMD_SAVE_CLI_STRING                  	"save"
#define GAME_CMD_QUIT_GAME_CLI_STRING             	"quit"

#define CLI_STR_GET_MOVES_USAGE              "usage: \"get_moves <x,y>\"\n"
#define CLI_STR_GET_BEST_MOVES_USAGE         "usage: \"get_best_moves d\"\n"
#define CLI_STR_GET_SCORE_USAGE              "usage: \"get_score d move <x,y> to <i,j> x\"\n"
#define CLI_STR_SAVE_USAGE                   "usage: \"save filepath\"\n"

//#define CLI_STR_WRONG_ROOK_POSITION                "Wrong position for a rook\n" 
//#define CLI_STR_ILLEGAL_CALTLING_MOVE              "Illegal castling move\n"  

#define CLI_STR_PROMPT_MOVE						"%s player - enter your move:\n"
#define CLI_STR_DISPLAY_MOVE_NO_PROMOTION		"<%c,%c> to <%c,%c>\n"
#define CLI_STR_DISPLAY_MOVE_WITH_PROMOTION		"<%c,%c> to <%c,%c> %s\n"
#define CLI_STR_DISPLAY_COMPUTER_MOVE			"Computer: move <%c,%c> to <%c,%c> %s\n"
#define CLI_STR_GAME_TIE						"The game ends in a tie\n"
#define CLI_STR_CHECK							"Check!\n"
#define CLI_STR_CHECK_MATE						"Mate! %s player wins the game\n"
#define CLI_STR_WHITE_PLAYER					"White"
#define CLI_STR_BLACK_PLAYER					"Black"

// promotion representation
#define CLI_STRING_PIECE_TYPE_BLANK     ""
#define CLI_STRING_PIECE_TYPE_KING      "king"
#define CLI_STRING_PIECE_TYPE_QUEEN     "queen"
#define CLI_STRING_PIECE_TYPE_ROOK      "rook"
#define CLI_STRING_PIECE_TYPE_KNIGHT    "knight"
#define CLI_STRING_PIECE_TYPE_BISHOP    "bishop"
#define CLI_STRING_PIECE_TYPE_PAWN      "pawn"

/* Board CLI Representation */
#define  BOARD_DISPLAY_ROW_DIVIDER  " |-------------------------------|\n"
#define  BOARD_DISPLAY_ROW_HEADER  "   a   b   c   d   e   f   g   h  \n"

#define BLANK_POSITION_CLI_REPRESENTATION	" "
#define WHITE_PAWN_CLI_REPRESENTATION		"m"
#define WHITE_BISHOP_CLI_REPRESENTATION 	"b"
#define WHITE_KNIGHT_CLI_REPRESENTATION		"n"
#define WHITE_ROOK_CLI_REPRESENTATION		"r"
#define WHITE_QUEEN_CLI_REPRESENTATION		"q"
#define WHITE_KING_CLI_REPRESENTATION		"k"
#define BLACK_PAWN_CLI_REPRESENTATION		"M"
#define BLACK_BISHOP_CLI_REPRESENTATION		"B"
#define BLACK_KNIGHT_CLI_REPRESENTATION		"N"
#define BLACK_ROOK_CLI_REPRESENTATION		"R"
#define BLACK_QUEEN_CLI_REPRESENTATION		"Q"
#define BLACK_KING_CLI_REPRESENTATION		"K"



#endif