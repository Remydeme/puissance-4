#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef unsigned char uc;

#define ONE 1
#define P1_TOKEN 'X'
#define P2_TOKEN 'O'
#define DEFAULT_MAX -1000
#define DEFAULT_MIN 1000
#define EMPTY ' '
#define IA_MAX_SIZE h * w
#define WIN 4
#define START 1
#define SECOND_STRIKE 2
#define DEFAULT_DEPHT 8


int IA_insert_pos = 0;
static int strike = 1;


static int is_filled_1(uc* grid, int column, int w, int h)
{
    return grid[(h - 1) * w  +  column - 1] != EMPTY;
}

static bool is_taken(uc* grid, int column, int w, int h)
{
    return (column >= 0 && column < h * w) && grid[column - 1] != EMPTY ? true : false;
}

static int is_empty(uc case_value)
{
    return case_value == EMPTY; 
}

static int insert(uc* grid, int column, uc token, int w, int h)
{
    int position = column - 1;
    for (int i = 0 ; (i < h && position < IA_MAX_SIZE) ; i++)
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





static int IA_check_vertical(int pos, uc* grid, int w, int h, bool mode)
{
    int cursor = pos;
    uc token = grid[pos];
    int counter = 1;
    bool find = true;
    /*start by checking the right side*/
    while (cursor < IA_MAX_SIZE && find)
    {
        cursor += w;
        if (grid[cursor] == token)
            counter++;
         else if (grid[cursor] == EMPTY && (cursor + w) < IA_MAX_SIZE && grid[cursor + w] == token && mode)
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
        else if (grid[cursor] == EMPTY && cursor - w >= 0 && grid[cursor - w] ==  token && mode)
            counter++;
        else
            find = false;
    }
    return counter;
}

static int IA_check_horizontal(int pos, uc* grid, int w, int h, bool mode)
{
    int cursor = pos + ONE;
    uc token = grid[pos];
    int counter = 1;
    bool find = true;
    int low_limit = ((pos / w) * w);
    low_limit = low_limit < 0 ? 0 : low_limit;
    int hight_limit = ((pos / w) + 1) * w;
    hight_limit = hight_limit > IA_MAX_SIZE ? IA_MAX_SIZE : hight_limit;
    /*start by checking the right side*/
    while (cursor < hight_limit && find)
    {
        if (grid[cursor] == token)
            counter++;
       else if (grid[cursor] == EMPTY 
           && (cursor + ONE) < hight_limit 
            && grid[cursor + ONE]
            && mode)
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
        else if (grid[cursor] == EMPTY && (cursor - ONE) >= low_limit 
            && grid[cursor - ONE] && mode)
            counter++;
        else
            find = false;
        cursor -= ONE;
    }
    return counter;
}

static int IA_check_rdiagonal(int pos, uc* grid, int w, int h, int mode)
{
    int cursor = pos;
    uc token = grid[pos];
    int counter = ONE;
    bool find = true;
    /*start by checking the right side*/
    while (((cursor + 1) % w) && cursor < IA_MAX_SIZE  && find && mode)
    {
        cursor += (w + 1);
        if (grid[cursor] == token)
            counter++;
        else if (grid[cursor] == EMPTY 
            && cursor + (w + 1) % w && cursor + w + 1 < IA_MAX_SIZE
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
        else if (grid[cursor] == EMPTY 
            && ((cursor - (w + 1)) % w)  && (cursor - (w + 1)) >= 0 && mode 
            && grid[cursor - (w + 1)] == token)
            counter++;
        else
            find = false;
    }
    return counter;
}

static int IA_check_ldiagonal(int pos, uc* grid, int w, int h, int mode)
{
    int cursor = pos;
    uc token = grid[pos];
    int counter = 1;
    bool find = true;
    /*start by checking the right side*/
    while ((cursor % w) && cursor < IA_MAX_SIZE && find)
    {
        cursor += (w - 1);
        if (grid[cursor] == token)
            counter++;
        else if (mode && grid[cursor] == EMPTY 
            && (cursor % w) && cursor + (w - 1) < IA_MAX_SIZE 
            && grid[cursor + (w - 1)] == token)
            counter++;
        else
            find = false;
    }
    /* check the left side */
    cursor = pos;
    find = true;
    while (((cursor + 1) % w) && cursor - (w - 1) >= 0 && find)
    {
        cursor -= (w - 1);
        if (grid[cursor] == token)
            counter++;
         else if (mode && grid[cursor] == EMPTY 
            && (cursor % w) && cursor >= 0 
            && grid[cursor - (w - 1)] == token)
            counter++;
        else
            find = false;
    }
    return counter;
}


static int alignement(uc * grid, int location, int w, int h, int mode)
{
    int total = 0;
    total += IA_check_vertical(location, grid, w, h, mode);
    total += IA_check_horizontal(location, grid, w, h, mode);
    total += IA_check_rdiagonal(location, grid, w, h, mode);
    total += IA_check_ldiagonal(location, grid, w, h, mode);

    return total;
}


static int IA_is_winner(int location, uc* grid, int w, int h, bool mode)
{
    if (IA_check_horizontal(location, grid, w, h, mode) >= WIN)
    {
       // printf ("horizontal : %d ", location);
        return 1;
    }
    else if (IA_check_vertical(location, grid, w, h,  mode) >= WIN)
    {
       // printf ("vertical : %d ", location);
        return 1;
    }
    else if (IA_check_rdiagonal(location, grid, w, h, mode) >= WIN)
    {
      //  printf ("r diagonal %d ", location);
        return 1;
    }
    else if (IA_check_ldiagonal(location, grid, w, h,  mode) >= WIN)
    {
       // printf ("l diagonal %d ", location);
        return 1;
    }
    else
        return 0;
}



static int heuristique(uc* grid, uc token, int location, int size_x, int size_y)
{
    int nb_pions = 0;
    bool end = false;
    for (int i = 0; i < size_y * size_x && ! end; i++)
    {
        if (grid[i] == ' ')
            end = true;
        else
            nb_pions++;
    }

    if (IA_is_winner(location, grid, size_x, size_y, false))
    {
        /* test if it's player one => IA*/
        if (token == grid[location])
            return DEFAULT_MIN - nb_pions;
        else
            return DEFAULT_MAX + nb_pions;
    }
    return (token == grid[location]) ? alignement(grid, location, size_x, size_y, true) : - alignement(grid, location, size_x,
    size_y, true);
}


static int min(uc* grid, int profondeur, int location, int size_x, int size_y, uc token);

static int max(uc* grid, int profondeur, int location, int size_x, int size_y, uc token)
{
    if (profondeur == 0 || IA_is_winner(location, grid, size_x, size_y,  false))
        return heuristique(grid, (token == P1_TOKEN) ? P2_TOKEN : P1_TOKEN, location, size_x, size_y);
    int max = DEFAULT_MAX;
    int rv_max = 0;
    uc next_token = (token == P1_TOKEN) ? P2_TOKEN : P1_TOKEN;
    for (int i = 1; i <= size_x; i++)
    {
        if (!is_filled_1(grid, i, size_x, size_y))
        {
            int pos = insert(grid, i, next_token, size_x, size_y);
            rv_max = min(grid, profondeur - 1, pos, size_x, size_y, next_token);
            if (rv_max > max)
                max = rv_max;
            grid[pos] = EMPTY;
        }
    }
    return max;
}


static int min(uc* grid, int profondeur, int location, int size_x, int size_y, uc token)
{
    if (profondeur == 0 || IA_is_winner(location, grid, size_x, size_y,  false))
        return  heuristique(grid, token, location, size_x, size_y);
    int min = DEFAULT_MIN;
    int rv_min = 0;
    uc next_token = (token == P1_TOKEN) ? P2_TOKEN : P1_TOKEN;
    for (int i = 1; i <= size_x; i++)
    {
        if (!is_filled_1(grid, i, size_x, size_y))
        {
            int pos = insert(grid, i, next_token, size_x, size_y);
            rv_min = max(grid, profondeur - 1, pos, size_x, size_y, next_token);
            if (rv_min < min)
                min = rv_min;
            grid[pos] = EMPTY;
        }
    }
    return min;
}



void p4_IA_3(unsigned char* grid, int size_x, int size_y, unsigned char token)
{
    int max = DEFAULT_MAX;
    int rv_min = 0;
    int profondeur = DEFAULT_DEPHT;
    int rank = 0;
    static int prec_strike;
    switch (strike)
    {
        case START:
            if (!is_taken(grid, size_x / 2, size_x, size_y))
                prec_strike = insert(grid, size_x / 2, token, size_x, size_y) + 1;
            else
                prec_strike = insert(grid, size_x / 2 + 1, token, size_x, size_y) + 1;
            /*get the value in oder to realise the verification*/
          break;
      case SECOND_STRIKE :
                if (!is_taken(grid, prec_strike + size_x, size_x, size_y))
                    prec_strike = insert(grid, prec_strike + size_x, token, size_x, size_y);
                else if (!is_taken(grid, prec_strike + ONE, size_x, size_y))
                    prec_strike = insert(grid, prec_strike + ONE, token, size_x, size_y);
                else
                    prec_strike = insert(grid, prec_strike - ONE, token, size_x, size_y);
           break;
        default :
            {
                for (int i = 1; i <= size_x; i++)
                {
                    if (!is_filled_1(grid, i, size_x, size_y))
                    {
                        int location = insert(grid, i, token, size_x, size_y);
                        rv_min = min(grid, profondeur - 1, location, size_x,size_y,  token);
                        if (rv_min > max)
                        {
                            max = rv_min;
                            rank = i;
                        }
                        grid[location] = EMPTY;
                    }
                }
                insert(grid, rank, token, size_x, size_y);
            }
         break;
    }
    strike++;
}





