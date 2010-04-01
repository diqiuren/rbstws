/*   This file is prepared for Doxygen automatic documentation generation   */
/*! \file ********************************************************************
 *
 * \brief
 *      Linux kernel module for JTAG protocol.
 *
 * \par References:
 *      AVR32408: Using GPIO with Linux on the AVR32\n
 *      Carlos Becker: <a href="http://carlosbecker.com.ar">
 *                      Interfacing a character LCD display</a>
 *
 * \par Documentation:
 *      For comprehensive code documentation, see readme.html
 *
 * \date 2009-12-18
 *
 * \author
 *      Américo Dias <americo.dias@fe.up.pt>
 *
 ****************************************************************************/

#include <linux/module.h>
#include "chardev.h"
#include "version.h"

/*! \brief Module init function.
 *  This function is called when the module is loaded into the kernel.
 */
int init_module(void)
{
    int ret_val;

    //register char device
    ret_val = register_jtag_chrdev();
    if ( ret_val < 0 )
    {
        printk( KERN_ALERT "JTAG module: couldn't register device\n" );
        return ret_val;
    }
  
    printk(KERN_DEBUG "JTAG module: module loaded (version %s)\n", DISP(VERSION));
  
    return 0;
}

/*! \brief Module exit function.
 *  This function is called when the module is unloaded from the kernel
 */
void cleanup_module(void)
{
    printk(KERN_DEBUG "JTAG module: closing device\n");
    // Unregister character device from kernel
    unregister_jtag_chrdev();
}

MODULE_DESCRIPTION("JTAG protocol driver");
MODULE_AUTHOR("Americo Dias <americo.dias@fe.up.pt>");
MODULE_LICENSE("GPL");

