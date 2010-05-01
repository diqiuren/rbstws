/*   This file is prepared for Doxygen automatic documentation generation   */
/*! \file ********************************************************************
 *
 * \brief
 *      TCP/IP connection handler definitions
 *  
 * $Id$
 *
 ****************************************************************************/
#ifndef __CONNECTION_H
#define __CONNECTION_H

    
int connection_open(int port);
int connection_listen();
int connection_close();
int connection_write(char *buff, int num_bytes);
int connection_read(char *buff, int num_bytes);
int connection_get_client_name(void *str, int num_bytes);

#endif //__CONNECTION_H
