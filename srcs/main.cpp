
// TODO(yonishi) delete
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>

#include "includes/webserv.hpp"
#include "server/Webserv.hpp"
#include "config/Config.hpp"

int main(int argc, char const *argv[]) {
    if (argc > 2) {
        exit(EXIT_FAILURE);
    }
    try {
        Config::parseConfig(argv[1]);
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        exit(EXIT_FAILURE);
    }
    // Webserv serv;
    // serv.init();
    // serv.loop();
    // serv.finalize();

    return 0;
}
