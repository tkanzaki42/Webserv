// simple echo web server
//  compile:
//      clang++ simple_webserver.cpp -o webserv
//  run:
//      ./webserv
//      then connect it with curl by the following command
//          curl localhost:8888
//      or connect from a web browser with the above path

const int HTTP_PORT = 8888;

// c includes
#include <unistd.h>
#include <errno.h>
#include <string.h>
// c network includes
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
// cpp includes
#include <iostream>
#include <sstream>

int run_webserver_simple() {
    // ソケット生成
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (!socket_fd) {
        std::cout << "Failed to initialize a socket." << std::endl;
        return 1;
    }

    // ソケットのオプション設定
    int optval = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR,
            &optval, sizeof(optval)) == -1) {
        std::cout << "Failed to setsocket" << std::endl;
        close(socket_fd);
        return 1;
    }

    // アドレス作成
    struct sockaddr_in  server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(HTTP_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // ソケットの名前付け(ソケットへのアドレス割り当て)
    if (bind(socket_fd, (struct sockaddr *)&server_addr,
            sizeof(server_addr)) != 0) {
        std::cout << "Failed to bind. error:" << strerror(errno) << std::endl;
        close(socket_fd);
        return 1;
    }

    // ソケットの接続を待つ
    if (listen(socket_fd, 5) != 0) {
        std::cout << "Failed to listen. error:" << errno << std::endl;
        close(socket_fd);
        return 1;
    }

    // レスポンスデータの作成
    std::string body = std::string("Hello World!\r\n");
    std::string response = std::string("");
    std::ostringstream oss;

    oss << "Content-Length: " << body.length() << "\r\n";

    response.append("HTTP/1.1 200 OK\r\n");
    response.append("Content-Type: text/html; charset=UTF-8\r\n");
    response.append(oss.str());
    response.append("Connection: Keep-Alive\r\n");
    response.append("\r\n");
    response.append(body);

    std::cout << "response:" << response << std::endl;

    char inbuf[2048];
    while (true) {
        // 接続を受ける
        int connfd = accept(socket_fd, NULL, NULL);
        if (connfd < 0) {
            std::cout << "Failed to accept." << std::endl;
            break;
        }

        // ソケットからメッセージを受け取る
        memset(inbuf, 0, sizeof(inbuf));
        int read_size = recv(connfd, inbuf, sizeof(inbuf), 0);
        if (read_size == -1) {
            std::cout << "Failed to send." << std::endl;
            return 1;
        }

        // ソケットへメッセージを送る
        if (send(connfd, response.c_str(), response.length(), 0) == -1) {
            std::cout << "Failed to send." << std::endl;
        }
        close(connfd);
    }

    close(socket_fd);
    return 0;
}

int main() {
    run_webserver_simple();
    return 0;
}
