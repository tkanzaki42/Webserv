#include "srcs/server/HttpRequest.hpp"

void HttpRequest::analyze_request() {
    std::size_t read_idx = 0;

    read_idx = parse_method_(read_idx);
    read_idx = skip_space_(read_idx);
    read_idx = parse_request_path_(read_idx);
    read_idx = skip_space_(read_idx);

    generate_path_to_file_();
}

std::size_t HttpRequest::skip_space_(std::size_t read_idx) {
    while (recieved_line_[read_idx] == ' ' && read_idx < recieved_line_.length()) {
        ++read_idx;
    }
    return read_idx;
}

std::size_t HttpRequest::parse_method_(std::size_t read_idx) {
    int ret = read_idx;

    if (recieved_line_.compare(read_idx, 4, "POST") == 0) {
        method = METHOD_POST;
        ret += 4;
    } else if (recieved_line_.compare(read_idx, 3, "GET") == 0) {
        method = METHOD_GET;
        ret += 3;
    } else if (recieved_line_.compare(read_idx, 6, "DELETE") == 0) {
        method = METHOD_DELETE;
        ret += 6;
    }
    return ret;
}

std::size_t HttpRequest::parse_request_path_(std::size_t read_idx) {
    char path_buffer[128];
    int path_buffer_idx = 0;

    while (recieved_line_[read_idx] != ' '
        && read_idx < recieved_line_.length()) {
        path_buffer[path_buffer_idx++] = recieved_line_[read_idx];
        read_idx++;
    }
    path_buffer[path_buffer_idx] = '\0';
    request_path = std::string(path_buffer);
    return read_idx;
}

void HttpRequest::generate_path_to_file_() {
    if (request_path[request_path.length() - 1] == '/') {
        path_to_file = kBaseHtmlPath + request_path + kIndexHtmlFileName;
    } else {
        path_to_file = kBaseHtmlPath + request_path;
    }
}

int HttpRequest::recv_until_double_newline_() {
    ssize_t read_size = 0;
    char buf[BUF_SIZE];
    recieved_line_ = std::string();
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
            recieved_line_.append(buf);
        }
        if ((recieved_line_[recieved_line_.length()-4] == '\r') &&
            (recieved_line_[recieved_line_.length()-3] == '\n') &&
            (recieved_line_[recieved_line_.length()-2] == '\r') &&
            (recieved_line_[recieved_line_.length()-1] == '\n')) {
            break;
        }
    } while (read_size > 0);
    return 0;
}

void HttpRequest::set_accept_fd(int accept_fd) {
    this->accept_fd_ = accept_fd;
}

const std::string &HttpRequest::get_received_line() {
    return this->recieved_line_;
}
