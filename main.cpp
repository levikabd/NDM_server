
#include <string>
#include <iostream>
#include <thread>

#include "server.h"

int main(int argc, char* argv[]) {
    // if (argc < 2) {
    //     std::cout << "Not enough parameters!" << std::endl;
    //     return 1;
    // };
    
    int mode = atoi(argv[1]);   
    // std::string num2 = "";

    // if (mode==2)
    // {
    //}
    //     std::cout << "Invalid parameter: " << mode << std::endl;
         // if (argc>=3){
        //     num2 = std::string(argv[2]);
        // };
   
    // log out
    //std::cout << "Mode: " << mode << std::endl;

    // try {
    //     //if (mode==1){        };
    // } catch (const std::exception& e) {
    //     std::cerr << "Ошибка: " << e.what() << std::endl;
    //     return 1;
    // };

    ServerState tcp_state;
    ServerState udp_state;
    
    int ret = prepareServer(&tcp_state, &udp_state);
    if (ret==1){return 1;};
    
    std::thread tcp_thread(&run_server, std::ref(tcp_state));
    std::thread udp_thread(&run_server, std::ref(udp_state));
    if (tcp_thread.joinable()){tcp_thread.join(); };
    if (udp_thread.joinable()){udp_thread.join(); };

    return 0;
};
