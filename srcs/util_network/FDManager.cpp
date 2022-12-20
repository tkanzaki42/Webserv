// Copyright 2022 tkanzaki
#include "srcs/util_network/FDManager.hpp"
#include "srcs/server/Webserv.hpp"
#include "srcs/util/StringConverter.hpp"
FDManager::FDManager() : active_socket_index_(-1) {
    std::set<int> set = Config::getAllListen();
    std::set<int>::iterator begin= set.begin();
    std::set<int>::iterator end= set.end();
    for (std::set<int>::iterator iter = begin; iter != end; ++iter) {
        std::cout << *iter << std::endl;
        int port = *iter;
        Socket soc;
        soc.set_port(port);
        socketSet_.push_back(soc);
    }

    // 処理用のファイルディスクリプタを初期化する
    max_fd_ = -1;

    // 接続待ちのディスクリプタをディスクリプタ集合に設定する
    FD_ZERO(&received_fd_collection_);
    FD_ZERO(&sendable_fd_collection_);
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

bool FDManager::select_active_socket() {
    // 接続＆受信を待ち受ける
    if (!select_wr_()) {
        // やり直し
        return false;
    }
    // 接続されたソケットを確認
    for (size_t i = 0; i < socketSet_.size(); i++) {
        if (FD_ISSET(socketSet_[i].get_listen_fd(), &received_fd_collection_)) {
            active_socket_index_ = i;
            break;
        }
    }
    return true;
}

bool FDManager::check_established() {
    if (FD_ISSET(socketSet_[active_socket_index_].get_listen_fd(), &received_fd_collection_)) {
        std::cout << "socket:" << socketSet_[active_socket_index_].get_listen_fd();
        std::cout << " is connected to accept." << std::endl;
        return false;
    } else {
        std::cout << "socket:" << socketSet_[active_socket_index_].get_listen_fd();
        std::cout << " has been established to recieve." << std::endl;
        search_accept_fd_index_();
        return true;
    }
}

void FDManager::accept() {
    // 接続されたならクライアントからの接続を確立する
    // 受信待ちのディスクリプタをディスクリプタ集合に設定する
    accept_fd_.push_back(socketSet_[active_socket_index_].accept());
    accept_fd_it_ = accept_fd_.end() - 1;
    FD_SET(*accept_fd_it_, &received_fd_collection_);
    if (*accept_fd_it_ > max_fd_) {
        max_fd_ = *accept_fd_it_;
    }
    std::cout << "accept_fd_:" << *accept_fd_it_;
    std::cout << " accept connection from socket:" << socketSet_[active_socket_index_].get_listen_fd();
    std::cout << "." << std::endl;
}

bool FDManager::select_wr_() {
    // 全てのソケットのFDを読み込み集合に追加
    for (size_t i = 0; i < socketSet_.size(); i++) {
        FD_SET(socketSet_[i].get_listen_fd(), &received_fd_collection_);
        if (max_fd_ < socketSet_[i].get_listen_fd()) {
            max_fd_ = socketSet_[i].get_listen_fd();
        }
    }

    // 全ての受け入れ済みのFDを読み込み集合に追加
    for (size_t i = 0; i < accept_fd_.size(); i++)
    {
        FD_SET(accept_fd_[i], &received_fd_collection_);
        if (accept_fd_[i] > max_fd_) {
            max_fd_ = accept_fd_[i];
        }
    }
    
    // selectのタイムアウト時間(ここは毎回初期化しないと0になる)
    select_time_.tv_sec  = FDManager::SELECT_TIME_SECOND;
    select_time_.tv_usec = FDManager::SELECT_TIME_U_SECOND;

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
    memset(buf, 0, sizeof(char) * BUF_SIZE);
    int read_size = -1;
    // クライアントから受信する
    read_size = ::recv(*accept_fd_it_,
        buf,
        sizeof(char) * BUF_SIZE - 1,
        0);
    if (read_size <= 0) {
        // 切断された場合、クローズする
        return -1;
    }
    // 受信成功の場合
    FD_SET(*accept_fd_it_, &sendable_fd_collection_);
    if (*accept_fd_it_ > max_fd_) {
        max_fd_ = *accept_fd_it_;
    }
    std::cout << "accept_fd_:" << *accept_fd_it_;
    std::cout << " received." << std::endl;
    return read_size;
}

void FDManager::search_accept_fd_index_() {
    for (
        std::vector<int>::iterator it = accept_fd_.begin();
        it < accept_fd_.end();
        it++)
    {
        if (FD_ISSET(*it, &received_fd_collection_)) {
            accept_fd_it_ = it;
            return ;
        }
    }
    accept_fd_it_ = accept_fd_.end();
}

bool FDManager::send(const std::string &str) {
    if (::send(*accept_fd_it_, str.c_str(),
        str.length(), 0) == -1) {
        std::cout << "FDManager::send failed." << std::endl;
        return false;
    }
    std::cout << "FDManager::send success to fd: ";
    std::cout << *accept_fd_it_ << std::endl;
    return true;
}

void FDManager::disconnect() {
    // クライアントとの接続を切断する
    if (accept_fd_.size()) {
        std::cout << "accept_fd_:" << *accept_fd_it_;
        std::cout << " disconnected." << std::endl;
        close(*accept_fd_it_);
        FD_CLR(*accept_fd_it_, &sendable_fd_collection_);
        FD_CLR(*accept_fd_it_, &received_fd_collection_);
        accept_fd_.erase(accept_fd_it_);
        active_socket_index_ = -1;
    }
}

void FDManager::release() {
    for (
        std::vector<int>::iterator it = accept_fd_.begin();
        it < accept_fd_.end();
        it++)
    {
        std::cout << "accept_fd_:" << *it;
        std::cout << " disconnected." << std::endl;
        close(*it);
        FD_CLR(*it, &sendable_fd_collection_);
        FD_CLR(*it, &received_fd_collection_);
        accept_fd_.erase(it);
    }
    active_socket_index_ = -1;
}

void FDManager::create_socket() {
    for (size_t i = 0; i < socketSet_.size(); i++) {
        socketSet_[i].prepare();
    }
}

void FDManager::destory_socket() {
    for (size_t i = 0; i < socketSet_.size(); i++) {
        socketSet_[i].cleanup();
    }
}

struct sockaddr_in FDManager::get_client_addr() {
    return socketSet_[active_socket_index_].get_client_addr();
}
