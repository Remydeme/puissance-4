#pragma once

#define VRAI 1
#define FAUX 0

#define P1_JETON 'X'
#define P2_JETON 'O'

// players jeton amount 
#define NB_JETON 21

// power 4 game default dimension
#define DEFAULT_H 6
#define DEFAULT_W 7


// error message 

#define ERR_BAD_CHECK "Error while checking."

#define ALLOCATION_ERROR "Null pointer ! Allocation have failed." 

#define  COLUMN_FILLED "This column is filled ! Just Try again ^^."

// PLayer 

#define PLAYER_ONE  "player one turn"
#define PLAYER_TWO "player two turn"

#define PLAY_MSG "which column do you want to play "

#define MATCH_NULL "match Null !!"


struct player_s
{
   char name[250];
   int tokens; 
};


typedef unsigned char uc;


/* display the code */

void display(uc* grid);

/* main function P4_GAME used to launch the game */

int p4_game();
/*insertion function */

int insert(uc* grid, int column, uc token);

// play the game => select the player 

int turn (int player, uc* grid);

/*create tab*/

uc* create_tab(int w, int h);

/* check if the case is empty  ' '*/

int is_empty(uc value);

/*turn select the player and call thge insert function */

/*check if a column is not filled of token */

int is_filled(uc* grid, int column);


/* check horizontaly if the token that have been put finished the game */
int check_horizontal();

/* check verticaly if the token that have been put finished the game */
int check_vertical(int pos, char *tab);

/* check right diagonal  if the token that have been put finished the game */
int check_rdiagonal();

/* check left diagonal  if the token that have been put finished the game */
int check_ldiagonal();
