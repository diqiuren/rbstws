/*   This file is prepared for Doxygen automatic documentation generation   */
/*! \file ********************************************************************
 *
 * \brief
 *      JTAG Server main
 *
 * $Id: server.c 4 2010-01-10 22:10:13Z adias $
 *
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "connection.h"
#include "parser.h"

#define BUFF_LENGHT		512
#define HOST_LENGHT		256
void error(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

/* The signal handler function */
void handler( int signal ) {
    psignal( signal, "\nSignal");
    connection_close();
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    int result, n;
    char buffer[BUFF_LENGHT+1];
    char client_name[HOST_LENGHT+1];
    
	
    /* Registering the handler, catching SIGINT signals */
    signal( SIGINT, handler );

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    

    result = connection_open(atoi(argv[1]));
    
    if(result < 0)
        error("ERROR opening connection");

    while(1) {
        result = connection_listen();
        if(result < 0)
            error("ERROR listening connection");
            
        result = connection_get_client_name(&client_name, HOST_LENGHT);
        if(result < 0)
            error("ERROR getting client name");        
        
        printf("New connection from '%s'.\n", client_name);

                 
        while(1) {
            n = connection_read(buffer,BUFF_LENGHT);
            if (n < 0)
                error("ERROR reading from socket");

			result = command_parser(buffer);
			
			if(result == 1)
				break;
			else if(result < 0)
				error("Error processing command");
        }
    }

    
    return 0; 
}
