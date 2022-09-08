#include "../includes/webserv.hpp"
#include "server/Webserv.hpp"

// TODO delete
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <errno.h>
#include <sstream>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>

int main() {
    Webserv serv;
    serv.init();
    serv.loop();
    serv.finalize();
    
    return 0;
}
