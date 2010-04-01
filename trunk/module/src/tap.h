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

#define CLOCK_DELAY     100		// T = 100-uS FTCK = 10-kHz

typedef enum {
	TEST_LOGIC_RESET = 0x00,
	RUN_TEST_IDLE,

	SELECT_DR,
	CAPTURE_DR,
	SHIFT_DR,
	EXIT1_DR,
	PAUSE_DR,
	EXIT2_DR,
	UPDATE_DR,

	SELECT_IR,
	CAPTURE_IR,
	SHIFT_IR,
	EXIT1_IR,
	PAUSE_IR,
	EXIT2_IR,
	UPDATE_IR
} tap_state_t;

typedef enum {
	TDI = 0x00,
	TDO,
	TMS,
	TCK,
	TRST
} tap_pin_t;

typedef enum {
	OFF = 0x00,
	ON
} tap_pin_state_t;

void tap_initialize(void);
void tap_soft_reset(unsigned int tap);
void tap_reset(unsigned int tap);
char *tap_goto_state(unsigned int tap, tap_state_t next_state);
void tap_tdi(unsigned int tap, char *b, char *c);
/*void tap_set_end_state(unsigned int tap, tap_state_t state);*/
void tap_runtest(unsigned int tap, unsigned int count);
void tap_set_io(unsigned char pin, unsigned char value);
tap_pin_state_t tap_get_io(unsigned char pin);

#endif //_TAP_H_
