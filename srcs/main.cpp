#include "includes/webserv.hpp"
#include "server/Webserv.hpp"
#include "config/Config.hpp"

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        std::cout << "Invalid arguments" << std::endl;
        exit(EXIT_FAILURE);
    }
    Config::init(argv[1]);
    // Webserv serv;
    // serv.init();
    // serv.loop();
    // serv.finalize();

    return 0;
}
