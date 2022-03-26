***********************************
PROGRAM: REVERSI OTHELO GAME 
***********************************

CREATED BY: AREEB TARIQ

DESCRIPTION:
	This program implements the Reversi game against a human player by implementing alpha-beta method. 

	The size of the board (space) is user defined NxN.
	The disk color for human is a choice of the user
	The user can choose the value of 'level' which is equivalent to the number of levels the alpha-beta method should proceed in order to apply its evaluation function at the states of the last level.

PROCESS FLOW:

	The program sequentially asks for following initial information before proceeding with the game:
		a. newgame black|white <size> <level>
		b. 'play <move>' command like playb3. This is the position human player wishes to play.
		c. 'cont' command for the computer player to play its move. The current state is printed afterwards, indicating by a * all legal moves for the human player.
		d. 'showstate' command prints the state of current board  indicating by a * all legal moves for the human player, if it is his/her turn to play.
		e. 'quit' command is to quit the game
