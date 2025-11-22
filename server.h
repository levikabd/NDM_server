#include <cstddef>

struct ServerState {
    int epoll_fd;
    int listen_fd;
    bool is_udp;
};

// Обработчик входящих данных для TCP
void handle_tcp_data(int client_fd, const char* data, size_t len);

// Обработчик входящих данных для UDP
void handle_udp_data(int socket_fd, const char* data, size_t len, const struct sockaddr_in* addr);

// Создание сокета
int create_socket(bool is_udp, int port);

// Основная функция обработки событий
void run_server(ServerState& state);

int prepareServer(ServerState* tcp_state, ServerState* udp_state);