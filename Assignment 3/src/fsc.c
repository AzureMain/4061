/* CSCI 4061 Assignment 3
Student ID: 4255134, 4735981
Student Name: Yuhang Zhao, Derek Blom
fsc.c
*/
#include <inc/mini_filesystem.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <error.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define TRILLION 1000000000000

char timebuf[64];

void gettime(char* timebuf){
  memset(timebuf, 0 ,64);
  char            buffer[64];
  struct timeval  tv;
  struct tm       *tm;

  gettimeofday(&tv, NULL);
  if((tm = localtime(&tv.tv_sec)) != NULL)
  {
    strftime(buffer, sizeof(buffer), "%H:%M:%S %%06uMS", tm);
    sprintf(timebuf, buffer, tv.tv_usec);
  }
}

void logaccess (char* structure, char* accesstype)
{
  FILE *fd;
  fd = fopen(Log_Filename, "a");
  if(!fd){
    error(-1, 5, "Error creating logfile");
    fclose(fd);
    return;
  } else {

    gettime(timebuf);
    fprintf(fd, "%s | %s | %s\n",timebuf,structure,accesstype);
  }
  fclose(fd);
}

void logmessage(char* message)
{
  FILE *fd;
  fd = fopen(Log_Filename, "a");
  if(!fd){
    error(-1, 5, "Error creating logfile");
    fclose(fd);
    return;
  } else {
    fputs(message, fd);
  }
  fclose(fd);
}


int Search_Directory(char* filename){

  if(LOGGING){logaccess("Directory", "Read");}

  Directory dir;
  for (int i=0; i<MAXFILES; i++){
    Count++;
    dir = Directory_Structure[i];
    if (strcmp(dir.Filename, filename) == 0){
      return dir.Inode_Number;
    }
  }
  return -1;
}

int Add_to_Directory(char* filename, int inode_number){

  if(LOGGING){logaccess("Directory", "Write");}

  // If filecount is greater than 128 return with -1
  if(inode_number > MAXFILES){ return -1;}
  for(int i = 0; i < MAXFILES; i++){
    Count++;

    if(strlen(Directory_Structure[i].Filename) == 0 ){
      strncpy(Directory_Structure[i].Filename, filename, 21);
      Directory_Structure[i].Inode_Number = inode_number;
      return 0;
    }
  }

  // If we get here then the file limit has been reached and the file was
  // unable to be added.
  return 0;
}

int Get_file_in_Directory(char* filename, int dirpos){

  if(LOGGING){logaccess("Directory", "Read");}

  Count++;
  // If filecount is greater than 128 return with -1
  char* fname = Directory_Structure[dirpos].Filename;
  if( strcmp(fname, "") != 0){
    strncpy(filename, fname, 21);
    return 0;
  }
  return -1;
  // If we get here then the file limit has been reached and the file was
  // unable to be added.
}

Inode Inode_Read(int inode_number){

  if(LOGGING){logaccess("Inode", "Read");}

  for (int i = 0; i < MAXFILES; i++) {
    Count++;
    if(Inode_List[i].Inode_Number == inode_number){
      return Inode_List[i];
    }
  }
  error(-1, 5, "Unable to read inode");
  Inode nullinode;
  return nullinode;
}

int Inode_Write(int inode_number, Inode input_inode){

  if(LOGGING){logaccess("Inode", "Write");}
  Count++;

  Inode_List[inode_number] = input_inode;
  return 0;
}

int Block_Read(int block_number, int num_bytes, char* to_read){
  if(num_bytes > BLOCKSIZE){
    error(-1, 5, "Attempting to read past block");
    return -1;
  }

  if(LOGGING){logaccess("Block", "Read");}
  Count++;

  for(int i = 0; i < num_bytes; i++){
    to_read[i] = Disk_Blocks[block_number][i];
  }

  return 0;
}

int Block_Write(int block_number, int num_bytes, char* to_write){
  Count++;
  if(num_bytes > BLOCKSIZE){
    error(-1, 5, "Attempting to write past block");
    return -1;
  }

  if(LOGGING){logaccess("Block", "Write");}

  memcpy(Disk_Blocks[block_number], to_write, num_bytes);

  return 0;
}

Super_block Superblock_Read(){
  if(LOGGING){logaccess("Superblock", "Read");}
  Count++;

  return Superblock;
}

int Superblock_Write(Super_block input_superblock){
  if(LOGGING){logaccess("Superblock", "Write");}
  Count++;

  Superblock = input_superblock;
  return 0;
}
