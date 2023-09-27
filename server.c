#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "common.h"

int** mount_table(char *file)
{
    int** table = (int**)malloc(TABLE_DIMENSION * sizeof(int*));
    for (int i = 0; i < TABLE_DIMENSION; i++) 
        table[i] = (int*)malloc(TABLE_DIMENSION * sizeof(int));
    
    FILE *table_input = fopen(file, "r");
    if (table_input == NULL) 
    {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    for (int i = 0; i < TABLE_DIMENSION; i++)
        for(int j = 0; j < TABLE_DIMENSION; j++)
            fscanf(table_input, "%d,", &table[i][j]);

    return table;
}

int create_socket(char *ip_version)
{
    int sockfd;
    
    if(strcmp(ip_version, "v4") != 0 && strcmp(ip_version, "v6") != 0)
    {
        printf("Error! Invalid IP Version\n");
        exit(1);
    }
    if(strcmp(ip_version, "v4") == 0)
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

    else if(strcmp(ip_version, "v6") == 0)
        sockfd = socket(AF_INET6, SOCK_STREAM, 0);

    if(sockfd == -1) 
        logexit("socket");
    
    return sockfd;
}

int main(int argc, char *argv[])
{    
    if(argc != 5)
    {
        printf("Error! No IP Version, input file or Port specified\n");
        return 1;
    }

    extern char * optarg;
    char *input_file;

    int opt;
    
    char *ip_version = argv[1];
    int port = atoi(argv[2]);
    
    while ((opt = getopt(argc, argv, "i:")) != -1) 
    {
        switch (opt) 
        {
            case 'i':
                input_file = optarg;
                break;

            default:
                printf("Error! No input File specified\n");
                return 1;
        }
    }

    printf("IP Version: %s\n", ip_version);
    printf("Port: %d\n", port);
    
    int** table = mount_table(input_file);

    int sockfd = create_socket(ip_version);
    
    //bind(sockfd,);
    //listen(sockfd);
    

    return 0;
}