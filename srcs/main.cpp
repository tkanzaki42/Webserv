
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

#include "../includes/webserv.hpp"
#include "server/Webserv.hpp"

int main() {
    Webserv serv;
    serv.loop();

    return 0;
}
