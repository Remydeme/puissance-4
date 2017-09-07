#include "p4.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

extern struct player_s *players;
extern struct sigaction new_action, old_action;
extern int h;
extern int w;
extern int level;
int main()
{
    int choice = 0;
    players = create_player();
    system ("./p4.sh"); 
    handle_signal();
    /* Set up the structure to specify the new action. */
    do
    {
        printf ( "1 - 1 VS 1 \n");
        printf ( "2 - 1 VS JARVIS \n");
        printf ( "3 - RESTORE GAME \n"); 
        printf ( "4 - RULES \n");
        printf ( "5 - MAKE YOUR GRID\n");
        printf ("6 - EXIT \n\n");
        puts ("What do you want to do ? \n\n");
        scanf ("%d", &choice);
        system("clear");
        getc(stdin);// to remove the '\n'
        switch (choice)
        {
            case VS_1:
                config_player(VS_1);
                p4_game(NORMAL);
                break;

            case VS_IA:
                // ADD A FUN SENTANCE OR PIC
                config_player(VS_IA);
                IA_mode_p4_game();
                break;

            case RESTORE_MODE:
                p4_game(RESTORE_MODE);
                break;
            case RULES:
            break;
            case BUILD_MODE:
            do {
               printf ("Choose your grid column number (Min 5 - max 15) : \n");
               scanf("%d", &w);
               getc(stdin);
               if (w < 5 && w > 15)
                   fprintf (stderr, "%s \n", "Caution : your column value must be between 5 and 15 !");
            } while ( w < 5 && w > 15);

            do{
               printf ("Choose your grid hight (Min 5 - MAx 16)");
               scanf("%d", &h);
               getc(stdin);
               if (h < 5 && h > 15)
                   fprintf (stderr, "%s \n", "Caution : your column value m st be between 5 and 15 !");
            } while (h < 5 && h > 16);
            system ("clear");
            printf ("1 - YOU VS JARVIS \n");
            printf ("2 - 1 VS 1 \n");
            int choice = 0;
            scanf ("%d", &choice);
            getc(stdin);
            if (choice == VS_1)
                p4_game(NORMAL);
            else if (choice == VS_IA)
                IA_mode_p4_game();
            else 
                fprintf (stderr, "Bad input you ! return to the main menu");
            break;
            default :
                puts ("SOONER");
                system( "more p4.sh");
                exit (0);
                break;
                return 0;
        }
    } while (choice != 4);
}
