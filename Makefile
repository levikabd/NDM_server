all: ndm_server
	g++ main.cpp server.cpp -o ndm_server

clean:
	rm -rf *.o ndm_server

