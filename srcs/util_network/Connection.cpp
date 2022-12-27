#include "Connection.hpp"

Connection::Connection():
accepted_fd_(-1), last_time_(0){
}

Connection::Connection(const Connection &obj) {
    *this = obj;
}

Connection &Connection::operator=(const Connection &obj) {
    this->accepted_fd_ = obj.accepted_fd_;
    this->last_time_   = obj.last_time_;
    return *this;
}

Connection::~Connection(){
}

int    Connection::get_accepted_fd() const {
    return accepted_fd_;
}

void   Connection::set_accepted_fd(int accepted_fd) {
    accepted_fd_ = accepted_fd;
}

time_t Connection::get_last_time() const {
    return last_time_;
}

void   Connection::set_last_time(time_t last_time) {
    last_time_ = last_time;
}
