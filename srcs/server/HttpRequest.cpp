#include "srcs/server/HttpRequest.hpp"

HttpRequest::HttpRequest() :
    http_method(NOT_DEFINED), request_path(""), path_to_file(""),
    http_ver(""), status_code(200), received_line_(""), accept_fd_(-1) {
}

HttpRequest::~HttpRequest() {
}

HttpRequest::HttpRequest(const HttpRequest &obj) {
    *this = obj;
}

HttpRequest& HttpRequest::operator=(const HttpRequest &obj) {
    this->accept_fd_     = obj.accept_fd_;
    this->received_line_ = obj.received_line_;

    this->http_method    = obj.http_method;
    this->request_path   = obj.request_path;
    this->path_to_file   = obj.path_to_file;
    this->http_ver       = obj.http_ver;
    this->header_field   = std::map<std::string, std::string>(obj.header_field);

    return *this;
}

const std::string& HttpRequest::get_received_line() {
    return this->received_line_;
}

void HttpRequest::set_accept_fd(int accept_fd) {
    this->accept_fd_ = accept_fd;
}

int HttpRequest::recv_until_double_newline_() {
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
            received_line_.append(buf);
        }
        if ((received_line_[received_line_.length()-4] == '\r') &&
            (received_line_[received_line_.length()-3] == '\n') &&
            (received_line_[received_line_.length()-2] == '\r') &&
            (received_line_[received_line_.length()-1] == '\n')) {
            break;
        }
    } while (read_size > 0);
    return 0;
}

void HttpRequest::analyze_request() {
    std::size_t read_idx = 0;

    read_idx = parse_method_(read_idx);
    read_idx = parse_request_path_(read_idx);
    read_idx = parse_http_ver_(read_idx);
    read_idx = parse_header_field_(read_idx);

    generate_path_to_file_();
}

void HttpRequest::print_debug() {
    typedef std::map<std::string, std::string>::iterator map_iter;

    std::cout << "//-----received_line_ start-----" << std::endl;
    std::cout << received_line_ << std::endl;
    std::cout << "\\\\-----received_line_ end-----" << std::endl;
    std::cout << std::endl;

    std::cout << "[req]" << std::endl;
    std::cout << "  http_method   : " << http_method << std::endl;
    std::cout << "  request_path  : " << request_path << std::endl;
    std::cout << "  base_html_path: " << kBaseHtmlPath << std::endl;
    std::cout << "  path_to_file  : " << path_to_file << std::endl;
    std::cout << "  http_ver      : " << http_ver << std::endl;

    std::cout << "  header_field  :" << std::endl;
    for (map_iter it = header_field.begin();
            it != header_field.end(); it++) {
        std::cout << "    " << it->first << ": " << it->second << std::endl;
    }
    std::cout << std::endl;
}

std::size_t HttpRequest::skip_space_(std::size_t read_idx) {
    while (received_line_[read_idx] == ' '
            && read_idx < received_line_.length()) {
        ++read_idx;
    }
    return read_idx;
}

std::size_t HttpRequest::skip_crlf_(std::size_t read_idx) {
    if (received_line_[read_idx] == '\r' && read_idx < received_line_.length())
        ++read_idx;
    if (received_line_[read_idx] == '\n' && read_idx < received_line_.length())
        ++read_idx;
    return read_idx;
}

std::size_t HttpRequest::parse_method_(std::size_t read_idx) {
    if (received_line_.compare(read_idx, 4, "POST") == 0) {
        http_method = METHOD_POST;
        read_idx += 4;
    } else if (received_line_.compare(read_idx, 3, "GET") == 0) {
        http_method = METHOD_GET;
        read_idx += 3;
    } else if (received_line_.compare(read_idx, 6, "DELETE") == 0) {
        http_method = METHOD_DELETE;
        read_idx += 6;
    }
    read_idx = skip_space_(read_idx);
    return read_idx;
}

std::size_t HttpRequest::parse_request_path_(std::size_t read_idx) {
    char buffer[1280];
    int buffer_idx = 0;

    while (received_line_[read_idx] != ' '
            && read_idx < received_line_.length()) {
        buffer[buffer_idx++] = received_line_[read_idx];
        read_idx++;
    }
    buffer[buffer_idx] = '\0';
    request_path = std::string(buffer);

    read_idx = skip_space_(read_idx);
    return read_idx;
}

std::size_t HttpRequest::parse_http_ver_(std::size_t read_idx) {
    char buffer[1280];
    int buffer_idx = 0;

    if (received_line_.compare(read_idx, 5, "HTTP/") == 0) {
        read_idx += 5;
    }
    while (received_line_[read_idx] != '\r'
            && read_idx < received_line_.length()) {
        buffer[buffer_idx++] = received_line_[read_idx];
        read_idx++;
    }
    buffer[buffer_idx] = '\0';
    http_ver = std::string(buffer);

    read_idx = skip_crlf_(read_idx);
    return read_idx;
}

std::size_t HttpRequest::parse_header_field_(std::size_t read_idx) {
    while (received_line_[read_idx] != '\r'
            && read_idx < received_line_.length()) {
        read_idx = parse_one_header_field_(read_idx);
        read_idx = skip_crlf_(read_idx);
    }
    return read_idx;
}

std::size_t HttpRequest::parse_one_header_field_(std::size_t read_idx) {
    char buffer[1280];
    int buffer_idx = 0;

    while (received_line_[read_idx] != ':'
            && read_idx < received_line_.length()) {
        buffer[buffer_idx++] = received_line_[read_idx];
        read_idx++;
    }
    buffer[buffer_idx] = '\0';
    std::string field_name = std::string(buffer);

    ++read_idx;  // skip ':'
    read_idx = skip_space_(read_idx);

    buffer_idx = 0;
    while (received_line_[read_idx] != '\r'
            && read_idx < received_line_.length()) {
        buffer[buffer_idx++] = received_line_[read_idx];
        read_idx++;
    }
    buffer[buffer_idx] = '\0';
    std::string field_value = std::string(buffer);
    rtrim_(field_value);

    header_field.insert(std::make_pair(field_name, field_value));
    return read_idx;
}

void HttpRequest::rtrim_(std::string &str) {
    std::string const &whitespace = " \r\n\t\v\f";
    str.erase(str.find_last_not_of(whitespace) + 1);
}

void HttpRequest::generate_path_to_file_() {
    if (request_path[request_path.length() - 1] == '/') {
        path_to_file = kBaseHtmlPath + request_path + kIndexHtmlFileName;
    } else {
        path_to_file = kBaseHtmlPath + request_path;
    }
}
