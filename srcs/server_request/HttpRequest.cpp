#include "srcs/server_request/HttpRequest.hpp"
#include "srcs/util/StringConverter.hpp"

HttpRequest::HttpRequest(FDManager *fd_manager)
        : fd_manager_(fd_manager),
          auth_(HttpAuth()),
          parser_(HttpParser(received_line_)),
          file_type_(FILETYPE_NOT_DEFINED),
          status_code_(200),
          virtual_host_index_(-1),
          is_autoindex_(false) {
}

HttpRequest::~HttpRequest() {
}

HttpRequest::HttpRequest(const HttpRequest &obj)
        : fd_manager_(obj.fd_manager_),
          parser_(HttpParser(obj.parser_)) {
    *this = obj;
}

HttpRequest& HttpRequest::operator=(const HttpRequest &obj) {
    auth_               = HttpAuth();
    parser_             = HttpParser(obj.parser_);
    file_type_          = obj.file_type_;
    status_code_        = obj.status_code_;
    virtual_host_index_ = obj.virtual_host_index_;
    is_autoindex_       = obj.is_autoindex_;
    return *this;
}

int HttpRequest::receive_header() {
    ssize_t  read_size = 0;
    char     buf[BUF_SIZE];

    memset(buf, 0, sizeof(buf));
    read_size = fd_manager_->receive(buf);
    // std::cout << "read_size: " << read_size << std::endl;
    if (read_size < 0) {
        int recv_errno = errno;
        if (recv_errno == 0 || recv_errno == 9) {
            std::cerr << "connection closed by peer."
                << " recv() errno: " << recv_errno << std::endl;
        } else {
            std::cerr << "recv() failed."
                << " ERROR: " << recv_errno << std::endl;
        }
        return EXIT_FAILURE;  // 受信に失敗したので処理中断
    }
    const char *found_empty_line = strstr(buf, "\r\n\r\n");
    if (!found_empty_line) {
        std::cerr << "Failed to recognize header." << std::endl;
        status_code_ = 400;  // Bad Request
        return EXIT_SUCCESS;  // ヘッダ解析に失敗しただけ、400を返す正常ルート
    }
    received_line_.append(buf);

    return EXIT_SUCCESS;
}

void HttpRequest::analyze_request() {
    // リクエストのパース
    status_code_ = parser_.parse();

    // HTTPバージョンの確認
    // TODO(someone)
    // virtual_host_index_の設定
    this->virtual_host_index_ =
         Config::getVirtualServerIndex(parser_.get_host_name());
    // デフォルトパスの設定
    parser_.setIndexHtmlFileName
        (Config::getVectorStr(this->virtual_host_index_, "index"));
    parser_.setBaseHtmlPath
        (Config::getSingleStr(this->virtual_host_index_, "root"));

    // パースした情報からQUERY_STRING、PATH_INFOを切り出し
    parser_.separate_querystring_pathinfo();

    // パスの補完(末尾にindex.htmlをつけるなど)
    parser_.autocomplete_path();

    // ファイル存在チェック
    if (!PathUtil::is_file_exists(get_path_to_file())) {
        std::cerr << "File not found: " << get_path_to_file() << std::endl;
        status_code_ = 404;  // Not Found
    }

    // リダイレクト確認
    if (status_code_ == 200 || status_code_ == 404)
        check_redirect_();

    // 認証の確認
    check_authorization_();

    // オートインデックスの実施
    bool autoindex = true;  // TODO(kfukuta) コンフィグで指定
    if (status_code_ == 404 && autoindex == true)
        is_autoindex_ = true;

    // ファイルタイプの判定
    const std::string file_extension
            = PathUtil::get_file_extension(get_path_to_file());
    if (file_extension == "cgi" || file_extension == "py")
        file_type_ = FILETYPE_SCRIPT;
    else if (file_extension == "out")
        file_type_ = FILETYPE_BINARY;
    else
        file_type_ = FILETYPE_STATIC_HTML;

    // エラーの場合は判定終了
    if (status_code_ != 200) {
        return;
    }

    // POSTの場合データを読む、DELETEの場合ファイルを削除する
    if (get_http_method() == METHOD_POST) {
        if (file_type_ == FILETYPE_STATIC_HTML) {
            status_code_ = receive_and_store_to_file_();
        } else {
            // TODO(someone) QUERY_STRINGをPOSTデータから読む処理を追加
        }
    } else if (get_http_method() == METHOD_DELETE) {
        status_code_ = delete_file_();
    }
}

void HttpRequest::print_debug() {
    typedef std::map<std::string, std::string>::const_iterator map_iter;

    std::cout << "//-----received_line_ start-----" << std::endl;
    std::cout << received_line_ << std::endl;
    std::cout << "\\\\-----received_line_ end-----" << std::endl;
    std::cout << std::endl;

    std::cout << "[request data]" << std::endl;
    std::cout << "  http_method_      : "
        << parser_.get_http_method() << std::endl;
    std::cout << "  request_target_   : "
        << parser_.get_request_target() << std::endl;
    std::cout << "  base_html_path    : "
        << parser_.getBaseHtmlPath() << std::endl;
    std::cout << "  query_string_     : "
        << parser_.get_query_string() << std::endl;
    std::cout << "  path_to_file_     : "
        << parser_.get_path_to_file() << std::endl;
    std::cout << "  path_info_        : "
        << parser_.get_path_info() << std::endl;
    std::cout << "  http_ver_         : "
        << parser_.get_http_ver() << std::endl;

    std::cout << "  header_field_  :" << std::endl;
    for (map_iter it = parser_.get_header_field_map().begin();
            it != parser_.get_header_field_map().end(); it++) {
        std::cout << "    " << it->first << ": " << it->second << std::endl;
    }
    std::cout << std::endl;
}

HttpMethod HttpRequest::get_http_method() const {
    return parser_.get_http_method();
}

const std::string& HttpRequest::get_request_target() const {
    return parser_.get_request_target();
}

const std::string& HttpRequest::get_query_string() const {
    return parser_.get_query_string();
}

const std::string& HttpRequest::get_path_info() const {
    return parser_.get_path_info();
}

const std::string& HttpRequest::get_path_to_file() const {
    return parser_.get_path_to_file();
}

const std::string& HttpRequest::get_http_ver() const {
    return parser_.get_http_ver();
}

const std::string& HttpRequest::get_header_field(const std::string& key) {
    return parser_.get_header_field(key);
}

const std::map<std::string, std::string>&
        HttpRequest::get_header_field_map() const {
    return parser_.get_header_field_map();
}

FileType HttpRequest::get_file_type() const {
    return file_type_;
}

int HttpRequest::get_status_code() const {
    return status_code_;
}

int HttpRequest::get_virtual_host_index() const {
    return this->virtual_host_index_;
}

bool HttpRequest::get_is_autoindex() const {
    return is_autoindex_;
}

struct sockaddr_in HttpRequest::get_client_addr() {
    return fd_manager_->get_client_addr();
}

void HttpRequest::set_file_type(FileType file_type) {
    file_type_ = file_type;
}

void HttpRequest::check_authorization_() {
    // TODO(someone)
    // コンフィグに認証設定がなかったらなにもしない
    // Nginx で Basic 認証(https://qiita.com/kotarella1110/items/be76b17cdbe61ff7b5ca)
    if (BASIC_AUTH == false) {
        return ;
    }
    // リクエストにAuthorizationヘッダがあるかどうか
    const std::map<std::string, std::string>& map = parser_.get_header_field_map();
    if (map.count(std::string("Authorization")) == 0){
        status_code_ = 401;
        return ;
    }

    // Basic認証でなければ400
    auth_.set_client_info(map.at(std::string("Authorization")));
    if (auth_.check_auth_type() != AUTH_BASIC) {
        status_code_ = 400;
        return ;
    }

    // Authorizationヘッダをbase64デコードしたものと/configs/.htpasswdに書かれたユーザーパスを照合する
    if (!auth_.do_basic()) {
        status_code_ = 401;
        return ;
    }
}

void HttpRequest::check_redirect_() {
    // ディレクトリ指定で最後のスラッシュがない場合
    if (get_path_to_file()[get_path_to_file().size() - 1] != '/'
            && PathUtil::is_folder_exists(get_path_to_file()) == true) {
        status_code_ = 301;  // Moved Permanently
    }

    // 仮のコンフィグ TODO(kfukuta)あとでコンフィグに置き換える
    std::map<std::string, std::string> temporary_redirect_url;
    temporary_redirect_url["./public_html/redirect_from.html"]
        = "http://127.0.0.1:5000/redirect_to.html";
    std::map<std::string, std::string> permanent_redirect_url;
    permanent_redirect_url["./public_html/redirect_from.html"]
        = "http://127.0.0.1:5000/redirect_to.html";

    if (temporary_redirect_url[get_path_to_file()] != "") {
        if (get_http_method() == METHOD_POST)
            status_code_ = 307;  // Temporary Redirect
        else
            status_code_ = 302;  // Found
    }
    if (permanent_redirect_url[get_path_to_file()] != "") {
        if (get_http_method() == METHOD_POST)
            status_code_ = 308;  // Permanent Redirect
        else
            status_code_ = 301;  // Moved Permanently
    }
}

int HttpRequest::receive_and_store_to_file_() {
    // ディレクトリがなければ作成
    if (PathUtil::is_folder_exists(TMP_POST_DATA_DIR) == false) {
        if (mkdir(TMP_POST_DATA_DIR, S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
            std::cerr << "Could not create dirctory: " << TMP_POST_DATA_DIR << std::endl;
            return 500;  // Internal Server Error
        }
    }

    // ファイルのオープン
    std::ofstream ofs_outfile;
    ofs_outfile.open(TMP_POST_DATA_FILE,
            std::ios::out | std::ios::binary | std::ios::trunc);
    if (!ofs_outfile) {
        std::cerr << "Could not open file during receiving the file: "
            << get_path_to_file() << std::endl;
        return 500;  // Internal Server Error
    }

    // ヘッダ読み込み時にバッファに残っている分を書きだす
    std::string remain_buffer = parser_.get_remain_buffer();
    ofs_outfile.write(remain_buffer.c_str(), remain_buffer.length());

    // 受信しながらファイルに書き出し
    ssize_t total_read_size = remain_buffer.length();
    ssize_t read_size = 0;
    char    buf[BUF_SIZE];
    do {
        if (total_read_size > REQUEST_ENTITY_MAX) {
            // デフォルト値1MB以上なら413
            ofs_outfile.close();
            std::remove(TMP_POST_DATA_FILE);
            return 413;
        } else if (total_read_size
                >= StringConverter::stoi(parser_.get_header_field("Content-Length"))
            ) {
            break;
        }
        read_size = fd_manager_->receive(buf);
        if (read_size == -1) {
            std::cerr << "recv() failed in "
                << "receive_and_store_to_file_()." << std::endl;
            // close(accept_fd_);
            // accept_fd_ = -1;
            fd_manager_->disconnect();
            return -1;
        }
        if (read_size > 0) {
            buf[read_size] = '\0';
            ofs_outfile.write(buf, read_size);
            total_read_size += read_size;
            std::cout << "read_size:" << read_size
                << ", total:" << total_read_size << std::endl;
        }
    } while (read_size > 0);

    ofs_outfile.close();
    return 201;  // Created
}

int HttpRequest::delete_file_() {
    if (std::remove(get_path_to_file().c_str()) != 0) {
        std::cerr << "Failed to delete file: "
            << get_path_to_file() << std::endl;
        return 204;  // No Content
    }
    return 204;  // No Content
}
