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
#include "tap_user.h"

#define SUCCESS     0

unsigned int tap;

void strcpy_inv(char *dest, char *source)
{
	int x;
	int len = strlen(source);
	
	dest += len;	
    *dest-- = '\0';
    
	for(x=len; x>0; x--){
		*dest-- = *source++;
	}

}

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
    void *pointer;
    state_command_data state_cmd_dt;
    shift_command_data shift_cmd_dt;
    io_command_data io_cmd_dt;
    unsigned long n_clocks;
    
#ifdef _DEBUG
    printk(KERN_DEBUG "JTAG module: device_ioctl(%p,%d,%p)\n", file, ioctl_num, (void *)arg);
#endif
    switch(ioctl_num) {
        // SELTAP command ******************************************************
        case SELTAP:
            if(copy_from_user(&tap, (unsigned int *)arg, sizeof(unsigned int))!=0)
                return -EFAULT;
#ifdef _DEBUG
            printk(KERN_DEBUG "JTAG module: Tap %d selected.\n", tap);
#endif
            break;
		// RESET command *******************************************************
		case RESET:
			tap_reset(tap);
#ifdef _DEBUG
            printk(KERN_DEBUG "JTAG module: Tap %d reseted.\n", tap);
#endif
			break;
        // STATE command *******************************************************
        case STATE:
            if(copy_from_user(&state_cmd_dt, (state_command_data *)arg, sizeof(state_command_data))!=0)
                return -EFAULT;
                
            pointer = tap_goto_state(tap,state_cmd_dt.state);
			strcpy_inv(state_cmd_dt.tms_buffer, pointer);
					 
            if(copy_to_user((state_command_data *)arg, &state_cmd_dt, sizeof(state_command_data))!=0)
                return -EFAULT;
                
#ifdef _DEBUG
            printk(KERN_DEBUG "JTAG module: Tap %d in state %d. TMS transitions: %s.\n", tap, state_cmd_dt.state, state_cmd_dt.tms_buffer);
#endif
            break;
        // GET_STATE command ***************************************************
        case GET_STATE:                            
#ifdef _DEBUG
            printk(KERN_DEBUG "JTAG module: Tap %d is on state %d.\n", tap, tap_get_state(tap));
#endif
            return tap_get_state(tap);
            break; 
		// SHIFT command *******************************************************
		case SHIFT:
            if(copy_from_user(&shift_cmd_dt, (shift_command_data *)arg, sizeof(shift_command_data))!=0)
                return -EFAULT;
                 
            tap_tdi(tap, shift_cmd_dt.tdi_buffer, shift_cmd_dt.tdo_buffer);
            
            if(copy_to_user((shift_command_data *)arg, &shift_cmd_dt, sizeof(shift_command_data))!=0)
                return -EFAULT;
                
#ifdef _DEBUG
            printk(KERN_DEBUG "JTAG module: Tap %d shift. Input %s. Output %s.\n", tap, shift_cmd_dt.tdi_buffer, shift_cmd_dt.tdo_buffer);
#endif
			break;
		// RUNTEST command *****************************************************
		case RUNTEST:
		    if(copy_from_user(&n_clocks, (unsigned long *)arg, sizeof(unsigned long))!=0)
                return -EFAULT; 
                
			tap_runtest(tap, n_clocks);
			
#ifdef _DEBUG
            printk(KERN_DEBUG "JTAG module: Tap %d runtest for %d clocks.\n", tap, (unsigned int)n_clocks);
#endif
			break;
		// SET_IO command ******************************************************
		case SET_IO:
            if(copy_from_user(&io_cmd_dt, (io_command_data *)arg, sizeof(io_command_data))!=0)
                return -EFAULT; 
                
            tap_set_io(io_cmd_dt.pin, io_cmd_dt.value);    

#ifdef _DEBUG
            printk(KERN_DEBUG "JTAG module: IO %d set to %d.\n", io_cmd_dt.pin, io_cmd_dt.value);
#endif
			break;
		// GET_IO command ******************************************************
		case GET_IO:
            if(copy_from_user(&io_cmd_dt, (io_command_data *)arg, sizeof(io_command_data))!=0)
                return -EFAULT;                     
                
            io_cmd_dt.value = tap_get_io(io_cmd_dt.pin);   
            
            if(copy_to_user((io_command_data *)arg, &io_cmd_dt, sizeof(io_command_data))!=0)
                return -EFAULT;
#ifdef _DEBUG
            printk(KERN_DEBUG "JTAG module: IO %d set to %d.\n", io_cmd_dt.pin, io_cmd_dt.value);
#endif              
			break;
        default:
#ifdef _DEBUG
            printk(KERN_DEBUG "JTAG module: Command not recognized!\n");
#endif
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

