#include <stdio.h>
#include <stdlib.h>

#include "p4.h"

extern uc* grid;
int IA_insert_pos;
extern struct player_s* players;
extern int h;
extern int w;

int strike = 0;
static int is_filled_1(uc* grid, int column)
{
    return grid[(h - 1) * w  +  column - 1] != EMPTY_CASE;
}

static bool is_taken(uc* grid, int column)
{
    return (column >= 0 && column < h * w) && grid[column - 1] != EMPTY_CASE ? true : false;
}




static int IA_check_column(int column)
{
    return column >= 1 && w >= column ? 1 : 0;
}

static int insert(uc* grid, int column, uc token)
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

static int IA_is_winner(int location, uc* grid)
{
    if (check_horizontal(location, grid) >= WIN_VAL)
    {
       // printf ("horizontal : %d ", location);
        return 1;
    }
    else if (check_vertical(location, grid) >= WIN_VAL)
    {
       // printf ("vertical : %d ", location);
        return 1;
    }
    else if (check_rdiagonal(location, grid) >= WIN_VAL)
    {
      //  printf ("r diagonal %d ", location);
        return 1;
    }
    else if (check_ldiagonal(location, grid) >= WIN_VAL)
    {
       // printf ("l diagonal %d ", location);
        return 1;
    }
    else
        return 0;
}





int IA_check_vertical(int pos, uc* grid)
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
         else if (grid[cursor] == EMPTY_CASE && (cursor + w) < MAX_SIZE && grid[cursor + w] == token)
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
        else if (grid[cursor] == EMPTY_CASE && cursor - w >= 0 && grid[cursor - w] ==  token)
            counter++;
        else
            find = false;
    }
    return counter;
}

static int IA_check_horizontal(int pos, uc* grid)
{
    int cursor = pos + ONE;
    uc token = grid[pos];
    int counter = 1;
    bool find = true;
    int low_limit = ((pos / w) * w);
    low_limit = low_limit < 0 ? 0 : low_limit;
    int hight_limit = ((pos / w) + 1) * w;
    hight_limit = hight_limit > MAX_SIZE ? MAX_SIZE : hight_limit;
    /*start by checking the right side*/
    while (cursor < hight_limit && find)
    {
        if (grid[cursor] == token)
            counter++;
       else if (grid[cursor] == EMPTY_CASE && (cursor + ONE) < hight_limit 
            && grid[cursor + ONE])
            counter++;
        else
            find = false;
        cursor += ONE;
    }
    /* check the left side */
    cursor = pos - ONE;
    find = true;
    while (cursor >= low_limit && find)
    {
        if (grid[cursor] == token)
            counter++;
        else if (grid[cursor] == EMPTY_CASE && (cursor - ONE) >= low_limit 
            && grid[cursor - ONE])
            counter++;
        else
            find = false;
        cursor -= ONE;
    }
    return counter;
}

static int IA_check_rdiagonal(int pos, uc* grid)
{
    int cursor = pos;
    uc token = grid[pos];
    int counter = ONE;
    bool find = true;
    /*start by checking the right side*/
    while (((cursor + 1) % w) && cursor < MAX_SIZE  && find)
    {
        cursor += (w + 1);
        if (grid[cursor] == token)
            counter++;
        else if (grid[cursor] == EMPTY_CASE 
            && cursor + (w + 1) 
            && grid[cursor + w + 1] == token)
            counter++;
        else
            find = false;
    }
    /* check the left side */
    cursor = pos;
    find = true;
    while ((cursor % w)  && cursor >= 0 && find)
    {
        cursor -= (w + 1);
        if (grid[cursor] == token)
            counter++;
        else if (grid[cursor] == EMPTY_CASE 
            && ((cursor % w)  && cursor >= 0) 
            && grid[cursor - (w + 1)] == token)
            counter++;
        else
            find = false;
    }
    return counter;
}

static int IA_check_ldiagonal(int pos, uc* grid)
{
    int cursor = pos;
    uc token = grid[pos];
    int counter = 1;
    bool find = true;
    /*start by checking the right side*/
    while ((cursor % w) && cursor < MAX_SIZE && find)
    {
        cursor += (w - 1);
        if (grid[cursor] == token)
            counter++;
        else if (grid[cursor] == EMPTY_CASE 
            && (cursor % w) && cursor < MAX_SIZE 
            && grid[cursor + (w - 1)] == token)
            counter++;
        else
            find = false;
    }
    /* check the left side */
    cursor = pos;
    find = true;
    while (((cursor + 1) % w) && cursor >= 0 && find)
    {
        cursor -= (w - 1);
        if (grid[cursor] == token)
            counter++;
        else
            find = false;
    }
    return counter;
}


static int alignement(uc * grid, int location)
{
    int total = 0;
    total += IA_check_vertical(location, grid);
    total += IA_check_horizontal(location, grid);
    total += IA_check_rdiagonal(location, grid);
    total += IA_check_ldiagonal(location, grid);

    return total;
}




static int heuristique(uc* grid, uc token, int location, int size_x)
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

    if (IA_is_winner(location, grid))
    {
        /* test if it's player one => IA*/
        if (token == grid[location])
            return DEFAULT_MIN_V - nb_pions;
        else
            return DEFAULT_MAX_V + nb_pions;
    }
    return (token == grid[location]) ? alignement(grid, location) : - alignement(grid, location);
}


static int min(uc* grid, int profondeur, int location, int size_x, uc token);

static int max(uc* grid, int profondeur, int location, int size_x, uc token)
{
    if (profondeur == 0 || IA_is_winner(location, grid))
        return heuristique(grid, (token == P1_JETON) ? P2_JETON : P1_JETON, location, size_x);
    int max = DEFAULT_MAX_V;
    int rv_max = 0;
    uc next_token = (token == P1_JETON) ? P2_JETON : P1_JETON;
    for (int i = 1; i <= size_x; i++)
    {
        if (!is_filled_1(grid, i))
        {
            int pos = insert(grid, i, next_token);
            rv_max = min(grid, profondeur - 1, pos, size_x, next_token);
            if (rv_max > max)
                max = rv_max;
            grid[pos] = EMPTY_CASE;
        }
    }
    return max;
}

static int min(uc* grid, int profondeur, int location, int size_x, uc token)
{
    if (profondeur == 0 || IA_is_winner(location, grid))
        return  heuristique(grid, token, location, size_x);
    int min = DEFAULT_MIN_V;
    int rv_min = 0;
    uc next_token = (token == P1_JETON) ? P2_JETON : P1_JETON;
    for (int i = 1; i <= size_x; i++)
    {
        if (!is_filled_1(grid, i))
        {
            int pos = insert(grid, i, next_token);
            rv_min = max(grid, profondeur - 1, pos, size_x, next_token);
            if (rv_min < min)
                min = rv_min;
            grid[pos] = EMPTY_CASE;
        }
    }
    return min;
}



void p4_IA_3(uc* grid, int size_x, int size_y, uc token)
{
    int max = DEFAULT_MAX_V;
    size_y = 0;
    int rv_min = 0;
    int profondeur = DEFAULT_DEPHT;
    int rank = 0;
    static int prec_strike;
    switch (strike)
    {
        case START:
            if (!is_taken(grid, size_x / 2))
                prec_strike = insert(grid, size_x / 2, token) + 1;
            else
                prec_strike = insert(grid, size_x / 2 + 1, token) + 1;
            /*get the value in oder to realise the verification*/
            IA_insert_pos = prec_strike - 1;
          break;
      case SECOND_STRIKE :
                if (!is_taken(grid, prec_strike + size_x))
                    prec_strike = insert(grid, prec_strike + size_x, token);
                else if (!is_taken(grid, prec_strike + ONE))
                    prec_strike = insert(grid, prec_strike + ONE, token);
                else
                    prec_strike = insert(grid, prec_strike - ONE, token);
           break;
        default :
            {
                for (int i = 1; i <= size_x; i++)
                {
                    if (!is_filled_1(grid, i))
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
                IA_insert_pos = insert(grid, rank, token);
            }
         break;
    }
    strike++;
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
            if (IA_check_column(column))
            {
                if (!is_filled_1(grid, column))
                {
                    IA_insert_pos = insert (grid, column,  P1_JETON);
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
            if (IA_is_winner(IA_insert_pos, grid))
            {
                printf ( "%s Wins \n", player->name);
                finished = END;
                strike = 0;
                player->score++;
                // retore the player tokens nb // 
                players[0].tokens = MAX_SIZE / 2;
                players[1].tokens = MAX_SIZE / 2;
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






