#include "srcs/util_network/FDManager.hpp"

FDManager::FDManager() {
    // ポート番号をセット
    socket_[0].set_port(HTTP_PORT1);
    socket_[1].set_port(HTTP_PORT2);
    
    (void)max_fd_;
    (void)received_fd_collection_;
    (void)timeout_;
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
    int active_socket_index;
    if (FD_ISSET(socket_[0].get_listen_fd(), &received_fd_collection_)) {
        active_socket_index = 0;
    } else if (FD_ISSET(socket_[1].get_listen_fd(), &received_fd_collection_)) {
        active_socket_index = 1;
    } else {
        return false;
    }
    // 接続されたならクライアントからの接続を確立する
    for (size_t i = 0;
        i < sizeof(packet_fd_)/sizeof(packet_fd_[0]);
        i++) {
        if (packet_fd_[i] == -1) {
            packet_fd_[i] = socket_[active_socket_index].accept();
            accept_fd_index_ = i;
            std::cout << "socket:" << packet_fd_[accept_fd_index_];
            std::cout << " connected." << std::endl;
            break;
        }
    }
    return true;
}

void FDManager::prepare_select_() {
    // 接続待ちのディスクリプタをディスクリプタ集合に設定する
    FD_ZERO(&received_fd_collection_);
    FD_SET(socket_[0].get_listen_fd(), &received_fd_collection_);
    FD_SET(socket_[1].get_listen_fd(), &received_fd_collection_);
    max_fd_ = socket_[0].get_listen_fd();
    if (max_fd_ < socket_[1].get_listen_fd()){
        max_fd_ = socket_[1].get_listen_fd();
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
    timeout_.tv_sec  = FDManager::TIMEOUT_SECOND;
    timeout_.tv_usec = FDManager::TIMEOUT_U_SECOND;
}

bool FDManager::select_() {
    // 接続＆受信を待ち受ける
    int count = ::select(max_fd_+1,
        &received_fd_collection_,
        NULL,
        NULL,
        &timeout_);
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

int FDManager::receive(char *buf){
    // 受信待ちディスクリプタにデータがあるかを調べる
    if (packet_fd_[accept_fd_index_] == -1) {
        return -1;
    }

    memset(buf, 0, sizeof(char) * BUF_SIZE);
    int read_size = -1;
    // データがあるならパケット受信する
    read_size = ::recv(packet_fd_[accept_fd_index_],
        buf,
        sizeof(char) * BUF_SIZE - 1,
        0);
    if (read_size <= 0) {
        // 切断された場合、クローズする
        std::cout << "socket:" << packet_fd_[accept_fd_index_];
        std::cout << "disconnected." << std::endl;
        close(packet_fd_[accept_fd_index_]);
        packet_fd_[accept_fd_index_] = -1;
        return -1;
    }
    // パケット受信成功の場合
    std::cout << "socket:" << packet_fd_[accept_fd_index_];
    std::cout << "received." << std::endl;
    return read_size;
}

bool FDManager::send(const std::string &str){
    // 受信待ちディスクリプタにデータがあるかを調べる
    if (packet_fd_[accept_fd_index_] == -1) {
        return false;
    }

    if (::send(packet_fd_[accept_fd_index_], str.c_str(),
        str.length(), 0) == -1) {
        std::cout << "FDManager::send failed." << std::endl;
        return false;
    }
    std::cout << "FDManager::send success to fd: " << packet_fd_[accept_fd_index_] << std::endl;
    return true;
}

void FDManager::disconnect(){
    // パケット送受信用ソケットクローズ
    close(packet_fd_[accept_fd_index_]);
    packet_fd_[accept_fd_index_] = -1;
}

void FDManager::create_socket() {
    socket_[0].prepare();
    socket_[1].prepare();
}

void FDManager::destory_socket() {
    socket_[0].cleanup();
    socket_[1].cleanup();
}
