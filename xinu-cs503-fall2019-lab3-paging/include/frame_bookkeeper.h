
#ifndef __FRAME_BOOKKEEPER_H_
#define __FRAME_BOOKKEEPER_H_

#define FRAME_FREE          0
#define FRAME_OCCUPIED      1

#define FRAME_PD            0
#define FRAME_PT            1
#define FRAME_PG            2

/* Structure for bookkeeping metadata frames */
struct frame_bookkeeper_t {
    int         state;
    int         type;
    pid32       pid;
    pageid_t    vpage;
    int         count;      // counting should be for each pd_ent/pt_frame
    int         fifo_tag;
};

// index start from frame 1024 [metadata]
extern struct frame_bookkeeper_t frame_bookkeeper[];

/* in frame_bookkeeper.c */
extern syscall      find_free_frame();
extern syscall      bookkeep_frame_addr(char*, int, pageid_t);
extern syscall      bookkeep_frame_id(frameid_t, int, pageid_t);
extern syscall      bookkeep_frame_reset(frameid_t);

/* structure used by GCA */
struct gca_t {
    int reference;
    int modify;
};

extern struct gca_t gca_keeper[];
extern int gca_rr_idx;

// page replacement policy
extern frameid_t    fifo_find_frame();
extern frameid_t    gca_find_frame();

extern int time_tag;

#endif
