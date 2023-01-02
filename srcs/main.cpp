#include "includes/webserv.hpp"
#include "server/Webserv.hpp"
#include "config/Config.hpp"

int main(int argc, char const *argv[]) {
    switch (argc) {
    case 1:
        Config::init("configs/default.conf");
        break;
    case 2:
        Config::init(argv[1]);
        break;
    default:
        std::cerr << "Invalid arguments" << std::endl;
        exit(EXIT_FAILURE);
        break;
    }
    puts("Config OK");
    Webserv serv;
    serv.init();
    serv.loop();
    serv.finalize();

    return 0;
}
