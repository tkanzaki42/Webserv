#include "srcs/server/Webserv.hpp"

Webserv::Webserv() {
}
Webserv::~Webserv() {
}

void Webserv::init() {
    if (!fd_manager_.create_socket()) {
        exit(0);
    }
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
        switch (fd_manager_.check_event()) {
            case Connect:
#ifdef DEBUG
                std::cout << std::endl;
                std::cout << "Event triggered: Connect" << std::endl;
#endif
                // 新たな接続なら接続を確立してからselectに戻る
                fd_manager_.accept();
                continue;
            case Read:
#ifdef DEBUG
                std::cout << std::endl;
                std::cout << "Event triggered: Read" << std::endl;
#endif
                if (fd_manager_.receive() == false) {
                    fd_manager_.disconnect();
                }
                continue;
            case Write:
#ifdef DEBUG
                std::cout << std::endl;
                std::cout << "Event triggered: Write" << std::endl;
#endif
                if (fd_manager_.send() == false) {
                    fd_manager_.disconnect();
                }
                if (fd_manager_.is_disconnect()) {
                    fd_manager_.disconnect();
                } else {
                    // 送信した接続の時間を更新
                    fd_manager_.update_time();
                    fd_manager_.reset();
                }
                break;
            default:
                break;
        }
    }
}

void Webserv::finalize() {
    fd_manager_.destory_socket();
}
