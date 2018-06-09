#pragma once

#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>
#include <cstring>

/* #define section, for now we will define the number of rows and columns */
#define ROWS 3
#define COLUMNS 3
#define PORTNUMBER 7171

int checkWin(char board[ROWS][COLUMNS])
{
    /************************************************************************/
    /* brute force check to see if someone won, or if there is a draw       */
    /* return a 0 if the game is 'over' and return -1 if game should go on  */
    /************************************************************************/
    if (board[0][0] == board[0][1] && board[0][1] == board[0][2]) // row matches
        return 1;

    else if (board[1][0] == board[1][1] && board[1][1] == board[1][2]) // row matches
        return 1;

    else if (board[2][0] == board[2][1] && board[2][1] == board[2][2]) // row matches
        return 1;

    else if (board[0][0] == board[1][0] && board[1][0] == board[2][0]) // column
        return 1;

    else if (board[0][1] == board[1][1] && board[1][1] == board[2][1]) // column
        return 1;

    else if (board[0][2] == board[1][2] && board[1][2] == board[2][2]) // column
        return 1;

    else if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) // diagonal
        return 1;

    else if (board[2][0] == board[1][1] && board[1][1] == board[0][2]) // diagonal
        return 1;

    else if (board[0][0] != '1' && board[0][1] != '2' && board[0][2] != '3' &&
             board[1][0] != '4' && board[1][1] != '5' && board[1][2] != '6' &&
             board[2][0] != '7' && board[2][1] != '8' && board[2][2] != '9')

        return 0; // Return of 0 means game over
    else
        return -1; // return of -1 means keep playing
}

void print_board(char board[ROWS][COLUMNS])
{
    /*****************************************************************/
    /* brute force print out the board and all the squares/values    */
    /*****************************************************************/

    std::cout << std::endl
              << std::endl
              << std::endl
              << "\tCurrent TicTacToe Game"
              << std::endl
              << std::endl;

    std::cout << "Player 1 (X)  -  Player 2 (O)"
              << std::endl
              << std::endl
              << std::endl;

    std::cout << "     |     |     " << std::endl;
    std::cout << "  " << board[0][0] << "  |  " << board[0][1] << "  |  " << board[0][2] << " " << std::endl;
    std::cout << "_____|_____|_____" << std::endl;
    std::cout << "     |     |     " << std::endl;
    std::cout << "  " << board[1][0] << "  |  " << board[1][1] << "  |  " << board[1][2] << " " << std::endl;
    std::cout << "_____|_____|_____" << std::endl;
    std::cout << "     |     |     " << std::endl;
    std::cout << "  " << board[2][0] << "  |  " << board[2][1] << "  |  " << board[2][2] << " " << std::endl;
    std::cout << "     |     |     "
              << std::endl
              << std::endl;
}

int initSharedState(char board[ROWS][COLUMNS])
{
    /* this just initializing the shared state aka the board */
    int i, j, count = 1;
    std::cout << "in sharedstate area" << std::endl;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
        {
            board[i][j] = count + '0';
            count++;
        }
    return 0;
}