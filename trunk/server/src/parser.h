/*   This file is prepared for Doxygen automatic documentation generation   */
/*! \file ********************************************************************
 *
 * \brief
 *      Command parser definitions
 * 
 * $Id: parser.h 4 2010-01-10 22:10:13Z adias $
 *
 ****************************************************************************/
#ifndef __PARSER_H
#define __PARSER_H

#define CMD_EXIT 			"EXIT"
#define CMD_PING 			"PING"
#define CMD_PONG 			"PONG"
#define CMD_PROG 			"PROG"
#define CMD_PROG_END 		"ENDPROG"
#define CMD_ACK				"ACK"
#define CMD_NACK			"NACK"

#define TEXT_BUF_LEN	1024
#define FILE_NAME_LEN	512

#define FILE_NAME		"program.svf"
int command_parser(char *command);


#endif //__PARSER_H