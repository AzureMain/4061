/* CSCI 4061 Assignment 3
Student ID: 4255134, 4735981
Student Name: Yuhang Zhao, Derek Blom
fsi.c
*/
#include <inc/mini_filesystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <sys/types.h>
#include <sys/stat.h>

int Initialize_Filesystem(char* log_filename){
	int i;
	Count = 0;
	Log_Filename = log_filename;

	for(i = 0; i < MAXBLOCKS; i++){
		Disk_Blocks[i] = malloc(sizeof(char) * BLOCKSIZE);
	}

	Super_block newsuperblock;
	newsuperblock.next_free_inode = 0;
	newsuperblock.next_free_block = 0;
	Superblock_Write(newsuperblock);
	for(i = 0; i < MAXFILES; i++){
		Inode newinode;
		newinode.Inode_Number = i;
		if(i == 0){
			newinode.Start_Block = 0;
		}
		Inode_Write(i, newinode);
	}
	return 0;
}


int Create_File(char* filename){
	FILE *fp;
	long filesize;
	char *buffer;

	fp = fopen ( filename , "rb" );
	if( !fp ) perror(filename),exit(1);

	fseek( fp , 0L , SEEK_END);
	filesize = ftell( fp );
	rewind( fp );

	// This allocates the memory for the buffer
	buffer = calloc( 1, filesize+1 );
	if( !buffer ) {
		fclose(fp);
		fputs("could not allocate memory for buffer",stderr);
		exit(1);
	}
	// This will copy the file into a buffer
	if( 1!=fread( buffer , filesize, 1 , fp) ) {
		fclose(fp);
		free(buffer);
		fputs("could not read file into buffer",stderr);
		exit(1);
	}
	fclose(fp);

	int reqblocks = (filesize + BLOCKSIZE - 1) / BLOCKSIZE;
	int startblock = Superblock_Read().next_free_block;
	int next_free_inode = Superblock_Read().next_free_inode;

	Inode temp_inode;
	temp_inode.Inode_Number = next_free_inode;
	temp_inode.User_Id = getuid();
	temp_inode.Group_Id = getgid();
	temp_inode.File_Size = filesize;
	temp_inode.Start_Block = startblock;
	temp_inode.End_Block = startblock + reqblocks - 1;
	temp_inode.Flag = 0;

	char fnam[21];
	strcpy(fnam, strrchr(filename,'/') + 1);


	if(Search_Directory(fnam) == -1){
		if(Add_to_Directory(fnam, next_free_inode)==-1){
			error(-1, 5, "Could not write file, at file limit");
			return -1;
		}
	}

	Inode_Write(next_free_inode, temp_inode);


	int prev_inode = next_free_inode;
	Super_block newsuperblock;
	int next_free_block = startblock + reqblocks;
	newsuperblock.next_free_inode = ++next_free_inode;
	newsuperblock.next_free_block = next_free_block;
	Superblock_Write(newsuperblock);

	Inode temp = Inode_Read(next_free_inode);
	temp.Start_Block = next_free_block;
	Inode_Write(next_free_inode, temp);
	Write_File(prev_inode, 0, buffer);


	return 0;
}

int Open_File(char* filename){
	// Get the inode containing the corresponding filename
	int ret_inode = Search_Directory(filename);
	// Check its validity
	if(ret_inode >= 0){
		// Set the flag to 1

		Inode inode = Inode_Read(ret_inode);
		inode.Flag = 1;
		Inode_Write(ret_inode, inode);

		// Return the inode number
		return ret_inode;
	}
	// If the file was not found return with -1
	return -1;
}

int Read_File(int inode_number, int offset, int count, char* to_read){
	// If the Inode's "open" flag is not set to open then don't read
	// if(!Inode_List[inode_number].Flag){ return -1;}
	Inode inode = Inode_Read(inode_number);
	int filesize = inode.File_Size;

	if((offset > filesize)||((offset + count) > filesize)){
		// If we try to read an offset that is not within the file or the number of
		// bytes we are supposed to read takes us out of the file then return with
		// a negative 1.
		return -1;
	}

	// After applying the offset what block are we on. Ex: if we have an offset of
	// say 783, then we will skip the starting block and advance onto the next with
	// the remainder of 783 / 512 as the new offset
	int offsetblock = (offset / BLOCKSIZE) + inode.Start_Block;

	// Calculate the number of blocks to be read
	int blockstoread = inode.End_Block + 1 - offsetblock;


	// Incase we need to reference it.
	//int newoffset = offset % BLOCKSIZE;
	int bytesread = 0;

	for(int i = 0; i < blockstoread; i++){
		// If count is less than 0 then stop
		if(count > 0){
			// If count is greater or equal to BLOCKSIZE then limit count to BLOCKSIZE
			if(count >= BLOCKSIZE){
				// Use BLOCKSIZE as to not try to read past the block
				Block_Read(offsetblock + i, BLOCKSIZE, (to_read + bytesread));
				bytesread += BLOCKSIZE;

				// Reduce count by BLOCKSIZE
				count -= BLOCKSIZE;
				// Else count is less then BLOCKSIZE so we need to be specific
			} else {
				Block_Read(offsetblock + i, count, (to_read + bytesread));
				count = 0;
			}
		}
	}
	return 0;
}
int Write_File(int inode_number, int offset, char* to_write){
	// If the Inode's "open" flag is not set to open then don't write
	Inode inode = Inode_Read(inode_number);
	//if(!inode.Flag){ return -1; }

	// Get filesize of file associated to Inode x
	int filesize = inode.File_Size;

	if((offset > filesize)){
		// If we try to read an offset that is not within the file or the number of
		// bytes we are supposed to write takes us out of the file then return with
		// a negative 1.
		return -1;
	}

	int next_free_block = Superblock_Read().next_free_block;
	// If there is not enough space to write to contiguous blocks,
	// then we need to move the file to an area with enough unwritten blocks
	// in this case we will default to the next_free_block member of Superblock

	if((inode.End_Block + 1) != next_free_block){
		Inode inode_moved = Inode_Read(inode_number);
		int blockstomove = inode_moved.End_Block + 1 - inode_moved.Start_Block;
		int newstartblock = next_free_block;
		char blockcpy[BLOCKSIZE];

		for(int i = 0; i < blockstomove; i++){
			Block_Read(inode_moved.Start_Block + i, BLOCKSIZE, blockcpy);
			Block_Write(newstartblock + i, BLOCKSIZE, blockcpy);
			memset(blockcpy, 0, BLOCKSIZE);
		}
		// Change the associated inode to represent the new block locations
		inode_moved.Start_Block = newstartblock;
		inode_moved.End_Block = newstartblock + blockstomove - 1;

	}
	// Find the starting block after applying the offset.
	int startblock = (offset / BLOCKSIZE) + inode.Start_Block;

	// Calculate the number of blocks that will be written to
	// int blockstowrite = startblock - Inode_List[inode_number].End_Block + 1;
	// Incase we need to reference it.

	//int newoffset = offset % BLOCKSIZE;

	// Here we will fill the final block
	//int remainingbytes = BLOCKSIZE - (filesize % BLOCKSIZE);
	// char lastblockcontents[filesize % BLOCKSIZE];
	// Block_Read(startblock, filesize % BLOCKSIZE, lastblockcontents);

	int bytestowrite = filesize;
	int blockstowrite = bytestowrite/BLOCKSIZE;
	int byteswritten = 0;

	for(int i = 0; i <= blockstowrite; i++){

		// If count is less than 0 then stop
		if(bytestowrite > 0){
			// If count is greater or equal to BLOCKSIZE then limit count to BLOCKSIZE
			if(bytestowrite >= BLOCKSIZE){
				// Use BLOCKSIZE as to not try to read past the block
				Block_Write(startblock + i, BLOCKSIZE, (to_write + byteswritten));
				// Reduce count by BLOCKSIZE
				byteswritten += BLOCKSIZE;
				bytestowrite -= BLOCKSIZE;
				// Else count is less then BLOCKSIZE so we need to be specific
			} else {
				Block_Write(startblock + i, bytestowrite, (to_write + byteswritten));
				bytestowrite -= bytestowrite;
			}
		}
	}
	return 0;
}


int Close_File(int inode_number){
	// Set flag for given inode_number to 0
	Inode inode = Inode_Read(inode_number);
	inode.Flag = 0;
	Inode_Write( inode_number, inode);
	return 1;
}
