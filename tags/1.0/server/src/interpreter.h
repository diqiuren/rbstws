#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#define MAX_INSTRUCTION_BUFFER      65535

typedef enum {
	UP_STATE = 0x01,
    UP_SDR,
    UP_SIR,
    UP_ENDDR,
    UP_ENDIR,
    UP_RUNTEST,
    UP_SELTAP,
    UP_JMP,
    UP_JMPE,
    UP_RSTE,
    UP_JMPI,
    UP_SETI,
    UP_RSTI,
    
    UP_NOP = 0xFE,
    UP_END = 0xFF
} upcode_t;

typedef struct {
    unsigned int line;
    unsigned int breakpoint;
    unsigned int upcode;
    char *arg1;
    char *arg2;
    char *arg3;
    char *arg4;
    struct instruction_t *next_instruction;
} instruction_t;

typedef enum {
	OFF = 0x01,
	ON = 0x00
} tap_pin_state_t;

int code_parser();
void *interpreter_run(void *arg);
int interpreter_step(void);
void interpreter_reset(void);
int interpreter_init(char *fname);
void interpreter_close(void);
void interpreter_start_run(void);
void interpreter_abort_run(void);
void interpreter_add_breakpoint(int line);
void interpreter_clear_breakpoints(void);

#endif //_INTERPRETER_H_

