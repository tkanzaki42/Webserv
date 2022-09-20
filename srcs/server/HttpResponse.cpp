#include "srcs/server/HttpResponse.hpp"

HttpResponse::HttpResponse(const HttpRequest& request)
        : request_(request),
          header_(HttpHeader()),
          message_body_(HttpBody()),
          response_(std::string()) {
}

HttpResponse::~HttpResponse() {
}

HttpResponse::HttpResponse(const HttpResponse &obj)
        : request_(obj.request_) {
    *this = obj;
}

HttpResponse &HttpResponse::operator=(const HttpResponse &obj) {
    this->header_       = obj.header_;
    this->message_body_ = obj.message_body_;
    this->response_     = obj.response_;
    return *this;
}

void HttpResponse::make_response() {
    // レスポンス、リクエストヘッダ、リクエストボディの初期化
    response_.clear();
    header_.clear_contents();
    message_body_.clear_contents();

    // リクエストヘッダ、リクエストボディの作成
    make_header_();
    make_message_body_();

    // リクエストヘッダ、リクエストボディの内容をレスポンスにまとめる
    std::vector<std::string> header_content = header_.get_content();
    std::vector<std::string> body_content = message_body_.get_content();
    for (std::size_t i = 0; i < header_content.size(); i++) {
        response_.append(header_content[i].c_str());
    }
    for (std::size_t i = 0; i < body_content.size(); i++) {
        response_.append(body_content[i].c_str());
    }
}
void HttpResponse::make_header_() {
    // 取得したパスのファイルを開いて内容を取得する
    message_body_.read_contents_from_file();
    // TODO(tkanzaki) ここの条件分岐はHttpHeaderクラスに書いてもいいかも
    // ポリモーフィズムを使うのもあり
    // (void)version;
    // (void)is_file_exist;
    // (void)path;

    // if (HTTP_VERSION == 1 && path != "") {
    //     return HttpHeader::make_response302(path);
    // }
    // else if (HTTP_VERSION == 2) {
    //     return HttpHeader::make_responseUpgrade();
    // }
    // else if (is_file_exist == 1) {
    //     return HttpHeader::make_response404();
    // }
    // else {
    int body_length = message_body_.get_content_length();
    header_.set_body_length(body_length);
    header_.make_response(request_.status_code);
    // }
}

void HttpResponse::make_message_body_() {
    message_body_.make_response(request_.status_code);
}

const std::string &HttpResponse::get_response() {
    return response_;
}
