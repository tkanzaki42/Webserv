#include "srcs/server/HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest *request)
        : request_(request),
          header_(HttpHeader()),
          message_body_(HttpBody(*request)),
          cgi_(NULL),
          response_(std::string()),
          status_code_(request->get_status_code()) {
}

HttpResponse::~HttpResponse() {
    if (cgi_ != NULL) {
        delete cgi_;
        cgi_ = NULL;
    }
}

HttpResponse::HttpResponse(const HttpResponse &obj)
        : message_body_(HttpBody(*obj.request_)) {
    *this = obj;
}

HttpResponse &HttpResponse::operator=(const HttpResponse &obj) {
    this->request_      = obj.request_;
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

    // リクエストヘッダ、リクエストボディの作成
    make_message_body_();
    make_header_();
    merge_header_and_body_();
}

void HttpResponse::make_message_body_() {
    // オートインデックス表示の場合
    if (request_->get_is_autoindex()) {
        message_body_.make_autoindex_response();
        status_code_ = 200;
        return;
    }

    // CGIの場合実行結果を取得する
    if (status_code_ == 200 &&
            (request_->get_file_type() == FILETYPE_SCRIPT
            || request_->get_file_type() == FILETYPE_BINARY)) {
        cgi_ = new CGI(request_);
        int cgi_ret = cgi_->exec_cgi(request_->get_file_type());
        if (cgi_ret == EXIT_FAILURE) {
            delete cgi_;
            cgi_ = NULL;
            status_code_ = 500;  // Internal Server Error
            request_->set_file_type(FILETYPE_STATIC_HTML);
        }
    }

    // 静的HTMLページの場合、およびCGI実行が失敗した場合
    if (status_code_ != 200
            || request_->get_file_type() == FILETYPE_STATIC_HTML) {
        // リクエストヘッダ、リクエストボディの作成
        status_code_ = message_body_.make_response(status_code_);
    }
}

void HttpResponse::make_header_() {
    int body_length;
    if (request_->get_file_type() == FILETYPE_SCRIPT
            || request_->get_file_type() == FILETYPE_BINARY) {
        body_length = cgi_->get_content_length();
    } else if(message_body_.is_compressed()) {
        body_length = message_body_.get_content_tail()
            - message_body_.get_content_head() + 1;
        header_.set_header("Content-Range: bytes "
            + StringConverter::itos(message_body_.get_content_head())
            + "-"
            + StringConverter::itos(message_body_.get_content_tail())
            + "/"
            + StringConverter::itos(message_body_.get_content_length())
            + "\r\n");
    }else {
        body_length = message_body_.get_content_length();
    }
    header_.set_body_length(body_length);
    header_.make_response(status_code_);

    // 仮のコンフィグ TODO(kfukuta)あとでコンフィグに置き換える
    std::map<std::string, std::string> temporary_redirect_url;
    temporary_redirect_url["./public_html/redirect_from.html"]
        = "http://127.0.0.1:5000/redirect_to.html";
    std::map<std::string, std::string> permanent_redirect_url;
    permanent_redirect_url["./public_html/redirect_from.html"]
        = "http://127.0.0.1:5000/redirect_to.html";

    // 307 Temporary Redirect / 302 Found
    if (status_code_ == 307 || status_code_ == 302)
        header_.set_header("Location: "
            + temporary_redirect_url[request_->get_path_to_file()]
            + "\r\n");

    // 308 Permanent Redirect / 301 Moved Permanently
    if (status_code_ == 308 || status_code_ == 301) {
        if (permanent_redirect_url[request_->get_path_to_file()] != "") {
            header_.set_header("Location: "
                + permanent_redirect_url[request_->get_path_to_file()]
                + "\r\n");
        } else {
            // ディレクトリ指定時の最後スラッシュなしの場合
            header_.set_header("Location: http://"
                + request_->get_header_field("Host")
                + request_->get_request_target()
                + "/\r\n");
        }
    }
}

void HttpResponse::merge_header_and_body_() {
    // ヘッダのマージ
    response_.append(header_.get_status_line());
    std::map<std::string, std::string> header_content
            = header_.get_content();
    std::map<std::string, std::string> header_content_cgi;
    if (request_->get_file_type() == FILETYPE_SCRIPT
            || request_->get_file_type() == FILETYPE_BINARY) {
        header_content_cgi = cgi_->get_header_content();
    }
    for (std::map<std::string, std::string>::iterator it
                = header_content.begin();
            it != header_content.end(); it++) {
        if (header_content_cgi.count(it->first) == 0)
            response_.append(it->first + ": " + it->second);
    }
    if (request_->get_file_type() == FILETYPE_SCRIPT
            || request_->get_file_type() == FILETYPE_BINARY) {
        for (std::map<std::string, std::string>::iterator it
                    = header_content_cgi.begin();
                it != header_content_cgi.end(); it++) {
            response_.append(it->first + ": " + it->second);
        }
    }
    response_.append("\r\n");

    // ボディのマージ
    std::vector<std::string> body_content;
    if (request_->get_file_type() == FILETYPE_SCRIPT
            || request_->get_file_type() == FILETYPE_BINARY) {
        body_content = cgi_->get_body_content();
    } else {
        body_content = message_body_.get_content();
    }
    for (std::size_t i = 0; i < body_content.size(); i++) {
        response_.append(body_content[i].c_str());
    }
}

const std::string& HttpResponse::get_response() {
    return response_;
}

bool HttpResponse::get_is_keep_alive() {
    return header_.get_is_keep_alive();
}
