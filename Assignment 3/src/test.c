/* Main function */

#include <inc/mini_filesystem.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

int errchk(int argc, const char* input_dir, const char* output_dir);


int main(int argc, char* argv[]){
  if(errchk(argc,argv[1],argv[2])==-1){
    return -1;
  }

  write_into_filesystem(argv[1], argv[3]);
  make_filesystem_summary(SUMMARYFILENAME);
  read_images_from_filesystem_and_write_to_output_directory(argv[2]);
  generate_html_file(HTMLFILENAME, argv[2]);

  char logstr[256];
  snprintf(logstr, 256, "Total filesystem accesses: %d\n", Count);
  printf("%s\n",logstr);
  return 0;
}

int errchk(int argc, const char* input_dir, const char* output_dir){
  if(argc != 4){
    fprintf (stderr, "Error, usage incorrect. \
    \nProper usage ./test <input directory> <output directory> <log file>\n");
    return -1;
  }

  //// Check input directory ////
  if (0 != access(input_dir, F_OK)) {
    if (ENOENT == errno) {
      printf("Input directory does not exist.\n");
      return -1;
      // indir does not exist
    }
    if (ENOTDIR == errno) {
      printf("Input directory not a directory.\n");
      return -1;
      // indir not a directory
    }
  }

  char cmd[64];
   snprintf(cmd, strlen(output_dir)+17 , "mkdir -p %s", output_dir);
   if(system(cmd)==-1){
     fprintf (stderr, "error trying to create output directory.");
     return -1;
   }
   return 0;
}
