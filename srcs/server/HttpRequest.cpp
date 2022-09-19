#include "srcs/server/HttpRequest.hpp"

void HttpRequest::analyze_request(std::string &recv_str) {
    std::size_t read_idx = 0;

    read_idx = parse_method_(recv_str, read_idx);
    read_idx = parse_request_path_(recv_str, read_idx);
    read_idx = parse_http_ver_(recv_str, read_idx);
    read_idx = parse_header_field_(recv_str, read_idx);

    generate_path_to_file_();
}

std::size_t HttpRequest::skip_space_(
        std::string &recv_str, std::size_t read_idx) {
    while (recv_str[read_idx] == ' ' && read_idx < recv_str.length()) {
        ++read_idx;
    }
    return read_idx;
}

std::size_t HttpRequest::skip_crlf_(
        std::string &recv_str, std::size_t read_idx) {
    if (recv_str[read_idx] == '\r' && read_idx < recv_str.length())
        ++read_idx;
    if (recv_str[read_idx] == '\n' && read_idx < recv_str.length())
        ++read_idx;
    return read_idx;
}

std::size_t HttpRequest::parse_method_(
        std::string &recv_str, std::size_t read_idx) {
    if (recv_str.compare(read_idx, 4, "POST") == 0) {
        method = METHOD_POST;
        read_idx += 4;
    } else if (recv_str.compare(read_idx, 3, "GET") == 0) {
        method = METHOD_GET;
        read_idx += 3;
    } else if (recv_str.compare(read_idx, 6, "DELETE") == 0) {
        method = METHOD_DELETE;
        read_idx += 6;
    }
    read_idx = skip_space_(recv_str, read_idx);
    return read_idx;
}

std::size_t HttpRequest::parse_request_path_(
        std::string &recv_str, std::size_t read_idx) {
    char buffer[1280];
    int buffer_idx = 0;

    while (recv_str[read_idx] != ' ' && read_idx < recv_str.length()) {
        buffer[buffer_idx++] = recv_str[read_idx];
        read_idx++;
    }
    buffer[buffer_idx] = '\0';
    request_path = std::string(buffer);

    read_idx = skip_space_(recv_str, read_idx);
    return read_idx;
}

std::size_t HttpRequest::parse_http_ver_(
        std::string &recv_str, std::size_t read_idx) {
    char buffer[1280];
    int buffer_idx = 0;

    if (recv_str.compare(read_idx, 5, "HTTP/") == 0) {
        read_idx += 5;
    }
    while (recv_str[read_idx] != '\n' && read_idx < recv_str.length()) {
        buffer[buffer_idx++] = recv_str[read_idx];
        read_idx++;
    }
    buffer[buffer_idx] = '\0';
    http_ver = std::string(buffer);

    read_idx = skip_space_(recv_str, read_idx);
    read_idx = skip_crlf_(recv_str, read_idx);
    return read_idx;
}

std::size_t HttpRequest::parse_header_field_(
        std::string &recv_str, std::size_t read_idx) {
    while (recv_str[read_idx] != '\r' && read_idx < recv_str.length()) {
        read_idx = parse_one_header_field_(recv_str, read_idx);
        read_idx = skip_crlf_(recv_str, read_idx);
    }
    return read_idx;
}

std::size_t HttpRequest::parse_one_header_field_(
        std::string &recv_str, std::size_t read_idx) {
    char buffer[1280];
    int buffer_idx = 0;

    while (recv_str[read_idx] != ':' && read_idx < recv_str.length()) {
        buffer[buffer_idx++] = recv_str[read_idx];
        read_idx++;
    }
    buffer[buffer_idx] = '\0';
    std::string field_name = std::string(buffer);

    read_idx = skip_space_(recv_str, ++read_idx);

    buffer_idx = 0;
    while (recv_str[read_idx] != '\r' && read_idx < recv_str.length()) {
        buffer[buffer_idx++] = recv_str[read_idx];
        read_idx++;
    }
    buffer[buffer_idx] = '\0';
    std::string field_value = std::string(buffer);
    rtrim_(field_value);

    header_field.insert(std::make_pair(field_name, field_value));
    return read_idx;
}

void HttpRequest::generate_path_to_file_() {
    if (request_path[request_path.length() - 1] == '/') {
        path_to_file = kBaseHtmlPath + request_path + kIndexHtmlFileName;
    } else {
        path_to_file = kBaseHtmlPath + request_path;
    }
}

void HttpRequest::rtrim_(std::string &str) {
    std::string const &whitespace = " \r\n\t\v\f";
    str.erase(str.find_last_not_of(whitespace) + 1);
}
