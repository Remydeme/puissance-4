#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include "p4.h"


/////// GLOBAL VAR 

struct player_s *players = NULL;

int h = DEFAULT_H;
int w = DEFAULT_W;
struct sigaction new_action, old_action;
int insert_pos = 0;

uc* grid = NULL;


///////////////////////////// player ////////////////////////////

void handle_signal()
{
    new_action.sa_handler = save_game;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction (SIGINT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
        sigaction (SIGINT, &new_action, NULL);
    sigaction (SIGHUP, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
        sigaction (SIGHUP, &new_action, NULL);
}

bool restore_game()
{
    FILE * file = fopen(SAVED_FILE, "r");
    if (file)
    {
        char garbage = ' ';
        for (int i = 0; i < NB_PLAYER_MAX; i++)
            fscanf(file, "%s %d %d %c", players[i].name, &players[i].tokens, &players[i].score, &garbage);
        fscanf(file, "%d %d %c", &h, &w, &garbage);
        int car = ' ';
        int i = 0;
        while (((car = fgetc(file)) != EOF) && i < MAX_SIZE)
        {
            grid[i] = car;
            i++;
        }
        return true;
    }
    else
    {
        fprintf (stderr, "No saved file found \n");
        return false;
    }
}

void save_game(int signum)
{
    if (SIGINT == signum)
        printf ("RECEIVED SIGINT SIGNAL ! SAVING ..." );
    fflush(stdout);
    FILE* file = fopen(SAVED_FILE, "w+");
    if (file && grid && players)
    {
        for (int i = 0; i < NB_PLAYER_MAX; i++)
        {
            fprintf (file, "%s %d %d \n", players[i].name, players[i].tokens, 
                    players[i].score);
        }
        fprintf(file, "%d %d\n", h, w);
        for (int i = 0; i < h * w; i++)
            fputc(grid[i], file);
        free (grid);
        free (players);
    }
    else
    {
        if (!grid)
            fprintf (stderr, "err : %s ", "empty grid don't need to save");
        else
            free(grid);
        if (!players)
            fprintf (stderr, "err : %s" , "no players info don't need to save the party");
        else
            free(grid);
        if (!file)
            fprintf (stderr,"err : %s", "err : Error while opening file ");
    }
    exit(EXIT_SUCCESS);
}



struct player_s * create_player()
{
    return calloc (2, sizeof (struct player_s)); 
}

void config_player(int choice)
{
    if (choice == 1)
    {
        for (int i = 0; i < 2; i++)
        {
            printf ("Enter the name of the player %d : ", i + 1);
            scanf ("%s", players[i].name);
            players[i].tokens = MAX_SIZE / 2;
            printf ("\n");
        }
    }
    else if (choice == 2)
    {
        puts ("Enter the name of the player : ");
        scanf ("%s", players[0].name);
        memmove (players[1].name, IA_NAME, sizeof (IA_NAME));
    }
}



struct player_s* turn(int whom)
{
    int column = 0;
    int played = 0;
    while (!played)
    {
        if (!(whom % 2))
        {
            fprintf (stdout, "\n It's your turn %s \n\n %s \n", 
                    players[0].name, PLAY_MSG);
            scanf("%d", &column);
            getc(stdin);
            if (check_column(column))
            {
                if (!is_filled(grid, column))
                {
                    insert_pos = insert (grid, column,  P1_JETON);
                    players[0].tokens--;
                    played = 1;
                }
                else
                    fprintf (stderr, "err : %s \n", COLUMN_FILLED); 
            }
            else fprintf (stderr, "err: %s \n",  ERR_INPUT);
        }
        else
        {
            fprintf (stdout, "\n It's your turn %s \n\n %s \n", 
                    players[1].name, PLAY_MSG);
            scanf("%d", &column);
            getc(stdin);
            // get the '\n'
            if (check_column(column))
            {
                if (!is_filled(grid, column))
                {
                    insert_pos = insert (grid, column,  P2_JETON);
                    players[1].tokens--;
                    played = 1;
                }
                else
                    fprintf (stderr, "err : %s", COLUMN_FILLED);
            }
            else fprintf (stderr, "err: %s",  ERR_INPUT);
        }
    }
    return &players[whom % 2]; 
}


////////////////////////////// tools /////////////////////////////////

// read function with the fgets here 

void check_party_null(int whom, int *finished)
{
    if (whom == MAX_SIZE)
    {
        fprintf (stdout, "msg : %s \n", MATCH_NULL);
        *finished = END;
    }

}

void display(uc* grid)
{
    for (int i = 0; i < w; i++)
        printf ("|%d|", i + 1);
    printf ("\n");
    puts (BARRE);
    for (int i = (h - 1) * w; i >= 0; i -= 7)
    {
        printf ("| ");
        for (int j = i; j < i + w; j++)
        {
            printf ("%c", grid[j]);
            printf (" |");
        }
        printf ("\n");
    }
}

int is_winner(int location, uc* grid)
{
    if (check_horizontal(location, grid) >= WIN_VAL)
    {
        //  printf ("Winner horizontal : location : %d", location);
        return 1;
    }
    else if (check_vertical(location, grid) >= WIN_VAL)
    {
        //    printf ("Winner horizontal : location : %d", location);
        return 1;
    }
    else if (check_rdiagonal(location, grid) >= WIN_VAL)
    {
        //    printf ("Winner horizontal : location : %d", location);
        return 1;
    }
    else if (check_ldiagonal(location, grid) >= WIN_VAL)
    {
        //    printf ("Winner horizontal : location : %d", location);
        return 1;
    }
    else
        return 0;
}




int check_column(int column)
{
    return column >= 1 && w >= column ? 1 : 0;
}


int check_horizontal(int pos, uc* grid)
{
    int cursor = pos;
    uc token = grid[pos];
    int counter = 1;
    bool find = true;
    /*start by checking the right side*/
    while (cursor < MAX_SIZE && find)
    {
        cursor += w;
        if (grid[cursor] == token)
            counter++;
        else
            find = false;
    }

    cursor = pos;
    find = true;
    /* check the left side */
    while (cursor >= 0 && find)
    {
        cursor -= w;
        if (grid[cursor] == token)
            counter++;
        else
            find = false;
    }
    return counter;
}

int check_vertical(int pos, uc* grid)
{
    int cursor = pos;
    uc token = grid[pos];
    int counter = 1;
    bool find = true;
    /*start by checking the right side*/
    while (cursor < MAX_SIZE && find)
    {
        cursor += ONE;
        if (grid[cursor] == token)
            counter++;
        else
            find = false;
    }
    /* check the left side */
    cursor = pos;
    find = true;
    while (cursor >= 0 && find)
    {
        cursor -= ONE;
        if (grid[cursor] == token)
            counter++;
        else
            find = false;
    }
    return counter;
}

int check_rdiagonal(int pos, uc* grid)
{
    int cursor = pos;
    uc token = grid[pos];
    int counter = ONE;
    bool find = true;
    /*start by checking the right side*/
    while (cursor < MAX_SIZE && find)
    {
        cursor += (w + 1);
        if (grid[cursor] == token)
            counter++;
        else
            find = false;
    }
    /* check the left side */
    cursor = pos;
    find = true;
    while (cursor >= 0 && find)
    {
        cursor -= (w + 1);
        if (grid[cursor] == token)
            counter++;
        else
            find = false;
    }
    return counter;
}

int check_ldiagonal(int pos, uc* grid)
{
    int cursor = pos;
    uc token = grid[pos];
    int counter = 1;
    bool find = true;
    /*start by checking the right side*/
    while (cursor < MAX_SIZE && find)
    {
        cursor += (w - 1);
        if (grid[cursor] == token)
            counter++;
        else
            find = false;
    }
    /* check the left side */
    cursor = pos;
    find = true;
    while (cursor >= 0 && find)
    {
        cursor -= (w - 1);
        if (grid[cursor] == token)
            counter++;
        else
            find = false;
    }
    return counter;
}


//////////////////////////////////// grid //////////////////////////////


/*
   | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
   _ ___________________________
   | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
   _ ___________________________
   | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
   _ ___________________________
   | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
   _ ___________________________

   | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
   _ ____________________________ 
 */



int is_empty(uc case_value)
{
    return case_value == EMPTY_CASE;
}

int insert(uc* grid, int column, uc token)
{
    int position = column - 1;
    for (int i = 0 ; (i < h && position < MAX_SIZE) ; i++)
    {
        if (is_empty(grid[position]))
        {
            grid[position] = token;
            break;
        }
        position += w;
    }
    return position;
}


uc* create_tab(int w, int h)
{
    uc* tmp =  calloc (MAX_SIZE, sizeof (uint8_t));
    for (int i = 0; i < MAX_SIZE; i++)
        tmp[i] = EMPTY_CASE;
    return tmp;
}


int is_filled(uc* grid, int column)
{
    return grid[(h - 1) * w  +  column - 1] != EMPTY_CASE;
}

/////////////////// TREE ///////////////////////////

/*
   struct node_s*  init_tree(uc* grid)
   {
   struct node_s* new_node = calloc (1, sizeof (struct node_s));
   new_node->grid = grid;
   return new_node;
   }

   struct node_s* insert_child(struct node_s* root, int rank, uc token, int player)
   {
   root->children[rank] = new_node();
   if (root->children[rank])
   {
// create a function for the grid 
int position = insert(root->grid, rank, token); 
root->children[rank]->grid = 
if (player == IA)
root->children[rank]->
//    root->children[rank]->weight = 
}
}
 */


/////////////////// GAME ///////////////////////////


uc* p4_game(int flag)
{
    int finished = START;
    int whom = 0;
    struct player_s* player = NULL;
    grid = create_tab(w, h);
    if (flag == RESTORE_MODE)
        restore_game();
    if (grid)
    {
        while (!finished)
        {
            display(grid);
            player = turn(whom);
            if (is_winner(insert_pos, grid))
            {
                finished = END;
                player->score++;
                display(grid);
                break;
            }
            whom++;
            check_party_null(whom, &finished);
            system("clear");
        }
    }
    else
        fprintf(stderr, "mssg : %s \n", ALLOCATION_ERROR);
    return grid;
}


