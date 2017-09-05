#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <limits.h>

#include "p4.h"


static struct player_s *players = NULL;

int h = DEFAULT_H;
int w = DEFAULT_W;

static int insert_pos = 0;


///////////////////////////// player ////////////////////////////


struct player_s * create_player()
{
    return calloc (2, sizeof (struct player_s)); 
}

void config_player()
{
    for (int i = 0; i < 2; i++)
    {
        scanf ("%s", players[i].name);
        players[i].tokens = MAX_SIZE / 2;
    }
}



struct player_s* turn(int whom, uc* grid)
{
    int column = 0;
    int played = 0;
    fprintf (stdout, "%s \n", PLAY_MSG);
    while (!played)
    {
        if (!(whom % 2))
        {
            fprintf (stdout, "%s \n", players[0].name);
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
            fprintf (stdout, "%s \n", players[1].name);
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
    for (int i = (h - 1) * w; i >= 0; i -= 7)
    {
        printf ("|");
        for (int j = i; j < i + w; j++)
        {
            printf ("%c", grid[j]);
            printf ("|");
        }
        printf ("\n");
    }
}

int is_winner(uc* grid)
{
    if (check_horizontal(insert_pos, grid) >= WIN_VAL)
        return 1;
    else if (check_vertical(insert_pos, grid) >= WIN_VAL)
        return 1;
    else if (check_rdiagonal(insert_pos, grid) >= WIN_VAL)
        return 1;
    else if (check_ldiagonal(insert_pos, grid) >= WIN_VAL)
        return 1;
    else
        return 0;
}


int check_column(int column)
{
    return column >= 0 && w > column ? 1 : 0;
}


int check_horizontal(int pos, uc* grid)
{
    int cursor = pos;
    uc token = grid[pos];
    int counter = 1;
    /*start by checking the right side*/
    while (cursor < MAX_SIZE)
    {
        cursor += w;
        if (grid[cursor] == token)
            counter++;
        else
            break;
    }
    /* check the left side */
    cursor = pos;
    while (cursor > 0)
    {
        cursor -= w;
        if (grid[cursor] == token)
            counter++;
        else
            break;
    }
    return counter;
}

int check_vertical(int pos, uc* grid)
{
    int cursor = pos;
    uc token = grid[pos];
    int counter = 1;
    /*start by checking the right side*/
    while (cursor < MAX_SIZE)
    {
        cursor += ONE;
        if (grid[cursor] == token)
            counter++;
        else
            break;
    }
    /* check the left side */
    cursor = pos;
    while (cursor > 0)
    {
        cursor -= ONE;
        if (grid[cursor] == token)
            counter++;
        else
            break;
    }
    return counter;
}

int check_rdiagonal(int pos, uc* grid)
{
    int cursor = pos;
    uc token = grid[pos];
    int counter = ONE;
    /*start by checking the right side*/
    while (cursor < MAX_SIZE)
    {
        cursor += (w + 1);
        if (grid[cursor] == token)
            counter++;
        else
            break;
    }
    /* check the left side */
    cursor = pos;
    while (cursor > 0)
    {
        cursor -= (w + 1);
        if (grid[cursor] == token)
            counter++;
        else
            break;
    }
    return counter;
}

int check_ldiagonal(int pos, uc* grid)
{
    int cursor = pos;
    uc token = grid[pos];
    int counter = 1;
    /*start by checking the right side*/
    while (cursor < MAX_SIZE)
    {
        cursor += (w - 1);
        if (grid[cursor] == token)
            counter++;
        else
            break;
    }
    /* check the left side */
    cursor = pos;
    while (cursor > 0)
    {
        cursor -= (w - 1);
        if (grid[cursor] == token)
            counter++;
        else
            break;
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
    return case_value == ' ';
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
        tmp[i] = ' ';
    return tmp;
}


int is_filled(uc* grid, int column)
{
    return grid[(h - 1) * w  +  column - 1] != ' ';
}



/////////////////// GAME ///////////////////////////


uc* p4_game()
{
    players = create_player();
    int finished = START;
    int whom = 0;
    struct player_s* player = NULL;
    uc* grid = create_tab(w, h);
    if (grid)
    {
        while (!finished)
        {
            display(grid);
            player = turn(whom, grid);
            if (is_winner(grid))
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

