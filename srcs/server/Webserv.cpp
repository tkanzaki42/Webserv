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
                std::cout << "Event triggered: Connect" << std::endl;
                // 新たな接続なら接続を確立してからselectに戻る
                fd_manager_.accept();
                continue;
            case Read:
                std::cout << "Event triggered: Read" << std::endl;
                if (fd_manager_.receive() == -1) {
                    fd_manager_.disconnect();
                }
                continue;
            case Write:
                std::cout << "Event triggered: Write" << std::endl;
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
    }
}

void Webserv::finalize() {
    fd_manager_.destory_socket();
}
