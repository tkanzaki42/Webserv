#include "srcs/server/HttpBody.hpp"

std::string HttpBody::get_error_description_(int status_code) {
    if (status_code == 404)
        return std::string("The requested URL was not found on this server.");
    return std::string("Unknown Error");
}

void HttpBody::make_error_response_(int status_code) {
    std::ostringstream oss_body;
    oss_body << "<html><body><h1>" << status_code << " "
        << HttpHeader::get_reason_phrase(status_code)
        << "</h1><p>" << get_error_description_(status_code)
        << "</p><hr><address>Webserv</address></body></html>\r\n";

    content_.push_back(oss_body.str());
}

HttpBody::HttpBody():
content_(std::vector<std::string>()) {
}

HttpBody::~HttpBody() {
}

HttpBody::HttpBody(const HttpBody &obj) {
    *this = obj;
}

HttpBody &HttpBody::operator=(const HttpBody &obj) {
    this->content_     = obj.content_;
    return *this;
}

void HttpBody::make_response(int status_code) {
    if (status_code != 200)
        make_error_response_(status_code);
    return;
    if (output_file_.fail() != 0) {
        std::cerr << "File was not found." << std::endl;
    }
    output_file_.read(read_file_buf_, HttpBody::BUFFER_SIZE);
    content_.push_back(read_file_buf_);
    // body_content_length_ = output_file_.gcount();
}

void HttpBody::read_contents_from_file() {
    // is_file_exist = -1;  // TODO(someone) remove
    // std::ifstream output_file(path_string.c_str());
    // char line[256];
    // is_file_exist = output_file.fail();
    // while (output_file.getline(line, 256-1)) {
    //     body_length += strlen(line);
    //     message_body.push_back(std::string(line));
    // }
    // 使い終わったファイルのクローズ
    // output_file.close();
    content_.push_back("hello world");  // TODO(someone) remove
}

std::size_t HttpBody::get_content_length() {
    return (content_[0].length());  // TODO(someone) remove
}

const std::vector<std::string> &HttpBody::get_content() {
    return this->content_;
}

void HttpBody::clear_contents() {
    content_.clear();
}
