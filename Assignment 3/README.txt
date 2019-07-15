README.txt should include
a. Personal information for the group
	Derek Blom 4735981, Yuhang "Jack" Zhao 4255134

b. CSELab machine you tested your code on.
	CSE 1260-15

c. Syntax and usage pointers for your program (details on how to run/use your program)
	$ make clean; make;
	$ ./filesys <convert_count> <output_dir> <input_dir>
d. Any special test cases or anomalies handled / not handled by your program.
	No.
e. Experiment Results:
	Our program reports that there were a total of 6086 Filesystem accesses from processing all the files in the input_dir.tar. Dividing
	by two yields an approximation for the number of reads (3043).

	Use the following disk parameters for this calculation:
	? Rotation Rate � 15,000 RPM
	? Average Seek time � 4 ms
	? Average number of sectors per track � 1000
	? Sector size = Block size = 512 bytes.
	Assume that the time to position the head over the first block is Average Seek Time + Average
	Rotation time

		Based on the drive configuration the time for one revolution is 4 ms which means that the average rotational latency is 2ms.
		Also, a 512KB file will need 1000 logical blocks.

			Best case:
			The 1000 logical blocks are on the same cylinder (so the head will not need
				to move once it is in the correct position) in contiguous sectors. So, once the head moves to
				the correct position, it will only take one rotation (1000 sectors per rotation) to read the file.
				So the total time to read is (average seek time) + (average rotational latency) + (time for full rotation)
				4ms + 2ms + 4ms = 10ms.

			Average case:
			In an average case we will need to seek out every new block. So there will be 1000 head moves.
				1000 � ((average seek time) + (average rotational latency) + (transfer time [1/1000 of a full rotation]) )
				1000 � (8ms + 2ms + 0.004ms) = 10004 ms.
