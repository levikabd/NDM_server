ndm_server: main.cpp server.cpp
	g++ -o ndm_server main.cpp server.cpp

clean:
	rm -rf *.o ndm_server

