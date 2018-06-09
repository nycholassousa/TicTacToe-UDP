#pragma once

#include "../config/config.h"

int tictactoe(char board[ROWS][COLUMNS]);

char serverIP[29];

int runClient(std::string ip)
{
    int rc;
    char board[ROWS][COLUMNS];
    std::strcpy(serverIP, ip.c_str());

    rc = initSharedState(board); // Initialize the 'game' board
    rc = tictactoe(board);       // call the 'game'
    return 0;
}

int tictactoe(char board[ROWS][COLUMNS])
{
    /* this is the meat of the game, you'll look here for how to change it up */
    int sd;
    struct sockaddr_in server_address, from_address;
    memset(&server_address, 0, sizeof(server_address));
    socklen_t fromLength;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd == -1)
    {
        perror("client: socket");
        exit(1);
    }

    struct timeval tv;
    int TIMEOUT = 30;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORTNUMBER);
    server_address.sin_addr.s_addr = inet_addr(serverIP);

    //bind(sd, (struct sockaddr *)&server_address, sizeof(server_address));

    printf("Connected! Game Start!\n");

    int player = 1; // keep track of whose turn it is
    int i, choice;  // used for keeping track of choice user makes
    int row, column;
    char mark = 'X'; // either an 'x' or an 'o'
    int conv;
    int start = 0;
    int rc;
    struct pollfd mypoll = {STDIN_FILENO, POLLIN | POLLPRI};
    /* loop, first print the board, then ask player 'n' to make a move */

    do
    {
        print_board(board);
        if (start)
        {
            player = 2;
            rc = recvfrom(sd, &conv, sizeof(conv), 0, NULL, NULL);
            if (rc < 0)
            {
                printf("Player2 disconnected or timeout! Game Over! You win!\n");
                close(sd);
                exit(0);
            }
            int player2_choice = ntohl(conv);
            row = (int)((player2_choice - 1) / ROWS);
            column = (player2_choice - 1) % COLUMNS;
            board[row][column] = 'O';
            print_board(board);
            i = checkWin(board);
            if (i != -1)
                break;
        }
        player = 1;
        start = 1;

        while (1)
        {
            printf("Player %d, enter a number within %ds:  \n", player, TIMEOUT); // print out player so you can pass game
            if (poll(&mypoll, 1, TIMEOUT * 1000))
            {
                scanf("%d", &choice); //using scanf to get the choice
            }
            else
            {
                printf("Input timeout! You loss!\n");
                close(sd);
                return 0;
            }
            /******************************************************************/
            /** little math here. you know the squares are numbered 1-9, but  */
            /* the program is using 3 rows and 3 columns. We have to do some  */
            /* simple math to conver a 1-9 to the right row/column            */
            /******************************************************************/
            row = (int)((choice - 1) / ROWS);
            column = (choice - 1) % COLUMNS;

            /* first check to see if the row/column chosen is has a digit in it, if it */
            /* square 8 has and '8' then it is a valid choice                          */

            if (choice < 10 && choice > 0 && board[row][column] == (choice + '0'))
            {
                board[row][column] = mark;
                int conv = htonl(choice);
                rc = sendto(sd, &conv, sizeof(conv), 0, (struct sockaddr *)&server_address, sizeof(server_address));
                if (rc < 0)
                {
                    perror("client: sendto");
                    close(sd);
                    exit(1);
                }
                break;
            }
            else
            {
                printf("Invalid move\n");
                getchar();
            }
        }
        /* after a move, check to see if someone won! (or if there is a draw */
        i = checkWin(board);

    } while (i == -1); // -1 means no one won

    /* print out the board again */
    print_board(board);

    if (i == 1) // means a player won!! congratulate them
        printf("==> \tPlayer %d wins\n ", player);
    else
        printf("==> \tGame draw"); // ran out of squares, it is a draw
    close(sd);
    return 0;
}