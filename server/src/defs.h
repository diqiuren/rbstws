/*   This file is prepared for Doxygen automatic documentation generation     */
/*! \file **********************************************************************
 *
 * \brief
 *      General definitions
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
#ifndef __DEFS_H
#define __DEFS_H

#define VERSION             "0.9.0"

#ifndef _REV
#define _REV                0
#endif

#ifdef _DEBUG
#undef _DEBUG
#define _DEBUG              1        // Change to 0 to disable debug messages
#endif

#define DEFAULT_BAUDRATE    57600UL  // Core51 serial port baudrate
#define DEFAULT_PORT        5010     // Default connection port for command
                                     // connection
#define SERIAL_PORT_DELAY   250000UL // Serial port refresh period
#define BUFF_LENGHT		    512     
#define HOST_LENGHT		    256
#define MAX_CONNECTIONS     100      // Maximum number of connections
#define BACKLOG 10	                 // How many pending connections queue 
                                     // will hold
#define SOCKET_BUFFER_SIZE	2048

#define PROG_FILE_NAME      "/tmp/rbstws.svf"       // Program file
#define LOG_FILE_NAME       "/var/log/rbstws.log"   // Log file

struct args {
    unsigned long int baudrate;
	int port;
	int socket_fd;
	int slave;
};


#endif //__DEFS_H
