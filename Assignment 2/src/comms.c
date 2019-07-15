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
#include <include/shared.h>


/*
junkhandler() is the function that deletes the junk files, this is done by
supplying it with a junkfile. It uses the in member of the supplied junkfile
as the target for the remove() call. In addition to that it stores the pid of
the child that deleted the file. This may not be necessary, but if I want to
log this later it better to have it now then try to implement it later.
*/

int junkhandler(fileinfo_t *junkfile){
  if(junkfile->locked == 1){
    return 0;
  } else {
    junkfile->locked = 1;
    junkfile->convby = getpid();
    remove(junkfile->in);
    return 1;
  }
}

/*
convertfile() takes in a file, the input directory, and the output directory
If the file is locked it returns with a 0. This tells the child that called it
that the file is currently being worked on by another process. If the file is
not locked then the locked flag is set to true and the construction of the
convert command begins. once the command is created it is executed by the call
to system(command). Once the conversion has happened the convby member of the
file is changed to the appropriate pid.
*/
int convertfile(fileinfo_t *file, const char* input_dir, const char* output_dir){
  if(file->locked == 1){
    //printf("LOCKED: %s\n",file->in);
    return 0;
  } else {
    file->locked = 1;
    //printf("TEST: %s\n",file->in);
    char cmdthmb[strlen(input_dir)+strlen(output_dir)+strlen(file->in)+strlen(file->out)+39];
    snprintf(cmdthmb, sizeof(cmdthmb), "convert -thumbnail 200x200 %s %s/%s_thumb.jpg", file->in, output_dir, file->out);
    //printf("COMMAND: %s\n", cmdthmb);
    if(system(cmdthmb)==-1){
      fprintf (stderr, "unable to execute \"convert\" command");
    };
    file->convby = getpid();
    return 1;
  }
}
