#include <sys/stat.h>
#include <dirent.h>


struct fileinfo {
  int fileid;
  char filename[128];
  char filetype[16];
  int filesize;
  char timeofmodification[256];
  long unsigned threadid;
};



char *out;

struct logdata {
  time_t starttime;
  int ms;
  int threadscreated;
  int numdirs;
  int numfiles;
  int jpgs, bmps, pngs, gifs;
};
struct inout{
  char in[256];
  struct logdata *data;
};

struct inoutv2{
  char in[256];
  char ext[4];
  struct logdata *data;
};

struct v2fileargstruct{
  char in[256];
  char ext[4];
  struct logdata *data;
};
struct v3fileargstruct{
  char in[256];
  struct dirent *entry;
  struct logdata *data;
};

volatile int done;


int errcheck(int c, char* ipd, char* opd);
int variant1(char *in, char *out);
int variant2(char *in, char *out);
int variant3(char *in, char *out);
int createhtml();
int writetohtml(const char *location, const struct fileinfo finfo);
int closehtml();
int createlog(int variant);
void *logtimer(void *args);
int finishlog(struct logdata *ld);
int getnumdirs(char *in);
int getnumfiles(char *in);
int entrytype(struct dirent *entry);
