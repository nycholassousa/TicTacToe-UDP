#include <iostream>
#include "server/server.h"
#include "client/client.h"

int main(int argc, char **argv)
{
    char choice;
    std::string ipNumber;

    do
    {
        std::cout << "server (s) or client (c)?: " << std::endl;
        std::cin >> choice;
    } while (choice != 's' && choice != 'S' && choice != 'C' && choice != 'c');

    std::cout << choice << std::endl;

    if (choice == 'C' || choice == 'c')
    {
        std::cout << "Type IP: " << std::endl;
        std::cin >> ipNumber;
        runClient(ipNumber);
    }
    else
    {
        runServer();
    }

    return 0;
}