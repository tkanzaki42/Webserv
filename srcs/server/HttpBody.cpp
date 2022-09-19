#include "srcs/server/HttpBody.hpp"

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

void HttpBody::make_response() {
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
    content_.clear();
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
