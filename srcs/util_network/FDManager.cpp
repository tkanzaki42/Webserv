// Copyright 2022 tkanzaki
#include "srcs/util_network/FDManager.hpp"

#define USLEEP_FIN_DELAY 3000

FDManager::FDManager() {
    std::set<int> set = Config::getAllListen();
    std::set<int>::iterator begin = set.begin();
    std::set<int>::iterator end = set.end();
    for (std::set<int>::iterator iter = begin; iter != end; ++iter) {
#ifdef DEBUG
        std::cout << *iter << std::endl;
#endif
        int port = *iter;
        Socket soc;
        soc.set_port(port);
        sockets_.push_back(soc);
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

bool FDManager::select_fd_collection() {
    select_prepare_();
    // 接続＆受信を待ち受ける
    if (!select_fd_()) {
        // やり直し
        return false;
    }
    return true;
}

void FDManager::select_prepare_() {
    // 全てのソケットのFDを読み込み集合に追加
    for (
        std::vector<Socket>::iterator it = sockets_.begin();
        it != sockets_.end();
        it++)
    {
        FD_SET((*it).get_listen_fd(), &received_fd_collection_);
        if (max_fd_ < (*it).get_listen_fd()) {
            max_fd_ = (*it).get_listen_fd();
        }
    }

    // 全ての受け入れ済みのFDを読み込み集合に追加
    for (
        std::vector<Connection>::iterator it = connections_.begin();
        it != connections_.end();
        it++)
    {
        FD_SET((*it).get_accepted_fd(), &received_fd_collection_);
        if (max_fd_ < (*it).get_accepted_fd()) {
            max_fd_ = (*it).get_accepted_fd();
        }
    }
    
    // selectのタイムアウト時間(ここは毎回初期化しないと0になる)
    select_time_.tv_sec  = FDManager::SELECT_TIME_SECOND;
    select_time_.tv_usec = FDManager::SELECT_TIME_U_SECOND;
}

bool FDManager::select_fd_() {
    // 接続＆受信を待ち受ける
    int count = ::select(max_fd_+1,
        &received_fd_collection_,
        &sendable_fd_collection_,
        NULL,
        &select_time_);
    if (count < 0) {
#ifdef DEBUG
        if (errno == EINTR) {
            // シグナル受信によるselect終了の場合、再度待ち受けに戻る
            std::cout << "Interrupted system call." << std::endl;
        }
#endif
        // それ以外はexit
        std::cerr << "select() failed." << std::endl;
        exit(0);
    } else if (count == 0) {
        // タイムアウトした場合、再度待ち受けに戻る
#ifdef DEBUG
        std::cout << "resources released." << std::endl;
#endif
        return false;
    } else {
#ifdef DEBUG
        std::cout << "FDManager::select ";
        std::cout << count;
        std::cout << " events gotten ready, ";
        std::cout << connections_.size();
        std::cout << " connections keep alive.";
        std::cout << std::endl;
#endif
        return true;
    }
}

enum E_Event FDManager::check_event() {
    // ソケット
    sockets_it_ = sockets_.end();
    for (
        std::vector<Socket>::iterator it = sockets_.begin();
        it != sockets_.end();
        it++)
    {
        if (FD_ISSET((*it).get_listen_fd(), &received_fd_collection_)) {
            // 接続されたソケットを確定
            sockets_it_ = it;
#ifdef DEBUG
            std::cout << "socket:" << (*it).get_listen_fd() << "[" << (*it).get_port() << "]";
            std::cout << " is connected to accept." << std::endl;
#endif
            return Connect;
        }
    }

    // 受け入れ済みのFD
    for (
        std::vector<Connection>::iterator it = connections_.begin();
        it != connections_.end();
        it++)
    {
        if (FD_ISSET((*it).get_accepted_fd(), &received_fd_collection_)) {
            connections_it_ = it;
#ifdef DEBUG
            std::cout << "fd:" << (*it).get_accepted_fd();
            std::cout << " has been established to recieve." << std::endl;
#endif
            return Read;
        }
    }
    
    // 受け入れ済みのFD
    for (
        std::vector<Connection>::iterator it = connections_.begin();
        it != connections_.end();
        it++)
    {
        if (FD_ISSET((*it).get_accepted_fd(), &sendable_fd_collection_)) {
            connections_it_ = it;
#ifdef DEBUG
            std::cout << "fd:" << (*it).get_accepted_fd();
            std::cout << " has been established to send." << std::endl;
#endif
            return Write;
        }
    }
    return Other;
}

void FDManager::accept() {
    // 接続されたならクライアントからの接続を確立する
    // 受信待ちのディスクリプタをディスクリプタ集合に設定する
    Connection connection;
    connection.set_accepted_fd((*sockets_it_).accept());
    connection.set_port((*sockets_it_).get_port());
    connection.set_last_time(time(NULL));
    connection.set_client_addr(get_client_addr());
    connections_.push_back(connection);
#ifdef DEBUG
    std::cout << "connected_fds_:" << connection.get_accepted_fd();
    std::cout << " accept connection from socket:" << (*sockets_it_).get_listen_fd();
    std::cout << "." << std::endl;
#endif
}

bool FDManager::receive() {
    // クライアントから受信する
    char     buf[BUF_SIZE];
    StringConverter::ft_memset(buf, 0, sizeof(char) * BUF_SIZE);
    int read_size = ::recv((*connections_it_).get_accepted_fd(),
        buf,
        sizeof(char) * BUF_SIZE - 1,
        0);
#ifdef DEBUG
    std::cout << "read_size:" << read_size << std::endl;
#endif
    if (read_size < 0) {
        // 切断された場合
        return false;
    } else if (read_size == 0) {
        // 正常終了の場合
        return false;
    }
#ifdef DEBUG
    std::cout << "connected_fds_:" << (*connections_it_).get_accepted_fd();
    std::cout << " received." << std::endl;
#endif

    // 受信したデータを解析
    if ((*connections_it_).receive_from_pipe(buf) == false) {
        // ヘッダ読み込みが最後まで終わっていない場合、そのまま抜けて再度Readイベントを待つ
        return true;
    }

    // 書き込みイベントフラグをセット
    FD_SET((*connections_it_).get_accepted_fd(), &sendable_fd_collection_);
#ifdef DEBUG
    std::cout << "connected_fds_:" << (*connections_it_).get_accepted_fd();
    std::cout << " set sendable_fds." << std::endl;
#endif
    return true;
}

bool FDManager::send() {
    usleep(USLEEP_FIN_DELAY);
    // データをクライアントに送信
    if (::send((*connections_it_).get_accepted_fd(),
        (*connections_it_).get_response().c_str(),
        (*connections_it_).get_response().length(), 0) == -1) {
#ifdef DEBUG
        std::cout << "FDManager::send failed." << std::endl;
#endif
        return false;
    }
    // (*connections_it_).reset();

    FD_CLR((*connections_it_).get_accepted_fd(), &sendable_fd_collection_);
#ifdef DEBUG
    std::cout << "FDManager::send success to fd: ";
    std::cout << (*connections_it_).get_accepted_fd() << std::endl;
#endif
    return true;
}

bool FDManager::is_disconnect() {
    if ((*connections_it_).get_response_status_code() == 400
    || (*connections_it_).get_response_status_code() == 505)
        return true;
    return false;
}

void FDManager::disconnect() {
    // クライアントとの接続を切断する
    if (!connections_.size()) {
#ifdef DEBUG
        std::cout << "Empty Connections." << std::endl;
#endif
        return ;
    }
#ifdef DEBUG
    std::cout << "connected_fds_:" << (*connections_it_).get_accepted_fd();
    std::cout << " disconnected." << std::endl;
#endif
    // usleep(USLEEP_FIN_DELAY);
    close((*connections_it_).get_accepted_fd());
    FD_CLR((*connections_it_).get_accepted_fd(), &sendable_fd_collection_);
    FD_CLR((*connections_it_).get_accepted_fd(), &received_fd_collection_);
    connections_.erase(connections_it_);
}

void FDManager::release() {
    if (!connections_.size()) {
        return ;
    }
    std::vector<Connection>::iterator it = connections_.begin();
    while (it != connections_.end())
    {
        if (time(NULL) - (*it).get_last_time() > TIMEOUT_CONNECTION) {
#ifdef DEBUG
            std::cout << "connected_fds_:" << (*it).get_accepted_fd();
            std::cout << " disconnected." << std::endl;
#endif
            close((*it).get_accepted_fd());
            FD_CLR((*it).get_accepted_fd(), &sendable_fd_collection_);
            FD_CLR((*it).get_accepted_fd(), &received_fd_collection_);
            it = connections_.erase(it);
            continue;
        }
        it++;
    }
}

void FDManager::update_time() {
    (*connections_it_).set_last_time(time(NULL));
}

bool FDManager::create_socket() {
    for (
        std::vector<Socket>::iterator it = sockets_.begin();
        it != sockets_.end();
        it++)
    {
        // もしソケットの作成に失敗したら
        if (!(*it).prepare()) {
            return false;
        }
    }
    return true;
}

void FDManager::destory_socket() {
    for (
        std::vector<Socket>::iterator it = sockets_.begin();
        it != sockets_.end();
        it++)
    {
        (*it).cleanup();
    }
}

struct sockaddr_in FDManager::get_client_addr() {
    return (*sockets_it_).get_client_addr();
}

void FDManager::reset() {
    (*connections_it_).reset();
}
