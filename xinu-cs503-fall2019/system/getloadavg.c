/* getloadavg.c - getloadavg*/

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getloadavg -  Return the loadavg (system-wide)
 *------------------------------------------------------------------------
 */
fix16_t getloadavg() { return load_avg; }
