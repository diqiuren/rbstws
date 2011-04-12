/*   This file is prepared for Doxygen automatic documentation generation     */
/*! \file **********************************************************************
 *
 * \brief
 *      Safecheck functions definitions
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
#ifndef __SAFECHECK_H
#define __SAFECHECK_H

#define __SAFECHECK_TB		1

typedef struct {
    unsigned int nbits;
    char *tdo;
    char *mask;
    struct safecheck_t *next_safecheck;
} safecheck_t;

void safecheck_clear(unsigned int tap);
int safecheck_add(unsigned int tap, safecheck_t *safechk);
unsigned int safecheck(void);

#endif //__SAFECHECK_H
