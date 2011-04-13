#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include "jtag.h"

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

char *hex2bin(int nbits, char *string, char *result)
{
	int counter;
	char *ptr1, *ptr2;
	unsigned char nibble;
	
	if(nbits < 1 || strlen(string)*4 < nbits) {
		return NULL;
	}
	
	ptr1 = string + strlen(string) - 1;
	ptr2 = result + nbits;
	
	*ptr2-- = '\0';
	
	while(ptr1 >= string) {
		nibble = 0;
		
		if (*ptr1 >= '0' && *ptr1 <= '9')
            nibble = (*ptr1 - '0');
        else if (*ptr1 >= 'A' && *ptr1 <= 'F')
            nibble = (*ptr1 - 'A' + 10);
        else if (*ptr1 >= 'a' && *ptr1 <= 'f')
            nibble = (*ptr1 - 'a' + 10);
		else 
			return NULL;
		
		for(counter=1;counter<0x10 && ptr2 >= result;counter<<=1) {
			if(nibble & (unsigned char)counter)
				*ptr2 = '1';
			else
				*ptr2 = '0';	
			ptr2--;
		}
		
		ptr1--;
	}
	
	return result;
}

int seltap(int *fd, unsigned int tap) {
    int res;
    
    res = ioctl(*fd, SELTAP, &tap);

    return res;
}

int check_tdi(char *expected, char *received, char *mask) 
{
    int n;
    
    for(n=0; n<strlen(expected); n++) {
        if(mask[n] == '1') {
            if(expected[n] != received[n])
                return -1;
        }
    }
    
    return 0;
}

tap_state_t get_state(int *fd)
{
    return ioctl(*fd, GET_STATE, NULL);
}


int state(int *fd, tap_state_t state, char *tdi_buffer, char *tdo_buffer, char *tms_buffer, tap_state_t *initial_state)
{
    int res;
    int counter;
    state_command_data state_cmd_dt;
    
    strcpy(tdi_buffer, "");
    strcpy(tdo_buffer, "");
    strcpy(tms_buffer, "");
      
    state_cmd_dt.state = state;
    state_cmd_dt.tms_buffer[0] = '\0';
    
    *initial_state = get_state(fd);
    
    res = ioctl(*fd, STATE, &state_cmd_dt);

    if(res)
        return res;
            
    strcat(tms_buffer, state_cmd_dt.tms_buffer);
    for(counter=0; counter < strlen(state_cmd_dt.tms_buffer); counter++) {
        strcat(tdi_buffer, "x");
        strcat(tdo_buffer, "x");
    }
    
    return 0;
}


int sir(int *fd, unsigned int nbits, char *tdi, char *tdo, char *mask, char *tdi_buffer, char *tdo_buffer, char *tms_buffer, tap_state_t *initial_state)
{
    int res;
    int counter;
    state_command_data state_cmd_dt;
    shift_command_data shift_cmd_dt;

    char *tdi_bin, *tdo_bin, *mask_bin, *temp;
    
    temp = calloc(nbits+1, sizeof(char)); 
    tdi_bin = calloc(nbits+1, sizeof(char));
    tdo_bin = calloc(nbits+1, sizeof(char));
    mask_bin = calloc(nbits+1, sizeof(char));
    
    if( hex2bin(nbits, tdi, tdi_bin) == NULL ||
        hex2bin(nbits, tdo, tdo_bin) == NULL || 
        hex2bin(nbits, mask, mask_bin) == NULL ) {
        res = -1;
        goto sir_exit;
    }

    strcpy(tdi_buffer, "");
    strcpy(tdo_buffer, "");
    strcpy(tms_buffer, "");
    
    *initial_state = get_state(fd);
    
    /** Goto SHIFT-IR state ***************************************************/
    state_cmd_dt.state = SHIFT_IR;
    state_cmd_dt.tms_buffer[0] = '\0';
    
    res = ioctl(*fd, STATE, &state_cmd_dt);

    if(res)
        goto sir_exit;
            
    strcat(tms_buffer, state_cmd_dt.tms_buffer);
    for(counter=0; counter < strlen(state_cmd_dt.tms_buffer); counter++) {
        strcat(tdi_buffer, "x");
        strcat(tdo_buffer, "x");
    }
    
    /** SHIFT IR **************************************************************/     
    strcpy(shift_cmd_dt.tdi_buffer, tdi_bin);
    shift_cmd_dt.tdo_buffer[0] = '\0';
    
    res = ioctl(*fd, SHIFT, &shift_cmd_dt);
    
    if(res)
        goto sir_exit;
        
    strcpy_inv(temp, tdi_bin);
    strcat(tdi_buffer, temp);
    strcpy_inv(temp, shift_cmd_dt.tdo_buffer);
    strcat(tdo_buffer, temp);

    for(counter=0; counter < strlen(tdi_bin)-1; counter++) {
        strcat(tms_buffer, "0");
    }
    strcat(tms_buffer, "1");
    
    /** Goto UPDATE-IR state **************************************************/
    state_cmd_dt.state = UPDATE_IR;
    state_cmd_dt.tms_buffer[0] = '\0';
    
    res = ioctl(*fd, STATE, &state_cmd_dt);
     
    if(res)
        goto sir_exit;      
        
    strcat(tms_buffer, state_cmd_dt.tms_buffer);
    for(counter=0; counter < strlen(state_cmd_dt.tms_buffer); counter++) {
        strcat(tdi_buffer, "x");
        strcat(tdo_buffer, "x");
    }   
   
    if(check_tdi(tdo_bin, shift_cmd_dt.tdo_buffer, mask_bin))
        res = -2;

sir_exit:
	free(temp);
    free(tdi_bin);
    free(tdo_bin);
    free(mask_bin);
    return res;
}

int sdr(int *fd, unsigned int nbits, char *tdi, char *tdo, char *mask, char *tdi_buffer, char *tdo_buffer, char *tms_buffer, tap_state_t *initial_state)
{
    int res;
    int counter;
    state_command_data state_cmd_dt;
    shift_command_data shift_cmd_dt;

    char *tdi_bin, *tdo_bin, *mask_bin, *temp;    
    
    temp = calloc(nbits+1, sizeof(char));
    tdi_bin = calloc(nbits+1, sizeof(char));
    tdo_bin = calloc(nbits+1, sizeof(char));
    mask_bin = calloc(nbits+1, sizeof(char));
    
    if( hex2bin(nbits, tdi, tdi_bin) == NULL ||
        hex2bin(nbits, tdo, tdo_bin) == NULL || 
        hex2bin(nbits, mask, mask_bin) == NULL ) {
        res = -1;
        goto sdr_exit;
    }

    strcpy(tdi_buffer, "");
    strcpy(tdo_buffer, "");
    strcpy(tms_buffer, "");
    
    *initial_state = get_state(fd);
    
    /** Goto SHIFT-IR state ***************************************************/
    state_cmd_dt.state = SHIFT_DR;
    state_cmd_dt.tms_buffer[0] = '\0';
    
    res = ioctl(*fd, STATE, &state_cmd_dt);

    if(res)
        goto sdr_exit;
        
    strcat(tms_buffer, state_cmd_dt.tms_buffer);
    for(counter=0; counter < strlen(state_cmd_dt.tms_buffer); counter++) {
        strcat(tdi_buffer, "x");
        strcat(tdo_buffer, "x");
    }
    
    /** SHIFT IR **************************************************************/     
    strcpy(shift_cmd_dt.tdi_buffer, tdi_bin);
    shift_cmd_dt.tdo_buffer[0] = '\0';
    
    res = ioctl(*fd, SHIFT, &shift_cmd_dt);
    
    if(res)
        goto sdr_exit;
    
    strcpy_inv(temp, tdi_bin);
    strcat(tdi_buffer, temp);
    strcpy_inv(temp, shift_cmd_dt.tdo_buffer);
    strcat(tdo_buffer, temp);
    
    for(counter=0; counter < strlen(tdi_bin)-1; counter++) {
        strcat(tms_buffer, "0");
    }
    strcat(tms_buffer, "1");
    
    /** Goto UPDATE-IR state **************************************************/
    state_cmd_dt.state = UPDATE_DR;
    state_cmd_dt.tms_buffer[0] = '\0';
    
    res = ioctl(*fd, STATE, &state_cmd_dt);
     
    if(res)
        goto sdr_exit;      
        
    strcat(tms_buffer, state_cmd_dt.tms_buffer);
    for(counter=0; counter < strlen(state_cmd_dt.tms_buffer); counter++) {
        strcat(tdi_buffer, "x");
        strcat(tdo_buffer, "x");
    }   
    
    if(check_tdi(tdo_bin, shift_cmd_dt.tdo_buffer, mask_bin))
        res = -2;

sdr_exit:
    free(temp);
    free(tdi_bin);
    free(tdo_bin);
    free(mask_bin);
    return res;
}

int runtest(int *fd, unsigned int nclocks, char *tdi_buffer, char *tdo_buffer, char *tms_buffer, tap_state_t *initial_state)
{
    int res;
    int counter;
    state_command_data state_cmd_dt;


    strcpy(tdi_buffer, "");
    strcpy(tdo_buffer, "");
    strcpy(tms_buffer, "");
    
    *initial_state = get_state(fd);
    
    /** Goto RUN_TEST_IDLE state **********************************************/
    state_cmd_dt.state = RUN_TEST_IDLE;
    state_cmd_dt.tms_buffer[0] = '\0';
    
    res = ioctl(*fd, STATE, &state_cmd_dt);

    if(res)
        return res;
            
    strcat(tms_buffer, state_cmd_dt.tms_buffer);
    for(counter=0; counter < strlen(state_cmd_dt.tms_buffer); counter++) {
        strcat(tdi_buffer, "x");
        strcat(tdo_buffer, "x");
    }
    
    /** SHIFT IR **************************************************************/    
    res = ioctl(*fd, RUNTEST, &nclocks);
    
    if(res)
        return res;
    
    for(counter=0; counter < nclocks; counter++) {
        strcat(tms_buffer, "0");
        strcat(tdi_buffer, "x");
        strcat(tdo_buffer, "x");
    }
    
    return 0;
}

int get_input(int *fd, unsigned char input)
{
    int res;
    io_command_data io_command_dt;
    
    io_command_dt.pin = input;
    
    res = ioctl(*fd, GET_IO, &io_command_dt);
    
    if(res < 0)
        return res;
        
    return (int)io_command_dt.value;
    
}

int set_output(int *fd, unsigned char output, unsigned char state)
{
    int res;
    io_command_data io_command_dt;
    
    io_command_dt.pin = output;
    io_command_dt.value = state;
    
    res = ioctl(*fd, GET_IO, &io_command_dt);
    
    if(res < 0)
        return res;
        
    return 0;
    
}

