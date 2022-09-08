#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "../util_network/Socket.hpp"
#include "HttpResponse.hpp"

#include <iostream>
#include <vector>

const int HTTP_PORT = 5000;

int run_webserver();

#endif
