/**
  * Name: Mubashwer Salman Khurshid
  * Username: mskh
  * StudentID: 601738
  *
  * Server program
  */
  
#include <stdio.h>
#include <string.h>  
#include <stdlib.h>   
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>   
#include <pthread.h>
#include <time.h> 

#include "connect4.h"
#include "server.h"

/* Mutex lock need for logging */
pthread_mutex_t lock;


int main(int argc , char *argv[])
{
    int server_socket_id, socket_length, port_number;
    struct sockaddr_in server , client;
    client_nfo client_data;
    client_nfo *client_data_pointer;
    FILE *log_fp = NULL;

    if (argc < 2) 
    {
        fprintf(stderr,"usage %s port\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    /* Optional daemonize (run on background) */
    if(argc == 3 && !strcmp(argv[2], "daemonize"))
        daemon(1, 0);
    
    port_number = atoi(argv[1]);

    /* Create socket */
    server_socket_id = socket(AF_INET , SOCK_STREAM , 0);
    if (server_socket_id == -1)
    {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    /* Update socket structure details */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port_number);
    
    /* Bind */
    if(bind(server_socket_id,(struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind error");
        close(server_socket_id);
        exit(EXIT_FAILURE);
    }
     
    /* Listen from socket */
    listen(server_socket_id, BACKLOG);
 
    /* Waiting for connections */
    printf("Waiting...\n");
    
    /* Thread for new connection */
    pthread_t tid;

    /* Initialize mutex */
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("mutex init failed\n");
        close(server_socket_id);
        exit(EXIT_FAILURE);
    }   
    
    socket_length = sizeof(struct sockaddr_in);

    /* Create logfile */
    log_fp = fopen(LOG_FILE, "w+");
    fclose(log_fp);

    /* Accept connections */
    while(1)
    {
        client_data.socket_id = accept(server_socket_id,
                    (struct sockaddr *)&client, (socklen_t *)&socket_length);

        if (client_data.socket_id < 0)
        {
            perror("accept failed");
            continue;
        }

        /* Get client ip address */
        if(inet_ntop(AF_INET,&client.sin_addr.s_addr,client_data.ip_address,
                INET_ADDRSTRLEN) == NULL)
            strcpy(client_data.ip_address, "unknown");

        /* Make a new copy of client data */
        client_data_pointer = malloc(sizeof(client_nfo));
        client_data_pointer->socket_id = client_data.socket_id;
        strcpy(client_data_pointer->ip_address, client_data.ip_address);
        
        /* Create thread for game */
        if(pthread_create(&tid ,NULL ,game_runner, 
        	(void*)client_data_pointer) < 0)
        {
            if(client_data_pointer != NULL) free(client_data_pointer);
            perror("thread creation error");
            continue;
        }
    } 


    /* Wrap up */
    close(server_socket_id); 
    pthread_mutex_destroy(&lock);
    if(client_data_pointer != NULL) free(client_data_pointer);
    return 0;
}
 
/*
 * This handles the game for each client.
 */
void *game_runner(void *client_data_pointer)
{
    /* Get client and details */
    client_nfo client = *(client_nfo*)client_data_pointer;
    int socket_id = client.socket_id;

    int status = 1, client_status = 1, move = 0, ai_move = 0, read_size;
    uint32_t move_nbo, ai_move_nbo;
    c4_t board;

    /* To ensure memory is freed at the end */
    pthread_detach(pthread_self());
    
    /* Set-up the game */
    srand(time(NULL));
    init_empty(board, NO_PRINT);

    /* log that client is connected */
    append_log(STATUS_CONNECTED, 0, &client, STATUS_CONNECTED, 0);

    /* Receive move from client */
    while((read_size = recv(socket_id, &move_nbo, sizeof(uint32_t), 0)) > 0)
    {
        if(status < 0) break;
        
        /* Get move from (client move in network byte order)*/
        move = ntohl(move_nbo);

        /* Make client move */
        status = human_play(board, move, NO_PRINT);
        client_status = status; /* needed for log */

        /* Make AI move if game is not over */
        if(status > 0)
        {
            ai_move = suggest_move(board, RED);
            status = ai_play(board, ai_move, NO_PRINT);
        }
        /* log the moves */
        append_log(status, ai_move, &client, client_status, move);

        /* If game is over, the old ai_move will be sent
           to client again but game will be terminated just after the
           client move is made in client program, so the ai move
           won't be made
        */

        /* Send the AI move to client */
        ai_move_nbo = htonl(ai_move);
        if(send(socket_id, &ai_move_nbo, sizeof(uint32_t), 0) < 0)
        {
            perror("send error");
            break;
        }
    }

    if(read_size == 0) {
        /* Client has been disconnected from server */
        if (status > 0) status = STATUS_ABNORMAL;
    }
    else if(read_size == -1)
    { 
        perror("recv error");
        if (status > 0) status = STATUS_ABNORMAL;
    }
    /* log any abnormal end of game */
    if(status == STATUS_ABNORMAL)
        append_log(STATUS_ABNORMAL, 0, &client, STATUS_ABNORMAL, 0);
    
    free(client_data_pointer);
    return 0;
}

/*
 * This handles logging server and client interaction.
 */
void append_log(int ai_status, int ai_move, client_nfo *client,
 int client_status, int client_move)
{
    FILE *log_fp = NULL;
    char timestamp[BUFFER_LEN], client_label[BUFFER_LEN],
         ai_label[BUFFER_LEN], client_log[BUFFER_LEN],
         ai_log[BUFFER_LEN], other[BUFFER_LEN];
    time_t raw_time;
    
    /* get timestamp from raw time */
    time(&raw_time);
    strftime(timestamp,BUFFER_LEN,"[%d %m %Y %T]",localtime(&raw_time)); 
    
    /* client log label */
    sprintf(client_label, "%s(%s)(soc_id %d)", timestamp, client->ip_address,
        client->socket_id);
    /* ai move log label */
    sprintf(ai_label, "%s(0.0.0.0)", timestamp); 

    /* for sanity */
    client_log[0] = '\0'; ai_log[0] = '\0'; other[0] = '\0';
    
    /* log client's and/or server's move(s) */
    if(client_status != STATUS_ABNORMAL && client_status != STATUS_CONNECTED)
        sprintf(client_log, "%s client's move = %d\n", client_label, 
            client_move); 
    if(ai_status != STATUS_ABNORMAL && ai_status != STATUS_CONNECTED &&
        client_status > 0)
        sprintf(ai_log, "%s server's move = %d\n", ai_label, ai_move); 

    /* other: log if client has just connected or game is over? */
    if(client_status == STATUS_CONNECTED || ai_status == STATUS_CONNECTED)
        sprintf(other, "%s client connected\n", client_label);
    
    else if(client_status < 0)
        sprintf(other,"%s game over, code = %d\n",client_label,client_status);

    else if(ai_status < 0)
        sprintf(other,"%s game over, code = %d\n",client_label,ai_status);

    
    /* test */
    printf("%s%s%s", client_log, ai_log, other);
    fflush(stdout);

    /* Write to log.txt */
    pthread_mutex_lock(&lock);
    log_fp = fopen (LOG_FILE, "a+");
    fprintf(log_fp, "%s%s%s", client_log, ai_log, other);
    fclose(log_fp);
    pthread_mutex_unlock(&lock);
}
