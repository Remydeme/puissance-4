/**
* \file p4.h
* \auhtor Remy.deme
*/
#pragma once

#define VRAI 1
#define FAUX 0

// MAIN FUNCION 

// menu 

#define VS_1 1
#define VS_IA 2
#define RULES 3

// GAME 

#define IA_NAME "JARVIS"

#define SAVED_FILE "game_saved.txt"
#define NB_PLAYER_MAX 2

#define ONE 1
#define EIGHT 8
#define SIX 6

#define MAX_SIZE (h * w)
#define WIN_VAL 4

#define END 1
#define START 0

#define P1_JETON 'X'
#define P2_JETON 'O'

// players jeton amount 
#define NB_JETON 21

// power 4 game default dimension
#define DEFAULT_H 6
#define DEFAULT_W 7


// error message 

#define ERR_BAD_CHECK "Error while checking."

#define ERR_INPUT "Bad input ! column is not int the range (1 - 7). Just try again "

#define ALLOCATION_ERROR "Null pointer ! Allocation have failed." 

#define  COLUMN_FILLED "This column is filled ! Just Try again ^^."

// PLayer 

#define PLAYER_ONE  "player one turn"
#define PLAYER_TWO "player two turn"

#define PLAY_MSG "which column do you want to play "

#define MATCH_NULL "match Null !!"

/* \brief alias */
typedef unsigned char uc;

/*! \struct player_s*/
struct player_s
{
   char name[250];
   int tokens;
   int score;
   int win;
};


struct game_s
{
    int h; 
    int w;
    int insert_pos;
    uc* grid;
    struct player_s* players;
};


/**   \brief display the code */

/** \fn */void display(uc* grid);

/** \brief check if the game is NULL (egalite)*/
void check_party_null(int whom, int *finished);

/** \brief main function P4_GAME used to launch the game */

uc* p4_game();

/** \brief set the structure sigaction to handle the  sigint signal 
 *in order to save file  
 */

void handle_signal();

/** \brief function to save the game */

void save_game(int signum);

/* \brief config player */

void config_player(int choice);

/** \brief return one if there is a winner else 0 */

int is_winner();

/** \brief insertion function */

int insert(int column, uc token);

/** \brief check the validity of the column  */

int check_column(int column);

/** \brief alloc the memory for new player */

struct player_s* create_player();

/** \brief  play the game => select the player */

struct player_s* turn (int whom);

/**
* \fn uc * create_tab(int w, int h)
* \brief create tab
*/

uc* create_tab(int w, int h);

/** \brief  check if the case is empty  ' '*/

int is_empty(uc value);

/** \brief turn select the player and call thge insert function */

/** \brief check if a column is not filled of token */

int is_filled(int column);


/** check horizontaly if the token that have been put finished the game */
int check_horizontal(int pos, uc* grid);

/* check verticaly if the token that have been put finished the game */
int check_vertical(int pos, uc *grid);

/* check right diagonal  if the token that have been put finished the game */
int check_rdiagonal(int pos, uc* grid);

/* check left diagonal  if the token that have been put finished the game */
int check_ldiagonal(int pos, uc* grid);
