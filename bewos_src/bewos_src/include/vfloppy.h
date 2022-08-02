#define VFLOPPY_READONLY 4
char vfloppy_flags;
int vfloppy_rd(int blocknum,int buffer);
int vfloppy_wt(int blocknum,int buffer);
