/*   This file is prepared for Doxygen automatic documentation generation   */
/*! \file ********************************************************************
 *
 * \brief
 *      JTAG device implementation definitions
 *
 * \par References:
 *      Carlos Becker: <a href="http://carlosbecker.com.ar">
 *                      Interfacing a character LCD display</a>
 *
 * \date 2009-12-18
 *
 * \par Documentation
 *      For comprehensive code documentation, see readme.html
 *
 * \author
 *      Américo Dias <americo.dias@fe.up.pt>
 *
 ****************************************************************************/
#ifndef _CHARDEV_H_
#define _CHARDEV_H_
#include <linux/ioctl.h>

/*! \brief MAJOR number, static here, used with mknod in user space */
#define MAJOR_NUM   100

/*! \brief Device file name, to register in /dev/jtag */
#define DEVICE_FILE_NAME    "jtag"


//Prototypes for (un)register functions
int register_jtag_chrdev( void );
int unregister_jtag_chrdev( void );
#endif

