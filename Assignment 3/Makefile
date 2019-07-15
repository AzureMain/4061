CFLAGS = -std=gnu99 -Wall -O2 -I.
DEBUG =
CC = gcc


test: inc/mini_filesystem.h src/test.c fslib.a
	$(CC) -o test -I. src/test.c fslib.a
	
fslib.a: fsa.o fsc.o fsi.o
		ar -cvq fslib.a fsa.o fsc.o fsi.o	
		
fsa.o: inc/mini_filesystem.h src/fsa.c
	$(CC) $(DEBUG) $(CFLAGS) -c src/fsa.c

fsc.o: inc/mini_filesystem.h src/fsc.c
	$(CC) $(DEBUG) $(CFLAGS) -c src/fsc.c

fsi.o: inc/mini_filesystem.h src/fsi.c
	$(CC) $(DEBUG) $(CFLAGS) -c src/fsi.c
	
clean:
	rm -f -r out/
	rm -f *.o
	rm -f log.txt
	rm -f fslib.a
# The line below will take copy all files in the master folder to the in folder
# This is highly specific and will do anything if the input directory is not
# "in". This line is to completely reset the execution environment for testing.
	rm -f test
	rm -f fss.csv
