/**
  * Name: Mubashwer Salman Khurshid
  * Username: mskh
  * StudentID: 601738
  *
  * Client program
  */
  
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>    
#include <sys/socket.h>    
#include <arpa/inet.h> 
#include <netdb.h>
#include <unistd.h>

#include "connect4.h"

#define PRINT 1

int main(int argc , char *argv[])
{
    int socket_id, port_number, status = 1, move = 0,ai_move = 0,autoplay = 0;
    uint32_t move_nbo, ai_move_nbo;
    struct sockaddr_in server;
    c4_t board;
    struct hostent *host;

    if (argc < 3) 
    {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Get server port number */
    port_number = atoi(argv[2]);

    /* Translate hostname to ip address */
    host = gethostbyname(argv[1]);
    if (host == NULL) 
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(EXIT_FAILURE);
    }
    
    /* Optional daemonize and autoplay */
    if(argc == 4 && !strcmp(argv[3], "daemonize"))
    {
        daemon(1, 0);
        autoplay = 1;
    }

    /* Create a socket */
    if((socket_id = socket(AF_INET ,SOCK_STREAM , 0)) == -1)
    {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    /* Update socket structure details */
    memcpy((char*)&server.sin_addr.s_addr,(char*)host->h_addr,host->h_length);
    server.sin_family = AF_INET;
    server.sin_port = htons(port_number);
 
    /* Connect to server */
    if (connect(socket_id, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect error");
        close(socket_id);
        exit(EXIT_FAILURE);
    }
    
    /* Set-up the game */
    init_empty(board, PRINT);
    print_config(board); 

    /* Play with server AI */
    while(status > 0)
    { 
        /* Get a move from human or autoplay if daemonized*/
        if(autoplay)
            move = suggest_move(board, YELLOW);
        else if((move = get_move(board)) == EOF) 
            break;
        
        /* Send the move to server in network byte order */
        move_nbo = htonl(move);
        if(send(socket_id, &move_nbo, sizeof(uint32_t), 0) < 0)
        {
            perror("send error");
            break;
        }

        /* Receive ai_move from server in network byte order */
        if(recv(socket_id , &ai_move_nbo , sizeof(uint32_t) , 0) < 0)
        {
            perror("recv error");
            break;
        }
        
        /* Get the AI move */
        ai_move = ntohl(ai_move_nbo);
        
        /* Make the human move */
        status = human_play(board, move, PRINT);
        
        /* If game is not over, make the AI move */
        if(status > 0)
            status = ai_play(board, ai_move, PRINT);       
    }
     
    close(socket_id);
    return 0;
}