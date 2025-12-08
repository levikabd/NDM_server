
#include <iostream>
#include "server.h"

int main(int argc, char* argv[]) {
    // if (argc >1) {
    // //     std::cout << "Not enough parameters!" << std::endl;
    //     int mode = atoi(argv[1]);
    // //     return 1;
    // };
    
    // if (mode==2)    // {    //};
    //     std::cout << "Invalid parameter: " << mode << std::endl;
    // if (argc>=3){        //     num2 = std::string(argv[2]);        // };
   
    // try {    //     //if (mode==1){        };
    // } catch (const std::exception& e) {
    //     std::cerr << "Ошибка: " << e.what() << std::endl;
    //     return 1;
    // };

    Server server;

    int ret = server.prepareServer("127.0.0.1", 8887, 8888);
    if (ret==1){ std::cerr << "NDM_server is DOWN. \n"; return 1;};
    
    server.run_ndm_server();

    std::cout << "NDM_server is DOWN. \n";

    return 0;
};
