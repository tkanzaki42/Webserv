#include "srcs/server_request/HttpParser.hpp"

HttpParser::HttpParser(const std::string& received_line)
        : read_idx_(0), received_line_(received_line) {
}

HttpParser::~HttpParser() {}

HttpParser::HttpParser(const HttpParser &obj)
        : received_line_(obj.received_line_) {
    *this = obj;
}

HttpParser& HttpParser::operator=(const HttpParser &obj) {
    read_idx_      = obj.read_idx_;
    http_method_   = obj.http_method_;
    request_path_  = obj.request_path_;
    http_ver_      = obj.http_ver_;
    header_field_  = std::map<std::string, std::string>(obj.header_field_);

    return *this;
}

int HttpParser::parse() {
    parse_method_();
    parse_request_path_();
    parse_http_ver_();
    parse_header_field_();
    int status_code = validate_parsed_data_();

    return status_code;
}

HttpMethod HttpParser::get_http_method() const {
    return http_method_;
}

const std::string& HttpParser::get_request_path() const {
    return request_path_;
}

const std::string& HttpParser::get_query_string() const {
    return query_string_;
}
const std::string& HttpParser::get_http_ver() const {
    return http_ver_;
}

const std::map<std::string, std::string>&
        HttpParser::get_header_field_map() const {
    return header_field_;
}

const std::string& HttpParser::get_header_field(const std::string& key) {
    return header_field_[key];
}

const std::string HttpParser::get_remain_buffer() {
    return received_line_.substr(
            read_idx_, received_line_.length() - read_idx_);
}

void HttpParser::parse_method_() {
    if (received_line_.compare(read_idx_, 4, "POST") == 0) {
        http_method_ = METHOD_POST;
        read_idx_ += 4;
    } else if (received_line_.compare(read_idx_, 3, "GET") == 0) {
        http_method_ = METHOD_GET;
        read_idx_ += 3;
    } else if (received_line_.compare(read_idx_, 6, "DELETE") == 0) {
        http_method_ = METHOD_DELETE;
        read_idx_ += 6;
    }
    skip_space_();
}

void HttpParser::parse_request_path_() {
    char buffer[1280];
    int buffer_idx = 0;

    while (received_line_[read_idx_] != ' '
            && read_idx_ < received_line_.length()) {
        buffer[buffer_idx++] = received_line_[read_idx_];
        read_idx_++;
    }
    buffer[buffer_idx] = '\0';
    std::string request_path_original = std::string(buffer);

    std::string remaining_path = split_query_string_(request_path_original);

    skip_space_();
}

std::string HttpParser::split_query_string_(
        std::string &request_path_original) {
    std::string remaining_path;

    std::string::size_type question_pos = request_path_original.find("?");
    if (question_pos == std::string::npos) {
        // query_stringがない場合
        remaining_path = request_path_original;
        query_string_ = "";
    } else {
        // query_stringがある場合
        remaining_path = request_path_original.substr(0, question_pos);
        query_string_ = request_path_original.substr(question_pos + 1,
            request_path_original.size() - question_pos - 1);
    }
    return remaining_path;
}

void HttpParser::parse_http_ver_() {
    char buffer[1280];
    int buffer_idx = 0;

    if (received_line_.compare(read_idx_, 5, "HTTP/") == 0) {
        read_idx_ += 5;
    }
    while (received_line_[read_idx_] != '\r'
            && read_idx_ < received_line_.length()) {
        buffer[buffer_idx++] = received_line_[read_idx_];
        read_idx_++;
    }
    buffer[buffer_idx] = '\0';
    http_ver_ = std::string(buffer);

    skip_crlf_();
}

void HttpParser::parse_header_field_() {
    while (received_line_[read_idx_] != '\r'
            && read_idx_ < received_line_.length()) {
        header_field_.insert(parse_one_header_field_());
        skip_crlf_();
    }
    skip_crlf_();
}

std::pair<std::string, std::string> HttpParser::parse_one_header_field_() {
    char buffer[1280];
    int buffer_idx = 0;

    while (received_line_[read_idx_] != ':'
            && read_idx_ < received_line_.length()) {
        buffer[buffer_idx++] = received_line_[read_idx_];
        read_idx_++;
    }
    buffer[buffer_idx] = '\0';
    std::string field_name = std::string(buffer);

    ++read_idx_;  // skip ':'
    skip_space_();

    buffer_idx = 0;
    while (received_line_[read_idx_] != '\r'
            && read_idx_ < received_line_.length()) {
        buffer[buffer_idx++] = received_line_[read_idx_];
        read_idx_++;
    }
    buffer[buffer_idx] = '\0';
    std::string field_value = std::string(buffer);
    rtrim_(field_value);

    return std::make_pair(field_name, field_value);
}

void HttpParser::skip_space_() {
    while (received_line_[read_idx_] == ' '
            && read_idx_ < received_line_.length()) {
        ++read_idx_;
    }
}

void HttpParser::skip_crlf_() {
    if (received_line_[read_idx_] == '\r'
            && read_idx_ < received_line_.length())
        ++read_idx_;
    if (received_line_[read_idx_] == '\n'
            && read_idx_ < received_line_.length())
        ++read_idx_;
}

void HttpParser::rtrim_(std::string &str) {
    std::string const &whitespace = " \r\n\t\v\f";
    str.erase(str.find_last_not_of(whitespace) + 1);
}

int HttpParser::validate_parsed_data_() {
    if (http_method_ == METHOD_NOT_DEFINED)
        return 400;  // Bad Request
    if (request_path_ == "")
        return 400;
    if (http_ver_ == "")
        return 400;
    else if (http_ver_ != "1.1")
        return 505;  // HTTP Version Not Supported
    if (header_field_.empty())
        return 400;
    return 200;
}
