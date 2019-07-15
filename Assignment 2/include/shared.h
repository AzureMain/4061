/* Information
CSci 4061 Spring 2017 Assignment 2
Name1=Derek Blom
Name2=Yuhang Zhao
StudentID1=4735981
StudentID2=4255134
Commentary=
- in is the full filename
- out is the stripped down filename (no path, or extenstion)
- locked is a flag that will be used to determine if another process
is working or has worked on the file
- convby allows for the storage of the file that converted the file, or in the
case of junkfiles, deleted it.
*/

#include <unistd.h>


typedef struct fileinfo_t {
  char in[100];
  char out[100];
  int locked;
  int convby;
} fileinfo_t;

int patterncounter(const char* input_dir, char* pattern);
fileinfo_t* patternarray(const char* input_dir, char* pattern, int count);

int junkcounter(const char* input_dir);
fileinfo_t* junkarray(const char* input_dir, int count);


int junkhandler(fileinfo_t *junkfile);
int errchk(int argc, int convert_count , const char* input_dir, const char* output_dir);
int convertfile(fileinfo_t *file, const char* input_dir, const char* output_dir);
int dologs(fileinfo_t *arrs[], int counts[],const char* output_dir);
int makehtml(fileinfo_t *arrs[], int counts[],const char* output_dir);
