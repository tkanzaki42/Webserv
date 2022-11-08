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
    if (request_->get_file_type() == FILETYPE_STATIC_HTML) {
        body_length = message_body_.get_content_length();
    } else {
        body_length = cgi_->get_content_length();
    }
    header_.set_body_length(body_length);
    header_.make_response(status_code_);

    // Configから取得したリダイレクトURL（key:status_code, value:redirect_url）
    std::map<int, std::string> redirect_url_map =
        Config::getMapIntStr(request_->get_virtual_host_index(), "return");
    // 307 Temporary Redirect / 302 Found
    // 308 Permanent Redirect / 301 Moved Permanently
    if (status_code_ == 307 || status_code_ == 302
     || status_code_ == 308 || status_code_ == 301)
        std::cout << "redirect_url_map[]:" << redirect_url_map[status_code_] << std::endl;
        header_.set_header("Location: "
            + redirect_url_map[status_code_]
            + "\r\n");
}

void HttpResponse::merge_header_and_body_() {
    // ヘッダのマージ
    response_.append(header_.get_status_line());
    std::map<std::string, std::string> header_content
            = header_.get_content();
    std::map<std::string, std::string> header_content_cgi;
    if (request_->get_file_type() != FILETYPE_STATIC_HTML) {
        header_content_cgi = cgi_->get_header_content();
    }
    for (std::map<std::string, std::string>::iterator it
                = header_content.begin();
            it != header_content.end(); it++) {
        if (header_content_cgi.count(it->first) == 0)
            response_.append(it->first + ": " + it->second);
    }
    if (request_->get_file_type() != FILETYPE_STATIC_HTML) {
        for (std::map<std::string, std::string>::iterator it
                    = header_content_cgi.begin();
                it != header_content_cgi.end(); it++) {
            response_.append(it->first + ": " + it->second);
        }
    }
    response_.append("\r\n");

    // ボディのマージ
    std::vector<std::string> body_content;
    if (request_->get_file_type() == FILETYPE_STATIC_HTML) {
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
