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
#include <fstream>

#include <ctime>
#include <csignal>

#include "server.h"

bool status=false;
std::string cmd="";

void signalHandler(int signum) {
    if (signum == SIGTERM || signum == SIGINT) 
    {        
        std::cout << "TERM signal! \n";
        cmd="/shutdown";        
        status = false;                
    };
};

bool createPidFile(const std::string& filename) {
    pid_t pid = getpid();
    
    if (access(filename.c_str(), F_OK) == 0) {
        std::cerr << "The PID file already exists: " << filename << std::endl;
        return false;
    };
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error when creating the file: " << filename << std::endl;
        return false;
    };
    
    file << pid;
    if (file.fail()) {
        std::cerr << "Error when writing to a file: " << filename << std::endl;
        return false;
    };
    
    file.close();
    return true;
};

bool removePidFile(const std::string& filename) {
    if (access(filename.c_str(), F_OK) == -1) {
        std::cerr << "The PID file does not exist: " << filename << std::endl;
        return false;
    };

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening the PID file: " << filename << std::endl;
        return false;
    };

    if (remove(filename.c_str()) == 0) {
        std::cout << "The PID file has been successfully deleted: " << filename << std::endl;
        return true;
    } else {
        std::cerr << "Error deleting the PID file: " << errno << std::endl;
        return false;
    };
};

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
    // int conn_tcp = 0;
    // int conn_udp = 0;
    // void (*funcptr)(int);
// // public:
// //     // Server(/* args */);
// //     // ~Server();
    void Server::show_time()
    {
        cmd="/time";
        //std::cout << "cmd /time \n";
        time_t rawtime;
        struct tm * timeinfo;
        char buffer [80];                              
        
        time ( &rawtime );                            
        timeinfo = localtime ( &rawtime );       

        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);  
        std::cout << "Current Date and Time: " << buffer << std::endl;
    };

    void Server::show_stats()
    {
        cmd="/stats";
        //std::cout << "cmd /stats \n";

        std::cout << "Total number of connections: " << count << " \n";
        std::cout << "Number of active connections: " << (conn_tcp+conn_udp) << "\n";
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
            std::cout << "cmd! ... \n";
            return true;
        };
        return false;
    };

    void Server::handle_dt(std::string data, size_t len)
    {
        int sz = data.size();
        //send(client_fd, data, len, 0);  
        if (sz>=5){
            if (data.substr(0, 5)=="/time")
            {
                show_time();
                return;
            };
        };
               
        if (sz>=6){
            if(data.substr(0, 6)=="/stats"){
                show_stats();
                return;
            };
        };

        if (sz>=9){
            if(data.substr(0, 9)=="/shutdown"){
                cmd_shutdown();
                return;
            };
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

        if (sock < 0){perror("TCP error create socket!"); return -1;};
        
        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ipaddress);
        //addr.sin_addr.s_addr = INADDR_ANY;
        
        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("Error bind TCP!");
            close(sock);
            return -1;
        };

        std::cout << "TCP port ctreated. \n";
        int resL = listen(sock, 10);
        if (resL==-1)
        {
            std::cerr << "ERROR listen TCP. \n";
        }else{
            tcp_state.is_up=1;                
        };
        
        return sock;
    };

    int Server::create_socket_udp(bool is_udp, int port) {
        is_udp = true;            
        int type = SOCK_DGRAM;
        int sock = socket(AF_INET, type, 0);

        if (sock < 0){perror("UDP error create socket!"); return -1;};
        
        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ipaddress);
        
        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("UDP error bind!");
            close(sock);
            return -1;
        };
        
        int flags = fcntl(udp_state.listen_fd, F_GETFL, 0);
        fcntl(udp_state.listen_fd, F_SETFL, flags | O_NONBLOCK);

        std::cout << "UDP port ctreated. \n";   
        // int resL = listen(sock, 10);
        // if (resL==-1)
        // {
        //     std::cerr << "ERROR listen UDP. \n";
        // }else{
            udp_state.is_up=1;
        // };

        return sock;
    };

    int Server::prepareServer(const char* hostadress, int p_tcp, int p_udp)
    {
        ipaddress=hostadress;

        //TCP
        tcp_state.epoll_fd = epoll_create1(0);
        if(tcp_state.epoll_fd == -1){perror("ERROR epoll create TCP!"); return 1;}; 
        port_tcp=p_tcp;
        tcp_state.listen_fd = create_socket_tcp(false, port_tcp);
        if(tcp_state.listen_fd == -1){return 1;};  
        epoll_event event_tcp;
        event_tcp.data.fd = tcp_state.listen_fd;
        event_tcp.events = EPOLLIN | EPOLLET;
        if (epoll_ctl(tcp_state.epoll_fd, EPOLL_CTL_ADD, tcp_state.listen_fd, &event_tcp) == -1) 
        {
            perror("ERROR add listen_fd, epoll_ctl TCP! "); 
            close(tcp_state.epoll_fd);
            close(tcp_state.listen_fd);            
            return 1;
        }; 
        std::cout << "TCP prepare OK.\n";

        udp_state.epoll_fd = epoll_create1(0);           
        if(udp_state.epoll_fd == -1){perror("ERROR epoll create UDP!"); return 1;};
        port_udp=p_udp;
        udp_state.listen_fd = create_socket_udp(true, port_udp);
        if(udp_state.listen_fd == -1){            std::cerr << "UDP create ERROR!\n";            return 1;        };
        epoll_event event_udp;        
        event_udp.data.fd = udp_state.listen_fd;
        event_udp.events = EPOLLIN;        
        if (epoll_ctl(udp_state.epoll_fd, EPOLL_CTL_ADD, udp_state.listen_fd, &event_udp) == -1) 
        {
            perror("ERROR add listen_fd, epoll_ctl UDP! "); 
            close(udp_state.epoll_fd);
            close(udp_state.listen_fd);            
            return 1;
        };         
        std::cout << "UDP prepare OK.\n";
        
        return 0;
    };

    void Server::run_tcp()
    {
        if (tcp_state.is_up!=1)
        {
            std::cerr << "TCP DOWN. \n";
            conn_tcp=0;
            return;
        };
        
        while (status) 
        {
            int nfds_tcp = epoll_wait(tcp_state.epoll_fd, listen_events_tcp, 1024, 900); 
            conn_tcp=nfds_tcp;
            //std::cout << nfds_tcp << " tcp's... \n";
              
            if (nfds_tcp<1){
                // if (nfds_tcp==0)
                // {
                //     //std::cerr << "TCP epoll_wait() return -1! \n"; 
                //     continue;
                // };
                if (nfds_tcp==-1)
                {
                    if (errno!=EINTR)
                    {
                        std::cerr << "TCP epoll_wait() return -1! \n"; 
                    };
                };   
                continue;             
            };

            for (int i = 0; i < nfds_tcp; i++)
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
                } else {
                    char buffer[1024];
                    ssize_t bytes_read;                    
                    bytes_read = recv(fd_tcp, buffer, sizeof(buffer) - 1, 0);      
                    
                    if (bytes_read == 0) {
                        close(fd_tcp);
                        epoll_ctl(tcp_state.epoll_fd, EPOLL_CTL_DEL, fd_tcp, nullptr);
                        continue;
                    };
                    
                    buffer[bytes_read] = '\0';
                    handle_tcp_data(fd_tcp, buffer, bytes_read);        
                };
            };  
            
            funcptr = signal(SIGTERM, signalHandler);
        };   

        std::cout << "TCP DOWN. \n";
    };

    void Server::run_udp()
    {
        if (udp_state.is_up!=1)
        {
            std::cerr << "UDP DOWN. \n";
            conn_udp=0;
            return;
        };
        
        while (status) 
        {
            int nfds_udp = epoll_wait(udp_state.epoll_fd, listen_events_udp, 1024, 900);  
            conn_udp=nfds_udp;  
            //std::cout << nfds_udp << " udp's... \n";
 
            if (nfds_udp<1){
                if (nfds_udp==-1)
                {
                    if (errno!=EINTR)
                    {
                        std::cerr << "TCP epoll_wait() return -1! \n"; 
                    };
                };   
                continue;
            };

            for (int i = 0; i < nfds_udp; i++) {
                int fd_udp = listen_events_udp[i].data.fd;                
                if (fd_udp == udp_state.listen_fd) {
                    count++;
                    char buffer[1024];                   
                    struct sockaddr_in client_addr;
                    socklen_t addrlen = sizeof(client_addr);
                    
                    ssize_t bytes_read = recvfrom(fd_udp, buffer, 1024, 0, (struct sockaddr*)&client_addr, &addrlen);                            
                    //ssize_t bytes_read = recvfrom(fd_udp, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_addr, &addrlen);                            
                    
                    if (bytes_read < 1){ std::cerr << "UDP read<1!"; continue;};
                    //std::cout << "UDP read>0!";
					//buffer[bytes_read] = '\0';
                    handle_udp_data(fd_udp, buffer, bytes_read, &client_addr);                    
                };
            };
            funcptr = signal(SIGTERM, signalHandler);
        };   

        std::cout << "UDP DOWN. \n"; 
    };

    int Server::run_ndm_server() 
    {
        std::string pidFilename = "/run/ndm-server.pid";        
        if (createPidFile(pidFilename)) {
            std::cout << "The PID file has been created successfully:" << pidFilename << std::endl;
        } else {
            std::cerr << "Failed to create a PID file! " << std::endl;
            return 1;
        };     
        
        signal(SIGTERM, signalHandler);
        signal(SIGINT, signalHandler);
        signal(SIGHUP, SIG_IGN);
        signal(SIGCHLD, SIG_IGN);

        std::thread tcp_thread(&Server::run_tcp,this);
        std::thread udp_thread(&Server::run_udp,this);
        status=true;

        std::cout << "SERVER is RUN! IP: " << ipaddress << ", TCP: " << port_tcp << ", UDP: " << port_udp << " \n";


        if (tcp_thread.joinable()){tcp_thread.join(); }else{return 1;};
        if (udp_thread.joinable()){udp_thread.join(); }else{return 1;};

        close(tcp_state.epoll_fd);
        close(udp_state.epoll_fd);

        removePidFile(pidFilename);

        return 0;
    };

//};
