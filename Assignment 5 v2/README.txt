Derek Blom 4735981
Yuhang "Jack" Zhao 4255134

To run the program:

"client.c" “client.config” “iclient.config” “md5sum.c” “md5sum.h” MUST be in the folder called "Client"


"server.c" “server.config” “md5sum.c” “md5sum.h” MUST be in the folder called "Server",

A folder named “images” containing all the images must be in the folder “Server”
A folder named "images" needs to be in the folder "Client"

There are 3 makefiles, 1 goes into the Client and Server folders and run their respective
makefiles.

After running make in terminal, open a new terminal.
In the old terminal, in Server folder, run the server by using ./server server.config
In the new terminal, in Client folder, run the client by using ./client client.config
