#include "Connection.hpp"

Connection::Connection():
accepted_fd_(-1), last_time_(0), response_(&request_) {
    pp_recv_[0] = -1;
    pp_recv_[1] = -1;
}

Connection::Connection(const Connection &obj) : response_(&request_) {
    *this = obj;
}

Connection &Connection::operator=(const Connection &obj) {
    this->accepted_fd_ = obj.accepted_fd_;
    this->last_time_   = obj.last_time_;
    this->port_        = obj.port_;
    this->pp_recv_[0] = -1;
    this->pp_recv_[1] = -1;
    return *this;
}

Connection::~Connection() {
    if (pp_recv_[0] != -1) {
        close(pp_recv_[0]);
        std::cout << "pipe closed: " << pp_recv_[0] << std::endl;
        close(pp_recv_[1]);
        std::cout << "pipe closed: " << pp_recv_[1] << std::endl;
    }
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

void   Connection::set_client_addr(struct sockaddr_in client_addr) {
    request_.set_client_addr(client_addr);
}

int Connection::get_write_pipe() {
    std::cout << "pipe status pp_recv_[0]:" << pp_recv_[0] << std::endl;
    // パイプを開く
    if (pp_recv_[0] == -1) {
        if (pipe(pp_recv_) == -1) {
            std::cerr << "Failed to pipe() in Connection()" << std::endl;
        }
#ifdef DEBUG
        std::cout << "  pp_recv_[0] = " << pp_recv_[0] << std::endl;
        std::cout << "  pp_recv_[1] = " << pp_recv_[1] << std::endl;
#endif
    }

    return pp_recv_[1];
}

int Connection::get_status_code() {
    return request_.get_status_code();
}

// 戻り値 true : 問題なく読み込み完了、継続読み込み可
//       false : エラー発生、読み込み終了などで継続読み込み不可
bool Connection::receive_from_pipe() {
    // ヘッダ読み込み
    request_.set_readpipe(pp_recv_[0]);
    if (request_.receive_header() == EXIT_FAILURE) {
        // ヘッダが不十分なら読み込みを継続
        return false;
    }

    // リクエストデータを解析
    if (request_.get_status_code() == 200)
        request_.analyze_request(port_);

    request_.print_debug();

    response_.make_response();
#ifdef DEBUG
    std::cout << response_.get_response() << std::endl;
    std::cout << "---------------------------------------" << std::endl;
#endif
    return true;
}

const std::string& Connection::get_response() {
    return response_.get_response();
}

void   Connection::set_port(int port) {
    this->port_ = port;
}

void Connection::reset() {
    request_.reset();
    response_.reset();
}
