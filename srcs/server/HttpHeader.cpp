#include "srcs/server/HttpHeader.hpp"

HttpHeader::HttpHeader():
content_(std::vector<std::string>()),
body_length_(0) {
}

HttpHeader::~HttpHeader() {
}

HttpHeader::HttpHeader(const HttpHeader &obj) {
    *this = obj;
}

HttpHeader &HttpHeader::operator=(const HttpHeader &obj) {
    this->content_     = obj.content_;
    this->body_length_ = obj.body_length_;
    return *this;
}

void HttpHeader::make_response200() {
    std::ostringstream oss;
    oss << "Content-Length: " << body_length_ << "\r\n";

    content_.push_back("HTTP/1.1 200 OK\r\n");
    content_.push_back("Content-Type: text/html; charset=UTF-8\r\n");
    content_.push_back(oss.str());
    content_.push_back("Connection: Keep-Alive\r\n");
    content_.push_back("\r\n");
}

// std::vector<std::string>& HttpHeader::make_response404() {
//     static std::vector<std::string> header;
//     header.push_back("HTTP/1.1 404 Not Found\r\n");
//     header.push_back("Content-Type: text/html; charset=UTF-8\r\n");
//     header.push_back("Connection: close\r\n");
//     header.push_back("\r\n");
//     header.push_back("<html><body><h1>404 Not found</h1><p>The requested URL was not found on this server.</p><hr><address>Original Server</address></body></html>\r\n");

//     return header;
// }

// std::vector<std::string>& HttpHeader::make_response302(std::string path) {
//     std::ostringstream oss;
//     if (path != "") {
//         oss << "Location: https://127.0.0.1:5001" << path.c_str() << "\r\n";
//     }
//     else {
//         oss << "Location: https://127.0.0.1:5001\r\n";
//     }
//     static std::vector<std::string> header;
//     header.push_back("HTTP/1.1 302 Found\r\n");
//     header.push_back(oss.str());
//     header.push_back("\r\n");
//     return header;
// }

// std::vector<std::string>& HttpHeader::make_responseUpgrade() {
//     static std::vector<std::string> header;
//     header.push_back("HTTP/1.1 101 Switching Protocols\r\n");
//     header.push_back("Connection: Upgrade\r\n");
//     header.push_back("Upgrade: h2c\r\n");
//     header.push_back("\r\n");

//     return header;
// }

const std::vector<std::string> &HttpHeader::get_content() {
    return this->content_;
}

void HttpHeader::set_body_length(int body_length) {
    body_length_ = body_length;
}

void HttpHeader::clear_contents() {
    content_.clear();
}
