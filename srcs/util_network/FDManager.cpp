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

    // 通信用ディスクリプタの配列を初期化する
    for (size_t i = 0;
        i < sizeof(packet_fd_)/sizeof(packet_fd_[0]);
        i++ ) {
        packet_fd_[i] = -1;
    }
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
    for (size_t i = 0; i < socketSet_.size(); i++) {
        if (FD_ISSET(socketSet_[i].get_listen_fd(), &received_fd_collection_)) {
            active_socket_index_ = i;
            break;
        }
    }

    // for (size_t i = 0;
    //     i < sizeof(packet_fd_)/sizeof(packet_fd_[0]);
    //     i++) {
    //     std::cout << i << ":" << packet_fd_[i] << std::endl;
    // }
    

    // 接続されたならクライアントからの接続を確立する
    for (size_t i = 0;
        i < sizeof(packet_fd_)/sizeof(packet_fd_[0]);
        i++) {
        if (packet_fd_[i] == -1) {
            packet_fd_[i] = socketSet_[active_socket_index_].accept();
            accept_fd_index_ = i;
            std::cout << "socket:" << packet_fd_[accept_fd_index_];
            std::cout << " connected." << std::endl;
            break;
        }
    }

    std::cout << " connected." << std::endl;
    return true;
}

void FDManager::prepare_select_() {
    // 接続待ちのディスクリプタをディスクリプタ集合に設定する
    FD_ZERO(&received_fd_collection_);
    max_fd_ = -1;
    for (size_t i = 0; i < socketSet_.size(); i++) {
        FD_SET(socketSet_[i].get_listen_fd(), &received_fd_collection_);
        if (max_fd_ < socketSet_[i].get_listen_fd()) {
            max_fd_ = socketSet_[i].get_listen_fd();
        }
    }

    // 受信待ちのディスクリプタをディスクリプタ集合に設定する
    for (size_t i = 0;
        i < sizeof(packet_fd_)/sizeof(packet_fd_[0]); i++) {
        if (packet_fd_[i] != -1) {
            FD_SET(packet_fd_[i], &received_fd_collection_);
            if (packet_fd_[i] > max_fd_) {
                max_fd_ = packet_fd_[i];
            }
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
    // 接続中かどうか
    if (packet_fd_[accept_fd_index_] == -1) {
        return -1;
    }

    memset(buf, 0, sizeof(char) * BUF_SIZE);
    int read_size = -1;
    // クライアントから受信する
    read_size = ::recv(packet_fd_[accept_fd_index_],
        buf,
        sizeof(char) * BUF_SIZE - 1,
        0);
    if (read_size <= 0) {
        // 切断された場合、クローズする
        std::cout << "socket:" << packet_fd_[accept_fd_index_];
        std::cout << " disconnected." << std::endl;
        close(packet_fd_[accept_fd_index_]);
        packet_fd_[accept_fd_index_] = -1;
        return -1;
    }
    // 受信成功の場合
    std::cout << "socket:" << packet_fd_[accept_fd_index_];
    std::cout << "received." << std::endl;
    return read_size;
}

bool FDManager::send(const std::string &str) {
    // 接続中かどうか
    if (packet_fd_[accept_fd_index_] == -1) {
        return false;
    }

    if (::send(packet_fd_[accept_fd_index_], str.c_str(),
        str.length(), 0) == -1) {
        std::cout << "FDManager::send failed." << std::endl;
        return false;
    }
    std::cout << "FDManager::send success to fd: ";
    std::cout << packet_fd_[accept_fd_index_] << std::endl;
    return true;
}

void FDManager::disconnect() {
    // クライアントとの接続を切断する
    // close(packet_fd_[accept_fd_index_]);
    packet_fd_[accept_fd_index_] = -1;
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
