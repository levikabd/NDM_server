A C/C++ server application for GNU/Linux that asynchronously
processes TCP and UDP client connections using 
multiplexing mechanism (based on epoll).

To compile the program, go to the "ndm-server" directory and run the "make" command.

To build the package .deb run the following command:
"dpkg-deb --build --root-owner-group ndm-server_1.0-1_amd64".