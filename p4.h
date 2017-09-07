/**
* \file p4.h
* \auhtor Remy.deme
*/
#pragma once

#include <stdbool.h>


#define VRAI 1
#define FAUX 0

///////////////// MAIN FUNCION 

//////////////// menu 

#define VS_1 1
#define VS_IA 2
#define RULES 4
#define RESTORE_MODE 3
#define BUILD_MODE 5

//////////// GAME 

#define BARRE_D "___________________________________"
#define BARRE_U "-----------------------------------"

///////////// VAR 
#define NORMAL 1

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

#define P1_TOKEN 'X'
#define P2_TOKEN 'O'
#define EMPTY_CASE ' '

// players jeton amount 
#define NB_JETON 21

// power 4 game default dimension
#define DEFAULT_H 6
#define DEFAULT_W 7

#define DEFAULT_H_2 9
#define DEFAULT_W_2 8


///////////// IA  MIN MAX CONSTANTE

#define DEFAULT_MAX_V -1000
#define DEFAULT_MIN_V 1000

#define DEFAULT_DEPHT 8

#define START 0
#define SECOND_STRIKE 1

// LEVEL

#define AMATEUR 4
#define MASTER 8
#define CHAMPION 10 

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


///////////////// IA & Min Max /////////////////


uc* IA_mode_p4_game();

/** Ia turn function */
struct player_s* IA_turn(int whom, int level);



/** \brief This function is an implementation of the Min Max algorithm */

void p4_IA_3(uc *grid, int size_x, int size_y, uc token);

/** \brief MinMax Min function */
int min(uc* grid, int profondeur, int location, int size_x, int size_y, uc token);

int insert(uc* grid, int column, uc token);
/** \brief MinMax Max function */

int max(uc* grid, int profondeur, int location, int size_x, int size_y, uc token);

/** Evaluation function */


///////////// The game ///////////


/** */

/**   \brief display the code */

/** \fn */void display(uc* grid);

/** \brief check if the game is NULL (egalite)*/
void check_party_null(int whom, int *finished);

/** \brief main function P4_GAME used to launch the game */

uc* p4_game();

/** \brief set the structure sigaction to handle the  sigint signal 
 *in order to save file  
 */

int is_winner_1(int location, uc* grid);

void handle_signal();

/** \brief function to save the game */

void save_game(int signum);

/* \brief config player */

void config_player(int choice);

/** \brief return one if there is a winner else 0 */

int is_winner(int location, uc* grid);

/** \brief insertion function */

int insert_1(uc* grid, int column, uc token);

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

int is_filled(uc* grid, int column);


/** check horizontaly if the token that have been put finished the game */
int check_horizontal(int pos, uc* grid);

/* check verticaly if the token that have been put finished the game */
int check_vertical(int pos, uc *grid);

/* check right diagonal  if the token that have been put finished the game */
int check_rdiagonal(int pos, uc* grid);

/* check left diagonal  if the token that have been put finished the game */
int check_ldiagonal(int pos, uc* grid);



