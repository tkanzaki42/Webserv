// Copyright 2022 tkanzaki
#include "srcs/util_network/FDManager.hpp"
#include "srcs/server/Webserv.hpp"
#include "srcs/util/StringConverter.hpp"
FDManager::FDManager() {
    host0["PORT"] = "5000";
    host1["PORT"] = "5001";
    config["0"] = host0;
    config["1"] = host1;
    // ポート番号をセット
    for (size_t i = 0; i < sizeof(socket_)/sizeof(socket_[0]); i++) {
        int port = StringConverter::stoi(config[StringConverter::itos(i)]["PORT"]);
        socket_[i].set_port(port);
    }

    // 通信用ディスクリプタの配列を初期化する
    accept_fd_ = -1;
}

FDManager::~FDManager() {
}

FDManager::FDManager(const FDManager &obj) {
    *this = obj;
}

FDManager &FDManager::operator=(const FDManager &obj) {
    max_fd_ = obj.max_fd_;
    return *this;
}

bool FDManager::accept() {
    prepare_select_();

    // 接続＆受信を待ち受ける
    if (!select_()) {
        // やり直し
        return false;
    }

    // 接続されたソケットを確認
    int active_socket_index = -1;
    for (size_t i = 0; i < sizeof(socket_)/sizeof(socket_[0]); i++) {
        if (FD_ISSET(socket_[i].get_listen_fd(), &received_fd_collection_)) {
            active_socket_index = i;
            break;
        }
    }

    // 接続されたならクライアントからの接続を確立する
    accept_fd_ = socket_[active_socket_index].accept();
    std::cout << "socket:" << accept_fd_;
    std::cout << " connected." << std::endl;
    return true;
}

void FDManager::prepare_select_() {
    // 接続待ちのディスクリプタをディスクリプタ集合に設定する
    FD_ZERO(&received_fd_collection_);
    max_fd_ = -1;
    for (size_t i = 0; i < sizeof(socket_)/sizeof(socket_[0]); i++) {
        FD_SET(socket_[i].get_listen_fd(), &received_fd_collection_);
        if (max_fd_ < socket_[i].get_listen_fd()) {
            max_fd_ = socket_[i].get_listen_fd();
        }
    }

    // 受信待ちのディスクリプタをディスクリプタ集合に設定する
    if (accept_fd_ != -1) {
        FD_SET(accept_fd_, &received_fd_collection_);
        if (accept_fd_ > max_fd_) {
            max_fd_ = accept_fd_;
        }
    }

    // selectのタイムアウト時間(ここは毎回初期化しないと0になる)
    select_time_.tv_sec  = FDManager::SELECT_TIME_SECOND;
    select_time_.tv_usec = FDManager::SELECT_TIME_U_SECOND;
}

bool FDManager::select_() {
    // 接続＆受信を待ち受ける
    int count = ::select(max_fd_+1,
        &received_fd_collection_,
        NULL,
        NULL,
        &select_time_);
    if (count < 0) {
        if (errno == EINTR) {
            // シグナル受信によるselect終了の場合、再度待ち受けに戻る
            std::cout << "Interrupted system call." << std::endl;
            return false;
        }
        // それ以外はexit
        std::cerr << "select() failed." << std::endl;
        _exit(1);
    } else if (count == 0) {
        // タイムアウトした場合、再度待ち受けに戻る
        std::cout << "resources released." << std::endl;
        return false;
    } else {
        std::cout << "FDManager::select ";
        std::cout << count;
        std::cout << " connection gotten ready,";
        std::cout << std::endl;
        return true;
    }
}

int FDManager::receive(char *buf) {
    // 受信待ちディスクリプタにデータがあるかを調べる
    if (accept_fd_ == -1) {
        return -1;
    }

    memset(buf, 0, sizeof(char) * BUF_SIZE);
    int read_size = -1;
    // データがあるならパケット受信する
    read_size = ::recv(accept_fd_,
        buf,
        sizeof(char) * BUF_SIZE - 1,
        0);
    if (read_size <= 0) {
        // 切断された場合、クローズする
        std::cout << "socket:" << accept_fd_;
        std::cout << "disconnected." << std::endl;
        close(accept_fd_);
        accept_fd_ = -1;
        return -1;
    }
    // パケット受信成功の場合
    std::cout << "socket:" << accept_fd_;
    std::cout << "received." << std::endl;
    return read_size;
}

bool FDManager::send(const std::string &str) {
    // 受信待ちディスクリプタにデータがあるかを調べる
    if (accept_fd_ == -1) {
        return false;
    }

    if (::send(accept_fd_, str.c_str(),
        str.length(), 0) == -1) {
        std::cout << "FDManager::send failed." << std::endl;
        return false;
    }
    std::cout << "FDManager::send success to fd: ";
    std::cout << accept_fd_ << std::endl;
    return true;
}

void FDManager::disconnect() {
    // パケット送受信用ソケットクローズ
    close(accept_fd_);
    accept_fd_ = -1;
}

void FDManager::create_socket() {
    for (size_t i = 0; i < sizeof(socket_)/sizeof(socket_[0]); i++) {
        socket_[i].prepare();
    }
}

void FDManager::destory_socket() {
    for (size_t i = 0; i < sizeof(socket_)/sizeof(socket_[0]); i++) {
        socket_[i].cleanup();
    }
}
