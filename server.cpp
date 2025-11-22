
#include <sys/epoll.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <fcntl.h>

#include <cstdio>

// Структура для хранения состояния сервера
struct ServerState {
    int epoll_fd;
    int listen_fd;
    bool is_udp;
};

// Обработчик входящих данных для TCP
void handle_tcp_data(int client_fd, const char* data, size_t len) {
    // Пример: эхо-сервер
    send(client_fd, data, len, 0);
};

// Обработчик входящих данных для UDP
void handle_udp_data(int socket_fd,  const char* data, size_t len, const struct sockaddr_in* addr) {
    // Пример: эхо-сервер
    sendto(socket_fd, data, len, 0, (struct sockaddr*)addr, sizeof(*addr));
};

// Создание сокета
int create_socket(bool is_udp, int port) {
    int type = is_udp ? SOCK_DGRAM : SOCK_STREAM;
    int sock = socket(AF_INET, type, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    };
    
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        return -1;
    }
    
    if (!is_udp) {
        listen(sock, 10);
    }
    
    return sock;
};

// Основная функция обработки событий
void run_server(ServerState& state) {
    epoll_event events[1024];
    
    while (true) {
        int nfds = epoll_wait(state.epoll_fd, events, 1024, -1);
        
        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;
            
            if (fd == state.listen_fd) {
                // Новое подключение
                if (state.is_udp) {
                    // Для UDP ничего делать не нужно
                } else {
                    struct sockaddr_in client_addr;
                    socklen_t addrlen = sizeof(client_addr);
                    int client_fd = accept(state.listen_fd, (struct sockaddr*)&client_addr, &addrlen);
                    
                    if (client_fd > 0) {
                        fcntl(client_fd, F_SETFL, O_NONBLOCK);
                        epoll_event client_event;
                        client_event.data.fd = client_fd;
                        client_event.events = EPOLLIN | EPOLLET;
                        epoll_ctl(state.epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
                    };
                };
            } else {
                // Входящие данные
                char buffer[1024];
                ssize_t bytes_read;
                
                if (state.is_udp) {
                    struct sockaddr_in client_addr;
                                            socklen_t addrlen = sizeof(client_addr);
                        bytes_read = recvfrom(fd, buffer, sizeof(buffer) - 1, 0, 
                            (struct sockaddr*)&client_addr, &addrlen);
                        
                        if (bytes_read > 0) {
                            buffer[bytes_read] = '\0';
                            handle_udp_data(fd, buffer, bytes_read, &client_addr);
                        }
                } else {
                        // Для TCP
                        char buffer[1024];
                        bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);
                        
                        if (bytes_read > 0) {
                            buffer[bytes_read] = '\0';
                            handle_tcp_data(fd, buffer, bytes_read);
                        } else if (bytes_read == 0) {
                            // Клиент отключился
                            close(fd);
                            epoll_ctl(state.epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
                        };
                    
                };
                
            };
        };
    };

};

int prepareServer(ServerState* tcp_state, ServerState* udp_state)
{

    // Инициализация epoll
    tcp_state->epoll_fd = epoll_create1(0);
    udp_state->epoll_fd = epoll_create1(0);
    
    if(tcp_state->epoll_fd == -1){perror("epoll_create"); return 1;};    
    if(udp_state->epoll_fd == -1){perror("epoll_create"); return 1;}

    // Создание сокетов
    tcp_state->listen_fd = create_socket(false, 8888);
    udp_state->listen_fd = create_socket(true, 8889);
    
    if(tcp_state->listen_fd == -1){return 1;};        
    if(udp_state->listen_fd == -1){return 1;}

    // Настройка событий epoll
    epoll_event listen_event;
    listen_event.data.fd = tcp_state->listen_fd;
    listen_event.events = EPOLLIN | EPOLLET;
    
    if (epoll_ctl(tcp_state->epoll_fd, EPOLL_CTL_ADD, tcp_state->listen_fd, &listen_event) == -1) {
        perror("epoll_ctl");
        return 1;
    };

    listen_event.data.fd = udp_state->listen_fd;
    if (epoll_ctl(udp_state->epoll_fd, EPOLL_CTL_ADD, udp_state->listen_fd, &listen_event) == -1) {
        perror("epoll_ctl");
        return 1;
    }

    tcp_state->is_udp = false;
    udp_state->is_udp = true;

    return 0;
};
