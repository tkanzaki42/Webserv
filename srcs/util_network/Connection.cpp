#include "Connection.hpp"

Connection::Connection():
accepted_fd_(-1), last_time_(0), response_(&request_) {
    if (pipe(pp_recv_) == -1) {
        std::cerr << "Failed to pipe() in Connection()" << std::endl;
    }
    std::cout << "  pp_recv_[0] = " << pp_recv_[0] << std::endl;
    std::cout << "  pp_recv_[1] = " << pp_recv_[1] << std::endl;

    if (pipe(pp_send_) == -1) {
        std::cerr << "Failed to pipe() in Connection()" << std::endl;
    }
    std::cout << "  pp_send_[0] = " << pp_send_[0] << std::endl;
    std::cout << "  pp_send_[1] = " << pp_send_[1] << std::endl;
}

Connection::Connection(const Connection &obj) : response_(&request_) {
    *this = obj;
}

Connection &Connection::operator=(const Connection &obj) {
    this->accepted_fd_ = obj.accepted_fd_;
    this->last_time_   = obj.last_time_;
    this->pp_recv_[0] = obj.pp_recv_[0];
    this->pp_recv_[1] = obj.pp_recv_[1];
    this->pp_send_[0] = obj.pp_send_[0];
    this->pp_send_[1] = obj.pp_send_[1];
    return *this;
}

Connection::~Connection() {
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
    return pp_recv_[1];
}

int Connection::get_read_pipe() {
    return pp_send_[0];
}

int Connection::get_status_code() {
    return request_.get_status_code();
}

// 戻り値 true : 問題なく読み込み完了、継続読み込み可
//       false : エラー発生、読み込み終了などで継続読み込み不可
bool Connection::receive_from_pipe() {
    request_.set_readpipe(pp_recv_[0]);
    if (request_.receive_header() == EXIT_FAILURE) {
        // クライアントからEOFが来たらその接続を切断
        return false;
    }

    // リクエストデータを解析
    if (request_.get_status_code() == 200)
        request_.analyze_request();

    request_.print_debug();

    response_.make_response();
    std::cout << response_.get_response() << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    return true;
}

void   Connection::send_to_pipe() {
    // パイプに書き込み
    const std::string& send_data = response_.get_response();
    int write_ret
        = write(pp_send_[1], send_data.c_str(), sizeof(send_data.c_str()));
    if (write_ret <= 0) {
        std::cerr << "Failed to write to pipe in Connection::send_to_pipe()."
            << std::endl;
        return;
    }
}
