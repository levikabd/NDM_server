
#include <sys/epoll.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <fcntl.h>

#include <cstring>

#include <cstdio>
#include <thread>

#include <iostream>
#include <sstream>

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
//     int port_tcp = 0, port_udp = 0;
//     const char *ipaddress = "";

//     epoll_event listen_events_tcp[1024];
//     epoll_event listen_events_udp[1024];
// // public:
// //     // Server(/* args */);
// //     // ~Server();
    void Server::show_time()
    {
        cmd="/time";
        std::cout << "cmd /time \n";


    };

    void Server::show_stats()
    {
        cmd="/stats";
        std::cout << "cmd /stats \n";


    };

    void Server::cmd_shutdown()
    {
        cmd="/shutdown";
        status = false;
        std::cout << "cmd /shutdown \n";
    };    

    bool Server::determine_cmd(std::string data)
    {
        //std::string data = "0" + in_data + "0";
        std::cout << data << " \n";
        //std::cout << data << " \n";
        std::cout << data.size() << "\n";
        if (data.size()<1)
        {
            return false;
        };        
        if (data.substr(0,1)=="/")
        {
            std::cout << "cmd! \n";
            return true;
        };
        return false;
    };

    void Server::handle_dt(std::string data, size_t len)
    {
        int sz = data.size();
        //send(client_fd, data, len, 0);  
        if (sz>=5)
        {
            if (data.substr(0, 5)=="/time")
            {
                show_time();
            };
        };
                
        if(data.substr(0, 6)=="/stats"){
            show_stats();
        }else if(data.substr(0, 9)=="/shutdown"){
            cmd_shutdown();
        }else{
            
        };
    };

    void Server::handle_tcp_data(int client_fd, const char* data, size_t len) {
        
        std::string s_data = data;
        //std::cout << data << " \n";
        if (determine_cmd(s_data))
        {
            handle_dt(s_data, len);
        }else{
            send(client_fd, data, len, 0);
        };
    };

    void Server::handle_udp_data(int socket_fd,  const char* data, size_t len, const struct sockaddr_in* addr) {
        std::string s_data = data;
        //std::cout << data << " \n";
        if (determine_cmd(s_data))
        {
            handle_dt(s_data, len);
        }else{
            sendto(socket_fd, data, len, 0, (struct sockaddr*)addr, sizeof(*addr));
        };
    };

    int Server::create_socket_tcp(bool is_udp, int port) {
        is_udp = false;
        int type = is_udp ? SOCK_DGRAM : SOCK_STREAM;
        int sock = socket(AF_INET, type, 0);

        if (sock < 0){perror("Error create socket!"); return -1;};
        
        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ipaddress);
        //addr.sin_addr.s_addr = INADDR_ANY;
        
        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("Error bind!");
            close(sock);
            return -1;
        };
        
        // if (!is_udp){
            int resL = listen(sock, 10);
            if (resL==-1)
            {
                std::cout << "ERROR listen TCP. \n";
            };
        // };

        std::cout << "TCP port ctreated. \n";
        
        return sock;
    };


// static int make_socket_non_blocking(int sockFd)
// {
//   int getFlag, setFlag; 
//   getFlag = fcntl(sockFd, F_GETFL, 0); 
//   if(getFlag == -1)  {    perror("fnctl");    return -1;  }; 
//   /* Set the Flag as Non Blocking Socket */
//   getFlag |= O_NONBLOCK; 
//   setFlag = fcntl(sockFd, F_GETFL, getFlag); 
//   if(setFlag == -1)  {    perror("fnctl");    return -1;  };
//   return 0;
//};

    int Server::create_socket_udp(bool is_udp, int port) {
        // int i, length, receivelen;
        // /* Socket Parameters */
        // int sockFd;
        // int optval = 1;   // Socket Option Always = 1
        // /* Server Address */
        // struct sockaddr_in serverAddr, receivesocket;
        // /* Epoll File Descriptor */
        // int epollFd;      
        // /* EPOLL Event structures */
        // struct epoll_event  ev;                  
        // struct epoll_event  events[1024];               
        // int numEvents;                        
        // int ctlFd; 
        // // Step 1: First Create UDP Socket         
        // /* Create UDP socket
        // * socket(protocol_family, sock_type, IPPROTO_UDP);
        // */
        // sockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        // /* Check socket is successful or not */
        // if (sockFd == -1)
        // {
        //     perror(" Create SockFd Fail \n");
        //     return -1;
        // };
        // // Step 2: Make Socket as Non Blocking Socket.
        // //         To handle multiple clients Asychronously, required to
        // //         configure socket as Non Blocking socket
        // /* Make Socket as Non Blocking Socket */
        // make_socket_non_blocking(sockFd);
        // // Step 3: Set socket options
        // //    One can set different sock Options as RE-USE ADDR, 
        // //    BROADCAST etc.        
        // /*  In this Program, the socket is set to RE-USE ADDR
        // *  So this gives flexibilty to other sockets to BIND to the 
        //     same port Num */
        // if(setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))== -1)
        // {
        //     perror("setsockopt Fail\n");
        //     return -1;
        // };
        // // Step 4: Bind to the Recieve socket
        // /* Bind to its own port Num  ( Listen on Port Number) */        
        // /* Setup the addresses */        
        // /* my address or Parameters
        //     ( These are required for Binding the Port and IP Address )
        //     Bind to my own port and Address */
        // memset(&receivesocket, 0, sizeof(receivesocket));
        // receivesocket.sin_family = AF_INET;
        // receivesocket.sin_addr.s_addr = htonl(INADDR_ANY);
        // receivesocket.sin_port = htons(port);
        // receivelen = sizeof(receivesocket);
        // /* Bind the my Socket */
        // if (bind(sockFd, (struct sockaddr *) &receivesocket, receivelen) < 0)
        // {
        //     perror("bind");
        //     return -1;
        // };
        // // EPOLL Implementation Starts
        // // Step 5: Create Epoll Instance
        // /* paramater is Optional */        
        // epollFd = epoll_create(6);
        // if(epollFd == -1)
        // {
        //     perror("epoll_create");
        //     return -1;
        // };
        // /* Add the udp Sock File Descriptor to Epoll Instance */
        // ev.data.fd = sockFd;        
        // /* Events are Read Only and Edge-Triggered */
        // ev.events = EPOLLIN | EPOLLET;        
        // // Step 6: control interface for an epoll descriptor
        // /* EPOLL_CTL_ADD
        //     Register the target file descriptor fd on the epoll instance
        //     referred to by the file descriptor epfd and
        //     associate the event event with the internal file linked to fd.
        // */
        // /* Add the sock Fd to the EPOLL */
        // ctlFd  = epoll_ctl(epollFd, EPOLL_CTL_ADD, sockFd, &ev);        
        // if (ctlFd == -1)
        // {
        //     perror ("epoll_ctl");
        //     return -1;
        // };

       

        // int server_fd;
        // struct sockaddr_in server_addr;
        // // Создание сокета
        // if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
        // {
        //     perror("socket creation failed");
        //     exit(EXIT_FAILURE);
        // };
        // memset(&server_addr, 0, sizeof(server_addr));
        // // Настройка адреса сервера
        // server_addr.sin_family = AF_INET;
        // server_addr.sin_addr.s_addr = inet_addr(ipaddress);
        // server_addr.sin_port = htons(port);
        // // Привязка сокета к адресу и порту
        // if (bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
        // {
        //     perror("bind failed");
        //     close(server_fd);
        //     exit(EXIT_FAILURE);
        // };

        // //int server_fd;
        // struct sockaddr_in server_addr;
        // // Создание сокета
        // int sock = socket(AF_INET, SOCK_DGRAM, 0);
        // if ((sock) < 0) {
        //     perror("socket creation failed");
        //     exit(EXIT_FAILURE);
        // };
        // memset(&server_addr, 0, sizeof(server_addr));
        // // Настройка адреса сервера
        // server_addr.sin_family = AF_INET;
        // server_addr.sin_addr.s_addr = inet_addr(ipaddress);
        // server_addr.sin_port = htons(port);
        // // Привязка сокета к адресу и порту
        // if (bind(sock, (const struct sockaddr *)&server_addr, 
        //     sizeof(server_addr)) < 0) {
        //     perror("bind failed");
        //     close(sock);
        //     exit(EXIT_FAILURE);
        // };


            // udp_state.epoll_fd = epoll_create1(0);  
            // // Настройка сокета на неблокирующий режим  
            // int flags = fcntl(udp_state.listen_fd, F_GETFL, 0);  
            // fcntl(udp_state.listen_fd, F_SETFL, flags | O_NONBLOCK);  
            // // Добавление сокета UDP в epoll  
            // struct epoll_event ev, events[1024];  
            // ev.events = EPOLLIN;  
            // ev.data.fd = udp_state.listen_fd;  
            // if (epoll_ctl(udp_state.epoll_fd, EPOLL_CTL_ADD, udp_state.listen_fd, &ev) == -1) {  
            //     perror("epoll_ctl");  
            //     exit(EXIT_FAILURE);  
            // };
            
        int type = SOCK_DGRAM;
        int sock = socket(AF_INET, type, 0);

        if (sock < 0){perror("Error create socket!"); return -1;};
        
        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ipaddress);
        //addr.sin_addr.s_addr = INADDR_ANY;
        
        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("Error bind!");
            close(sock);
            return -1;
        };
        
        std::cout << "UDP port ctreated. \n";   
        // // if (!is_udp){
            int resL = listen(sock, 10);
            if (resL==-1)
            {
                std::cout << "ERROR listen UDP. \n";
            };
            //listen(server_fd, 10);
        // // };    

    
        //std::cout << sock << " \n";
        
        //return server_fd;
        //return 0;
        return sock;
    };

    int Server::prepareServer(const char* hostadress, int p_tcp, int p_udp)
    {
        tcp_state.epoll_fd = epoll_create1(0);
        udp_state.epoll_fd = epoll_create1(0);
        
        if(tcp_state.epoll_fd == -1){perror("ERROR epoll create TCP!"); return 1;};    
        if(udp_state.epoll_fd == -1){perror("ERROR epoll create UDP!"); return 1;};

        ipaddress=hostadress;
        port_tcp=p_tcp;
        port_udp=p_udp;

        tcp_state.listen_fd = create_socket_tcp(false, port_tcp);
        udp_state.listen_fd = create_socket_udp(true, port_udp);
        
        if(tcp_state.listen_fd == -1){return 1;};        
        if(udp_state.listen_fd == -1){return 1;}

        epoll_event event_tcp;
        event_tcp.data.fd = tcp_state.listen_fd;
        event_tcp.events = EPOLLIN | EPOLLET;

        epoll_event event_udp;        
        event_udp.data.fd = udp_state.listen_fd;
        event_udp.events = EPOLLIN;
        //event_udp.events = EPOLLIN | EPOLLET;
        
        if (epoll_ctl(tcp_state.epoll_fd, EPOLL_CTL_ADD, tcp_state.listen_fd, &event_tcp) == -1) 
        {perror("ERROR add listen_fd, epoll_ctl TCP! "); return 1;};        

        if (epoll_ctl(udp_state.epoll_fd, EPOLL_CTL_ADD, udp_state.listen_fd, &event_udp) == -1) 
        {perror("ERROR add listen_fd, epoll_ctl UDP! "); return 1;};

        return 0;
    };

    void Server::run_tcp()
    {
        while (status) 
        {
            int nfds_tcp = epoll_wait(tcp_state.epoll_fd, listen_events_tcp, 1024, -1); 
            //std::cout << nfds_tcp << " tcp's... \n";
              
            if (nfds_tcp<1){continue;};

            //std::cout << "revision of the list values... \n";
            
            for (int i = 0; i < nfds_tcp; ++i)
            {
                int fd_tcp = listen_events_tcp[i].data.fd;                
                if (fd_tcp == tcp_state.listen_fd)
                {
                    struct sockaddr_in client_addr;
                    socklen_t addrlen = sizeof(client_addr);
                    int client_fd = accept(tcp_state.listen_fd, (struct sockaddr*)&client_addr, &addrlen);
                    
                    if (client_fd < 1) { continue;};

                        fcntl(client_fd, F_SETFL, O_NONBLOCK);
                        epoll_event client_event;
                        client_event.data.fd = client_fd;
                        client_event.events = EPOLLIN | EPOLLET;
                        epoll_ctl(tcp_state.epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
                    //};
                } else {
                    // Входящие данные
                    char buffer[1024];
                    ssize_t bytes_read;                    
                    bytes_read = recv(fd_tcp, buffer, sizeof(buffer) - 1, 0);      
                    
                    if (bytes_read == 0) {
                        // Клиент отключился
                        close(fd_tcp);
                        epoll_ctl(tcp_state.epoll_fd, EPOLL_CTL_DEL, fd_tcp, nullptr);
                        continue;
                    };
                    
                    buffer[bytes_read] = '\0';
                    handle_tcp_data(fd_tcp, buffer, bytes_read);                    
                    
                };
            };            
        };   

        std::cout << "TCP DOWN. \n";
        //return true; 
    };

    void Server::run_udp()
    {
        while (status) 
        {
            int nfds_udp = epoll_wait(udp_state.epoll_fd, listen_events_udp, 1024, -1);    
            //std::cout << nfds_udp << " udp's... \n";
 
            if (nfds_udp<1){continue;};
            
            //std::cout << "revision of the list values... \n";
            for (int i = 0; i < nfds_udp; ++i) {
                int fd_udp = listen_events_udp[i].data.fd;                
                if (fd_udp != udp_state.listen_fd) {
                    // Входящие данные
                    char buffer[1024];
                    ssize_t bytes_read;
                    
                    struct sockaddr_in client_addr;
                    socklen_t addrlen = sizeof(client_addr);
                    bytes_read = recvfrom(fd_udp, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_addr, &addrlen);                            
                    if (bytes_read > 0) {
                        buffer[bytes_read] = '\0';
                        handle_udp_data(fd_udp, buffer, bytes_read, &client_addr);
                    };
                };
            };
        };   

        std::cout << "UDP DOWN. \n"; 
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

        std::cout << "SERVER is RUN! \n";
        // std::cout << tcp_state.epoll_fd << ", " << tcp_state.listen_fd << "; \n";
        // std::cout << udp_state.epoll_fd << ", " << udp_state.listen_fd << ". \n";

        if (tcp_thread.joinable()){tcp_thread.join(); }else{return 1;};
        if (udp_thread.joinable()){udp_thread.join(); }else{return 1;};
        std::cout << "SERVER is DOWN! \n";

        close(tcp_state.epoll_fd);
        close(udp_state.epoll_fd);

        return 0;
    };

//};