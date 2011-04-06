/*   This file is prepared for Doxygen automatic documentation generation   */
/*! \file ********************************************************************
 *
 * \brief
 *
 *
 * \date 2010-05-09
 *
 * \par Documentation
 *      For comprehensive code documentation, see readme.html
 *
 * \author
 *      Américo Dias <americo.dias@fe.up.pt>
 *
 ****************************************************************************/
#ifndef _TAP_USER_H_
#define _TAP_USER_H_

#define BUFFER_LEN  2048

typedef enum {
    SELTAP = 0,
    RESET,
    STATE,
    GET_STATE,
    SHIFT,
    RUNTEST,
    SET_IO,
    GET_IO
} tap_command_t;

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

typedef struct {
    tap_state_t state;
    char tms_buffer[BUFFER_LEN];
} state_command_data;


typedef struct {
    char tdi_buffer[BUFFER_LEN];
    char tdo_buffer[BUFFER_LEN];
} shift_command_data;

typedef struct {
    unsigned char pin;
    unsigned char value;
} io_command_data;

#endif //_TAP_USER_H_   
