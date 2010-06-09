/*   This file is prepared for Doxygen automatic documentation generation     */
/*! \file **********************************************************************
 *
 * \brief
 *      JTAG definitions
 *
 * \par Documentation
 *      For comprehensive code documentation, see readme.html
 *
 * \date 2009-12-18
 *
 * \author
 *      Américo Dias <americo.dias@fe.up.pt>
 *
 ******************************************************************************/
#ifndef _TAP_H_
#define _TAP_H_

#include "defs.h"
#include "tap_user.h"

#define CLOCK_DELAY     400		// T = 400-uS FTCK = 2.5-kHz

typedef enum {
	TDI = 0x00,
	TCK,
	TMS, 
	TRST, 
	TDO 
} tap_pin_t;

typedef enum {
	OFF = 0x00,
	ON = 0x01
} tap_pin_state_t;

void tap_initialize(void);
void tap_soft_reset(unsigned int tap);
void tap_reset(unsigned int tap);
char *tap_goto_state(unsigned int tap, tap_state_t next_state);
void tap_tdi(unsigned int tap, char *b, char *c);
void tap_runtest(unsigned int tap, unsigned long count);
tap_state_t tap_get_state(unsigned int tap);
void tap_set_io(unsigned char pin, unsigned char value);
tap_pin_state_t tap_get_io(unsigned char pin);

#endif //_TAP_H_
