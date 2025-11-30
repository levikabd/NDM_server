
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

#include <ctime>

#include "server.h"

// struct ServerState {
    // int epoll_fd=0;
    // int listen_fd=0;
    // int is_up=0;
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
//     int count=0;
// // public:
// //     // Server(/* args */);
// //     // ~Server();
    void Server::show_time()
    {
        cmd="/time";
        //std::cout << "cmd /time \n";

        // time_t t = time(0);  
        // tm* now = localtime(&t);  
        // char* buffer;  

        time_t rawtime;
        struct tm * timeinfo;
        char buffer [80];                                // строка, в которой будет храниться текущее время
        
        time ( &rawtime );                               // текущая дата в секундах
        timeinfo = localtime ( &rawtime );               // текущее локальное время, представленное в структуре
        
        // strftime (buffer,80,"Сейчас %I:%M%p.",timeinfo); // форматируем строку времени
        // std::cout << buffer << std::endl;

        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);  
        //strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now);  
        std::cout << "Current Date and Time: " << buffer << std::endl;
    };

    void Server::show_stats()
    {
        cmd="/stats";
        //std::cout << "cmd /stats \n";
        std::cout << "Number of connections: " << count << " \n";
    };

    void Server::cmd_shutdown()
    {
        cmd="/shutdown";
        status = false;
        std::cout << "cmd /shutdown \n";
    };    

    bool Server::determine_cmd(std::string data)
    {
        
        std::cout << data << " \n";
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

        std::cout << "TCP port ctreated. \n";
        // if (!is_udp){
            int resL = listen(sock, 10);
            if (resL==-1)
            {
                std::cout << "ERROR listen TCP. \n";
            }else{
                tcp_state.is_up=1;                
            };
        // };        
        
        return sock;
    };

    int Server::create_socket_udp(bool is_udp, int port) {
            
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
            }else{
                udp_state.is_up=1;
            };
        // // };    

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
        if (tcp_state.is_up!=1)
        {
            std::cout << "TCP DOWN. \n";
            return;
        };
        
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
                        count++;
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
        if (udp_state.is_up!=1)
        {
            std::cout << "UDP DOWN. \n";
            return;
        };
        
        while (status) 
        {
            int nfds_udp = epoll_wait(udp_state.epoll_fd, listen_events_udp, 1024, -1);    
            //std::cout << nfds_udp << " udp's... \n";
 
            if (nfds_udp<1){continue;};
            
            //std::cout << "revision of the list values... \n";
            for (int i = 0; i < nfds_udp; ++i) {
                int fd_udp = listen_events_udp[i].data.fd;                
                if (fd_udp != udp_state.listen_fd) {
                    count++;
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

        close(tcp_state.epoll_fd);
        close(udp_state.epoll_fd);

        return 0;
    };

//};