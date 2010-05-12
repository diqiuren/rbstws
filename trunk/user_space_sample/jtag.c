#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include "jtag.h"

char *hex2bin(int nbits, char *string, char *result)
{
	int counter;
	char *ptr1, *ptr2;
	unsigned char nibble;
	
	if(nbits < 1 || strlen(string)*4 < nbits) {
		return NULL;
	}
	
	ptr1 = string + strlen(string) - 1;
	ptr2 = result + nbits - 1;
	
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

int state(int *fd, tap_state_t state, char *tdi_buffer, char *tdo_buffer, char *tms_buffer)
{
    int res;
    int counter;
    state_command_data state_cmd_dt;
    
    strcpy(tdi_buffer, "");
    strcpy(tdo_buffer, "");
    strcpy(tms_buffer, "");
      
    state_cmd_dt.state = state;
    state_cmd_dt.tms_buffer[0] = '\0';
    
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

tap_state_t get_state(int *fd)
{
    return ioctl(*fd, GET_STATE, NULL);
}

int sir(int *fd, unsigned int nbits, char *tdi, char *tdo, char *mask, char *tdi_buffer, char *tdo_buffer, char *tms_buffer)
{
    int res;
    int counter;
    state_command_data state_cmd_dt;
    shift_command_data shift_cmd_dt;

    char *tdi_bin, *tdo_bin, *mask_bin;
    
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
    
    strcat(tdi_buffer, tdi_bin);
    strcat(tdo_buffer, shift_cmd_dt.tdo_buffer);
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
    free(tdi_bin);
    free(tdo_bin);
    free(mask_bin);
    return res;
}

int sdr(int *fd, unsigned int nbits, char *tdi, char *tdo, char *mask, char *tdi_buffer, char *tdo_buffer, char *tms_buffer)
{
    int res;
    int counter;
    state_command_data state_cmd_dt;
    shift_command_data shift_cmd_dt;

    char *tdi_bin, *tdo_bin, *mask_bin;
    
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
    
    strcat(tdi_buffer, tdi_bin);
    strcat(tdo_buffer, shift_cmd_dt.tdo_buffer);
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
    free(tdi_bin);
    free(tdo_bin);
    free(mask_bin);
    return res;
}

int main( int argc, char *argv[] )
{
    int fd, line, res, x;
    char tdi_buffer[1024];
    char tdo_buffer[1024];
    char tms_buffer[1024];
    
    printf("JTAG Linux Kernel Module demo.\n");  fflush(stdout);
    
     
    fd = open("/dev/jtag", O_RDWR );
    if ( fd < 0 )
    {
        printf("Couldn't open jtag! Make sure you've loaded the module.\n");
        return -1;
    }
    
    line = 1;
    
    seltap(&fd, 0);
    
    /**************************************************************************/
    res = state(&fd, TEST_LOGIC_RESET, tdi_buffer, tdo_buffer, tms_buffer);
    if(res < 0)
        printf("Error in STATE function!\n");
        
    printf("L:%d TDI:%s TDO:%s TMS:%s STATE:%d\n", line++, tdi_buffer, tdo_buffer, tms_buffer, get_state(&fd));
     
    /**************************************************************************/
    res = sir(&fd, 8, "80", "81", "FF", tdi_buffer, tdo_buffer, tms_buffer);
    if(res == -1)
        printf("Error in SIR function!\n");
    else if(res == -2)
        printf("TDO error in SIR function!\n");
    printf("L:%d TDI:%s TDO:%s TMS:%s STATE:%d\n", line++, tdi_buffer, tdo_buffer, tms_buffer, get_state(&fd));
    
    
    for(x = 0; x < 5; x++) {
        /**************************************************************************/
        res = sdr(&fd, 18, "2FFAA", "00000", "00000", tdi_buffer, tdo_buffer, tms_buffer);
        if(res == -1)
            printf("Error in SDR function!\n");
        else if(res == -2)
            printf("TDO error in SDR function!\n");
        printf("L:%d TDI:%s TDO:%s TMS:%s STATE:%d\n", line++, tdi_buffer, tdo_buffer, tms_buffer, get_state(&fd));
           
        sleep(1);
        
        /**************************************************************************/
        res = sdr(&fd, 18, "2FF55", "00000", "00000", tdi_buffer, tdo_buffer, tms_buffer);
        if(res == -1)
            printf("Error in SDR function!\n");
        else if(res == -2)
            printf("TDO error in SDR function!\n");
        printf("L:%d TDI:%s TDO:%s TMS:%s STATE:%d\n", line++, tdi_buffer, tdo_buffer, tms_buffer, get_state(&fd));

        sleep(1);
    }
    
    /**************************************************************************/
    res = state(&fd, TEST_LOGIC_RESET, tdi_buffer, tdo_buffer, tms_buffer);
    if(res < 0)
        printf("Error in STATE function!\n");
        
    printf("L:%d TDI:%s TDO:%s TMS:%s STATE:%d\n", line++, tdi_buffer, tdo_buffer, tms_buffer, get_state(&fd));
    
    
    close(fd);
    
    return 0;

   
}


