
#ifndef __FRAME_BOOKKEEPER_H_
#define __FRAME_BOOKKEEPER_H_

#define FRAME_FREE          0
#define FRAME_OCCUPIED      1

#define FRAME_PD            0
#define FRAME_PT            1
#define FRAME_RP            2

/* Structure for bookkeeping metadata frames */
struct frame_bookkeeper_t{
    int     state;
    int     type;
    pid32   pid;
};

// index start from frame 1024 [metadata]
extern struct frame_bookkeeper_t frame_bookkeeper[];

#endif
