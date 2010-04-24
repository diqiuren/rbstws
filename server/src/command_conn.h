/*   This file is prepared for Doxygen automatic documentation generation     */
/*! \file **********************************************************************
 *
 * \brief
 *      Command connection functions definitions
 * 
 * \author
 *      Américo Dias <americo.dias@fe.up.pt>
 *
 * $Revision: 11 $
 * $HeadURL: https://rmws.googlecode.com/svn/trunk/src/command_conn.h $
 * $Date: 2010-04-24 18:16:16 +0100 (Sat, 24 Apr 2010) $
 * $Author: americo.dias $
 * $Id: command_conn.h 11 2010-04-24 17:16:16Z americo.dias $
 *
 ******************************************************************************/
#ifndef __COMMAND_CONN_H
#define __COMMAND_CONN_H

void command_conn_init(void);
void *command_conn(void *arg);
void *command_conn_busy(void *arg);
void *command_conn_accept(void *arg);
void command_conn_set_status(int status);
int command_conn_get_status(void);
#endif
