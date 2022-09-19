#include "srcs/server/HttpRequest.hpp"

void HttpRequest::analyze_request(std::string &recv_str) {
    std::size_t read_idx = 0;

    read_idx = parse_method_(recv_str, read_idx);
    read_idx = skip_space_(recv_str, read_idx);
    read_idx = parse_request_path_(recv_str, read_idx);
    read_idx = skip_space_(recv_str, read_idx);

    generate_path_to_file_();
}

std::size_t HttpRequest::skip_space_(
        std::string &recv_str, std::size_t read_idx) {
    while (recv_str[read_idx] == ' ' && read_idx < recv_str.length()) {
        ++read_idx;
    }
    return read_idx;
}

std::size_t HttpRequest::parse_method_(
        std::string &recv_str, std::size_t read_idx) {
    int ret = read_idx;

    if (recv_str.compare(read_idx, 4, "POST") == 0) {
        method = METHOD_POST;
        ret += 4;
    } else if (recv_str.compare(read_idx, 3, "GET") == 0) {
        method = METHOD_GET;
        ret += 3;
    } else if (recv_str.compare(read_idx, 6, "DELETE") == 0) {
        method = METHOD_DELETE;
        ret += 6;
    }
    return ret;
}

std::size_t HttpRequest::parse_request_path_(
        std::string &recv_str, std::size_t read_idx) {
    char path_buffer[128];
    int path_buffer_idx = 0;

    while (recv_str[read_idx] != ' ' && read_idx < recv_str.length()) {
        path_buffer[path_buffer_idx++] = recv_str[read_idx];
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
