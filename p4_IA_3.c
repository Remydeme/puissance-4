#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "p4.h"

extern uc* grid;
extern int insert_pos;
extern struct player_s* players;
extern int h;
extern int w;

void p4_IA_3(uc* grid, int size_x, int size_y, uc token)
{
    static int strike;
    int max = DEFAULT_MAX_V;
    size_y = 0;
    int rv_min = 0;
    int profondeur = DEFAULT_DEPHT;
    int rank = 0;
    switch (strike)
    {
        case START:
            if (!is_filled(grid, size_x / 2))
                insert_pos = insert(grid, size_x / 2, token);
            else
                insert_pos = insert(grid, size_x / 2 + 1, token);
          break;
        case SECOND_STRIKE :
            break;

        default :
            {
                for (int i = 1; i <= size_x; i++)
                {
                    if (!is_filled(grid, i))
                    {
                        int location = insert(grid, i, token);
                        rv_min = min(grid, profondeur - 1, location, size_x, token);

                        if (rv_min > max)
                        {
                            max = rv_min;
                            rank = i;
                        }
                        grid[location] = EMPTY_CASE;
                    }
                }
                insert_pos = insert(grid, rank, token);
            }
         break;
    }
    strike++; /* use to choose the good strike*/
}


int max(uc* grid, int profondeur, int location, int size_x, uc token)
{
    if (profondeur == 0 || is_winner(location, grid))
        return heuristique(grid, (token == P1_JETON) ? P2_JETON : P1_JETON, location, size_x);
    int max = DEFAULT_MAX_V;
    int rv_max = 0;
    uc next_token = (token == P1_JETON) ? P2_JETON : P1_JETON;
    for (int i = 1; i <= size_x; i++)
    {
        if (!is_filled(grid, i))
        {
            int location = insert(grid, i, token);
            rv_max = min(grid, profondeur - 1, location, size_x, next_token);
            if (rv_max > max)
                max = rv_max;
            grid[location] = EMPTY_CASE;
        }
    }
    return max;
}

int min(uc* grid, int profondeur, int location, int size_x, uc token)
{
    if (profondeur == 0 || is_winner(location, grid))
        return heuristique(grid, token, location, size_x);
    int min = DEFAULT_MIN_V;
    int rv_min = 0;
    uc next_token = (token == P1_JETON) ? P2_JETON : P1_JETON;
    for (int i = 1; i <= size_x; i++)
    {
        if (!is_filled(grid, i))
        {
            int location = insert(grid, i, token);
            rv_min = max(grid, profondeur - 1, location, size_x, next_token);
            if (rv_min < min)
                min = rv_min;
            grid[location] = EMPTY_CASE;
        }
    }
    return min;
}

int heuristique(uc* grid, uc token, int location, int size_x)
{
    int nb_pions = 0;
    bool end = false;
    const int size_y = size_x == DEFAULT_W ? DEFAULT_H : DEFAULT_H_2;
    for (int i = 0; i < size_y * size_x && ! end; i++)
    {
        if (grid[i] == ' ')
            end = true;
        else
            nb_pions++;
    }

    if (is_winner(location, grid))
    {
        /* test if it's player one => IA*/
        if (token == grid[location])
            return DEFAULT_MIN_V - nb_pions;
        else
            return DEFAULT_MAX_V + nb_pions;
    }
    return 1;
}

struct player_s* IA_turn(int whom)
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
            players[1].tokens--;
            p4_IA_3(grid, w, h, P2_JETON);
            played = 1;
        }
    }
    return &players[whom % 2]; 
}

uc* IA_mode_p4_game()
{
    int finished = START;
    int whom = 0;
    struct player_s* player = NULL;
    grid = create_tab(w, h);
    if (grid)
    {
        while (!finished)
        {
            display(grid);
            player = IA_turn(whom);
            if (is_winner(insert_pos, grid))
            {
                finished = END;
                player->score++;
                display(grid);
                continue;
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


