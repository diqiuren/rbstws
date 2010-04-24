/*! \file **********************************************************************
 *
 * \brief
 *      Log file functions definitions
 * 
 * \author
 *      Américo Dias <americo.dias@fe.up.pt>
 *
 * $Revision: 11 $
 * $HeadURL: https://rmws.googlecode.com/svn/trunk/src/log.h $
 * $Date: 2010-04-24 18:16:16 +0100 (Sat, 24 Apr 2010) $
 * $Author: americo.dias $
 * $Id: log.h 11 2010-04-24 17:16:16Z americo.dias $
 *
 ******************************************************************************/
#ifndef __LOG_H
#define __LOG_H

void log_init(void);
void log_write(char *fmt, ... );

#endif
