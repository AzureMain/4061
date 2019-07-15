#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "md5sum.h"
#include <dirent.h>
#include <sys/stat.h>

// #define FILENAME "server.config"
#define MAXBUF 1024
#define DELIM "="
#define BELIM "-"
#define h_addr h_addr_list[0]

int type = 0;

struct configdata{
	char server_ip[MAXBUF];
	char port[MAXBUF];
	char chunk_size[MAXBUF];
	char image_type[MAXBUF];

};
/*
* get_config() will read the server.config file
*/
void get_config(struct configdata *cfg, char *filename){
	printf("opening %s\n",filename);
	FILE *file = fopen (filename, "r");
	if (file != NULL){
		char line[MAXBUF];
		int i = 0;

		while(fgets(line, sizeof(line), file) != NULL)
		{
			char *cfline;
			cfline = strstr((char *)line,DELIM);
			cfline = cfline + strlen(DELIM);
			char* pos;
			if((pos = strchr(cfline, '\n')) != NULL){
				*pos = '\0';
			}
			if(cfline[0] == ' '){
				cfline++;
			}
			if (i == 0){
				memcpy(cfg->server_ip,cfline,strlen(cfline));
				// printf("%s\n",configdata->server_ip);
			} else if (i == 1){
				memcpy(cfg->port,cfline,strlen(cfline));
				// printf("%s\n",configdata->port);
			} else if (i == 2){
				memcpy(cfg->chunk_size,cfline,strlen(cfline));
				// printf("%s\n",configdata->chunk_size);
			} else if (i == 3){
				memcpy(cfg->image_type,cfline,strlen(cfline));
				type = 1;
				// printf("%s\n",configdata->image_type);
			}
			i++;
		} // End while
		fclose(file);
	} else {
		fprintf(stderr, "Error opening config file: %s\n", strerror(errno));
		exit(1);
	}
	// End if file
}

struct catalogdata{
	char* filenames[MAXBUF];
	int max;
	int* md5mismatch;
};

struct catalogdata parsecatalog(char *filename){
	struct catalogdata catalog;
	FILE *file = fopen (filename, "r");
	if (file != NULL){
		char line[MAXBUF];
		catalog.max = 0;
		int first = 1;
		while(fgets(line, sizeof(line), file) != NULL)
		{
			if(first){
				first = !first;
				continue;
			}
			char *cfline, *end;
			cfline = line;
			if((end = strchr(cfline, ',')) != NULL){
				*end = '\0';
			}
			catalog.filenames[catalog.max] = (char *) malloc(strlen(cfline));
			memcpy(catalog.filenames[catalog.max],cfline,strlen(cfline));
			catalog.max++;
		} // End while
		fclose(file);
	} else {
		fprintf(stderr, "Error opening catalog: %s\n", strerror( errno ));
		exit(1);
	}
	// End if file
	return catalog;
}

void error(char *msg){
	perror(msg);
	exit(0);
}


void checkmd5(char* dir, int dled[], struct catalogdata *catalogdata){


	int i;
	printf("%d files to check:\n", dled[0]);
	for(i = 1; i <= dled[0]; i++){
		printf("\t%s\n", catalogdata->filenames[dled[i] - 1]);
	}


	char cata[] = "catalog.csv";
	FILE *f = fopen(cata, "rw");
	if(f != NULL){
		char activefile[256];
		int filei;
		FILE *fp = fopen(cata, "rw");
		for(filei = 1; filei <= dled[0]; filei++){
			strcpy(activefile,catalogdata->filenames[dled[filei] - 1]);
			printf("File: %s\n", activefile);

			// Now we generate a checksum for the local file
			unsigned char* checksum;
			checksum = malloc(sizeof(char)*MD5_DIGEST_LENGTH);
			char imagepath[256+strlen("images/")];
			sprintf(imagepath, "images/%s", activefile);
			md5sum(imagepath, checksum);
			char cslocal[MD5_DIGEST_LENGTH * 2 + 1] = "\0";

			int i;
			for(i = 0; i < MD5_DIGEST_LENGTH; i++){
				char temp[3];
				sprintf(temp, "%02x", checksum[i]);
				strcat(cslocal, temp);
			}
			cslocal[MD5_DIGEST_LENGTH * 2] = '\0';
			printf("\t Local Checksum:  %s\n", cslocal);

			// Here we get the checksum from the catalog
			int index = 0;
			catalogdata->md5mismatch[dled[filei] - 1] = 1;
			// printf("%d\n", dled[filei] - 1);
			char line[MAXBUF];
			while(fgets(line, sizeof(line), fp) != NULL)
			{
				if(index == dled[filei]){
					char *cfline;
					char filename[256];
					memset(filename, 0, 256);
					strncpy(filename, (char *)line, strcspn((char *)line, ","));
					cfline = strrchr(line, ',');
					cfline++;
					char csremote[MD5_DIGEST_LENGTH * 2 + 1] = "\0";
					strncpy(csremote, cfline, 32);
					csremote[MD5_DIGEST_LENGTH * 2] = '\0';;
					printf("\t Remote Checksum: %s\n", csremote);

					if (!strcmp(csremote, cslocal)){
						//set 0 once a match is found, no mismatch.
						//might need to put it in memory so when function terminates
						//the data still exist somehwere?
						printf("MATCH\n");
						catalogdata->md5mismatch[dled[filei] - 1] = 0;
						// printf("%d\n", catalogdata->md5mismatch[dled[filei] - 1]);
					}
					// if md5 didn't match
				}
				index++;
			}
			rewind(fp);
		}
		fclose(fp);
	} else {
		fprintf(stderr, "Error opening file: %s\n", strerror(errno));
		exit(1);
	}
	return;
}

int downloadfile(int socket, char* filename, int filesize){
	FILE *fp;
	fp = fopen(filename, "wb");
	if(fp == NULL){
		fprintf(stderr, "Error creating file: %s\n", strerror(errno));
		return 0;
	}
	// Receive catalog
	unsigned int vsb[filesize];
	int result;
	memset(vsb, 0, filesize);
	char chold[1];
	int write_result = write(socket, chold, 1);
	if(write_result < 0){
		error("ERROR performing a syncronization with the server");
	}

	int read_result, remaining = filesize;
	while ((read_result = read(socket, vsb, filesize)) > 0)
	{
		if (read_result < 1 )
		{ return 0; }
		result = fwrite(vsb, 1, read_result, fp);
		if (result < 1 )
		{ return 0; }
		remaining -= read_result;
		if(remaining <= 0){
			break;
		}
	}
	fclose(fp);
	return 1;
}

void genhtml(struct catalogdata catalog, int filesdled[]){
	printf("GENHTML");
	char* htmlfilename = "download.html";
	FILE *f = fopen(htmlfilename, "w");
	if (f == NULL) {
		printf("Error %s", strerror(errno));
	}
	fputs("<html><head><title>My Image Manager</title></head><body>", f);
	int i;
	for(i = 1; i <= filesdled[0]; i++){
		char* md5info;

			if(catalog.md5mismatch[i - 1]){
				md5info = "MD5 mismatch";
				fprintf(f, "<p align= \"left\"> %s, %s </p>", catalog.filenames[filesdled[i] - 1], md5info);
			} else {
				md5info = "MD5 matched";
				fprintf(f, "<a href=\"../Client/images/%s\"> <p align= \"left\"> %s </a>",
				catalog.filenames[filesdled[i]-1], catalog.filenames[filesdled[i] - 1]);
				fprintf(f, ", %s </p>", md5info);
			}
	}
	fprintf(f," .......... </body></html>");
	fclose(f);
}


int main(int argc, char *argv[])
{
	struct configdata *configdata = malloc(sizeof(struct configdata));
	struct sockaddr_in serv_addr;
	struct hostent *server;

	get_config(configdata, argv[1]);

	if (argc < 2) {
		fprintf(stderr,"usage %s <filename.config>\n", argv[0]);
		exit(0);
	}

	int portno = atoi(configdata->port);
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		error("ERROR opening socket");
	}
	server = gethostbyname(configdata->server_ip);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
		error("ERROR connecting");
	}
	printf("Connected to server.\n");
	char buffer[256];
	memset(buffer,0,256);
	// Send blocksize to server
	if (write(sockfd,configdata->chunk_size,strlen(configdata->chunk_size)) < 0){
		error("ERROR writing to socket");
	}
	int blocksize = atoi(configdata->chunk_size);
	memset(buffer,0,256);

	// This read gets the filesize from the server
	if(read(sockfd,buffer,256) < 0){
		error("ERROR reading filesize from socket");
	}
	int catalogsize = atoi(buffer);
	// Open catalog
	if(!downloadfile(sockfd, "catalog.csv", catalogsize)){
		error("Catalog failed to download");
	}
	// Send selection to server
	int i;
	struct catalogdata catalog;
	catalog = parsecatalog("catalog.csv");
	int mdfmm[catalog.max];
	catalog.md5mismatch = mdfmm;
	int *filesdled = malloc(sizeof(int)*(catalog.max + 1));
	filesdled[0] = 0;

	printf("===============================\n");
	printf("Connecting server at %s, port %s\n", configdata->server_ip, configdata->port);
	if(type){
		printf("Chunk size is %s bytes. Image type: %s.\n", configdata->chunk_size, configdata->image_type);
	} else {
		printf("Chunk size is %s bytes. No image type found.\n", configdata->chunk_size);
	}
	printf("===============================\n");
	for(i=0; i<catalog.max; i++){
		printf("[%d]%s\n", i+1, catalog.filenames[i]);
	}
	printf("===============================\n");
	//

	if(type){
		int filequeue[catalog.max];
		int filequeuesize = 0;
		int i;
		for(i = 0; i < catalog.max; i++){
			char activefile[256], temp[32];
			strcpy(activefile, catalog.filenames[i]);
			strcpy(temp, strrchr(activefile, '.')+1);
			if(strcmp(temp, configdata->image_type) == 0){
				filequeue[filequeuesize++] = i + 1;
			}
		}
		printf("Downloading files: ");
		for(i = 0; i < filequeuesize; i++){
			printf("%d, ", filequeue[i]);
		}
		printf("\n");
		int fqi = 0, fdi = 1;
		while(filequeuesize){
			// Send selection;

			int activeindex = filequeue[fqi];
			memset(buffer, '\0', 256);
			sprintf(buffer, "%d", activeindex);
			int write_result = write(sockfd, buffer, 256);
			if(write_result < 0){
				error("ERROR writing to the socket");
			}
			memset(buffer, '\0', 256);
			// Incoming file of size x
			if(read(sockfd,buffer,256) < 0){
				error("ERROR reading filesize from socket");
			}
			int incomingfilesize = atoi(buffer);
			char filename[256];
			char fileanddir[264];
			strcpy(filename, catalog.filenames[activeindex - 1]);
			sprintf(fileanddir, "images/%s", filename);
			if(downloadfile(sockfd,fileanddir,incomingfilesize)){
				printf("Downloaded %s over %d blocks.\n", filename, incomingfilesize%blocksize==0? incomingfilesize/blocksize : incomingfilesize/blocksize + 1);
				int j, duplicate = 0;
				for(j = 0; j <=filesdled[0]; j++){
					if(filesdled[j] == filequeue[fqi]){
						duplicate = 1;
					}
				}
				if(!duplicate){
				filesdled[0]++;
				filesdled[fdi++] = filequeue[fqi];
			}
			} else {
				printf("Failed to download %s\n",filename);
			};
			fqi++;
			filequeuesize--;
		}
		checkmd5("./images/", filesdled, &catalog);
		genhtml(catalog, filesdled);
		// printf("%s\n", catalog.max);
		// int test;
		// for (test = 0; test < catalog.max; test++){
		// 	printf("%s\n", catalog.md5mismatch[test]);
		// }


		int write_result = write(sockfd, buffer, 10);;
		if(write_result < 0){
			error("ERROR writing to the socket");
		}
		close(sockfd);
		return 0;
	} else {
		int fdi = 1;
		while(1){
			int selection = -1;
			while(selection == -1 || selection > catalog.max){
				if(selection > catalog.max){
					printf("File must be in the range 1 to %d.\n", catalog.max);
				}
				printf("Please select a file (0 will exit):");
				memset(buffer, '\0', 256);
				int scan_result = scanf("%s", buffer);
				if(scan_result < 0){
					error("ERROR scanning user input");
				}
				selection = atoi(buffer);
				if(!selection){
					printf("Computing checksums for downloaded files...\n");
					checkmd5("./images/", filesdled, &catalog);

					genhtml(catalog,filesdled);
					// printf("%d\n", catalog.max);
					// int test;
					// for (test = 0; test < catalog.max; test++){
					// 	printf("%s\n", catalog.filenames[test]);
					// }


					int write_result = write(sockfd, buffer, 10);;
					if(write_result < 0){
						error("ERROR writing to the socket");
					}
					close(sockfd);
					return 0;
				}

				int write_result = write(sockfd, buffer, 256);;;
				if(write_result < 0){
					error("ERROR writing to the socket");
				}
			}
			memset(buffer, '\0', 256);
			// Incoming file of size x
			if(read(sockfd,buffer,256) < 0){
				error("ERROR reading filesize from socket");
			}
			int incomingfilesize = atoi(buffer);
			char filename[256];
			char fileanddir[264];
			strcpy(filename,catalog.filenames[selection - 1]);
			sprintf(fileanddir, "images/%s", filename);
			if(downloadfile(sockfd,fileanddir,incomingfilesize)){
				printf("Downloaded %s over %d blocks.\n", filename, incomingfilesize%blocksize==0? incomingfilesize/blocksize : incomingfilesize/blocksize + 1);
				int j, duplicate = 0;
				for(j = 0; j <=filesdled[0]; j++){
					if(filesdled[j] == selection){
						duplicate = 1;
					}
				}
				if(!duplicate){
				filesdled[0]++;
				filesdled[fdi++] = selection;
			}
			} else {
				printf("Failed to download %s\n",filename);
			};

			// Collect file from server

		}
		close(sockfd);
	}
	return 0;
}
