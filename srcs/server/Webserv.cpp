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
        // 一定時間接続がない接続済みコネクションを解放
        fd_manager_.release();

        // ファイルディスクリプタ(ソケットも含め)の状態を確認。一定時間何も起こらなかったらコネクションを全て切断する
        if (!fd_manager_.select_fd_collection()) {
            // 全ての接続済みコネクションをクローズする
            continue;
        }

        // アクセスがあったソケットが接続済みかチェック
        switch (fd_manager_.check_event())
        {
            case Connect:
                // 新たな接続なら接続を確立してからselectに戻る
                fd_manager_.accept();
                continue;
            case Read:
#ifdef DEBUG
                std::cout << "  <Readable>" << std::endl;
#endif
                fd_manager_.receive();
                continue;
            case Write:
#ifdef DEBUG
                std::cout << "  <Writtable>" << std::endl;
#endif
                fd_manager_.send();
                if (fd_manager_.is_disconnect()) {
                    fd_manager_.disconnect();
                } else {
                    // 送信した接続の時間を更新
                    fd_manager_.update_time();
                }
                break;
            default:
                break;
        }

        // \r\n\r\nが来るまでメッセージ受信
        // HttpRequest request_;

        // 確立した接続から受信する
        // if (request_.receive_header() == EXIT_FAILURE) {
        //     // クライアントからEOFが来たらその接続を切断
        //     fd_manager_.disconnect();
        //     continue;
        // }

        // リクエストデータを解析
        // if (request_.get_status_code() == 200)
        //     request_.analyze_request();

        // request_.print_debug();

        // HTTPレスポンスを作成する
        // HttpResponse response_(&request_);
        // HttpResponse response_(&((*(fd_manager_.connections_it_)).request_));
        // response_.make_response();
        // std::cout << response_.get_response() << std::endl;
        // std::cout << "---------------------------------------" << std::endl;
        // // ソケットディスクリプタにレスポンス内容を書き込む
        // if (!fd_manager_.send(response_.get_response())) {
        //     std::cerr << "send() failed." << std::endl;
        // }

        // if (request_.get_status_code() == 400) {
        // if ((*(fd_manager_.connections_it_)).get_status_code() == 400) {
        //     fd_manager_.disconnect();
        // } else {
        //     // 送信した接続の時間を更新
        //     fd_manager_.update_time();
        // }
    }
}

void Webserv::finalize() {
    fd_manager_.destory_socket();
}
