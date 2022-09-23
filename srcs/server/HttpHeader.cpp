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

void HttpHeader::make_response(int status_code) {
    std::ostringstream oss_status_line;
    oss_status_line << "HTTP/1.1 " << status_code << " "
        << get_reason_phrase(status_code) << "\r\n";
    content_.push_back(oss_status_line.str());
    content_.push_back("Content-Type: text/html; charset=UTF-8\r\n");
    if (status_code == 200 || status_code == 201) {
        std::ostringstream oss_content_length;
        oss_content_length << "Content-Length: " << body_length_ << "\r\n";

        content_.push_back(oss_content_length.str());
        content_.push_back("Connection: Keep-Alive\r\n");
    } else {
        content_.push_back("Connection: close\r\n");
    }
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

// RFC9110のステータスコードで作成している
// https://httpwg.org/specs/rfc9110.html#rfc.section.15
std::string HttpHeader::get_reason_phrase(int status_code) {
    if (status_code == 100)
        return std::string("Continue");
    else if (status_code == 101)
        return std::string("Switching Protocols");
    else if (status_code == 200)
        return std::string("OK");
    else if (status_code == 201)
        return std::string("Created");
    else if (status_code == 202)
        return std::string("Accepted");
    else if (status_code == 203)
        return std::string(" Non-Authoritative Information");
    else if (status_code == 204)
        return std::string("No Content");
    else if (status_code == 205)
        return std::string("Reset Content");
    else if (status_code == 206)
        return std::string("Partial Content");
    else if (status_code == 300)
        return std::string("Multiple Choices");
    else if (status_code == 301)
        return std::string("Moved Permanently");
    else if (status_code == 302)
        return std::string("Found");
    else if (status_code == 303)
        return std::string("See Other");
    else if (status_code == 304)
        return std::string("Not Modified");
    else if (status_code == 305)
        return std::string("Use Proxy");
    else if (status_code == 307)
        return std::string("Temporary Redirect");
    else if (status_code == 308)
        return std::string("Permanent Redirect");
    else if (status_code == 400)
        return std::string("Bad Request");
    else if (status_code == 401)
        return std::string("Unauthorized");
    else if (status_code == 402)
        return std::string("Payment Required");
    else if (status_code == 403)
        return std::string("Forbidden");
    else if (status_code == 404)
        return std::string("Not Found");
    else if (status_code == 405)
        return std::string("Method Not Allowed");
    else if (status_code == 406)
        return std::string("Not Acceptable");
    else if (status_code == 407)
        return std::string("Proxy Authentication Required");
    else if (status_code == 408)
        return std::string("Request Timeout");
    else if (status_code == 409)
        return std::string("Conflict");
    else if (status_code == 410)
        return std::string("Gone");
    else if (status_code == 411)
        return std::string("Length Required");
    else if (status_code == 412)
        return std::string("Precondition Failed");
    else if (status_code == 413)
        return std::string("Content Too Large");
    else if (status_code == 414)
        return std::string("URI Too Long");
    else if (status_code == 415)
        return std::string("Unsupported Media Type");
    else if (status_code == 416)
        return std::string("Range Not Satisfiable");
    else if (status_code == 417)
        return std::string("Expectation Failed");
    else if (status_code == 421)
        return std::string("Misdirected Request");
    else if (status_code == 422)
        return std::string("Unprocessable Content");
    else if (status_code == 426)
        return std::string("Upgrade Required");
    else if (status_code == 500)
        return std::string("Internal Server Error");
    else if (status_code == 502)
        return std::string("Bad Gateway");
    else if (status_code == 503)
        return std::string("Service Unavailable");
    else if (status_code == 504)
        return std::string("Gateway Timeout");
    else if (status_code == 505)
        return std::string("HTTP Version Not Supported");
    return std::string("Unknown Error");
}
