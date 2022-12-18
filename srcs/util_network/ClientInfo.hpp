#ifndef SRCS_UTIL_NETWORK_CLIENT_INFO_HPP_
#define SRCS_UTIL_NETWORK_CLIENT_INFO_HPP_

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

struct ClientInfo {
  struct sockaddr_in address;
  socklen_t       addrlen;
  int             fd;
};

#endif  // SRCS_UTIL_NETWORK_CLIENT_INFO_HPP_
