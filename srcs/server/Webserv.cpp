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
        // ソケットの状態を確認。一定時間何も起こらなかったらコネクションを全て切断する
        if (!fd_manager_.select_active_socket()) {
            // 全ての接続済みコネクションをクローズする
            fd_manager_.release();
            continue;
        }

        // アクセスがあったソケットが接続済みかチェック
        if (!fd_manager_.check_established()) {
            // 新たな接続なら接続を確立してからselectに戻る
            fd_manager_.accept();
            continue;
        }

        // \r\n\r\nが来るまでメッセージ受信
        HttpRequest request_(&fd_manager_);

        // 確立した接続から受信する
        if (request_.receive_header() == EXIT_FAILURE) {
            // クライアントからEOFが来たらその接続を切断
            fd_manager_.disconnect();
            continue;
        }

        // リクエストデータを解析
        if (request_.get_status_code() == 200)
            request_.analyze_request();

        request_.print_debug();

        // HTTPレスポンスを作成する
        HttpResponse response_(&request_);
        response_.make_response();
        std::cout << response_.get_response() << std::endl;
        std::cout << "---------------------------------------" << std::endl;
        // ソケットディスクリプタにレスポンス内容を書き込む
        if (!fd_manager_.send(response_.get_response())) {
            std::cerr << "send() failed." << std::endl;
        }
    }
}

void Webserv::finalize() {
    fd_manager_.destory_socket();
}
