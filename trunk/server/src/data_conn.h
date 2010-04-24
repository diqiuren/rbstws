/*   This file is prepared for Doxygen automatic documentation generation     */
/*! \file **********************************************************************
 *
 * \brief
 *      Data connection functions definitions
 * 
 * \author
 *      Américo Dias <americo.dias@fe.up.pt>
 *
 * $Revision: 11 $
 * $HeadURL: https://rmws.googlecode.com/svn/trunk/src/data_conn.h $
 * $Date: 2010-04-24 18:16:16 +0100 (Sat, 24 Apr 2010) $
 * $Author: americo.dias $
 * $Id: data_conn.h 11 2010-04-24 17:16:16Z americo.dias $
 *
 ******************************************************************************/
#ifndef __DATA_CONN_H
#define __DATA_CONN_H

void data_conn_init(void);
void *data_conn(void *arg);
void *data_conn_busy(void *arg);
void *data_conn_accept(void *arg);
void data_conn_set_status(int status);
int data_conn_get_status(void);

#endif //__DATA_CONN_H

