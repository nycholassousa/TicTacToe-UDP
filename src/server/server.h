#pragma once

#include "../config/config.h"

int tictactoeServer(char board[ROWS][COLUMNS]);

int runServer()
{
    int rc;
    char board[ROWS][COLUMNS];

    rc = initSharedState(board); // Initialize the 'game' board
    rc = tictactoeServer(board); // call the 'game'
    return 0;
}

int tictactoeServer(char board[ROWS][COLUMNS])
{
    /* this is the meat of the game, you'll look here for how to change it up */
    int sd;
    struct sockaddr_in server_address;
    struct sockaddr_in from_address;
    memset(&server_address, 0, sizeof(server_address));
    socklen_t fromLength;

    int rc;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd == -1)
    {
        perror("server: socket");
        exit(1);
    }

    struct timeval tv;
    int TIMEOUT = 30;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORTNUMBER);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("server: bind");
        close(sd);
        exit(1);
    }

    printf("Connected! Game Start!\n");

    int player = 2; // keep track of whose turn it is
    int i, choice;  // used for keeping track of choice user makes
    int row, column;
    char mark = 'O'; // either an 'x' or an 'o'
    int start = 0;
    int conv;
    /* loop, first print the board, then ask player 'n' to make a move */

    struct pollfd mypoll = {STDIN_FILENO, POLLIN | POLLPRI};

    do
    {
        print_board(board);

        player = 1;
        fromLength = sizeof(from_address);
        rc = recvfrom(sd, &conv, sizeof(conv), 0, (struct sockaddr *)&from_address, &fromLength);
        //setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
        if (rc < 0)
        {
            printf("Player1 disconnected or timeout! Game Over! You win!\n");
            close(sd);
            exit(0);
        }
        int player1_choice = ntohl(conv);
        row = (int)((player1_choice - 1) / ROWS);
        column = (player1_choice - 1) % COLUMNS;
        board[row][column] = 'X';
        i = checkWin(board);
        if (i != -1)
            break;
        player = 2;
        print_board(board); // call function to print the board on the screen

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

            //mark = (player == 1) ? 'X' : 'O'; //depending on who the player is, either us x or o
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
                rc = sendto(sd, &conv, sizeof(conv), 0, (struct sockaddr *)&from_address, fromLength);
                if (rc < 0)
                {
                    perror("server: sendto");
                    close(sd);
                    exit(1);
                }
                break;
            }
            else
            {
                printf("Invalid move\n");
                getchar(); //handle character invalid input
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