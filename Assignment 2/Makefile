CFLAGS = -Wall -O2 -I.
DEBUG =
CC = gcc


parallel_convert: parallel_convert.o genarray.o comms.o makehtml.o
	$(CC) $^ -o $@

parallel_convert.o: include/shared.h
	$(CC) $(DEBUG) $(CFLAGS) -c src/parallel_convert.c

comms.o: include/shared.h
	$(CC) $(DEBUG) $(CFLAGS) -c src/comms.c

genarray.o: include/shared.h
	$(CC) $(DEBUG) $(CFLAGS) -c src/genarray.c

makehtml.o: include/shared.h
	$(CC) $(DEBUG) $(CFLAGS) -c src/makehtml.c

clean:
	rm -f *.o
# The line below will take copy all files in the master folder to the in folder
# This is highly specific and will do anything if the input directory is not
# "in". This line is to completely reset the execution environment for testing.
	rm -f parallel_convert
