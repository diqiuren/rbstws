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

int safecheck_add(unsigned int tap, unsigned int nbits, char *tdo, char *mask)
{
	safecheck_t *new_safechk = NULL;
	safecheck_t *last_item = NULL;
	
	if(nbits == 0)
		return -1;
	
	if(strlen(tdo) == 0 || strlen(mask) == 0 || 
		strlen(tdo) != strlen(mask))
		return -1;
		
	/* Memory allocation for the new structure */
	new_safechk = (safecheck_t *) malloc(sizeof(safecheck_t));
	
	if(new_safechk == NULL)
		return -1;
	
	/* nbits */
	new_safechk->nbits = nbits;
	
	/* tdo */
	new_safechk->tdo = calloc(strlen(tdo), sizeof(char));
	
	if(new_safechk->tdo == NULL)
		return -1;
		
	strcpy(new_safechk->tdo, tdo);
	
	/* mask */
	new_safechk->mask = calloc(strlen(mask), sizeof(char));
	
	if(new_safechk->mask == NULL)
		return -1;
		
	strcpy(new_safechk->mask, mask);	

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

unsigned int safecheck(unsigned int time)
{
	double tstart, tstop;
	//safecheck_t *check_list[2] = {NULL, NULL};
	
	tstart = (double)clock()/CLOCKS_PER_SEC;

	//check_list[0] = safecheck_list[0];
	//check_list[1] = safecheck_list[1];
	
	while(1) {
		
		
		tstop = (double)clock()/CLOCKS_PER_SEC;
		if((tstop - tstart) >= time)
			break;
	}
	
	return 0;
}

/* Testbench */
#if (__SAFECHECK_TB == 1)
int main(int argc, char *argv[])
{
	safecheck_t *list;
	char tdo[512];
	char mask[512];
	char character;
	int nbits;
	int counter;

	
	while(1) {
		memset(tdo, 0, 512);
		memset(mask, 0, 512);
		
		printf ("Add new element (y/n): ");

		character = getchar();
		while (getchar() != '\n');

		if(character == 'n')
			break;
		else {
			/* nbits */
			printf ("New element NBITS: ");
			scanf("%d", &nbits);
				
			/* tdo */
			printf ("New element TDO: ");
			scanf("%s", tdo);
			
			/* mask */
			printf ("New element MASK: ");
			scanf("%s", mask);
			
			safecheck_add(0, nbits, tdo, mask);
			safecheck_add(1, nbits, tdo, mask);
			
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
	
	printf("Testing safecheck (5s)... "); fflush(stdout);
	safecheck(5);
	printf("Done!\n"); fflush(stdout);
	
	return 0;
}
#endif

