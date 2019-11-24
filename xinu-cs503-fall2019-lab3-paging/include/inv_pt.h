/* inv_pt.h */

#define INVPT_FREE      0
#define INVPT_OCCUPIED  1

#define NINVPT          NFRAMES

struct inv_pt_entry {
    int             state;
    pid32           pid;
    pageid_t          vpage_id;
};

extern struct inv_pt_entry inv_pt[];
