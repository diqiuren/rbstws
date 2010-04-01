/*   This file is prepared for Doxygen automatic documentation generation   */
/*! \file ********************************************************************
 *
 * \brief
 *      JTAG device implementation
 *
 * \par References:
 *      Carlos Becker: <a href="http://carlosbecker.com.ar">
 *                      Interfacing a character LCD display</a>
 *
 * \par Documentation
 *      For comprehensive code documentation, see readme.html
 *
 * \date 2010-01-05
 *
 * \author
 *      Américo Dias <americo.dias@fe.up.pt>
 *
 ****************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include "chardev.h"
#include "tap.h"

#define SUCCESS     0

unsigned int tap;
static char *tdo_buffer[2];
static char *tms_buffer[2];

/*! \brief Holds whether the device was already opened */
static int Device_Open = 0;

/*! \brief Device open
 *  Called when user calls open
 */
static int device_open( struct inode *inode, struct file *file )
{
#ifdef _DEBUG
    printk(KERN_DEBUG "JTAG module: device_open(%p,%p)\n", inode, file);
#endif
    
    if ( Device_Open )
        return -EBUSY;      // Error: device already openned
    Device_Open++;          // Mark device as openned

    try_module_get( THIS_MODULE );
    
    tap_initialize();
    tap = 0;
    
    return SUCCESS;         // OK!
}

/*! \brief Device close
 *  Called when user calls close
 */
static int device_release( struct inode *inode, struct file *file )
{
#ifdef __user
    printk(KERN_DEBUG "JTAG module: device_release(%p,%p)\n", inode, file);
#endif

     Device_Open--;         // Flag that

     module_put( THIS_MODULE );
     return SUCCESS;        // OK!
}

//called when user calls ioctl()
/*! \brief Device IOCTL
 *  Called when user calls ioctl(). This function implements all
 *  the JTAG protocol.
 */
int device_ioctl( struct inode *inode,
                  struct file *file,
                  unsigned int ioctl_num,
                  unsigned long arg)
{
	unsigned char pin;
	unsigned char value;
	char *pointer;
	
#ifdef _DEBUG
    printk(KERN_DEBUG "JTAG module: device_ioctl(%p,%d,%p)\n", file, ioctl_num, 
			(void *)arg);
#endif
    switch(ioctl_num) {
        // SELTAP command ******************************************************
        case 0:
            if((unsigned int)&arg > 1)
                return -1;
            tap = (unsigned int)&arg;
            break;
		// RESET command *******************************************************
		case 1:
			tap_reset((unsigned int)&arg);
			break;
        // STATE command *******************************************************
        case 2:
            if((tap_state_t)&arg > 15)
                return -1;
            pointer = tap_goto_state(tap,(tap_state_t)&arg);
			strcpy(tms_buffer[tap], pointer);
            break;
		// SHIFT command *******************************************************
		case 3:
			tap_tdi(tap, (char *)&arg, tdo_buffer[tap]);
			break;
		// RUNTEST command *****************************************************
		case 4:
			tap_runtest(tap, (unsigned int)&arg);
			break;
		// SET_TDO_BUFFER command **********************************************
		case 5:
			tdo_buffer[tap] = (char *)&arg;
			break;
		// SET_TMS_BUFFER command **********************************************
		case 6:
			tms_buffer[tap] = (char *)&arg;
			break;
		// SET_IO command ******************************************************
		case 7:
			pin = (unsigned char)((unsigned int)&arg / 0x100);
			value = (unsigned char)((unsigned int)&arg & 0xFF);
			tap_set_io(pin, value);
			break;
		// GET_IO command ******************************************************
		case 8:
			return tap_get_io((unsigned char)(unsigned int)&arg);
			break;
        default:
            break;
            
    }
    return SUCCESS;
}

//neccesary to register as character device
struct file_operations Fops = {
     .read   = NULL,
     .write = NULL,
     .ioctl = device_ioctl,
     .open   = device_open,
     .release= device_release,
};

//exported function, used by module.c
int register_jtag_chrdev(void)
{
    int ret;
      
    ret = register_chrdev( MAJOR_NUM, DEVICE_FILE_NAME , &Fops );
    if ( ret < 0 )
        goto err;

	tap_initialize();
#ifdef _DEBUG
    printk(KERN_DEBUG "JTAG module: device registered\n");
#endif  
     return ret;

err:
    printk(KERN_WARNING "JTAG module: error registering device\n");
    return ret;
}
//exported function, used by module.c

int unregister_jtag_chrdev(void)
{
    unregister_chrdev( MAJOR_NUM, DEVICE_FILE_NAME );
#ifdef _DEBUG
    printk(KERN_DEBUG "JTAG module: device unregistered.\n");
#endif
    return 0;
}

