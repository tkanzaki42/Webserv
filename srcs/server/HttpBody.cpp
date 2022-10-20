#include "srcs/server/HttpBody.hpp"

int HttpBody::read_contents_from_file_() {
    // ファイルのオープン
    std::ifstream ifs_readfile;
    ifs_readfile.open(request_.get_path_to_file().c_str());

    // エラーチェック
    if (ifs_readfile.fail()) {
        // ファイルが存在しない
        std::cerr << "File not found: "
            << request_.get_path_to_file() << std::endl;
        return 404;  // Not Found
    } else if (!ifs_readfile) {
        // その他のファイルオープンエラー
        std::cerr << "Could not open file: "
            << request_.get_path_to_file() << std::endl;
        return 500;  // Internal Server Error
    }

    // ファイル読み込み
    char          read_line[256];
    while (ifs_readfile.getline(read_line, 256 - 1)) {
        content_.push_back(std::string(read_line));
    }

    ifs_readfile.close();
    return 200;
}

std::string HttpBody::get_status_description_(int status_code) {
    if (status_code == 404)
        return std::string("The requested URL was not found on this server.");
    else if (status_code == 201)
        return std::string("Successfully uploaded the file.");

    if (400 <= status_code && status_code <= 599)
        return std::string("Unknown Error");
    else
        return std::string("No message");
}

void HttpBody::make_status_response_(int status_code) {
    std::ostringstream oss_body;
    oss_body << "<html><body><h1>" << status_code << " "
        << HttpHeader::get_reason_phrase(status_code)
        << "</h1><p>" << get_status_description_(status_code)
        << "</p><hr><address>"
        << kServerSoftwareName
        << "</address></body></html>\r\n";

    content_.clear();
    content_.push_back(oss_body.str());
}

HttpBody::HttpBody(const HttpRequest& request)
        : request_(request),
          content_(std::vector<std::string>()) {
}

HttpBody::~HttpBody() {
}

HttpBody::HttpBody(const HttpBody &obj)
        : request_(obj.request_) {
    *this = obj;
}

HttpBody &HttpBody::operator=(const HttpBody &obj) {
    this->content_     = obj.content_;
    return *this;
}

int HttpBody::make_response(int status_code) {
    if (status_code == 200 && request_.get_http_method() == METHOD_GET)
        status_code = read_contents_from_file_();

    if (status_code != 200)
        make_status_response_(status_code);
    return status_code;
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
