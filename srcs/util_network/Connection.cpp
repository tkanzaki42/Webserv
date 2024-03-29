#include "Connection.hpp"

Connection::Connection():
accepted_fd_(-1), last_time_(0), response_(&request_) {
}

Connection::Connection(const Connection &obj) : response_(&request_) {
    *this = obj;
}

Connection &Connection::operator=(const Connection &obj) {
    this->accepted_fd_ = obj.accepted_fd_;
    this->last_time_   = obj.last_time_;
    this->port_        = obj.port_;
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

int Connection::get_status_code() {
    return request_.get_status_code();
}

int Connection::get_response_status_code() {
    return response_.get_status_code();
}

// 戻り値 true : 問題なく読み込み完了、継続読み込み可
//       false : エラー発生、読み込み終了などで継続読み込み不可
bool Connection::receive_from_pipe(const char *buf) {
    bool is_not_readed_header = false;

    // ヘッダが読み込み終わっていない場合
    if (request_.get_is_header_analyzed() == false) {
        is_not_readed_header = true;

        // ヘッダ読み込み
        if (request_.receive_header(buf) == false) {
            // ヘッダが不十分なら読み込みを継続
            return false;
        }

        // リクエストデータを解析
        if (request_.get_status_code() == 200)
            request_.analyze_request(port_);
    }

    // ヘッダ読み込みが終わり、ボディ読み込みが必要な場合
    if (request_.get_status_code() == 200
            && request_.get_http_method() == METHOD_POST) {
        // ボディ読み込み
        if (request_.receive_and_store_to_file(is_not_readed_header, buf) == false) {
            // ヘッダが不十分なら読み込みを継続
            return false;
        }
    }

    request_.print_debug();

    set_response_status_code_(get_status_code());
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

void   Connection::set_response_status_code_(int status_code) {
    response_.set_status_code(status_code);
}

void Connection::reset() {
    request_.reset();
    response_.reset();
}

void Connection::make_response() {
    response_.make_response();
}
