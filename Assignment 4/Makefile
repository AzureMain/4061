CFLAGS = -Wall -I. -O2 -g
DEBUG =
CC = gcc



image_manager: main.o threadv1.o threadv2.o threadv3.o fileoutput.o
	$(CC) $^ -o $@ -pthread

main.o: inc/shared.h src/main.c
	$(CC) $(DEBUG) $(CFLAGS) -c src/main.c -pthread

threadv1.o: inc/shared.h src/threadv1.c
	$(CC) $(DEBUG) $(CFLAGS) -c src/threadv1.c -pthread

threadv2.o: inc/shared.h src/threadv2.c
	$(CC) $(DEBUG) $(CFLAGS) -c src/threadv2.c -pthread

threadv3.o: inc/shared.h src/threadv3.c
	$(CC) $(DEBUG) $(CFLAGS) -c src/threadv3.c -pthread

fileoutput.o: inc/shared.h src/fileoutput.c
	$(CC) $(DEBUG) $(CFLAGS) -c src/fileoutput.c -pthread

# runv1: image_manager
# 	./image_manager v1 ~/4061/CSCI-4061/Assignment\ 4/dir0 /out
# runv2: image_manager
# 	./image_manager v2 ~/4061/CSCI-4061/Assignment\ 4/dir0 /out
# runv3: image_manager
# 	./image_manager v3 ~/4061/CSCI-4061/Assignment\ 4/dir0 /out

clean:
	rm -f *.o
	rm -f image_manager
	rm -r -f out/
