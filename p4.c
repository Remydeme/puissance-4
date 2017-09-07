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
int strike = 0;

///////////////////////////// player ////////////////////////////

int is_filled(uc* grid, int column)
{
    return grid[(h - 1) * w  +  column - 1] != EMPTY_CASE;
}

int is_winner_1(int location, uc* grid)
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
                    insert_pos = insert (grid, column,  P1_TOKEN);
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
                    insert_pos = insert (grid, column,  P2_TOKEN);
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



void display(uc* grid)
{
    for (int i = 0; i < w; i++)
        printf("-----");
    printf ("\n");
    for (int i = 0; i < w; i++)
        printf ("| %d |", i + 1);
    printf ("\n");
    for (int i = 0; i < w; i++)
        printf("-----");
    printf("\n");
    for (int i = (h - 1) * w; i >= 0; i -= 7)
    {
        for (int j = i; j < i + w; j++)
        {
            printf ("| ");
            printf ("%c", grid[j]);
            printf (" |");
        }
        printf("\n");
            for (int i = 0; i < w; i++)
                printf("-----");
        printf ("\n");
    }
}


//////////////////////////////////// grid //////////////////////////////


/*
   | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
   _ ___________________________
   | 0 | 1 | 0 | 3 | 4 | 5 | 6 |
   _ ___________________________
   | 0 | 1 | X | 3 | 4 | 5 | 6 |
   _ ___________________________
   | 0 | 1 | X | 3 | 4 | 5 | 6 |
   _ ___________________________

   | 0 | O | X | 3 | 4 | 5 | 6 |
   _ ____________________________ 
 */



int is_empty(uc case_value)
{
    return case_value == EMPTY_CASE;
}




uc* create_tab(int w, int h)
{
    uc* tmp =  calloc (MAX_SIZE, sizeof (uint8_t));
    for (int i = 0; i < MAX_SIZE; i++)
        tmp[i] = EMPTY_CASE;
    return tmp;
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

int check_horizontal(int pos, uc* grid)
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
        else
            find = false;
        cursor -= ONE;
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
    while (((cursor + 1) % w) && cursor < MAX_SIZE  && find)
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
    while ((cursor % w)  && cursor >= 0 && find)
    {
        cursor -= (w + 1);
        if (grid[cursor] == token)
            counter++;
        else
            find = false;
        //printf (" cursor : %d nb : %d  \n", cursor, counter);
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
    while ((cursor % w) && cursor < MAX_SIZE && find)
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
            if (is_winner_1(insert_pos, grid))
            {
                finished = END;
                player->score++;
                display(grid);
                printf ( "%s wins ! ", player->name);
                system("./win.sh");
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

void check_party_null(int whom, int *finished)
{
    if (whom == MAX_SIZE)
    {
        fprintf (stdout, "msg : %s \n", MATCH_NULL);
        *finished = END;
    }

}


int check_column(int column)
{
    return column >= 1 && w >= column ? 1 : 0;
}

////////////////////////////////////////////// IA //////////////////////////////////////////////


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


int IA_check_vertical(int pos, uc* grid, int w, int h, bool mode)
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
         else if (grid[cursor] == EMPTY_CASE && (cursor + w) < MAX_SIZE && grid[cursor + w] == token && mode)
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
        else if (grid[cursor] == EMPTY_CASE && cursor - w >= 0 && grid[cursor - w] ==  token && mode)
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
    hight_limit = hight_limit > MAX_SIZE ? MAX_SIZE : hight_limit;
    /*start by checking the right side*/
    while (cursor < hight_limit && find)
    {
        if (grid[cursor] == token)
            counter++;
       else if (grid[cursor] == EMPTY_CASE 
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
        else if (grid[cursor] == EMPTY_CASE && (cursor - ONE) >= low_limit 
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
    while (((cursor + 1) % w) && cursor < MAX_SIZE  && find && mode)
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
            && ((cursor % w)  && cursor >= 0 && mode) 
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
    while ((cursor % w) && cursor < MAX_SIZE && find)
    {
        cursor += (w - 1);
        if (grid[cursor] == token)
            counter++;
        else if (grid[cursor] == EMPTY_CASE 
            && (cursor % w) && cursor < MAX_SIZE 
            && grid[cursor + (w - 1)] == token && mode)
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
         else if (grid[cursor] == EMPTY_CASE 
            && (cursor % w) && cursor >= 0 
            && grid[cursor - (w - 1)] == token && mode)
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
    if (IA_check_horizontal(location, grid, w, h, mode) >= WIN_VAL)
    {
       // printf ("horizontal : %d ", location);
        return 1;
    }
    else if (IA_check_vertical(location, grid, w, h,  mode) >= WIN_VAL)
    {
       // printf ("vertical : %d ", location);
        return 1;
    }
    else if (IA_check_rdiagonal(location, grid, w, h, mode) >= WIN_VAL)
    {
      //  printf ("r diagonal %d ", location);
        return 1;
    }
    else if (IA_check_ldiagonal(location, grid, w, h,  mode) >= WIN_VAL)
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
            return DEFAULT_MIN_V - nb_pions;
        else
            return DEFAULT_MAX_V + nb_pions;
    }
    return (token == grid[location]) ? alignement(grid, location, size_x, size_y, true) : - alignement(grid, location, size_x,
    size_y, true);
}



int max(uc* grid, int profondeur, int location, int size_x, int size_y, uc token)
{
    if (profondeur == 0 || IA_is_winner(location, grid, size_x, size_y,  false))
        return heuristique(grid, (token == P1_TOKEN) ? P2_TOKEN : P1_TOKEN, location, size_x, size_y);
    int max = DEFAULT_MAX_V;
    int rv_max = 0;
    uc next_token = (token == P1_TOKEN) ? P2_TOKEN : P1_TOKEN;
    for (int i = 1; i <= size_x; i++)
    {
        if (!is_filled_1(grid, i))
        {
            int pos = insert(grid, i, next_token);
            rv_max = min(grid, profondeur - 1, pos, size_x, size_y, next_token);
            if (rv_max > max)
                max = rv_max;
            grid[pos] = EMPTY_CASE;
        }
    }
    return max;
}


int min(uc* grid, int profondeur, int location, int size_x, int size_y, uc token)
{
    if (profondeur == 0 || IA_is_winner(location, grid, size_x, size_y,  false))
        return  heuristique(grid, token, location, size_x, size_y);
    int min = DEFAULT_MIN_V;
    int rv_min = 0;
    uc next_token = (token == P1_TOKEN) ? P2_TOKEN : P1_TOKEN;
    for (int i = 1; i <= size_x; i++)
    {
        if (!is_filled_1(grid, i))
        {
            int pos = insert(grid, i, next_token);
            rv_min = max(grid, profondeur - 1, pos, size_x, size_y, next_token);
            if (rv_min < min)
                min = rv_min;
            grid[pos] = EMPTY_CASE;
        }
    }
    return min;
}


void p4_IA_level(uc* grid, int size_x, int size_y, uc token, int depht)
{
    int max = DEFAULT_MAX_V;
    int rv_min = 0;
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
            insert_pos = prec_strike - 1;
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
                        rv_min = min(grid, depht - 1, location, size_x, size_y, token);
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
    strike++;
}


struct player_s* IA_turn(int whom, int level)
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
                    insert_pos = insert (grid, column,  P1_TOKEN);
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
            p4_IA_level(grid, w, h, P2_TOKEN, level);
            played = 1;
        }
    }
    return &players[whom % 2]; 
}


uc* IA_mode_p4_game()
{
    printf ( "what is your level ?\n");
    printf (" 1- Amateur \n 2- Master \n 3- Champion \n");
    int level = 0;
    scanf ("%d", &level);
    getc (stdin);
    level = level > 3? 3 : level;
    level = level < 0 ? 0 : level;
    if (level == 1)
        level = AMATEUR;
    else if (level == 2)
        level = MASTER;
    else 
        level = CHAMPION; 
    int finished = START;
    int whom = 0;
    struct player_s* player = NULL;
    grid = create_tab(w, h);
    if (grid)
    {
        while (!finished)
        {
            display(grid);
            player = IA_turn(whom, level);
            if (IA_is_winner(insert_pos, grid, w, h,  false))
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
