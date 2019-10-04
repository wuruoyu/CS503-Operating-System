/* gprocess.h */

/* group number */
#define NUMGROUP 2

/* group id */
#define PSSCHED 0
#define MFQSCHED 1

#define INITGPRIO 10

struct groupent { /* Entry in the group table */
  pri16 gprio;
  pri16 initgprio;
};

extern struct groupent grouptab[];
