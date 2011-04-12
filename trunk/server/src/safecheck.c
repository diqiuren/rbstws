/*   This file is prepared for Doxygen automatic documentation generation     */
/*! \file **********************************************************************
 *
 * \brief
 *      Safecheck functions
 * 
 * \author
 *      Américo Dias <americo.dias@fe.up.pt>
 *
 * $Revision$
 * $HeadURL$
 * $Date$
 * $Author$
 * $Id$
 *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <time.h>
#include "safecheck.h"
#include "jtag.h"


safecheck_t *safecheck_list[2] = {NULL, NULL};

void safecheck_clear(unsigned int tap)
{
	safecheck_t *n_safecheck;
    safecheck_t *c_safecheck;

    c_safecheck = safecheck_list[tap];

    while(c_safecheck != NULL) {
        n_safecheck = (safecheck_t*) c_safecheck->next_safecheck;
        free(c_safecheck->tdo);
        free(c_safecheck->mask);
        free(c_safecheck);
        c_safecheck = n_safecheck;
    }

    safecheck_list[tap] = NULL;

    return;
}

safecheck_t *safecheck_last_item(unsigned int tap)
{
	safecheck_t *last_item;
	
	if(safecheck_list[tap] == NULL)
		return NULL;
		
	last_item = safecheck_list[tap];
	
	while(last_item->next_safecheck != NULL)
		last_item = (safecheck_t *)last_item->next_safecheck;
	
	return (safecheck_t *)last_item;
}

int safecheck_add(unsigned int tap, safecheck_t *safechk)
{
	safecheck_t *new_safechk = NULL;
	safecheck_t *last_item = NULL;
	
	if(safechk->nbits == 0)
		return -1;
	
	if(strlen(safechk->tdo) == 0 || strlen(safechk->mask) == 0 || 
		strlen(safechk->tdo) != strlen(safechk->mask))
		return -1;
		
	/* Memory allocation for the new structure */
	new_safechk = (safecheck_t *) malloc(sizeof(safecheck_t));
	
	if(new_safechk == NULL)
		return -1;
	
	/* nbits */
	new_safechk->nbits = safechk->nbits;
	
	/* tdo */
	new_safechk->tdo = calloc(strlen(safechk->tdo), sizeof(char));
	
	if(new_safechk->tdo == NULL)
		return -1;
		
	strcpy(new_safechk->tdo, safechk->tdo);
	
	/* mask */
	new_safechk->mask = calloc(strlen(safechk->mask), sizeof(char));
	
	if(new_safechk->mask == NULL)
		return -1;
		
	strcpy(new_safechk->mask, safechk->mask);	

	/* next_safecheck */
	new_safechk->next_safecheck = NULL;
	
	/* Link the new structure to the end of the safecheck_list */
	last_item = safecheck_last_item(tap);
	
	if(last_item == NULL) {
		safecheck_list[tap] = new_safechk;
	}
	else {
		last_item->next_safecheck = (struct safecheck_t *)new_safechk;
	}
	
	return 0;
}

unsigned int safecheck(void)
{
	double tstart, tstop, ttime;
	int done = 0;
	
	tstart = (double)clock()/CLOCKS_PER_SEC;

	while(1) {
		tstop = (double)clock()/CLOCKS_PER_SEC;
		if((tstop - tstart) > 5)
			break;
	}
}

/* Testbench */
#if (__SAFECHECK_TB == 1)
int main(int argc, char *argv[])
{
	safecheck_t *list;
	safecheck_t *check = NULL;
	char buffer[512];
	int done = 0;
	int nbits;
	int counter;
	
	/* Memory allocation for the new structure */
	check = (safecheck_t *) malloc(sizeof(safecheck_t));

	if(check == NULL) {
		printf("Memory allocation error! (check)\n");
		return -1;
	}
	
	while(done == 0) {
		memset(&buffer[0], 0, 512);

		printf ("Add new element (y/n): ");

		buffer[0] = getchar();
		while (getchar() != '\n');

		if(buffer[0] == 'n')
			done = 1;
		else {
			/* nbits */
			printf ("New element NBITS: ");
			scanf("%d", &nbits);
			
			check->nbits = (unsigned int)nbits;
				
			/* tdo */
			printf ("New element TDO: ");
			scanf("%s", buffer);
			
			check->tdo = calloc(strlen(buffer), sizeof(char));
			if(check->tdo == NULL) {
				printf("Memory allocation error! (check->tdo)\n");
				return -1;
			}
			strcpy(check->tdo, buffer);
			
			/* mask */
			printf ("New element MASK: ");
			scanf("%s", buffer);
			
			check->mask = calloc(strlen(buffer), sizeof(char));
			if(check->mask == NULL) {
				printf("Memory allocation error! (check->mask)\n");
				return -1;
			}
			strcpy(check->mask, buffer);
			
			check->next_safecheck = NULL;
			
			safecheck_add(0, check);
			safecheck_add(1, check);
			
			free(check->tdo);
			free(check->mask);
			
			fflush(stdin);
		    getchar();
			
		}
		
		fflush(stdin);
	}
	
		
	list = safecheck_list[0];
	
	counter = 0;
	
	while(list != NULL) {
		counter++;
		printf("Item %d:\n", counter);
		printf("NBITS: %d\n", list->nbits);
		printf("TDO:   %s\n", list->tdo);
		printf("MASK:  %s\n\n", list->mask);
		
		list = (safecheck_t *)list->next_safecheck;
	}
		
	safecheck_clear(0);
	safecheck_clear(1);
	
	printf("Testing safecheck... "); fflush(stdout);
	safecheck();
	printf("Done!\n"); fflush(stdout);
}
#endif

