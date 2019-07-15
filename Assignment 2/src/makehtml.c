/* Information
CSci 4061 Spring 2017 Assignment 2
Name1=Derek Blom
Name2=Yuhang Zhao
StudentID1=4735981
StudentID2=4255134
Commentary=Program converts images and creates a webpage with the converted images.
*/

/*
This file defines makehtml() found in main()
Make html will automatically make a properly indented html page that displays
all images thumbnails though the use of javascript instead of creating a bunch
of unnecessarly slow linking html pages.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <include/shared.h>


/*makehtml() takes fileinfo_t node that contains all the information in the
struct defined in the include/share.h file*/

int makehtml(fileinfo_t *arrs[], int counts[],const char* output_dir){

  /*first we need to make the html file*/
  char htmlfile[strlen(output_dir) + 11];
  snprintf(htmlfile, strlen(output_dir) + 11, "%s/file.html", output_dir);
  FILE *f = fopen(htmlfile, "w");
  if (f == NULL)
  {
    printf("Error generating html file, check permissions via ls -l\n change permissions via chmod a+x *");
    exit(1);
  }

  /*varibles need for the loop*/
  int i, j, k=0;
  /* print html head and add indentation */
  fprintf(f, "%s\n", "<!DOCTYPE html>\n  <html>\n     <head>\n       <title>change picture</title>\n       <script type = \"text/javascript\">\n            function displayNextImage() {\n                x = (x === images.length - 1) ? 0 : x + 1;\n                document.getElementById(\"img\").src = images[x];\n            }\n            function displayPreviousImage() {\n                x = (x <= 0) ? images.length - 1 : x - 1;\n                document.getElementById(\"img\").src = images[x];\n            }\n            function startTimer() {\n                setInterval(displayNextImage, 2000);\n            }\n            var images = [], x = -1;");

  /*declare global pointer needed for reference later*/
  char *pathptr_ptr = NULL;

  /*for loop that handles the printing of image locations*/
  for(i=0; i<4; i++){
    for (j = 0; j < counts[i]; j++) {

      /*adding a slash to the end of the output_dir
      output_dir does not have a '/' at the end of the directory
      */
      const char* outname = output_dir;
      char* slash = "/";
      char* output_dir_with_slash;
      output_dir_with_slash = malloc(strlen(outname)+1+1); /* make space for the new string (should check the return value ...) */
      strcpy(output_dir_with_slash, outname); /* copy name into the new var */
      strcat(output_dir_with_slash, slash);

      /*append the file name to the end of output_dir*/
      char* arrsname = arrs[i][j].out;
      char* outname_with_arrsnames;
      outname_with_arrsnames = malloc(strlen(output_dir_with_slash)+strlen(arrsname)+1); /* make space for the new string (should check the return value ...) */
      strcpy(outname_with_arrsnames, output_dir_with_slash); /* copy name into the new var */
      strcat(outname_with_arrsnames, arrsname);

      /*append the file type to the end of file name*/
      char* extension="_thumb.jpg";
      char* with_extension;
      with_extension = malloc(strlen(outname_with_arrsnames)+strlen("_thumb.jpg")+1);
      strcpy(with_extension, outname_with_arrsnames);
      strcat(with_extension, extension);

      /*check if the file is in the output directory*/
      char *imgname = with_extension;
      if (!(access(imgname, F_OK))){
        /*if file is in the output directory
        obtain the absolute path of file so that
        the html page can properly display them
        */
        char actualpath [1024];
        char *pathptr;
        pathptr = realpath(imgname, actualpath);
        fprintf(f,"            images[%d] = \"%s\";\n", k, pathptr);
        pathptr_ptr = pathptr;
        k++;
      }
    }
  }
  /*spaces in fprintf is left for indentation purposes, we have a beautiful html page
  */
  fprintf(f, "        </script>\n     </head>\n     <body onload = \"startTimer()\">\n            <img id=\"img\" src=\"%s\"/>\n    </body>\n</html>",pathptr_ptr);
  fclose(f);
  return 0;
}
