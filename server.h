#include <sys/epoll.h>

#include <cstddef>
#include <string>

struct ServerState 
{
    int epoll_fd=0;
    int listen_fd=0;
    int is_up=0;
};

class Server
{
private:
    ServerState tcp_state;
    ServerState udp_state;
   
    bool status=false;
    std::string cmd="";
    int port_tcp = 0, port_udp = 0;
    const char *ipaddress = "";

    epoll_event listen_events_tcp[1024];
    epoll_event listen_events_udp[1024];
    int count=0;
public:
    // Server(/* args */);
    // ~Server();
    void show_time();

    void show_stats();

    void cmd_shutdown();

    bool determine_cmd(std::string data);

    void handle_dt(std::string data, size_t len);

    void handle_tcp_data(int client_fd, const char* data, size_t len);
    //void handle_tcp_data();

    void handle_udp_data(int socket_fd, const char* data, size_t len, const struct sockaddr_in* addr);
    //void handle_udp_data();

    int create_socket_tcp(bool is_udp, int port);
    int create_socket_udp(bool is_udp, int port);
    //int create_socket();

    //int prepareServer(ServerState* tcp_state, ServerState* udp_state);
    int prepareServer(const char* hostadress, int p_tcp, int p_udp);

    void run_tcp();
    void run_udp();

    //void run_ndm_server(ServerState* tcp_state, ServerState* udp_state);
    int run_ndm_server();

};



