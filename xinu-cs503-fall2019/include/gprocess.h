/* gprocess.h */

/* group number */
#define NUMGROUP 2

/* group id */
#define PSSCHED 0
#define MFQSCHED 1

/* init prio */
#define INITPRIO 10

struct groupent {   /* Entry in the group table */
  uint16 prnum;
  pri16 gprio;
};

extern struct groupent grouptab[];
