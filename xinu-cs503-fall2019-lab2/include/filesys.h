
#ifndef NFILE
#define NFILE 50
#endif

#define FILE_FREE 	0
#define FILE_OCCUPIED 	1

#define FILE_UNOPEN 	0
#define FILE_OPEN 	1

#define DL_OPEN 	0
#define DL_CLOSE 	1

struct fileent {
	uint16 filestate;
	// TODO: not sure the content is not released
	char*  filepath;
	char*  filecontent;
	uint32 filesize;
	uint16 fileopen;
	int load_process[NPROC];
};

extern struct fileent filetab[];
