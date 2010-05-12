#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include "jtag.h"
#include "interpreter.h"
#include "data_conn.h"

unsigned int tap;
int jtag;
char filename[128];
static unsigned int error_flag;


instruction_t *instructions = NULL;
instruction_t *current_instruction = NULL;

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

char nibble_converter(char bit3, char bit2, char bit1, char bit0)
{
    unsigned char value = 0;
    char result;
    
    if(bit3 == '1' || bit3 == 'x')
        value |= 0x08;
    if(bit2 == '1' || bit2 == 'x')
        value |= 0x04;
    if(bit1 == '1' || bit1 == 'x')
        value |= 0x02;
    if(bit0 == '1' || bit0 == 'x')
        value |= 0x01;

    sprintf (&result, "%X", value);

    return result;
}

char *iconverter(char *tdi, char *tdo, char *tms, tap_state_t initial_state, char *output_buffer)
{
    tap_state_t state;
    unsigned int counter;
    int len = strlen(tdi);

    state = initial_state;
    output_buffer = calloc(2, sizeof(char));
    
    for(counter = 0; counter < len ; counter++) {
        output_buffer = realloc(output_buffer,counter*2+1);
        
        output_buffer[2*counter] = nibble_converter(tms[counter], tdi[counter], tdo[counter], '0');
        
        if(tms[counter] == '0')
            state = tap_next_state[state][0];
        else
            state = tap_next_state[state][1];
        
        sprintf (&output_buffer[2*counter+1], "%X", state);
    }

    return output_buffer;
}

void *clear_instructions(instruction_t *instructions)
{
    instruction_t *n_instruction;
    instruction_t *c_instruction;
    
    c_instruction = instructions;
    
    while(current_instruction != NULL) {
        n_instruction = (instruction_t*) c_instruction->next_instruction;
        free(c_instruction->arg1);
        free(c_instruction->arg1);
        free(c_instruction->arg1);
        free(c_instruction->arg1);
        free(c_instruction);
        c_instruction = n_instruction;
    }
    
    return NULL;
}

instruction_t* next_instruction(instruction_t *instructions) 
{
    return (instruction_t*) instructions->next_instruction;
}

instruction_t* locate_instruction(instruction_t *instructions, unsigned int line_number) 
{
    instruction_t *current_instruction = instructions;
    
    while(current_instruction != NULL) {
        if(current_instruction->line == line_number)
            return (instruction_t*) current_instruction;
            
        current_instruction = (instruction_t*) current_instruction->next_instruction;
    }
    
    return NULL;
}

instruction_t *code_parser(char *filename) {
    FILE *fd = NULL;
    char arg1[1024];
    char arg2[1024];
    char arg3[1024];
    char arg4[1024];
    char line[2048];
    unsigned int line_number;
    instruction_t *instruction = NULL;
	instruction_t *last_instruction = NULL;
	instruction_t *first_instruction = NULL;
	

    fd = fopen(filename, "r");

    if(fd == NULL) {
        printf("Could not open file.\n"); fflush(stdout);
        return NULL;
    }
    
    line_number = 0;

    while(fgets(line, 2048, fd) != NULL)
    {
        if(strlen(line) > 1) {
            instruction = (instruction_t *) malloc(sizeof(instruction_t));
            
            if(first_instruction == NULL) {
                first_instruction = (instruction_t*) instruction;
            }
            
            instruction->line = line_number;
            instruction->breakpoint = 0;
            sscanf (line,"%u %s %s %s %s", &instruction->upcode, arg1, arg2, arg3, arg4);
            instruction->arg1 = calloc(strlen(arg1), sizeof(char));
            strcpy(instruction->arg1, arg1);
            instruction->arg2 = calloc(strlen(arg2), sizeof(char));
            strcpy(instruction->arg2, arg2);
            instruction->arg3 = calloc(strlen(arg3), sizeof(char));
            strcpy(instruction->arg3, arg3);
            instruction->arg4 = calloc(strlen(arg4), sizeof(char));
            strcpy(instruction->arg4, arg4);
            instruction->next_instruction = NULL;
            
            if(last_instruction != NULL)
                last_instruction->next_instruction = (struct instruction_t*) instruction;

            last_instruction = instruction;
            
            line_number++;
        }
    }

    fclose(fd);
    
    return first_instruction;
}

instruction_t *process_instruction(int *fd, instruction_t *instruction)
{
    int res = 0;
    instruction_t *next_instruction = NULL;
    unsigned int next_instruction_line;
    tap_state_t initial_state;
    char tdi_buffer[1024];
    char tdo_buffer[1024];
    char tms_buffer[1024];
    char *signal = NULL;
    char *buffer = NULL; 
    char result = 'X';
    
    switch(instruction->upcode) {
        case UP_STATE:
            res = state(fd, atoi(instruction->arg1), tdi_buffer, tdo_buffer, tms_buffer, &initial_state);
            signal = iconverter(tdi_buffer, tdo_buffer, tms_buffer, initial_state, signal);
            next_instruction = (instruction_t*) instruction->next_instruction;
            break;
        case UP_SDR:
            res = sdr(fd, atoi(instruction->arg1), instruction->arg2, instruction->arg3, instruction->arg4, tdi_buffer, tdo_buffer, tms_buffer, &initial_state);
            if(res == -2)
                error_flag = 1;
                
            signal = iconverter(tdi_buffer, tdo_buffer, tms_buffer, initial_state, signal);
            next_instruction = (instruction_t*) instruction->next_instruction;
            break;
        case UP_SIR:
            res = sir(fd, atoi(instruction->arg1), instruction->arg2, instruction->arg3, instruction->arg4, tdi_buffer, tdo_buffer, tms_buffer, &initial_state);
            if(res == -2)
                error_flag = 1;
            signal = iconverter(tdi_buffer, tdo_buffer, tms_buffer, initial_state, signal);
            next_instruction = (instruction_t*) instruction->next_instruction;
            break;
        case UP_ENDDR:
            break;
        case UP_ENDIR:
            break;
        case UP_RUNTEST:
            res = runtest(fd, atoi(instruction->arg1), tdi_buffer, tdo_buffer, tms_buffer, &initial_state);
            signal = iconverter(tdi_buffer, tdo_buffer, tms_buffer, initial_state, signal);
            next_instruction = (instruction_t*) instruction->next_instruction;
            break;
        case UP_SELTAP:
            res = seltap(fd, atoi(instruction->arg1));
            next_instruction = (instruction_t*) instruction->next_instruction;
            tap = atoi(instruction->arg1);
            break;
        case UP_JMP:
            next_instruction = locate_instruction(instructions, atoi(instruction->arg1));
            break;
        case UP_JMPE:
            if(error_flag == 1)
                next_instruction = locate_instruction(instructions, atoi(instruction->arg1));
            else
                next_instruction = (instruction_t*) instruction->next_instruction;
            break;
        case UP_RSTE:
            error_flag = 0;
            next_instruction = (instruction_t*) instruction->next_instruction;
            break;
        case UP_JMPI:
            if(get_input(fd, atoi(instruction->arg1)) == ON)
                next_instruction = locate_instruction(instructions, atoi(instruction->arg2));
            else
                next_instruction = (instruction_t*) instruction->next_instruction;
            break;
        case UP_SETI:
            set_output(fd, atoi(instruction->arg1), 1);
            next_instruction = (instruction_t*) instruction->next_instruction;
            break;
        case UP_RSTI:
            set_output(fd, atoi(instruction->arg1), 0);
            next_instruction = (instruction_t*) instruction->next_instruction;
            break;
        default:
            next_instruction = (instruction_t*) instruction->next_instruction;
            break;
    }
            
    if(signal == NULL) {
        signal = calloc(2, sizeof(char));
        signal[0] = 'X';
        signal[1] = '\0';
    }
    
    if(next_instruction == NULL)
        next_instruction_line = 0;
    else
        next_instruction_line = next_instruction->line;
    
    switch(res) {
        case 0:
            result = 'O';   // OK
            break;
        case -1:
            result = 'F';   // FATAL ERROR
            break;
        case -2:
            result = 'E';   // TDO ERROR
            break;
        default:
            result = 'X';
            break;
    }
    
    buffer = calloc(1024, sizeof(char));
    printf("L:%d N:%d T:%d S:%s R:%c\n", instruction->line, next_instruction_line, tap, signal, result); fflush(stdout);
    sprintf(buffer, "L:%d N:%d T:%d S:%s R:%c\n", instruction->line, next_instruction_line, tap, signal, result);
    data_con_write(buffer);
    free(buffer);
    
    
    if(signal != NULL)
        free(signal);
    
    return next_instruction;

}

int interpreter_run(void) {   
    if(instructions == NULL || current_instruction == NULL ) {
        instructions = code_parser(filename);
        current_instruction = instructions;
        error_flag = 0;
        if(instructions == NULL)
            return -1;
    }
    
    while(current_instruction != NULL) {
        current_instruction = process_instruction(&jtag, current_instruction);
    }
    
    interpreter_reset();
    
    return 0;
}

int interpreter_step(void) {
    if(instructions == NULL || current_instruction == NULL ) {
        instructions = code_parser(filename);
        current_instruction = instructions;
        error_flag = 0;
        if(instructions == NULL)
            return -1;
    }
    current_instruction = process_instruction(&jtag, current_instruction);

    if(current_instruction == NULL)
        interpreter_reset();

    return 0;

}

void interpreter_reset(void) {
    instructions = clear_instructions(instructions);
    current_instruction = NULL;
}

int interpreter_init(char *fname) {

    strcpy(filename, fname);
    error_flag = 0;
    
    jtag = open("/dev/jtag", O_RDWR );
    if ( jtag < 0 )
    {
        printf("Couldn't open jtag! Make sure you've loaded the module.\n");
        return -1;
    }
    
    
    return 0;
}

void interpreter_close(void) {
    interpreter_reset();
    close(jtag);
}

/*
int main( int argc, char *argv[] )
{
    interpreter_init("/root/rbstws.svf");
    interpreter_step();
    interpreter_step();
    interpreter_run();
    interpreter_run();
    interpreter_close();
    return 0;
}
*/


