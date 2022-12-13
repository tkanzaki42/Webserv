#include "srcs/server/HttpHeader.hpp"
#include "srcs/server/StatusDescription.hpp"

HttpHeader::HttpHeader()
    : header_field_(std::map<std::string, std::string>()),
      body_length_(0),
      is_keep_alive_(true) {
}

HttpHeader::~HttpHeader() {
}

HttpHeader::HttpHeader(const HttpHeader &obj) {
    *this = obj;
}

HttpHeader &HttpHeader::operator=(const HttpHeader &obj) {
    this->header_field_  = obj.header_field_;
    this->body_length_   = obj.body_length_;
    this->is_keep_alive_ = obj.is_keep_alive_;
    return *this;
}

void HttpHeader::make_response(int status_code) {
    // is_keep_alive_を設定
    if (status_code == 200 || status_code == 201 || status_code == 401)
        is_keep_alive_ = true;
    else
        is_keep_alive_ = false;

    // ステータス行を生成 (例:HTTP/1.1 200 OK)
    std::ostringstream oss_status_line;
    oss_status_line << "HTTP/1.1 " << status_code << " "
        << StatusDescription::get_reason(status_code) << "\r\n";
    status_line_ = oss_status_line.str();

    // ヘッダ行を生成
    set_header("Content-Type: text/html; charset=UTF-8\r\n");
    if (status_code == 200 || status_code == 201 || status_code == 206) {
        std::ostringstream oss_content_length;
        oss_content_length << "Content-Length: " << body_length_ << "\r\n";

        set_header(oss_content_length.str());
    }
    if (status_code == 401) {
        // TODO(someone)
        // 設定ファイルから読み取ったメッセージを返す
        set_header("WWW-Authenticate: Basic realm=\"hogehoge\"\r\n");
    }
    if (is_keep_alive_) {
        set_header("Connection: keep-alive\r\n");
        if (status_code != 206){
            set_header("Accept-Ranges: bytes\r\n");
        }
    } else {
        set_header("Connection: close\r\n");
    }
}

std::string HttpHeader::get_status_line() {
    return status_line_;
}

const std::map<std::string, std::string> &HttpHeader::get_content() {
    return this->header_field_;
}

void HttpHeader::set_body_length(int body_length) {
    body_length_ = body_length;
}

void HttpHeader::set_header(std::string header_line) {
    std::string key;
    std::string value;

    std::string::size_type colon_pos = header_line.find(":");
    if (colon_pos == std::string::npos)
        return;

    key = header_line.substr(0, colon_pos);
    colon_pos++;  // コロンをスキップ
    if (header_line[colon_pos] == ' ')
        colon_pos++;  // コロンの後のスペースをスキップ
    value = header_line.substr(colon_pos, header_line.size() - colon_pos);

    header_field_[key] = value;
}

void HttpHeader::clear_contents() {
    header_field_.clear();
}

bool HttpHeader::get_is_keep_alive() {
    return is_keep_alive_;
}
