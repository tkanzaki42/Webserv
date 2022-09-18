#include "srcs/server/Webserv.hpp"

int Webserv::init() {
    sock = new Socket(HTTP_PORT);
    sock->prepare();
    return 0;
}

void Webserv::loop() {
    while (true) {
        // 接続受付
        int accept_fd = accept(
            sock->get_listen_fd(), (struct sockaddr*)NULL, NULL);
        if (accept_fd == -1) {
            continue;
        }

        // \r\n\r\nが来るまでメッセージ受信
        char buf[BUF_SIZE];
        memset(buf, 0, sizeof(buf));
        std::string recv_str = "";
        if (read_until_double_newline_(recv_str, buf, accept_fd) == -1) {
            continue;
        }

        // リクエストされたパスを取得する
        std::string path = "";
        std::string path_string = "";
        get_request_path_(path, path_string);

        // 取得したパスのファイルを開いて内容を取得する
        int is_file_exist = -1;
        int body_length = 0;
        std::vector<std::string> message_body;
        read_contents_from_file_(is_file_exist, body_length, message_body);

        // HTTPレスポンスを作成する
        std::string server_response;
        create_response_(
            server_response, body_length, message_body, is_file_exist, path);

        // ソケットディスクリプタにレスポンス内容を書き込む
        if (send(accept_fd, server_response.c_str(),
                server_response.length(), 0) == -1) {
            std::cerr << "send() failed." << std::endl;
        }

        close(accept_fd);
        accept_fd = -1;
    }
}

int Webserv::read_until_double_newline_(
        std::string &recv_str, char buf[BUF_SIZE], int accept_fd) {
    ssize_t read_size = 0;

    do {
        read_size = recv(accept_fd, buf, sizeof(char) * BUF_SIZE - 1, 0);
        if (read_size == -1) {
            std::cerr << "recv() failed." << std::endl;
            std::cerr << "ERROR: " << errno << std::endl;
            close(accept_fd);
            accept_fd = -1;
            return -1;
        }
        if (read_size > 0) {
            recv_str.append(buf);
        }
        if ((recv_str[recv_str.length()-4] == 0x0d) &&
            (recv_str[recv_str.length()-3] == 0x0a) &&
            (recv_str[recv_str.length()-2] == 0x0d) &&
            (recv_str[recv_str.length()-1] == 0x0a)) {
            break;
        }
    } while (read_size > 0);
    return 0;
}

void Webserv::get_request_path_(std::string &path, std::string &path_string) {
    std::string executive_file = "/";
    // std::string exe = executive_file;
    // std::size_t pos = exe.rfind('/');
    // if (pos != std::string::npos) {
    //     exe = exe.substr(pos + 1);
    // }
    path_string.clear();
    path = "";  // TODO(someone) remove
    // path = HttpParser::get_requestline_path(buf);
    // path_string = HttpParser::argv_path_analyzer(
    //     path, executive_file.c_str(), exe.c_str());
    // std::cout << "path_string : " << path_string << std::endl;
}

void Webserv::read_contents_from_file_(int &is_file_exist,
        int &body_length, std::vector<std::string> &message_body) {
    is_file_exist = -1;  // TODO(someone) remove
    // std::ifstream output_file(path_string.c_str());
    // char line[256];
    // is_file_exist = output_file.fail();
    message_body.clear();
    // while (output_file.getline(line, 256-1)) {
    //     body_length += strlen(line);
    //     message_body.push_back(std::string(line));
    // }
    // 使い終わったファイルのクローズ
    // output_file.close();
    message_body.push_back("hello world");  // TODO(someone) remove
    body_length = strlen("hello world");  // TODO(someone) remove
}

void Webserv::create_response_(std::string &server_response,
        int body_length, std::vector<std::string> &message_body,
        int is_file_exist, std::string &path) {
    std::vector<std::string> header
        = HttpResponse::make_header(3, body_length, is_file_exist, path);
    server_response = HttpResponse::make_response(header, message_body);
    std::cout << server_response << std::endl;
}

int Webserv::finalize() {
    sock->cleanup();
    delete sock;
    return 0;
}
