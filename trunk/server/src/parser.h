/*   This file is prepared for Doxygen automatic documentation generation   */
/*! \file ********************************************************************
 *
 * \brief
 *      Command parser definitions
 * 
 * \author
 *      Américo Dias <americo.dias@fe.up.pt>
 *
 * $Revision$
 * $HeadURL$
 * $Date$
 * $Author$
 * $Id$
 *
 ******************************************************************************/
#ifndef __PARSER_H
#define __PARSER_H

#define CMD_EXIT 			"EXIT"
#define CMD_PING 			"PING"
#define CMD_PONG 			"PONG"
#define CMD_PROGRAM			"PROG"
#define CMD_END_PROGRAM		"ENDP"
#define CMD_ACKNOWLEDGE		"ACKN"
#define CMD_NOT_ACKNOWLEDGE "NACK"
#define CMD_READY           "REDY"
#define CMD_BUSY            "BUSY"

int send_ack(int *socket);
int send_nack(int *socket);
int command_parser(char *string, int *socket);


#endif //__PARSER_H

