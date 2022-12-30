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
#ifdef DEBUG
        std::cout << "Invalid arguments" << std::endl;
#endif
        exit(EXIT_FAILURE);
        break;
    }
    Webserv serv;
    serv.init();
    serv.loop();
    serv.finalize();

    return 0;
}
