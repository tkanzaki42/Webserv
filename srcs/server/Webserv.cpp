#include "srcs/server/Webserv.hpp"

int Webserv::init() {
    sock = new Socket(HTTP_PORT);
    sock->prepare();
    return 0;
}

void Webserv::loop() {
    while (true) {
        // 接続受付
        int accept_fd = accept(
            sock->get_listen_fd(),
            (struct sockaddr*)NULL, NULL);
        if (accept_fd == -1) {
            continue;
        }

        // \r\n\r\nが来るまでメッセージ受信
        request_.set_accept_fd(accept_fd);
        if (request_.recv_until_double_newline_() == -1) {
            continue;
        }

        // リクエストデータを解析
        request_.analyze_request();

        print_debug_();

        // HTTPレスポンスを作成する
        create_response_();

        // ソケットディスクリプタにレスポンス内容を書き込む
        if (send(accept_fd,
                response_.get_response().c_str(),
                response_.get_response().length(), 0) == -1) {
            std::cerr << "send() failed." << std::endl;
        }

        close(accept_fd);
        accept_fd = -1;  // TODO(tkanzaki) -1入れる必要性(?)
    }
}

void Webserv::create_response_() {
    response_.make_response();
    std::cout << response_.get_response() << std::endl;
    std::cout << "---------------------------------------" << std::endl;
}

int Webserv::finalize() {
    sock->cleanup();
    delete sock;
    return 0;
}

void Webserv::print_debug_() {
    std::cout << "//-----recieved_line start-----" << std::endl;
    std::cout << request_.get_received_line() << std::endl;
    std::cout << "\\\\-----recieved_line end-----" << std::endl;
    std::cout << std::endl;

    std::cout << "[req]" << std::endl;
    std::cout << "  req.method       : " << request_.method << std::endl;
    std::cout << "  req.request_path : " << request_.request_path << std::endl;
    std::cout << "  base_html_path   : " << kBaseHtmlPath << std::endl;
    std::cout << "  req.path_to_file : " << request_.path_to_file << std::endl;
    std::cout << std::endl;
}
