/*   This file is prepared for Doxygen automatic documentation generation   */
/*! \file ********************************************************************
 *
 * \brief
 *      Command parser
 * 
 * $Id: parser.c 4 2010-01-10 22:10:13Z adias $
 *
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "connection.h"
#include "parser.h"

int send_ack(void) {
	return connection_write(CMD_ACK, strlen(CMD_ACK));
}

int send_nack(void) {
	return connection_write(CMD_NACK, strlen(CMD_NACK));
}

int command_parser(char *command) 
{
	FILE *fp;
	char filename[FILE_NAME_LEN+1];
	char buff[TEXT_BUF_LEN+1];
	int n;
	
    if(strncmp(command, CMD_EXIT, strlen(CMD_EXIT)) == 0) {
		if((n=send_ack()) < 0) return n;
		sleep(1);
        return 1;
    }
	else if(strncmp(command, CMD_PROG, strlen(CMD_PROG)) == 0) {		
		bzero(filename,FILE_NAME_LEN+1);
		strncpy(filename, P_tmpdir, strlen(P_tmpdir));
		strncpy(filename+strlen(P_tmpdir), FILE_NAME, strlen(FILE_NAME));

		fp = fopen(filename, "w");
		
		if(fp = NULL)
			return -1;
		
		if((n=send_ack()) < 0) return n;
			
		while(1) {
			n = connection_read(buff, TEXT_BUF_LEN);
			if(n < 0)
				return n;
			
			if(strncmp(buff, CMD_PROG_END, strlen(CMD_PROG_END)) == 0) {
				break;
			}
			
			n = fwrite(buff, sizeof(char), strlen(buff), fp);
			if(n < 0)
				return n;
				
			fputc('\n', fp);
			
			if((n=send_ack()) < 0) return n;
		}
		
		fclose(fp);
		if((n=send_ack()) < 0) return n;
	}
    else if(strncmp(command, CMD_PING, strlen(CMD_PING)) == 0) {
        n = connection_write(CMD_PONG, strlen(CMD_PONG));
        if (n < 0)
			return n;
    }
	else {
		if((n=send_nack()) < 0) return n;
	}

	return 0;
}