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
        std::string recv_str;
        if (recv_until_double_newline_(recv_str, accept_fd) == -1) {
            continue;
        }

        // リクエストデータを解析
        HttpRequest req;
        req.analyze_request(recv_str);

        print_debug_(recv_str, req);

        // 取得したパスのファイルを開いて内容を取得する
        int is_file_exist = -1;
        int body_length = 0;
        std::vector<std::string> message_body;
        read_contents_from_file_(is_file_exist, body_length, message_body);

        // HTTPレスポンスを作成する
        std::string server_response;
        create_response_(
            server_response, body_length, message_body,
            is_file_exist, req);

        // ソケットディスクリプタにレスポンス内容を書き込む
        if (send(accept_fd, server_response.c_str(),
                server_response.length(), 0) == -1) {
            std::cerr << "send() failed." << std::endl;
        }

        close(accept_fd);
        accept_fd = -1;
    }
}

int Webserv::recv_until_double_newline_(std::string &recv_str, int accept_fd) {
    ssize_t read_size = 0;
    char buf[BUF_SIZE];

    do {
        memset(buf, 0, sizeof(buf));
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
        if ((recv_str[recv_str.length()-4] == '\r') &&
            (recv_str[recv_str.length()-3] == '\n') &&
            (recv_str[recv_str.length()-2] == '\r') &&
            (recv_str[recv_str.length()-1] == '\n')) {
            break;
        }
    } while (read_size > 0);
    return 0;
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
        int is_file_exist, HttpRequest &req) {
    std::vector<std::string> header
        = HttpResponse::make_header(
            3, body_length, is_file_exist, req.request_path);
    server_response = HttpResponse::make_response(header, message_body);
    std::cout << server_response << std::endl;
    std::cout << "---------------------------------------" << std::endl;
}

int Webserv::finalize() {
    sock->cleanup();
    delete sock;
    return 0;
}

void Webserv::print_debug_(std::string &recv_str, HttpRequest &req) {
    typedef std::map<std::string, std::string>::iterator map_iter;

    std::cout << "//-----recv_str start-----" << std::endl;
    std::cout << recv_str << std::endl;
    std::cout << "\\\\-----recv_str end-----" << std::endl;
    std::cout << std::endl;

    std::cout << "[req]" << std::endl;
    std::cout << "  req.method       : " << req.method << std::endl;
    std::cout << "  req.request_path : " << req.request_path << std::endl;
    std::cout << "  base_html_path   : " << kBaseHtmlPath << std::endl;
    std::cout << "  req.path_to_file : " << req.path_to_file << std::endl;
    std::cout << "  req.http_ver     : " << req.http_ver << std::endl;
    std::cout << std::endl;

    std::cout << "[header_field]" << std::endl;
    for (map_iter it = req.header_field.begin();
            it != req.header_field.end(); it++) {
        std::cout << "  " << it->first << ": " << it->second << std::endl;
    }
    std::cout << std::endl;
}
