/* bs_map.h */

#define NBS 8
#define NBSMAP (NBS * NPROC)

#define BSMAP_FREE      0
#define BSMAP_OCCUPIED  1

struct bs_map_entry {
    int     state;
    pid32   pid;
    pageid_t  vpage;
    int     npage;
    bsd_t   bs_id;
};

extern struct bs_map_entry bs_map[];
