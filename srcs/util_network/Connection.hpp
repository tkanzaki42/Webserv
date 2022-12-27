#ifndef SRCS_UTIL_NETWORK_CONNECTION_HPP_
#define SRCS_UTIL_NETWORK_CONNECTION_HPP_

#include <iostream>

class Connection
{
private:
    int    accepted_fd_;
    time_t last_time_;
public:
    Connection();
    Connection(const Connection &obj);
    Connection &operator=(const Connection &obj);
    ~Connection();

    int    get_accepted_fd() const;
    void   set_accepted_fd(int accepted_fd);
    time_t get_last_time() const;
    void   set_last_time(time_t last_time);
};

#endif  // SRCS_UTIL_NETWORK_CONNECTION_HPP_
