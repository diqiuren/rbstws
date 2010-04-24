/*   This file is prepared for Doxygen automatic documentation generation     */
/*! \file **********************************************************************
 *
 * \brief
 *      Timestamp functions definitions
 * 
 * \author
 *      Américo Dias <americo.dias@fe.up.pt>
 *
 * $Revision: 11 $
 * $HeadURL: https://rmws.googlecode.com/svn/trunk/src/timestamp.h $
 * $Date: 2010-04-24 18:16:16 +0100 (Sat, 24 Apr 2010) $
 * $Author: americo.dias $
 * $Id: timestamp.h 11 2010-04-24 17:16:16Z americo.dias $
 *
 ******************************************************************************/
#ifndef __TIMESTAMP_H
#define __TIMESTAMP_H

#define TIME_SIZE 40

double cpu_time(void);
void timestamp(FILE *fd);
char *timestring(void);

#endif //__TIMESTAMP_H
