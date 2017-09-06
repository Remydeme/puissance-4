#include "p4.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

extern struct player_s *players;
extern struct sigaction new_action, old_action;

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
        printf ( "2 - 1 VS JARVIS \n" );
        printf ( "3 - RULES \n");
        printf ("4 - EXIT \n\n");

        puts ("What do you want to do ? \n\n");
        scanf ("%d", &choice);
        system("clear");
        getc(stdin);// to remove the '\n'

        switch (choice)
        {
            case VS_1:
                config_player(VS_1);
                p4_game();
                break;

            case VS_IA:
                config_player(VS_IA);
                IA_mode_p4_game();
                break;

            case RULES:
                puts ("SOONER");
                system( "./p4.sh");
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
