/********************************************************************************************************************

***********************************
PROGRAM: REVERSI OTHELO GAME *
***********************************

CREATED BY: AREEB TARIQ
DATE: FEBRUARY 20 2022

DESCRIPTION:
____________
	This program implements the Reversi game against a human player by implementing alpha-beta method. 

	The size of the board (space) is user defined NxN.
	The disk color for human is a choice of the user
	The user can choose the value of 'level' which is equivalent to the number of levels the alpha-beta method should proceed in order to apply its evaluation function at the states of the last level.

PROCESS FLOW:
_____________

	The program sequentially asks for following initial information before proceeding with the game:
		a. newgame black|white <size> <level>
		b. 'play <move>' command like playb3. This is the position human player wishes to play.
		c. 'cont' command for the computer player to play its move. The current state is printed afterwards, indicating by a * all legal moves for the human player.
		d. 'showstate' command prints the state of current board  indicating by a * all legal moves for the human player, if it is his/her turn to play.
		e. 'quit' command is to quit the game

*********************************************************************************************************************/
/*Include header files needed for the implementation*/
#include "stdio.h"
#include "stdbool.h"
#include "string.h"

#pragma warning(disable : 4996)

/******************/
/*Global MACROS*/
/******************/

#define ZERO 0																/*Macro for magic number '0'*/
#define MOVE_NOT_ALLOWED 0													/*Macro for move not allowed*/
#define MOVE_ALLOWED 1														/*Macro for move allowed*/
#define INITIAL_LEVEL 1														/*Macro for initial level*/
#define COMPUTER_TURN 1														/*Macro for computer turn*/
#define COMPUTER_PLAYER 1
#define USER_TURN 2
#define USER_PLAYER 2
#define INITIAL_BLACK_DISKS 2
#define INITIAL_WHITE_DISKS 2
#define INVALID_VALUE 0xFFFFFF
#define VERY_NEGATIVE_VALUE -999
#define VERY_POSITIVE_VALUE  999

//Alpha_Beta_Return max_value(Board_Cell** current_board, int alpha_value, int beta_value, int search_level);
//Alpha_Beta_Return min_value(Board_Cell** current_board, int alpha_value, int beta_value, int search_level);

typedef enum Game_Status
{
	CONTINUE,
	COMPLETE,
	QUIT
}Game_Status;

typedef struct
{
	int row_index;
	int column_index;
	int value;
}Alpha_Beta_Return;

typedef struct
{
	int board_row;
	int row_index;
	char board_column;
	int column_index;
}Cell_Coordinates;

typedef struct
{
	char cell_disk_color;
	Cell_Coordinates cell_coordinates;
}Board_Cell;


int board_size									= INVALID_VALUE;
int game_level									= INVALID_VALUE;							
int current_turn								= INVALID_VALUE;
int current_black_disks							= INITIAL_BLACK_DISKS;
int current_white_disks							= INITIAL_WHITE_DISKS;
int current_level								= INITIAL_LEVEL;
char user_disk_color[6];
char computer_disk_color[6];
char* current_input_command						= NULL;
Board_Cell** board;								/*NxN grid*/

int global_alpha_value, global_beta_value;

void reset_parameters()
{
	game_level = INVALID_VALUE;
	current_turn = INVALID_VALUE;
	current_black_disks = INITIAL_BLACK_DISKS;
	current_white_disks = INITIAL_WHITE_DISKS;

	free(current_input_command);

	for (int i = 0; i < board_size; i++)
	{
		if (board[i] != NULL)
		{
			free(board[i]);
		}
	}
	if (board != NULL)
	{
		free(board);
	}
	board_size = INVALID_VALUE;

}

void switch_the_turn()
{
	if (current_turn == COMPUTER_TURN)
	{
		current_turn = USER_TURN;
	}
	else if (current_turn == USER_TURN)
	{
		current_turn = COMPUTER_TURN;
	}
}

bool is_opponent_disk(Board_Cell** current_board, Cell_Coordinates coordinates, char opponent_color)
{
	if (current_board[coordinates.row_index][coordinates.column_index].cell_disk_color == opponent_color)
	{
		return true;
	}
	return false;
}

bool is_player_disk(Board_Cell** current_board, Cell_Coordinates coordinates, char player_color)
{
	if (current_board[coordinates.row_index][coordinates.column_index].cell_disk_color == player_color)
	{
		return true;
	}
	return false;
}

bool is_empty_cell(Board_Cell** current_board, Cell_Coordinates coordinates)
{
	if (current_board[coordinates.row_index][coordinates.column_index].cell_disk_color == ' ')
	{
		return true;
	}
	return false;
}

bool is_cell_within_board_boundary(Cell_Coordinates coordinates)
{
	if (coordinates.row_index >= 0 && coordinates.row_index < board_size && coordinates.column_index >= 0 && coordinates.column_index < board_size)
	{
		return true;
	}
	return false;
}

char get_character_from_color(char* color)
{
	if (strcmp(color, "White") == 0)
	{
		return 'W';
	}
	else if (strcmp(color, "Black") == 0)
	{
		return 'B';
	}
}

bool is_move_allowed(int** move_board, Cell_Coordinates coordinates)
{
	if (move_board[coordinates.row_index][coordinates.column_index] == MOVE_ALLOWED)
	{
		return true;
	}
	return false;
}

bool no_valid_moves_exist(int** moves_board)
{
	for (int i = 0; i < board_size; i++)
	{
		for (int j = 0; j < board_size;j++)
		{
			if (moves_board[i][j] == MOVE_ALLOWED)
			{
				return false;
			}
		}
	}
	return true;
}

void update_disks_count(Board_Cell** current_board)
{
	int w_count = ZERO;
	int b_count = ZERO;

	for(int i = 0; i < board_size; i++)
	{
		for (int j = 0; j < board_size; j++)
		{
			if (current_board[i][j].cell_disk_color == 'W')
			{
				w_count++;
			}
			else if (current_board[i][j].cell_disk_color == 'B')
			{
				b_count++;
			}
		}
	}

	current_black_disks = b_count;
	current_white_disks = w_count;
}

int get_the_score(Board_Cell** current_board, int player_type)
{
	int net_score = 0;
	char player_color, opponent_color;
	Cell_Coordinates current_coordinates;

	if (player_type == USER_PLAYER)
	{
		player_color = get_character_from_color(user_disk_color);
		opponent_color = get_character_from_color(computer_disk_color);
	}
	else
	{
		player_color = get_character_from_color(computer_disk_color);
		opponent_color = get_character_from_color(user_disk_color);
	}


	for (current_coordinates.row_index = 0; current_coordinates.row_index < board_size; current_coordinates.row_index++)
	{
		for (current_coordinates.column_index = 0; current_coordinates.column_index < board_size; current_coordinates.column_index++)
		{
			if (current_board[current_coordinates.row_index][current_coordinates.column_index].cell_disk_color == player_color)
			{
				net_score++;
			}
			else if (current_board[current_coordinates.row_index][current_coordinates.column_index].cell_disk_color == opponent_color)
			{
				net_score--;
			}
		}
	}
	return net_score;
}

int min(int a, int b)
{
	if (a < b)
	{
		return a;
	}
	return b;
}

int max(int a, int b)
{
	if (a > b)
	{
		return a;
	}
	return b;
}

Board_Cell** create_the_board()
{
	Board_Cell** board;

	board = (Board_Cell**)malloc(board_size * sizeof(Board_Cell*));

	//printf("\n ");
	for (int i = 0; i < board_size; i++)
	{
		board[i] = (Board_Cell*)malloc(board_size * sizeof(Board_Cell));
		//printf("   %c", i + 'a');
	}

	for (int i = 0; i < board_size; i++)
	{
		
		for (int j = 0; j < board_size; j++)
		{
			board[i][j].cell_coordinates.board_row = i + 1;
			board[i][j].cell_coordinates.row_index = i;
			board[i][j].cell_coordinates.column_index = j;
			board[i][j].cell_coordinates.board_column = j + 'a';
			

			if (i + 1 == board_size / 2 && j + 1 == board_size / 2)
			{
				board[i][j].cell_disk_color = 'W';
			}
			else if (i + 1 == board_size / 2 && j + 1 == (board_size / 2) +1)
			{
				board[i][j].cell_disk_color = 'B';
			}
			else if (i + 1 == (board_size / 2) + 1 && j + 1 == board_size / 2)
			{
				board[i][j].cell_disk_color = 'B';
			}
			else if (i + 1 == (board_size / 2) + 1 && j + 1 == (board_size / 2) + 1)
			{
				board[i][j].cell_disk_color = 'W';
			}
			else 
			{
				board[i][j].cell_disk_color = ' ';
			}
		}
	}

	return board;
}

void delete_the_board(Board_Cell** current_board)
{
	for (int i = 0; i < board_size; i++)
	{
		if (current_board[i] != NULL)
		{
			free(current_board[i]);
		}
	}
	if (current_board != NULL)
	{
		free(current_board);
	}
}

void display_the_board(Board_Cell** current_board, int** allowed_moves_board, bool display_with_next_moves)
{
	for (int i = 0;i < board_size;i++)
	{
		printf("   %c", i + 'a');
	}

	for (int i = 0; i < board_size; i++)
	{
		printf("\n  +");
		for (int j = 0;j < board_size;j++)
		{
			printf("---+");
		}
		printf("\n%d |", i + 1);

		for (int j = 0; j < board_size; j++)
		{
			if (display_with_next_moves == true && allowed_moves_board[i][j] == MOVE_ALLOWED)
			{
				printf(" * |");
			}
			else
			{
				printf(" %c |", current_board[i][j].cell_disk_color);
			}
		}
	}
	printf("\n  +");

	for (int j = 0;j < board_size;j++)
	{
		printf("---+");
	}
	printf("\n");
}

void copy_the_board(Board_Cell** source_board, Board_Cell** destination_board)
{
	for (int i = 0; i < board_size;i++)
	{
		for (int j = 0; j < board_size; j++)
		{
			source_board[i][j].cell_disk_color = destination_board[i][j].cell_disk_color;
		}
	}
}

int** create_valid_moves_board()
{
	int** valid_moves_board;
	valid_moves_board = (int**)malloc(board_size * sizeof(int*));

	for (int i = 0; i < board_size; i++)
	{
		valid_moves_board[i] = (int*)malloc(board_size * sizeof(int));
	}
	for (int i = 0; i < board_size; i++)
	{
		for (int j = 0; j < board_size;j++)
		{
			valid_moves_board[i][j] = MOVE_NOT_ALLOWED;
		}
	}
	return valid_moves_board;
}

void delete_valid_moves_board(int** valid_moves_board)
{
	for (int i = 0; i < board_size; i++)
	{
		if (valid_moves_board[i] != NULL)
		{
			free(valid_moves_board[i]);
		}
	}
	if (valid_moves_board != NULL)
	{
		free(valid_moves_board);
	}
}

void update_valid_moves_board(Board_Cell** current_board,  int** valid_moves_board, int player_type)
{
	int opponent_player;
	char player_color, opponent_color;
	Cell_Coordinates current_coordinates = { INVALID_VALUE, INVALID_VALUE, ' ', INVALID_VALUE };
	Cell_Coordinates delta_coordinates = { INVALID_VALUE, INVALID_VALUE, ' ', INVALID_VALUE };

	if (player_type == USER_PLAYER)
	{
		player_color = get_character_from_color(user_disk_color);
		opponent_color = get_character_from_color(computer_disk_color);
		opponent_player = COMPUTER_PLAYER;
	}
	else
	{
		player_color = get_character_from_color(computer_disk_color);
		opponent_color = get_character_from_color(user_disk_color);
		opponent_player = USER_PLAYER;
	}

	for (current_coordinates.row_index = 0; current_coordinates.row_index < board_size; current_coordinates.row_index++)
	{
		for (current_coordinates.column_index = 0; current_coordinates.column_index < board_size; current_coordinates.column_index++)
		{
			if (!is_empty_cell(current_board, current_coordinates))
			{
				continue;
			}


			for (delta_coordinates.row_index = -1; delta_coordinates.row_index <= 1; delta_coordinates.row_index++)
			{
				for (delta_coordinates.column_index = -1; delta_coordinates.column_index <= 1; delta_coordinates.column_index++)
				{
					Cell_Coordinates temp_coordinates;
					temp_coordinates.row_index = current_coordinates.row_index + delta_coordinates.row_index;
					temp_coordinates.column_index = current_coordinates.column_index + delta_coordinates.column_index;

					
					if (!is_cell_within_board_boundary(temp_coordinates) || (delta_coordinates.row_index == 0 && delta_coordinates.column_index == 0))
					{
						continue;
					}

					if (is_opponent_disk(current_board, temp_coordinates, opponent_color))
					{
						
						for (;;)
						{
							temp_coordinates.row_index += delta_coordinates.row_index;
							temp_coordinates.column_index += delta_coordinates.column_index;
							
							if (!is_cell_within_board_boundary(temp_coordinates))
							{
								break;
							}
							if (is_empty_cell(current_board, temp_coordinates))
							{
								break;
							}
							if (is_player_disk(current_board, temp_coordinates, player_color))
							{
								valid_moves_board[current_coordinates.row_index][current_coordinates.column_index] = MOVE_ALLOWED;
								break;
							}
						}
					}

				}
			}
		}
	}
}

void copy_the_valid_moves_board(int** source_valid_board, int** destination_valid_board)
{
	for (int i = 0; i < board_size;i++)
	{
		for (int j = 0; j < board_size; j++)
		{
			source_valid_board[i][j] = destination_valid_board[i][j];
		}
	}
}

void register_move(Board_Cell** current_board, Cell_Coordinates choosen_move_coordinates, int player_type)
{
	Cell_Coordinates delta_coordinates = {0, 0, ' ', 0};

	char player_color;
	char opponent_color;
	int opponent_player;

	if (player_type == USER_PLAYER)
	{
		player_color = get_character_from_color(user_disk_color);
		opponent_color = get_character_from_color(computer_disk_color);
		opponent_player = COMPUTER_PLAYER;
	}
	else
	{
		player_color = get_character_from_color(computer_disk_color);
		opponent_color = get_character_from_color(user_disk_color);
		opponent_player = USER_PLAYER;
	}

	current_board[choosen_move_coordinates.row_index][choosen_move_coordinates.column_index].cell_disk_color = player_color;

	for (delta_coordinates.row_index = -1; delta_coordinates.row_index <= 1; delta_coordinates.row_index++)
	{
		for (delta_coordinates.column_index = -1; delta_coordinates.column_index <= 1; delta_coordinates.column_index++)
		{
			Cell_Coordinates temp_coordinates;
			temp_coordinates.row_index = choosen_move_coordinates.row_index + delta_coordinates.row_index;
			temp_coordinates.column_index = choosen_move_coordinates.column_index + delta_coordinates.column_index;

			if (!is_cell_within_board_boundary (temp_coordinates) || (delta_coordinates.row_index == 0 && delta_coordinates.column_index == 0))
			{
				continue;
			}

			if (is_opponent_disk(current_board, temp_coordinates, opponent_color))
			{
				for (;;)
				{
					temp_coordinates.row_index += delta_coordinates.row_index;
					temp_coordinates.column_index += delta_coordinates.column_index;

					if (!is_cell_within_board_boundary(temp_coordinates))
					{
						break;
					}
					if (is_empty_cell(current_board, temp_coordinates))
					{
						break;
					}
					if (is_player_disk(current_board, temp_coordinates, player_color))
					{
						temp_coordinates.row_index -= delta_coordinates.row_index;
						temp_coordinates.column_index -= delta_coordinates.column_index;
						while (is_opponent_disk(current_board, temp_coordinates, opponent_color))
						{
							current_board[temp_coordinates.row_index][temp_coordinates.column_index].cell_disk_color = player_color;
							temp_coordinates.row_index -= delta_coordinates.row_index;
							temp_coordinates.column_index -= delta_coordinates.column_index;
						}
						break;
					}
				}
			}
		}
	}
}

Alpha_Beta_Return max_value(Board_Cell** current_board, int alpha_value, int beta_value, int search_level);

Alpha_Beta_Return min_value(Board_Cell** current_board, int alpha_value, int beta_value, int search_level)
{
	int temp_value = VERY_POSITIVE_VALUE;
	Alpha_Beta_Return return_value = { INVALID_VALUE, INVALID_VALUE, INVALID_VALUE };
	Alpha_Beta_Return temp_max_store;
	Cell_Coordinates current_coordinates;
	int moves = 0;

	if (search_level == game_level)
	{
		return_value.value = get_the_score(current_board, COMPUTER_PLAYER);

		return_value.row_index = INVALID_VALUE;
		return_value.column_index = INVALID_VALUE;
		return return_value;
	}

	int** temp_valid_moves = create_valid_moves_board();

	update_valid_moves_board(current_board, temp_valid_moves, USER_PLAYER);

	for (current_coordinates.row_index = 0; current_coordinates.row_index < board_size; current_coordinates.row_index++)
	{
		for (current_coordinates.column_index = 0; current_coordinates.column_index < board_size; current_coordinates.column_index++)
		{
			if (!is_move_allowed(temp_valid_moves, current_coordinates))
			{
				continue;
			}
			Board_Cell** temp_board = create_the_board();
			copy_the_board(temp_board, current_board);
			register_move(temp_board, current_coordinates, USER_PLAYER);


			temp_max_store = max_value(temp_board, alpha_value, beta_value, search_level + 1);
			temp_value = min(temp_value, temp_max_store.value);
			return_value.value = temp_value;
			if (temp_value <= alpha_value)
			{
				delete_valid_moves_board(temp_valid_moves);
				delete_the_board(temp_board);
				return_value.row_index = current_coordinates.row_index;
				return_value.column_index = current_coordinates.column_index;
				return_value.value = temp_value;

				return return_value;
			}
			if (beta_value >= temp_value)
			{
				beta_value = temp_value;
				return_value.row_index = current_coordinates.row_index;
				return_value.column_index = current_coordinates.column_index;
				return_value.value = beta_value;
				global_beta_value = beta_value;
			}
			delete_the_board(temp_board);
			moves++;
		}
	}
	delete_valid_moves_board(temp_valid_moves);
	return return_value;
}


Alpha_Beta_Return max_value(Board_Cell** current_board, int alpha_value, int beta_value, int search_level)
{
	int temp_value = VERY_NEGATIVE_VALUE;
	Alpha_Beta_Return return_value = { INVALID_VALUE, INVALID_VALUE, INVALID_VALUE };
	Alpha_Beta_Return temp_min_store;
	Cell_Coordinates current_coordinates;
	int moves = 0;;

	if (search_level == game_level)
	{
		return_value.value = get_the_score(current_board, COMPUTER_PLAYER);

		return_value.row_index = INVALID_VALUE;
		return_value.column_index = INVALID_VALUE;
		return return_value;
	}

	int** temp_valid_moves = create_valid_moves_board();

	update_valid_moves_board(current_board, temp_valid_moves, COMPUTER_PLAYER);

	for (current_coordinates.row_index = 0; current_coordinates.row_index < board_size; current_coordinates.row_index++)
	{
		for (current_coordinates.column_index = 0; current_coordinates.column_index < board_size; current_coordinates.column_index++)
		{
			if (!is_move_allowed(temp_valid_moves, current_coordinates))
			{
				continue;
			}

			Board_Cell** temp_board = create_the_board();
			copy_the_board(temp_board, current_board);
			register_move(temp_board, current_coordinates, COMPUTER_PLAYER);

			temp_min_store = min_value(temp_board, alpha_value, beta_value, search_level + 1);
			temp_value = max(temp_value, temp_min_store.value);
			return_value.value = temp_value;
			if (temp_value >= beta_value)
			{
				delete_valid_moves_board(temp_valid_moves);
				delete_the_board(temp_board);
				return_value.row_index = current_coordinates.row_index;
				return_value.column_index = current_coordinates.column_index;
				return_value.value = temp_value;

				
				return return_value;
			}
			if (alpha_value <= temp_value)
			{
				alpha_value = temp_value;
				return_value.row_index = current_coordinates.row_index;
				return_value.column_index = current_coordinates.column_index;
				return_value.value = alpha_value;
				global_alpha_value = alpha_value;
			}
			if (search_level == ZERO)
			{
				alpha_value = global_alpha_value;
			}
			delete_the_board(temp_board);
			moves++;
		}
	}
	delete_valid_moves_board(temp_valid_moves);
	return return_value;
}

Cell_Coordinates alpha_beta_search(Board_Cell** current_board, int search_level)
{
	Cell_Coordinates selected_move_coordinate;
	Alpha_Beta_Return return_value;

	global_alpha_value = VERY_NEGATIVE_VALUE;
	global_beta_value = VERY_POSITIVE_VALUE;

	return_value = max_value(current_board, VERY_NEGATIVE_VALUE, VERY_POSITIVE_VALUE, search_level);

	selected_move_coordinate.row_index = return_value.row_index;
	selected_move_coordinate.column_index = return_value.column_index;

	return selected_move_coordinate;
}

Cell_Coordinates play_computer_move()
{
	Cell_Coordinates computer_move_coodinates;
	computer_move_coodinates = alpha_beta_search(board, ZERO);

	if (computer_move_coodinates.row_index == INVALID_VALUE && computer_move_coodinates.column_index == INVALID_VALUE)
	{

	}
	else
	{
		register_move(board, computer_move_coodinates, COMPUTER_PLAYER);
	}

	computer_move_coodinates.board_row = computer_move_coodinates.row_index + 1;
	computer_move_coodinates.board_column = computer_move_coodinates.column_index + 'a';

	return computer_move_coodinates;
}

Game_Status play_game()
{
	Cell_Coordinates current_move = { INVALID_VALUE, INVALID_VALUE,' ', INVALID_VALUE };

	
	if (current_input_command == NULL)
	{
		current_input_command = malloc(11 * sizeof(char));
	}

	printf("\n>");
	fgets(current_input_command,10,stdin);
	if(strcmp(current_input_command,"\n") == 0)
	{
		fgets(current_input_command,11,stdin);
	}
	current_input_command[strcspn(current_input_command, "\n")] = 0;

	if (strcmp(current_input_command, "quit") == 0)
	{
		return QUIT;
	}

	if (strcmp(current_input_command, "showstate") == 0)
	{
		if (current_turn == COMPUTER_TURN)
		{
			display_the_board(board, NULL, false); 
			update_disks_count(board);
			printf("%s player (computer) plays now\n", computer_disk_color);
			printf("White: %d - Black: %d\n", current_white_disks, current_black_disks);
		}
		else
		{
			int** allowed_moves_board = create_valid_moves_board();
			update_valid_moves_board(board, allowed_moves_board, USER_PLAYER);
			
			display_the_board(board, allowed_moves_board, true);
			delete_valid_moves_board(allowed_moves_board);

			update_disks_count(board);
			printf("%s player (human) plays now\n", user_disk_color);
			printf("White: %d - Black: %d\n", current_white_disks, current_black_disks);
		}
	}

	else if (strcmp(current_input_command, "cont") == 0)
	{
		current_move = play_computer_move();
		int** allowed_moves_board = create_valid_moves_board();
		update_valid_moves_board(board, allowed_moves_board, USER_PLAYER);

		display_the_board(board, allowed_moves_board, true); 
		delete_valid_moves_board(allowed_moves_board);

		update_disks_count(board);

		if (current_move.row_index == INVALID_VALUE && current_move.column_index == INVALID_VALUE)
		{
			printf("No move possible for computer\n");

			int** allowed_moves_board = create_valid_moves_board();
			update_valid_moves_board(board, allowed_moves_board, USER_PLAYER);

			if (no_valid_moves_exist(allowed_moves_board))
			{
				delete_valid_moves_board(allowed_moves_board);
				update_disks_count(board);
				printf("White: %d - Black: %d\n", current_white_disks, current_black_disks);
				printf("END OF GAME\n");
				return COMPLETE;
			}
			delete_valid_moves_board(allowed_moves_board);
		}
		else
		{
			printf("Move played: %c%d\n", current_move.board_column, current_move.board_row);
		}
		printf("%s player (human) plays now\n", user_disk_color);
		printf("White: %d - Black: %d\n", current_white_disks, current_black_disks);
		switch_the_turn();
	}
	else //play <move>
	{
		const char s[2] = " ";
		char* token;

		token = strtok(current_input_command, s);
		token = strtok(NULL, s);

		current_move.board_column = token[0];
		current_move.board_row = token[1] - '0';

		current_move.column_index = tolower(token[0]) - 'a';
		current_move.row_index = token[1] - '1';

		//check validity of the move
		int** allowed_moves_board = create_valid_moves_board();
		update_valid_moves_board(board, allowed_moves_board, USER_PLAYER);
		if (!is_move_allowed(allowed_moves_board, current_move))
		{
			//invalid input, retry
			printf("Invalid input\n");
			delete_valid_moves_board(allowed_moves_board);
			return CONTINUE;
		}
		delete_valid_moves_board(allowed_moves_board);
		register_move(board, current_move, USER_PLAYER);
		
		display_the_board(board, NULL, false);
		update_disks_count(board);
		printf("Move played: %c%d\n", current_move.board_column, current_move.board_row);
		printf("%s player (computer) plays now\n", computer_disk_color);
		printf("White: %d - Black: %d\n", current_white_disks, current_black_disks);
		switch_the_turn();
	}
	return CONTINUE;
}

int main()
{
	char* menu_selection = NULL;
	const char s[2] = " ";
	char* token;
	int menu_item = 0;
	Game_Status result = CONTINUE;
	

	if (menu_selection == NULL)
	{
		menu_selection = malloc(18 * sizeof(char));
	}
	printf(">");
	fgets(menu_selection,18,stdin);

	/* get the first token */
	token = strtok(menu_selection, s);

	/* walk through other menu selections*/
	while (token != NULL) 
	{
		menu_item++;

		token = strtok(NULL, s);

		if (menu_item == 1)
		{
			if (strcmp(token, "white") == 0)
			{
				strncpy(user_disk_color,"White",5);
				strncpy(computer_disk_color, "Black",5);
				current_turn = COMPUTER_TURN;
			}
			else
			{
				strncpy(user_disk_color, "Black",5);
				strncpy(computer_disk_color, "White",5);
				current_turn = USER_TURN;
			}
		}
		else if (menu_item == 2)
		{
			//check for valid and even number
			board_size = atoi(token);
		}
		else if (menu_item == 3)
		{
			game_level = atoi(token);
		}
	}

	//reset_parameters();
	board = create_the_board();

	while (result == CONTINUE)
	{
		result = play_game();
	}

	reset_parameters();
	free(menu_selection);
	
	return 0;

}

