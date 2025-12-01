ndm-server: main.cpp server.cpp
	g++ -o ndm-server main.cpp server.cpp

clean:
	rm -rf *.o ndm-server

