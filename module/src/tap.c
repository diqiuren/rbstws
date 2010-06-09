/*   This file is prepared for Doxygen automatic documentation generation   */
/*! \file ********************************************************************
 *
 * \brief
 *      JTAG protocol functions
 *
 * \par Documentation
 *      For comprehensive code documentation, see readme.html
 * 
 * \reference 
 *      http://bleyer.org/jjtag/
 *
 * \date 2009-12-18
 *  
 * \author
 *      Américo Dias <americo.dias@fe.up.pt>
 * 
 * The tap controller changes its internal states on TMS at rising edge off 
 * TCK.\n
 * All Inputs (TDO of DUT) are sampled after TCLK rising \n
 * All Outputs (TDI of DUT) are propagated after TCLK falling  
 * 
 *<pre>
 * +-------+
 * | Reset |<--------------------------------------------------+
 * +---+---+                                                   |
 *     | TMS=0                                                 |TMS=1
 *     Y                                                       |
 * +--------+ TMS=1      +-----------+ TMS=1    +-----------+  |
 * |RUN/IDLE|---->+----->|Sel DR-scan|--------->|Sel IR-scan|--+
 * +--------+     ^      +-----------+          +-----------+
 *     ^          |            | TMS=0                | TMS=0
 *     |          |            Y                      Y
 *     |          |      +-----------+ TMS=1    +-----------+ TMS=1
 *     |          |      |Capture DR |--+       |Capture IR |--+
 *     |          |      +-----------+  |       +-----------+  |
 *     |          |            | TMS=0  |             | TMS=0  |
 *     |          |            Y        |             Y        |
 *     |          |      +-----------+  |       +-----------+  |
 *     |          |   +->| Shift DR  |  |    +->| Shift IR  |  |
 *     |          |   |  +-----------+  |    |  +-----------+  |
 *     |          |   |        | TMS=1  |    |        | TMS=1  |
 *     |          |   |        Y        |    |        Y        |
 *     |          |   |  +-----------+ <+    |  +-----------+ <+
 *     |          |   |  | Exit 1 DR | TMS=1 |  | Exit 1 IR | TMS=1
 *     |          |   |  +-----------+--+    |  +-----------+--+
 *     |          |   |        | TMS=0  |    |        | TMS=0  |
 *     |          |   |        Y        |    |        Y        |
 *     |          |   |  +-----------+  |    |  +-----------+  |
 *     |          |   |  | Pause DR  |  |    |  | Pause IR  |  |
 *     |          |   |  +-----------+  |    |  +-----------+  |
 *     |          |   |        | TMS=1  |    |        | TMS=1  |
 *     |          |   |TMS=0   Y        |    |TMS=0   Y        |
 *     |          |   |  +-----------+  |    |  +-----------+  |
 *     |          |   +--| Exit 2 DR |  |    +--| Exit 2 IR |  |
 *     |          |      +-----------+  |       +-----------+  |
 *     |          |            | TMS=1  |             | TMS=1  |
 *     |          |            Y        |             Y        |
 *     |          |TMS=1 +-----------+ <+  TMS=1+-----------+ <+
 *     |          +<-----| Update DR |     +----| Update IR |
 *     |          |      +-----------+     |    +-----------+
 *     |          +------------|-----------+          |
 *     |                       | TMS=0                | TMS=0
 *     |                       Y                      |
 *     +-----------------------+<---------------------+
 *</pre>
 * @see IEEE Std 1149.1-1990 for all valid information of the TAP state machine
 ****************************************************************************/
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/string.h>
#include <asm/gpio.h>
#include <asm/arch/at32ap700x.h>
#include "tap.h"
#include "defs.h"

static const char *tap_transitions[][16] = {
	{"1", "0", "10", "010", "0010", "1010", "01010", "101010", "11010", "110", 
			"0110", "00110", "10110", "010110", "1010110", "110110"},
	{"111", "0", "1", "01", "001", "101", "0101", "10101", "1101", "11", "011", 
			"0011", "1011", "01011", "101011", "11011"},
	{"11", "011", "1110", "0", "00", "10", "010", "1010", "110", "1", "01", 
			"001", "101", "0101", "10101", "1101"},
	{"11111", "011", "111", "0111", "0", "1", "01", "101", "11", "1111", 
			"01111", "001111", "101111", "0101111", "10101111", "1101111"},
	{"11111", "011", "111", "0111", "0", "1", "01", "101", "11", "1111", 
			"01111", "001111", "101111", "0101111", "10101111", "1101111"},
	{"1111", "01", "11", "011", "010", "1010", "0", "10", "1", "111", "0111", 
			"00111", "10111", "010111", "1010111", "110111"},
	{"11111", "011", "111", "0111", "01", "101", "0", "1", "11", "1111", 
			"01111", "001111", "101111", "0101111", "10101111", "1101111"},
	{"1111", "01", "11", "011", "0", "10", "010", "1010", "1", "111", "0111", 
			"00111", "10111", "010111", "1010111", "110111"},
	{"111", "0", "1", "01", "001", "101", "0101", "10101", "1101", "11", "011", 
			"0011", "1011", "01011", "101011", "11011"},
	{"1", "01", "101", "0101", "00101", "10101", "010101", "1010101", "110101", 
			"1101", "0", "00", "10", "010", "1010", "110"},
	{"11111", "011", "111", "0111", "00111", "10111", "010111", "1010111", 
			"110111", "1111", "01111", "0", "1", "01", "101", "11"},
	{"11111", "011", "111", "0111", "00111", "10111", "010111", "1010111", 
			"110111", "1111", "01111", "0", "1", "01", "101", "11"},
	{"1111", "01", "11", "011", "0011", "1011", "01011", "101011", "11011", 
			"111", "0111", "010", "1010", "0", "10", "1"},
	{"11111", "011", "111", "0111", "00111", "10111", "010111", "1010111", 
			"110111", "1111", "01111", "01", "101", "0", "1", "11"},
	{"1111", "01", "11", "011", "0011", "1011", "01011", "101011", "11011", 
			"111", "0111", "0", "10", "010", "1010", "1"},
	{"111", "0", "1", "01", "001", "101", "0101", "10101", "1101", "11", "011", 
			"0011", "1011", "01011", "101011", "11011"}
};

static const tap_state_t tap_next_state[][2] = {
	{RUN_TEST_IDLE, TEST_LOGIC_RESET},
	{RUN_TEST_IDLE, SELECT_DR},
	{CAPTURE_DR, SELECT_IR},
	{SHIFT_DR, EXIT1_DR},
	{SHIFT_DR, EXIT1_DR},
	{PAUSE_DR, UPDATE_DR},
	{PAUSE_DR, EXIT2_DR},
	{SHIFT_DR, UPDATE_DR},
	{RUN_TEST_IDLE, SELECT_DR},
	{CAPTURE_IR, TEST_LOGIC_RESET},
	{SHIFT_IR, EXIT1_IR},
	{SHIFT_IR, EXIT1_IR},
	{PAUSE_IR, UPDATE_IR},
	{PAUSE_IR, EXIT2_IR},
	{SHIFT_IR, UPDATE_IR},
	{RUN_TEST_IDLE, SELECT_DR}
};

static const int tap_pin[][2] = {
	{GPIO_PIN_PA(0), GPIO_PIN_PA(10)},		// TDI
	{GPIO_PIN_PA(1), GPIO_PIN_PA(11)},		// TCK
	{GPIO_PIN_PA(2), GPIO_PIN_PA(12)},		// TMS
	{GPIO_PIN_PA(3), GPIO_PIN_PA(13)},		// TRST
	{GPIO_PIN_PA(4), GPIO_PIN_PA(14)}		// TDO
};

static const int tap_input[] = {
	GPIO_PIN_PA(5),							// IO0 (INPUT 0)
	GPIO_PIN_PA(15),						// IO1 (INPUT 1)
};

static const int tap_output[] = {
	GPIO_PIN_PA(16),						// IO2 (OUTPUT 0)
	GPIO_PIN_PA(19)							// IO3 (OUTPUT 1)
};

tap_state_t 	tap_state[2];
const char      reset_str[] = "11111";

/*******************************************************************************
* Local routines
*******************************************************************************/
void tap_tck(unsigned int tap, unsigned long int n) {
    return;
}

void tap_set_pin(unsigned int tap, tap_pin_t pin, tap_pin_state_t s) {
	gpio_set_value(tap_pin[pin][tap], s);
	return;
}

tap_pin_state_t tap_get_pin(unsigned int tap, tap_pin_t pin) {
	if(gpio_get_value(tap_pin[pin][tap]) == OFF)
		return OFF;
	else
		return ON;
}

/** Delay to wait while bit-banging */
void tap_hold(void) {
	udelay(CLOCK_DELAY/2);
	return;
}

/** Control TMS signal and TAP state using bits from string (right to left) */
void tap_tms(unsigned int tap, char *b) {
	int i,v;
	int bl = strlen(b);
	
	for (i=0; i<bl; ++i) {
		v = (b[bl-1-i] == '0') ? 0 : 1;
		tap_set_pin(tap, TMS, (v == 0) ? OFF : ON);
		tap_set_pin(tap, TCK, OFF);
		tap_hold();
		tap_set_pin(tap, TCK, ON);
		tap_hold();
		tap_state[tap] = tap_next_state[tap_state[tap]][v]; // update state
	}
	tap_set_pin(tap, TCK, OFF);
	
	return;
}



/*******************************************************************************
* Global routines
*******************************************************************************/

void tap_initialize(void) 
{
    static int initialized = 0;
    
	int x,y;
	
	if(initialized == 0) {
	    for(x=0; x<4; x++) {
		    for(y=0; y<2; y++) {
			    gpio_direction_output(tap_pin[x][y], ON);	// TAP outputs
		    }
	    }
	
	    for(x=0; x<2; x++) {
		    gpio_direction_input(tap_pin[4][x]);			// TDO
		    gpio_direction_input(tap_input[x]);
		    gpio_direction_output(tap_output[x],0);
		    tap_state[x] = TEST_LOGIC_RESET;
		    tap_set_pin(0, TRST, ON);
		    tap_reset(x);
	    }
	    
	    initialized = 1;
#ifdef _DEBUG
        printk(KERN_DEBUG "JTAG module: tap initialized!\n");
#endif  
	}
}

void tap_soft_reset(unsigned int tap) {
	tap_tms(tap, "11111");
}

void tap_reset(unsigned int tap) 
{
	tap_set_pin(tap, TRST, OFF);
	tap_hold();
	tap_set_pin(tap, TRST, ON);
	tap_state[tap] = TEST_LOGIC_RESET;
}

char *tap_goto_state(unsigned int tap, tap_state_t next_state)
{
    char *tms;
#ifdef _DEBUG
    printk(KERN_DEBUG "JTAG module: tap_goto_state(%d,%d)\n", tap, (int)next_state);
#endif
	if(next_state == TEST_LOGIC_RESET) {
		tap_soft_reset(tap);
		return (char *)reset_str;
	}
    
    tms = (char *)tap_transitions[tap_state[tap]][next_state];
    
	tap_tms(tap, tms);

	return tms;
}

/** Control TDI signal and TAP state using bits from string (right to left) */
void tap_tdi(unsigned int tap, char *b, char *c) {
	int i;
	int l = strlen(b);

	c[l] = '\0';	// String termination
	
	// assume TMS is 0 and we are in a shift state
	for (i=0; i<l; ++i) {
		tap_set_pin(tap, TDI, b[l-1-i] == '0' ? OFF : ON);
		tap_set_pin(tap, TCK, OFF); // assume TCK was high shift out TDO in this state
		if(i==l-1) {
		     tap_set_pin(tap, TMS, ON);
		} 
		tap_hold();
		tap_set_pin(tap, TCK, ON); // shift in TDI, TMS
		tap_hold();
		c[l-1-i] = (tap_get_pin(tap, TDO) == ON ) ? '1' : '0'; // sample TDO
	}
	
    tap_state[tap] = tap_next_state[tap_state[tap]][1];
	//tap_set_pin(tap, TDI, ON); 
	
	return;
}

void tap_runtest(unsigned int tap, unsigned long count)
{
	unsigned long i;
	
	for (i=0; i<count; i++) {
		tap_set_pin(tap, TCK, ON); // shift in TDI, TMS
		tap_hold();
		tap_set_pin(tap, TCK, OFF); // shift in TDI, TMS
		tap_hold();
	}
}

tap_state_t tap_get_state(unsigned int tap)
{
    return tap_state[tap];
}

void tap_set_io(unsigned char pin, unsigned char value) {
	if(pin>1)
		return;
	gpio_set_value(tap_output[pin], (value == 0) ? OFF : ON);
	
	return;
}

tap_pin_state_t tap_get_io(unsigned char pin) {
	if(pin>1)
		return OFF;
		
	return gpio_get_value(tap_output[pin]);
}


