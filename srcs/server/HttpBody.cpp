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

std::string HttpBody::get_status_description_(int status_code) {
    if (status_code == 404)
        return std::string("The requested URL was not found on this server.");
    else if (status_code == 201)
        return std::string("Successfully uploaded the file.");
    return std::string("Unknown Error");
}

void HttpBody::make_status_response_(int status_code) {
    std::ostringstream oss_body;
    oss_body << "<html><body><h1>" << status_code << " "
        << HttpHeader::get_reason_phrase(status_code)
        << "</h1><p>" << get_status_description_(status_code)
        << "</p><hr><address>Webserv</address></body></html>\r\n";

    content_.push_back(oss_body.str());
}

void HttpBody::make_response(int status_code) {
    if (status_code != 200)
        make_status_response_(status_code);
    else
        read_contents_from_file_();
    return;

    if (output_file_.fail() != 0) {
        std::cerr << "File was not found." << std::endl;
    }
    output_file_.read(read_file_buf_, HttpBody::BUFFER_SIZE);
    content_.push_back(read_file_buf_);
    // body_content_length_ = output_file_.gcount();
}

void HttpBody::read_contents_from_file_() {
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
    int content_length = 0;

    for (std::vector<std::string>::iterator it = content_.begin();
            it != content_.end(); ++it) {
        content_length += (*it).length();
    }
    return (content_length);
}

const std::vector<std::string> &HttpBody::get_content() {
    return this->content_;
}

void HttpBody::clear_contents() {
    content_.clear();
}
