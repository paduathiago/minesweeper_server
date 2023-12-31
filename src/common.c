#include "common.h"

#include <arpa/inet.h>

void logexit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

char** mount_answer_board(int **board)
{
    char** answer = (char**)malloc(TABLE_DIMENSION * sizeof(char*));
    for (int i = 0; i < TABLE_DIMENSION; i++) 
        answer[i] = (char*)malloc(TABLE_DIMENSION * sizeof(char));
    
    for (int i = 0; i < TABLE_DIMENSION; i++)
    {
        for(int j = 0; j < TABLE_DIMENSION; j++)
        {
            int count_bombs = 0;
            if(board[i][j] == BOMB)  // case when cell has a bomb
            {
                answer[i][j] = '*';
                continue;
            }
            if(board[i][j] == HIDDEN)
            {
                answer[i][j] = '-';
                continue;
            }
            if(board[i][j] == FLAGGED)
            {
                answer[i][j] = '>';
                continue;
            }
            if (i > 0) {
                if(board[i - 1][j] == BOMB)  //northern neighbor
                    count_bombs++;
            }
            if (i < TABLE_DIMENSION - 1) {
                if(board[i + 1][j] == BOMB)  // southern neighbor
                    count_bombs++;
            }
            if (j > 0) {
                if(board[i][j - 1] == BOMB)  // western neighbor
                    count_bombs++;
            }
            if (j < TABLE_DIMENSION - 1) {
                if(board[i][j + 1] == BOMB)  // eastern neighbor
                    count_bombs++;
            }
            if (i > 0 && j > 0) {
                if(board[i - 1][j - 1] == BOMB)  // northwestern neighbor
                    count_bombs++;
            }
            if (i > 0 && j < TABLE_DIMENSION - 1) {
                if(board[i - 1][j + 1] == BOMB)  // northeastern neighbor
                    count_bombs++;
            }
            if (i < TABLE_DIMENSION - 1 && j > 0) {
                if(board[i + 1][j - 1] == BOMB)  // southwestern neighbor
                    count_bombs++;
            }
            if (i < TABLE_DIMENSION - 1 && j < TABLE_DIMENSION - 1) {
                if(board[i + 1][j + 1] == BOMB)  // southeastern neighbor
                    count_bombs++;
            }
            sprintf(&answer[i][j], "%d", count_bombs);
        }
    }
    return answer;
}

void parse_addr(const char *addrstr, const char *addrport, struct sockaddr_storage *storage)
{
    if(addrport == NULL || addrstr == NULL)
    {
        printf("Error! No IP or Port specified\n");
        exit(1);
    }

    u_int16_t port = (u_int16_t)atoi(addrport);
    if(port == 0)
    {
        printf("Error! Invalid Port\n");
        exit(1);
    }

    port = htons(port);  // host to network short

    struct in_addr inaddr4;
    if(inet_pton(AF_INET, addrstr, &inaddr4))
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return;
    }

    struct in6_addr inaddr6;
    if(inet_pton(AF_INET6, addrstr, &inaddr6))
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return;
    }

    else 
    {
        printf("Error! Invalid IP\n");
        exit(1);
    }
}

void server_sockaddr_init(char *protocol, char * addr, struct sockaddr_storage *storage)
{
    if(strcmp(protocol, "v4") != 0 && strcmp(protocol, "v6") != 0)
    {
        printf("Error! Invalid IP Version\n");
        exit(1);
    }
    if(strcmp(protocol, "v4") == 0)
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = htons(atoi(addr));
        addr4->sin_addr.s_addr = INADDR_ANY;
    }
    else if(strcmp(protocol, "v6") == 0)
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = htons(atoi(addr));
        addr6->sin6_addr = in6addr_any;
    }
}

int encode_action(const char *action_str)
{
    if(!strcmp(action_str, "start"))
        return START;
    else if(!strcmp(action_str, "reveal"))
        return REVEAL;
    else if(!strcmp(action_str, "flag"))
        return FLAG;
    else if(!strcmp(action_str, "remove_flag"))
        return REMOVE_FLAG;
    else if(!strcmp(action_str, "reset"))
        return RESET;
    else if(!strcmp(action_str, "exit"))
        return EXIT;
    else
        return -1;
}

size_t receive_all(int socket, void *buffer, size_t size)
{
    size_t total_received = 0;

    while (total_received < size) {
        size_t bytes_received = recv(socket, buffer + total_received, size - total_received, 0);

        if (bytes_received <= 0)
        {
            if (bytes_received == 0)  // Connection closed by peer
                return total_received;
            else 
            {
                perror("Error receiving data");
                return -1;
            }
        }
        total_received += bytes_received;
    }

    return total_received;
}

void print_board(char **board)
{
    for(int i = 0; i < TABLE_DIMENSION; i++)
    {
        for(int j = 0; j < TABLE_DIMENSION; j++)
            printf("%c\t\t", board[i][j]);
        printf("\n");
    }
}
