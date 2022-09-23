#include "srcs/server/Webserv.hpp"

Webserv::Webserv():
request_(*this),
response_(*this),
socket_(HTTP_PORT),
fd_manager_(*this) {
    socket_.prepare();
}

Webserv::~Webserv() {
    socket_.cleanup();
}

void Webserv::loop() {
    while (true) {
        // 接続受付
        if (!fd_manager_.accept()) {
            continue;
        }

        // \r\n\r\nが来るまでメッセージ受信
        request_ = HttpRequest(*this);
        if (!request_.receive_header()) {
            fd_manager_.disconnect();
            continue;
        }

        // リクエストデータを解析
        if (!request_.analyze_request()) {
            fd_manager_.disconnect();
            continue;
        }

        request_.print_debug();

        // HTTPレスポンスを作成する
        HttpResponse response_(request_);
        response_.make_response();
        std::cout << response_.get_response() << std::endl;
        std::cout << "---------------------------------------" << std::endl;

        // ソケットディスクリプタにレスポンス内容を書き込む
        if (!fd_manager_.send(response_.get_response())) {
            std::cerr << "send() failed." << std::endl;
        }

        fd_manager_.disconnect();
    }
}

const FDManager &Webserv::get_fd_manager() const {
    return fd_manager_;
}

const Socket &Webserv::get_socket() const {
    return socket_;
}
