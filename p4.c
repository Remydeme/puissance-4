#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <limits.h>

#include "p4.h"


static struct player_s *players = NULL;

int h = DEFAULT_H;
int w = DEFAULT_W;

///////////////////////////// player ////////////////////////////


struct player_s * create_player(int nb_player)
{
    return nb_player >= 2 ? 
        calloc (2, sizeof (struct player_s)) : calloc (nb_player, sizeof (struct player_s)) ;
}

void config_player()
{
    for (int i = 0; i < 2; i++)
    {
        scanf ("%s", players[i].name);
        players[i].tokens = h * w / 2;
    }
}



int turn(int player, uc* grid)
{
    int column = 0;
    int played = 0;
    fprintf (stdout, "%s \n", PLAY_MSG);
    while (!played)
    {
        if (!(player % 2))
        {
            fprintf (stdout, "%s \n", players[0].name);
            scanf("%d", &column);
            // get the '\n'
            getc(stdin);
            if (!is_filled(grid, column))
            {
                insert (grid, column,  P1_JETON);
                players[0].tokens--;
                played = 1;
            }
            else
                fprintf (stderr, "err : %s ", COLUMN_FILLED); 
        }
        else
        {
            fprintf (stdout, "%s \n", players[1].name);
            scanf("%d", &column);
            // get the '\n'
            if (!is_filled(grid, column))
            {
                insert (grid, column,  P2_JETON);
                players[1].tokens--;
                played = 1;
            }
            else
                fprintf (stderr, "err : %s", COLUMN_FILLED);
        }
    }
    return EXIT_SUCCESS; 
}


////////////////////////////// tools ///////

// read function with the fgets here 


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

//////////////////////////////////// grid //////////////////////////////

int check_vertical(int pos, char *tab)
{
    char token = tab[pos];

    if ((pos >= 0 && pos <= 42) && (compt < 6))
    {
        compt += Vertical(pos+7, tab) + Vertical(pos-7, tab) +1;
    }
    else
    {
        if (compt >= 6)
        {
            fprintf (stderr, "err : %s \n", ERR_BAD_CHECK);
            return EXIT_FAILURE;
        }
        return compt;
    }
}

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
    for (int i = 0 ; (i < h && position < h * w) ; i++)
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
    uc* tmp =  calloc (w * h, sizeof (uint8_t));
    for (int i = 0; i < w * h; i++)
        tmp[i] = ' ';
    return tmp;
}


int is_filled(uc* grid, int column)
{
    return grid[(h - 1) * w  +  column - 1] != ' ';
}



/////////////////// GAME ///////////////////////////


int p4_game()
{
    int finished = 0;
    int player = 0;
    players = create_player(2);
    uc * grid = create_tab(w, h);
    if (grid)
    {
        while (!finished)
        {
            display(grid);
            turn(player, grid);
            player++;
            if (player == 42)
                fprintf (stdout, "msg : %s ", MATCH_NULL);
            system("clear");
        }
    }
    else
        fprintf(stderr, "mssg : %s ", ALLOCATION_ERROR);
    return 1;
}

