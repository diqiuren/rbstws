/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief JTAG Linux Kernel Module Documentation
 *
 * - Compiler:           GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a PIO module can be used.
 *                       The example is written for ICnova AP7000 Base.
 * - References:
 *      AVR32408: Using GPIO with Linux on the AVR32\n
 *      Carlos Becker: <a href="http://carlosbecker.com.ar">
 *                      Interfacing a character LCD display</a>
 *
 * \date 2009-12-18
 *
 * \author
 *      Américo Dias <americo.dias@fe.up.pt>
 *
 *****************************************************************************/

/*! \page License License
 *  JTAG Linux Kernel Module
 *  Copyright (C) 2009  Américo Dias, Daniel Oliveira, Miguel Pina
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \page Changelog Changelog
 * 12/22/09 - Added the interrupt bit.\n
 *          - Some bugs fixed.
 * 12/19/09 - Initial release
 */
 
/*! \mainpage
* \section intro Introduction
* This is the documentation for the data structures, functions, variables,
* defines, enums, and typedefs in the software for JTAG Linux Kernel Module.
* It also gives an examples of the usage of the driver.
*
* \section compinfo Compilation Info
* This software was written for the GNU GCC for AVR32.
* Other compilers may or may not work.
*
* \section deviceinfo Device Info
* All AVR32 devices with a PIO module can be used. This example has been tested
* with the following setup:
*
* - <a href="http://www.ic-board.de/product_info.php?info=p75_ICnova-AP7000-
* Base.html">ICnova AP7000 Base</a>
*
* \section setupinfo Setup Information
* CPU speed: <i>140 MHz</i>
*
* - Output pin: PD15
* - Input pin: PC31
* - Debug pin: PD14 *
*
* * Only with debug configuration. This pin changes its state on each bit
* read.
*
* \section application Application Usage
*
* The following code demonstrates the module usage:
*
* \code
* #include <stdio.h>
* #include <stdlib.h>
* #include <unistd.h>
* #include <fcntl.h>
* #include <string.h>
* #include <sys/ioctl.h>
* #include "jtag_common.h"
*
* int main( int argc, char *argv[] )
* {
*    (...)
*    return 1;
* }
* \endcode
*
* Don't forget to load the module first using:
* \verbatim insmod mod_jtag.ko \endverbatim
*
* To remove the module use:
* \verbatim rmmod mod_jtag \endverbatim
*
* Finally, if the module was compiled in debug mode, you can see module
* messages with the command \b dmesg \b.
*
* When recompiling a project, make sure to run \b make \b clean before recompiling
*/
