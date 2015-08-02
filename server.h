/**
  * Name: Mubashwer Salman Khurshid
  * Username: mskh
  * StudentID: 601738
  *
  * Library for Server program
  */
  
#ifndef _SERVER_INCLUDED_
#define _SERVER_INCLUDED_


/*****************************************************************************/
/* Constants and type definitions */
/*****************************************************************************/

#define BUFFER_LEN 256

#define LOG_FILE "log.txt"

#define STATUS_CONNECTED 0

#define NO_PRINT 0 /* Print Connect4 board and status? */

#define BACKLOG 511 /* Pending connections queue size */

/* Client information passed to thread handler */
typedef struct client_information
{
    int socket_id;
    char ip_address[INET_ADDRSTRLEN];

} client_nfo;


/*****************************************************************************/
/* Prototypes for Server functions */
/*****************************************************************************/

/* Thread handler function */
void *game_runner(void *);

/* Writes to log file */
void append_log(int, int, client_nfo *, int, int);

#endif
