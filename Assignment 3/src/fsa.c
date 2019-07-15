/* CSCI 4061 Assignment 3
Student ID: 4255134, 4735981
Student Name: Yuhang Zhao, Derek Blom
fsa.c
*/

#include <inc/mini_filesystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>

/* Test Helper Interface */
void write_into_filesystem(char* input_directory, char *log_filename){
  /*if old log file exists, delete it then make a new one*/
  if(access(log_filename, F_OK) != -1) {
    // file exists
    remove(log_filename);
  }
  Initialize_Filesystem(log_filename);
  char findfiles[strlen(input_directory)+21];
  int i = 0;
  snprintf(findfiles, 21 + strlen(input_directory),"find %s -type f -print0", \
  input_directory);
  FILE *pa = popen(findfiles, "r");
  if (pa == NULL){
    printf("error running command");
  }
  char line[256];
  char filename[21];
  while (fgets(line, 256, pa) != NULL)  {

    line[strlen(line)-1] = 0;
    Create_File(line);

    strcpy(filename, strrchr(line,'/') + 1);
    memset(filename, 0, 21);
    i++;
  }
  pclose(pa);
}
void make_filesystem_summary(char* filename){
  FILE *fd = fopen(filename, "w");
  if (fd == NULL){
    printf("Error opening file!\n");
    exit(1);
  }
  fprintf(fd, "Filename,Extension,Size,Inode,StartBlock,EndBlock\n");
  for(int i = 0; i < MAXFILES; i++){
    char filename[21];
    if(Get_file_in_Directory(filename, i) == -1){ continue; }
      Inode temp = Inode_Read(Search_Directory(filename));
      int inode = temp.Inode_Number;

      int StartBlock = temp.Start_Block;
      int EndBlock = temp.End_Block;
      int size = temp.File_Size;


      char *ext;
      char *dot = strrchr(filename, '.');
      if(!dot || dot == filename){
        *ext = 0;
      } else {
        *dot = '\0';
        ext = dot + 1;
      }
      fprintf(fd, "%s,%s,%d,%d,%d,%d\n",\
      filename, ext, size, inode, StartBlock, EndBlock);


  }
  fclose(fd);
}
void read_images_from_filesystem_and_write_to_output_directory\
(char* output_directory){
  for(int i = 0; i < MAXFILES; i++){
    char filename[21];
    char ffilename[21];
    if(Get_file_in_Directory(filename, i) == -1){ continue; };

    if(strcmp(filename, "") != 0){
      strncpy(ffilename, filename, 21);
      char *ext;
      char *dot = strrchr(filename, '.');
      if(!dot || dot == filename){
        *ext = 0;
      } else {
        *dot = '\0';
        ext = dot + 1;
      }
      if(strcmp(ext ,"jpg") == 0){
        int inodenum = Search_Directory(ffilename);
        int filesize = Inode_Read(Search_Directory(ffilename)).File_Size;
        int outstrlen = strlen(output_directory)+strlen(ffilename)+2;
        char outstr[outstrlen];
        snprintf(outstr, outstrlen, "%s/%s", output_directory,ffilename);
        FILE *fd = fopen(outstr, "wb");
        if (fd == NULL){
          printf("Error opening file!\n");
          exit(1);
        }
        char to_read[filesize];
        Read_File(inodenum, 0, filesize, to_read);
        fwrite(to_read, 1, filesize, fd);
        fclose(fd);
      }
    }
  }
}
void generate_html_file(char* filename, char* output_directory){
  //On Moodle form, TA Mahfuzur Rahman said we can pass a second argument
  //to keep track of output_directory, or make a global variable

  //get_filename_without_ext is a helper function that removes extension.
  char *get_filename_without_ext(char* mystr) {
    char *retstr;
    char *lastdot;
    if (mystr == NULL)
    return NULL;
    if ((retstr = malloc (strlen (mystr) + 1)) == NULL)
    return NULL;
    strcpy (retstr, mystr);
    lastdot = strrchr (retstr, '.');
    if (lastdot != NULL)
    *lastdot = '\0';
    return retstr;
  }

  const char *get_filename_ext(const char *fname) {
    const char *dot = strrchr(fname, '.');
    if(!dot || dot == fname) return "";
    return dot + 1;
  }

  char htmlfile[strlen(output_directory) + 25];
  snprintf(htmlfile, strlen(output_directory) + 25, "%s/%s", output_directory,\
   filename);
  FILE *f = fopen(htmlfile, "w");
  if (f == NULL)
  {
    printf("Error generating html file, check permissions via ls -l\n change \
    permissions via chmod a+x *");
    exit(1);
  }

  /* print html head and add indentation */
  fprintf(f, "%s\n", "<!DOCTYPE html>\n    <html>\n       <head>\n         \
  <title>HTML page</title>\n         <style>\n           body{\n          \
  min-height: 100vh;\n          	display: flex;\n          	padding:0px;\n \
             	margin: 0px;\n          }\n          body>  div{\n            \
     display: flex;\n            flex-direction: column;\n            \
    margin: auto;\n            justify-content: center;\n          }\n       \
       body> div> div{\n            display: flex;\n            \
    margin: auto;\n          }\n         </style>\n       </head>\n       \
    <body>\n              <div>");

  bool run_once=true;
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (output_directory)) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      if (!strcmp (ent->d_name, "."))
      continue;
      if (!strcmp (ent->d_name, ".."))
      continue;
      // printf ("%s\n", ent->d_name);
      if(!strcmp(get_filename_ext(ent->d_name), "jpg")){
        char cmdthmb[strlen(output_directory)*2+strlen(ent->d_name)*2+46];
        snprintf(cmdthmb, sizeof(cmdthmb), "convert -thumbnail 200x200 %s/%s \
        %s/%s_thumb.jpg", output_directory, ent->d_name, output_directory, \
        get_filename_without_ext(ent->d_name));
        if(system(cmdthmb)==-1){
          fprintf (stderr, "unable to execute \"convert\" command");
        }
        while(run_once){
          fprintf(f, "%s%s%s\n", "\
                  <img id=\"img\" alt=\"ads\" src = \"./",\
          get_filename_without_ext(ent->d_name), "_thumb.jpg\">\n    \
                 <div>\n               \
          <button id=\"prev\" type=\"button\">Previous</button>\n              \
           <button id=\"next\" type=\"button\">Next</button>\n            \
         </div>\n              </div>\n              <script>\n              \
          const time = 120000;\n      const images = [");
          run_once=false;
        }
        fprintf(f, "%s%s%s\n", "          \"./", ent->d_name, "\",");
        fprintf(f, "%s%s%s\n", "          \"./", get_filename_without_ext\
        (ent->d_name), "_thumb.jpg\",");

      }
    }

    closedir (dir);

  } else {
    /* could not open directory */
    perror ("");
  }
  fprintf(f, "      ];\n      var x = -1;\n        function displayNextImage() \
  {\n            x = (x === images.length - 1) ? 1 : x + 2;\n            \
    update();\n            resetTimer();\n        }\n        \
    function displayPreviousImage() {\n            x = (x <= 1) ? images.length\
       - 1 : x - 2;\n            update();\n            resetTimer();\n        \
     }\n        function update(){\n            document.getElementById(\"img\"\
   ).src = images[x];\n            document.getElementById(\"img\").onclick = \
   function() {\n              window.location.href = images[x-1];\n           \
   };\n        }\n        var timerId = -1;\n        function resetTimer() {\n\
           if(timerId >= 0)                 clearTimeout(timerId);\n           \
    timerId = setTimeout(timer, time);\n        }\n        function timer() {\n\
                x = (x + 2) %% images.length;\n            update();\n         \
       resetTimer();\n        }\n        resetTimer();\n        \
    document.querySelector(\"#next\").addEventListener(\"click\", \
    displayNextImage);\n        \
    document.querySelector(\"#prev\").addEventListener(\"click\", \
    displayPreviousImage);\n        </script>\n     </body>\n  </html>\n");
  fclose(f);

}
