#include "srcs/server/HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest& request)
        : request_(request),
          header_(HttpHeader()),
          message_body_(HttpBody(request)),
          cgi_(NULL),
          response_(std::string()),
          status_code_(request.get_status_code()) {
}

HttpResponse::~HttpResponse() {
}

HttpResponse::HttpResponse(const HttpResponse &obj)
        : request_(obj.request_),
          message_body_(HttpBody(obj.request_)) {
    *this = obj;
}

HttpResponse &HttpResponse::operator=(const HttpResponse &obj) {
    this->header_       = obj.header_;
    this->message_body_ = obj.message_body_;
    this->cgi_          = obj.cgi_;
    this->response_     = obj.response_;
    this->status_code_  = obj.status_code_;
    return *this;
}

void HttpResponse::make_response() {
    // レスポンス、リクエストヘッダ、リクエストボディの初期化
    response_.clear();
    header_.clear_contents();
    message_body_.clear_contents();

    // ファイルタイプの判定
    const std::string file_extension
            = PathUtil::get_file_extension(request_.get_path_to_file());
    if (file_extension == "cgi" || file_extension == "py")
        file_type = FILETYPE_SCRIPT;
    else if (file_extension == "out")
        file_type = FILETYPE_BINARY;
    else
        file_type = FILETYPE_STATIC_HTML;

    // リクエストヘッダ、リクエストボディの作成
    make_message_body_();
    make_header_();
    merge_header_and_body_();
}

void HttpResponse::make_message_body_() {
    // CGIの場合実行結果を取得する
    if (file_type == FILETYPE_SCRIPT || file_type == FILETYPE_BINARY) {
        cgi_ = new CGI(request_);
        int cgi_ret = cgi_->exec_cgi(file_type);
        if (cgi_ret == EXIT_FAILURE) {
            delete cgi_;
            cgi_ = NULL;
            status_code_ = 500;  // Internal Server Error
            file_type = FILETYPE_STATIC_HTML;
        }
    }

    // 静的HTMLページの場合、CGI実行が失敗した場合
    if (file_type == FILETYPE_STATIC_HTML) {
        // リクエストヘッダ、リクエストボディの作成
        status_code_ = message_body_.make_response(status_code_);
    }
}

void HttpResponse::make_header_() {
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

    int body_length;
    if (file_type == FILETYPE_STATIC_HTML) {
        body_length = message_body_.get_content_length();
    } else {
        body_length = cgi_->get_content_length();
    }
    header_.set_body_length(body_length);
    header_.make_response(status_code_);
    // }
}

void HttpResponse::merge_header_and_body_() {
    // ヘッダのマージ
    std::vector<std::string> header_content = header_.get_content();
    for (std::size_t i = 0; i < header_content.size(); i++) {
        if (header_content[i] != "\r\n") {
            response_.append(header_content[i].c_str());
        }
    }
    if (file_type != FILETYPE_STATIC_HTML) {
        std::vector<std::string> header_content_cgi
                = cgi_->get_header_content();
        for (std::size_t i = 0; i < header_content_cgi.size(); i++) {
            response_.append(header_content_cgi[i].c_str());
        }
    }
    response_.append("\r\n");

    // ボディのマージ
    std::vector<std::string> body_content;
    if (file_type == FILETYPE_STATIC_HTML) {
        body_content = message_body_.get_content();
    } else {
        body_content = cgi_->get_body_content();
    }
    for (std::size_t i = 0; i < body_content.size(); i++) {
        response_.append(body_content[i].c_str());
    }
}

const std::string& HttpResponse::get_response() {
    return response_;
}
