#include "srcs/server/Webserv.hpp"

Webserv::Webserv() {
}
Webserv::~Webserv() {
}

void Webserv::init() {
    fd_manager_.create_socket();
}

void Webserv::loop() {
    while (true) {
        // 接続受付
        if (!fd_manager_.accept()) {
            continue;
        }

        // \r\n\r\nが来るまでメッセージ受信
        HttpRequest request_(&fd_manager_);
        if (request_.receive_header() == -1)
            continue;

        // リクエストデータを解析
        request_.analyze_request();

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

void Webserv::finalize() {
    fd_manager_.destory_socket();
}
