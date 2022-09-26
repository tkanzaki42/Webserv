#include "srcs/server/FDManager.hpp"

FDManager::FDManager():
accept_fd_(-1),
socket_(HTTP_PORT) {
    (void)max_fd_;
    (void)received_fd_collection_;
    (void)timeout_;
    (void)from_addr_;
    from_addr_len_ = sizeof(struct sockaddr_in);
    // 通信用ディスクリプタの配列を初期化する
    for (size_t i = 0;
        i < sizeof(packet_fd_)/sizeof(packet_fd_[0]);
        i++ ) {
        packet_fd_[i] = -1;
    }
}

FDManager::~FDManager() {
}

FDManager::FDManager(const FDManager &obj):
accept_fd_(-1),
socket_(HTTP_PORT) {
    from_addr_len_ = sizeof(struct sockaddr_in);
    *this = obj;
}

FDManager &FDManager::operator=(const FDManager &obj) {
    accept_fd_     = obj.accept_fd_;
    return *this;
}

int FDManager::get_accept_fd() const {
    return accept_fd_;
}

bool FDManager::accept() {
    /*
    // 接続待ちのディスクリプタをディスクリプタ集合に設定する
    FD_ZERO(&received_fd_collection_);
    FD_SET(socket_.get_listen_fd(), &received_fd_collection_);
    max_fd_ = socket_.get_listen_fd();
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
    // タイムアウト時間を10sec+500000μsec に指定する。
    timeout_.tv_sec  = 10;
    timeout_.tv_usec = 500000;

    // 接続＆受信を待ち受ける

    accept_fd_ = select(max_fd_+1,
        &received_fd_collection_,
        NULL,
        NULL,
        &timeout_);
    if (accept_fd_ < 0) {
        if (errno == EINTR) {
            // シグナル受信によるselect終了の場合、再度待ち受けに戻る
            return false;
        }
        // std::cout << "!this!" << std::endl;
        // その他のエラーの場合、終了する。
        // exit;
        return false;
    } else if (accept_fd_ == 0) {
        // タイムアウトした場合、再度待ち受けに戻る
        return false;
    } else {

        // 接続待ちディスクリプタに接続があったかを調べる
        if (FD_ISSET(socket_.get_listen_fd(), &received_fd_collection_)) {
            // 接続されたならクライアントからの接続を確立する
            for (size_t i = 0;
                i < sizeof(packet_fd_)/sizeof(packet_fd_[0]);
                i++) {
                if (packet_fd_[i] == -1) {
                    if ((packet_fd_[i] = ::accept(socket_.get_listen_fd(),
                            (struct sockaddr *)&from_addr_,
                            &from_addr_len_)) < 0) {
                        // exit;
                        return false;
                    }
                    std::cout << "socket:" << packet_fd_[i];
                    std::cout << "connected." << std::endl;
                    return true;
                }
            }
        }
    }
    return false;
    */
    accept_fd_ = ::accept(socket_.get_listen_fd(),
         (struct sockaddr*)NULL, NULL);
    if (accept_fd_ == -1) {
         return false;
     }
     return true;
}

bool FDManager::send(const std::string &str) const {
    if (::send(accept_fd_, str.c_str(),
        str.length(), 0) == -1) {
        return false;
    }
    return true;
}

int FDManager::receive(char *buf) const {
    memset(buf, 0, sizeof(char) * BUF_SIZE);
/*
    int read_size = -1;
    for (size_t i = 0;
        i < sizeof(packet_fd_)/sizeof(packet_fd_[0]);
        i++) {
        // 受信待ちディスクリプタにデータがあるかを調べる
        if (FD_ISSET(packet_fd_[i], &received_fd_collection_)) {
            // データがあるならパケット受信する
            read_size = recv(packet_fd_[i],
                buf,
                sizeof(char) * BUF_SIZE - 1,
                0);
            if (read_size > 0) {
                // パケット受信成功の場合
                std::cout << "socket:" << packet_fd_[i];
                std::cout << "received." << std::endl;
                return read_size;
            } else if (read_size == 0) {
                // 切断された場合、クローズする
                std::cout << "socket:" << packet_fd_[i];
                std::cout << "disconnected." << std::endl;
                // close( packet_fd_[i] );
                // packet_fd_[i] = -1;
            } else {
                // exit;
            }
        }
    }
    return read_size;
    */
    int read_size = recv(accept_fd_,
        buf, sizeof(char) * BUF_SIZE - 1, 0);
    buf[read_size] = '\0';
    return read_size;
}

void FDManager::disconnect() const {
    // パケット送受信用ソケットクローズ
    /*
    for (size_t i = 0;
        i < sizeof(packet_fd_)/sizeof(packet_fd_[0]);
        i++) {
        close(packet_fd_[i]);
    }
    */
    close(accept_fd_);
}

void FDManager::create_socket() {
    socket_.prepare();
}

void FDManager::destory_socket() {
    socket_.cleanup();
}
