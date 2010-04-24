/*   This file is prepared for Doxygen automatic documentation generation     */
/*! \file **********************************************************************
 *
 * \brief
 *      Command parser
 * 
 * \author
 *      Américo Dias <americo.dias@fe.up.pt>
 *
 * $Revision: 11 $
 * $HeadURL: https://rmws.googlecode.com/svn/trunk/src/parser.c $
 * $Date: 2010-04-24 18:16:16 +0100 (Sat, 24 Apr 2010) $
 * $Author: americo.dias $
 * $Id: parser.c 11 2010-04-24 17:16:16Z americo.dias $
 *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "parser.h"
#include "defs.h"

int send_ack(int *socket) 
    return write(*socket, CMD_ACKNOWLEDGE, strlen(CMD_ACKNOWLEDGE));
}

int send_nack(int *socket) {
	return write(*socket, CMD_NOT_ACKNOWLEDGE, strlen(CMD_NOT_ACKNOWLEDGE));
}

int command_test(char *string, char *command) {
    if(strlen(string) < strlen(command))
        return 0;

    return (strncmp(string, command, strlen(command)) == 0) ? 1 : 0;
}

int command_parser(char *string, int *socket) 
{
    static int program_status = 0;
    
	FILE *fp;
	int n;
	
	/*
	 * Command exit:
	 * Close the connection!
	 */
    if(command_test(string, CMD_EXIT)) {
		if((n=send_ack(socket)) < 0) return n;
		sleep(1);
        return 1;
    }
    /*
     * Command program:
     * Receive the hexadecimal file from the TCP/IP connection and sent it
     * to the FILE_NAME in the system temporary directory.
     * If the file already exist, it is rewritten.
     */
	else if(command_test(string, CMD_PROGRAM)) {
		fp = fopen(HEX_FILE_NAME, "w");
		
		if(fp == NULL)
			return -1;
		
		fclose(fp);
		
		program_status = 1;
		
		if((n=send_ack(socket)) < 0) return n;
	}
    /*
     * Command end program:
     * Receive the hexadecimal file from the TCP/IP connection and sent it
     * to the FILE_NAME in the system temporary directory.
     * If the file already exist, it is rewritten.
     */
	else if(command_test(string, CMD_END_PROGRAM)) {				
		program_status = 0;
		
		if((n=send_ack(socket)) < 0) return n;
	}
	/*
	 * Command ping:
	 * Just for testing... answer with a "PONG".
	 */
    else if(command_test(string, CMD_PING)) {
        n = write(*socket, CMD_PONG, strlen(CMD_PONG));
        if (n < 0)
			return n;
    }
    /*
     * Else:
     * Command or data not understood. Send a NOT ACKOWLEDGE.
     */ 
	else {
		if((n=send_nack(socket)) < 0) return n;
	}

	return 0;
}

