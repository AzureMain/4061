/* Information
CSci 4061 Spring 2017 Assignment 2
Name1=Derek Blom
Name2=Yuhang Zhao
StudentID1=4735981
StudentID2=4255134
Commentary=Program converts images and creates a webpage with the converted images.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <include/shared.h>



int main(int argc, char const *argv[]) {

  if(errchk(argc, atoi(argv[1]), argv[3], argv[2]) != 0){
    return -1;
  }
  int convert_count = atoi(argv[1]);
  const char* output_dir = argv[2];
  const char* input_dir = argv[3];

  int h, i;

  int pngcount = patterncounter(input_dir, "*.png");
  int bmpcount = patterncounter(input_dir, "*.bmp");
  int gifcount = patterncounter(input_dir, "*.gif");
  int junkcount = junkcounter(input_dir);

  fileinfo_t *pngarr = patternarray(input_dir, "*.png", pngcount);
  fileinfo_t *bmparr = patternarray(input_dir, "*.bmp", bmpcount);
  fileinfo_t *gifarr = patternarray(input_dir, "*.gif", gifcount);
  fileinfo_t *junkarr = junkarray(input_dir, junkcount);

  //set up shared memory space
  //void *mmap(void *addr, size_t length, int prot, int flags,
  //              int fd, off_t offset);
  //      addr = NONE, prot = PROT_NONE, flags = MAP_SHARED
  fileinfo_t *shpngarr = NULL;
  fileinfo_t *shbmparr = NULL;
  fileinfo_t *shgifarr = NULL;
  fileinfo_t *shjunkarr = NULL;

  int counts[] = {pngcount, bmpcount, gifcount, junkcount};
  fileinfo_t *arrs[] = {pngarr, bmparr, gifarr, junkarr};
  fileinfo_t *sharrs[] = {shpngarr, shbmparr, shgifarr, shjunkarr};



  for(h = 0; h < 4; h++){
    sharrs[h] = mmap(&arrs[h], (sizeof(fileinfo_t)*counts[h]),\
    PROT_READ | PROT_WRITE, MAP_SHARED |\
    MAP_ANONYMOUS, -1, 0);

    for(i = 0; i < counts[h]; i++){
      strncpy(sharrs[h][i].in, arrs[h][i].in, 100);
      strncpy(sharrs[h][i].out, arrs[h][i].out, 100);
      sharrs[h][i].convby = arrs[h][i].convby;
      sharrs[h][i].locked = arrs[h][i].locked;
    }
    /*
    Now we have an array in shared memory space for each array of matching
    files (4) that will allow each of the child processes to access it at
    the same time.
    */
  }

  /*
  In the upper for loop above there is an if check to limit the execution of
  convertfile() and junkhandler() to child processes. In that if child there
  are more ifs to determine what array of found files that child will work on.
  The calls to convertfile() and junkhandler() are there to advance the child
  along the array, this is done to ensure that two children are not trying to
  modify the same file. This for loop also keeps track of the number of active
  child processes. If at any point it reaches 5 the for loop will wait until
  one terminates before it starts another.
  */
  int status, c, active = 0;
  for (i = 0; i < convert_count; ++i) {
    if (fork() == 0) {
      int pid = getpid();
      if( pid % 2 == 0 && pid % 3 == 0 ){
        // Handle .png's's
        for(c = 0; c < pngcount; c++){
          if(!convertfile(&sharrs[0][c], input_dir, output_dir)){
            continue;
          }
        }
      }
      else if ( pid % 2 == 0 ) {
        // Handle .bmp's
        for(c = 0; c < bmpcount; c++){
          if(!convertfile(&sharrs[1][c], input_dir, output_dir)){
            continue;
          }
        }
      }
      else if ( pid % 3 == 0 ) {
        // Handle .gif's
        for(c = 0; c < gifcount; c++){
          if(!convertfile(&sharrs[2][c], input_dir, output_dir)){
            continue;
          }
        }
      }
      else {
        for(c = 0; c < junkcount; c++){
          if(!junkhandler(&sharrs[3][c])){
            continue;
          }
        }
        // Handle junk
        // DO nothing for now.
      }
      exit(0);
    }
    if(++active > 5){
      wait(&status);
    }
  }
  //printf("%d\n", active);
  // wait all child processes
  for (i = 0; i < 5; ++i){
    wait(&status);
    //printf("%d\n", --active);
  }
  dologs(sharrs, counts, output_dir);
  makehtml(sharrs, counts, output_dir);

  return 0;
}

/*
dologs() generates the logFile and the nonImage files in the output directory
the creation and printing to logFile happens by iterating through the arrays
that contain the converted png bmp and gif files and outputting them to the
logFile in a specific format. The creation and printing to the nonImage file
works in a similar way. But only goes through the junk array.
*/
int dologs(fileinfo_t *arrs[], int counts[],const char* output_dir){
  int j, i;
  char logfile[strlen(output_dir)+13];
  snprintf(logfile, sizeof(logfile), "%s/logFile.txt", output_dir);
  FILE *flog = fopen(logfile, "w+");

  for(i = 0; i < 3; i++){
    for(j = 0; j < counts[i]; j++){
      if(arrs[i][j].convby != 0){
        //printf("File \"%s\" was converted by PID: %d\n",arrs[i][j].in, arrs[i][j].convby);
        fprintf(flog,"File \"%s\" was converted by PID: %d\n",arrs[i][j].in, arrs[i][j].convby);
      }
    }
  }
  fclose(flog);

  char nonimage[strlen(output_dir)+14];
  snprintf(nonimage, sizeof(nonimage), "%s/nonImage.txt", output_dir);
  FILE *fni = fopen(nonimage, "w+");
  for(j = 0; j < counts[3]; j++){
    if(arrs[3][j].convby != 0){
      //printf("%s\n", arrs[3][j].in);
      fprintf(fni, "%s\n",arrs[3][j].in);
    }
  }
  fclose(fni);
  return 1;
}

/*
errchk() is the first function called after main. If this returns a -1 then
main() will return a -1 as well.
This function checks for:
- correct number of args
- value of convert_count
- if the supplied inputdir is a directory
- if the supplied inputdir exists
- if the output directory exists
- if not it creates it
*/

int errchk(int argc, int convert_count, const char* input_dir, const char* output_dir){
  if (argc < 3) {
    fprintf (stderr, "Incorrect number of args. Please retry with an input and output directory and a pattern(s)\n \
    usage: ./parallel_convert <convert_count> <output directory> <input directory>");
    return -1;
  }

  if (convert_count < 1 || convert_count > 10) {
    fprintf (stderr, "convert_count not within range try again with a value between 0 and 10");
    return -1;
  }

  //// Check input directory ////
  if (0 != access(input_dir, F_OK)) {
    if (ENOENT == errno) {
      printf("Input directory does not exist.");
      return -1;
      // indir does not exist
    }
    if (ENOTDIR == errno) {
      printf("Input directory not a directory.");
      return -1;
      // indir not a directory
    }
  }


  char cmd[64];
  snprintf(cmd, strlen(output_dir)+17 , "mkdir -p %s", output_dir);
  if(system(cmd)==-1){
    fprintf (stderr, "error trying to create output directory.");
  }

  return 0;
}
