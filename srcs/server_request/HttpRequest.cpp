#include "srcs/server_request/HttpRequest.hpp"

HttpRequest::HttpRequest() :
    accept_fd_(-1), parser_(HttpParser()), status_code_(200) {
}

HttpRequest::~HttpRequest() {
}

HttpRequest::HttpRequest(const HttpRequest &obj) {
    *this = obj;
}

HttpRequest& HttpRequest::operator=(const HttpRequest &obj) {
    accept_fd_    = obj.accept_fd_;
    parser_       = HttpParser(obj.parser_);
    status_code_  = obj.status_code_;
    path_to_file_ = std::string(obj.path_to_file_);

    return *this;
}

void HttpRequest::set_accept_fd(int accept_fd) {
    this->accept_fd_ = accept_fd;
}

int HttpRequest::recv_until_double_newline() {
    ssize_t read_size = 0;
    char buf[BUF_SIZE];

    do {
        memset(buf, 0, sizeof(buf));
        read_size = recv(accept_fd_, buf, sizeof(char) * BUF_SIZE - 1, 0);
        if (read_size == -1) {
            std::cerr << "recv() failed." << std::endl;
            std::cerr << "ERROR: " << errno << std::endl;
            close(accept_fd_);
            accept_fd_ = -1;
            return -1;
        }
        if (read_size > 0) {
            parser_.get_received_line().append(buf);
        }
        int received_line_len = parser_.get_received_line().length();
        if ((parser_.get_received_line()[received_line_len - 4] == '\r') &&
            (parser_.get_received_line()[received_line_len - 3] == '\n') &&
            (parser_.get_received_line()[received_line_len - 2] == '\r') &&
            (parser_.get_received_line()[received_line_len - 1] == '\n')) {
            break;
        }
    } while (read_size > 0);
    return 0;
}

void HttpRequest::analyze_request() {
    parser_.parse();

    generate_path_to_file_();
}

void HttpRequest::print_debug() {
    typedef std::map<std::string, std::string>::const_iterator map_iter;

    std::cout << "//-----received_line_ start-----" << std::endl;
    std::cout << parser_.get_received_line() << std::endl;
    std::cout << "\\\\-----received_line_ end-----" << std::endl;
    std::cout << std::endl;

    std::cout << "[request data]" << std::endl;
    std::cout << "  http_method_   : "
        << parser_.get_http_method() << std::endl;
    std::cout << "  request_path_  : "
        << parser_.get_request_path() << std::endl;
    std::cout << "  http_ver_      : " << parser_.get_http_ver() << std::endl;
    std::cout << "  base_html_path : " << kBaseHtmlPath << std::endl;
    std::cout << "  path_to_file_  : " << path_to_file_ << std::endl;

    std::cout << "  header_field_  :" << std::endl;
    for (map_iter it = parser_.get_header_field().begin();
            it != parser_.get_header_field().end(); it++) {
        std::cout << "    " << it->first << ": " << it->second << std::endl;
    }
    std::cout << std::endl;
}

HttpMethod HttpRequest::get_http_method() const {
    return parser_.get_http_method();
}

const std::string& HttpRequest::get_request_path() const {
    return parser_.get_request_path();
}

const std::string& HttpRequest::get_http_ver() const {
    return parser_.get_http_ver();
}

const std::map<std::string, std::string>&
        HttpRequest::get_header_field() const {
    return parser_.get_header_field();
}

int HttpRequest::get_status_code() const {
    return status_code_;
}

const std::string& HttpRequest::get_path_to_file() const {
    return path_to_file_;
}

void HttpRequest::generate_path_to_file_() {
    if (get_request_path()[get_request_path().length() - 1] == '/') {
        path_to_file_ = kBaseHtmlPath + get_request_path() + kIndexHtmlFileName;
    } else {
        path_to_file_ = kBaseHtmlPath + get_request_path();
    }
}
