
#include <sys/epoll.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <fcntl.h>

#include <cstdio>
#include <thread>

#include "server.h"

// struct ServerState {
//     int  epoll_fd;
//     int  listen_fd;
// };

// class Server
// {
// private:
//     ServerState tcp_state;
//     ServerState udp_state;
   
//     bool status=false;
//     std::string cmd="";
    // int port_tcp = 0, port_udp = 0;

    // epoll_event listen_event_tcp;
    // epoll_event listen_event_udp;
// public:
//     // Server(/* args */);
//     // ~Server();

    void Server::handle_tcp_data(int client_fd, const char* data, size_t len) {
        send(client_fd, data, len, 0);
    };

    void Server::handle_udp_data(int socket_fd,  const char* data, size_t len, const struct sockaddr_in* addr) {
        sendto(socket_fd, data, len, 0, (struct sockaddr*)addr, sizeof(*addr));
    };

    int Server::create_socket(bool is_udp, int port) {
        int type = is_udp ? SOCK_DGRAM : SOCK_STREAM;
        int sock = socket(AF_INET, type, 0);

        if (sock < 0){perror("Error create socket!"); return -1;};
        
        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;
        
        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("Error bind!");
            close(sock);
            return -1;
        };
        
        // if (!is_udp){
            listen(sock, 10);
        // };
        
        return sock;
    };

    int Server::prepareServer(int p_tcp, int p_udp)
    {
        tcp_state.epoll_fd = epoll_create1(0);
        udp_state.epoll_fd = epoll_create1(0);
        
        if(tcp_state.epoll_fd == -1){perror("epoll_create"); return 1;};    
        if(udp_state.epoll_fd == -1){perror("epoll_create"); return 1;};

        port_tcp=p_tcp;
        port_udp=p_udp;

        tcp_state.listen_fd = create_socket(false, port_tcp);
        udp_state.listen_fd = create_socket(true, port_udp);
        
        if(tcp_state.listen_fd == -1){return 1;};        
        if(udp_state.listen_fd == -1){return 1;}

        epoll_event event_tcp;
        event_tcp.data.fd = tcp_state.listen_fd;
        event_tcp.events = EPOLLIN | EPOLLET;

        epoll_event event_udp;        
        event_udp.data.fd = udp_state.listen_fd;
        
        if (epoll_ctl(tcp_state.epoll_fd, EPOLL_CTL_ADD, tcp_state.listen_fd, &event_tcp) == -1) 
        {perror("ERROR add listen_fd, epoll_ctl!");return 1;};        
        if (epoll_ctl(udp_state.epoll_fd, EPOLL_CTL_ADD, udp_state.listen_fd, &event_udp) == -1) 
        {perror("ERROR add listen_fd, epoll_ctl!");return 1;};

        return 0;
    };

    void Server::run_tcp()
    {
        while (status) 
        {
            int nfds_tcp = epoll_wait(tcp_state.epoll_fd, listen_events_tcp, 1024, -1);   
            if (nfds_tcp<1){continue;};
            
            for (int i = 0; i < nfds_tcp; ++i)
            {
                int fd_tcp = listen_events_tcp[i].data.fd;                
                if (fd_tcp == tcp_state.listen_fd)
                {
                        struct sockaddr_in client_addr;
                        socklen_t addrlen = sizeof(client_addr);
                        int client_fd = accept(tcp_state.listen_fd, (struct sockaddr*)&client_addr, &addrlen);
                        
                        if (client_fd > 0) {
                            fcntl(client_fd, F_SETFL, O_NONBLOCK);
                            epoll_event client_event;
                            client_event.data.fd = client_fd;
                            client_event.events = EPOLLIN | EPOLLET;
                            epoll_ctl(tcp_state.epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
                        };
                } else {
                    // Входящие данные
                    char buffer[1024];
                    ssize_t bytes_read;                    
                        bytes_read = recv(fd_tcp, buffer, sizeof(buffer) - 1, 0);                        
                        if (bytes_read > 0) {
                            buffer[bytes_read] = '\0';
                            handle_tcp_data(fd_tcp, buffer, bytes_read);
                        } else if (bytes_read == 0) {
                            // Клиент отключился
                            close(fd_tcp);
                            epoll_ctl(tcp_state.epoll_fd, EPOLL_CTL_DEL, fd_tcp, nullptr);
                        };
                };
            };            
        };   
        //return true; 
    };

    void Server::run_udp()
    {
        while (status) {
            int nfds_udp = epoll_wait(udp_state.epoll_fd, listen_events_udp, 1024, -1);            
            for (int i = 0; i < nfds_udp; ++i) {
                int fd_udp = listen_events_udp[i].data.fd;                
                if (fd_udp != udp_state.listen_fd) {
                    // Входящие данные
                    char buffer[1024];
                    ssize_t bytes_read;
                    
                    struct sockaddr_in client_addr;
                    socklen_t addrlen = sizeof(client_addr);
                    bytes_read = recvfrom(fd_udp, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_addr, &addrlen);                            
                    if (bytes_read > 0) {buffer[bytes_read] = '\0';handle_udp_data(fd_udp, buffer, bytes_read, &client_addr);};
                };
            };
        };   
        //return true; 
    };

    // void Server::run_ndm_server(ServerState* tcp_state, ServerState* udp_state) 
    int Server::run_ndm_server() 
    {
        // epoll_event events_tcp[1024];
        // epoll_event events_udp[1024];

        //status=true;
        std::thread tcp_thread(&Server::run_tcp,this);
        std::thread udp_thread(&Server::run_udp,this);
        status=true;

        if (tcp_thread.joinable()){tcp_thread.join(); }else{return 1;};
        if (udp_thread.joinable()){udp_thread.join(); }else{return 1;};

        return 0;
    };

//};