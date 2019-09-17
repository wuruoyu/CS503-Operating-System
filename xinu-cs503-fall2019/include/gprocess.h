/* gprocess.h */

/* group id */
#define PSSCHED 0
#define MFQSCHED 1

struct groupent {   /* Entry in the group table */
  uint16 prnum;
  pri16 gprio;
};

extern struct groupent grouptab[];
