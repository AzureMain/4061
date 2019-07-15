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
fnconstructor() constructs a fileinfo_t and initializes its values it places
the full path of the string it was provided. It also strips the path and file
extension and places that in the out member of the struct. This is used in the
creation of the convert command. It also initializes the members: locked, and
convby
*/

fileinfo_t fnconstructor(char *input) {
	fileinfo_t ret;
	memset(ret.in, 0, 100);
	memset(ret.out, 0, 100);

	strncpy(ret.in, input, strlen(input) - 1);
	ret.in[strlen(input)] = '\0';

	char *in = input;
	char *nopath = strrchr(in,'/') + 1;
	char *noext = strrchr(in,'.');
	*noext = '\0';

	strcpy(ret.out, nopath);
	ret.locked = 0;
	ret.convby = 0;
	return ret;
}

/*
patternarray() takes in an input dir, a pattern and the memory address of one of
the count variables that were initialized in main. First it generates a find
command based on the pattern argument. Then it uses popen() to capture the
output of the find command. Each line of the "file" is a filepath that the find
command found. This line is supplied as an argument to the fnconstructor() call
and the resulting fileinfo_t is assigned to the array.
*/
int patterncounter(const char* input_dir, char* pattern){
	char findpattern[strlen(input_dir)+strlen(pattern)+16];
	int ch, lines = 0;
	snprintf(findpattern, 16 + strlen(input_dir) + strlen(pattern) ,"find %s -name %s;", input_dir, pattern);
	FILE *pc = popen(findpattern, "r");
	if (pc == NULL){
		printf("error running command");
		return -1;
	}
	// Count Lines
	while(!feof(pc)) {
		ch = fgetc(pc);
		if(ch == '\n') {
			lines++;
		}
	}
	//printf("Found %d files\n", lines);
	pclose(pc);
	return lines;
}

fileinfo_t* patternarray(const char* input_dir, char* pattern,int count){
	char findpattern[strlen(input_dir)+strlen(pattern)+16];
	int i = 0;
	snprintf(findpattern, 16 + strlen(input_dir) + strlen(pattern) ,"find %s -name %s;", input_dir, pattern);
	FILE *pa = popen(findpattern, "r");
	if (pa == NULL){
		printf("error running command");
		return NULL;
	}
	char line[256];
	fileinfo_t *arr = malloc(sizeof(fileinfo_t)*count);
	while (fgets(line, 256, pa) != NULL)  {
		arr[i] = fnconstructor(line);
		i++;
	}
	pclose(pa);
	return arr;
}
/*
junkarray() is similar in function to patternarray() in that it creates an
array of fileinfo_t's who correspond to junkfiles
*/
int junkcounter(const char* input_dir){
	int ch, lines = 0;
	char findjunk[strlen(input_dir)+72];
	snprintf(findjunk, sizeof(findjunk),"find %s/ -type f -not -name \"*.png\" -not -name \"*.bmp\" -not -name \"*.gif\"", input_dir);
	FILE *pc = popen(findjunk, "r");
	if (pc == NULL){
		printf("error running command");
		return -1;
	}
	// Count Lines
	while(!feof(pc)) {
		ch = fgetc(pc);
		if(ch == '\n') {
			lines++;
		}
	}
	//printf("Found %d files\n", lines);
	pclose(pc);
	return lines;
}


fileinfo_t* junkarray(const char* input_dir, int count){
	int i = 0;
	char findjunk[strlen(input_dir)+72];
	snprintf(findjunk, sizeof(findjunk),"find %s/ -type f -not -name \"*.png\" -not -name \"*.bmp\" -not -name \"*.gif\"", input_dir);
	FILE *pa = popen(findjunk, "r");
	if (pa == NULL){
		printf("error running command");
		return NULL;
	}
	char line[256];
	fileinfo_t *arr = malloc(sizeof(fileinfo_t)*count);
	while (fgets(line, 256, pa) != NULL)  {
		fileinfo_t junk;
		memset(junk.in, 0, sizeof(char)*100);
		memset(junk.out, 0, sizeof(char)*100);
		junk.convby = 0;
		junk.locked = 0;
		strncpy(junk.in, line, strlen(line) - 1);
		arr[i] = junk;
		i++;
	}
	pclose(pa);
	return arr;
}
