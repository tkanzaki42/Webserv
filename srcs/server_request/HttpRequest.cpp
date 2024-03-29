#include "srcs/server_request/HttpRequest.hpp"

HttpRequest::HttpRequest()
        : auth_(HttpAuth()),
          parser_(HttpParser(received_line_)),
          file_type_(FILETYPE_NOT_DEFINED),
          status_code_(200),
          virtual_host_index_(-1),
          is_autoindex_(false),
          client_max_body_size(-1),
          is_header_analyzed_(false),
          upload_data_("") {
}

HttpRequest::~HttpRequest() {
}

HttpRequest::HttpRequest(const HttpRequest &obj)
        : parser_(HttpParser(obj.parser_)) {
    *this = obj;
}

HttpRequest& HttpRequest::operator=(const HttpRequest &obj) {
    auth_                = HttpAuth();
    parser_              = HttpParser(obj.parser_);
    file_type_           = obj.file_type_;
    status_code_         = obj.status_code_;
    virtual_host_index_  = obj.virtual_host_index_;
    is_autoindex_        = obj.is_autoindex_;
    client_max_body_size = obj.client_max_body_size;
    is_header_analyzed_  = obj.is_header_analyzed_;
    upload_data_         = obj.upload_data_;
    return *this;
}

void HttpRequest::reset() {
    auth_               = HttpAuth();
    received_line_      = "";
    parser_             = HttpParser(received_line_);
    location_           = "";
    file_type_          = FILETYPE_NOT_DEFINED;
    status_code_        = 200;
    virtual_host_index_ = -1;
    is_autoindex_       = false;
    redirect_pair_      = std::make_pair(0, "");
    upload_dir          = "";
    client_addr_        = sockaddr_in();
    is_header_analyzed_ = false;
    upload_data_        = "";
}

// 戻り値 true : 読み込み終了
//       false  : 継続読み込み
bool HttpRequest::receive_header(const char *buf) {
    std::string buf_str = std::string(buf);
    if (buf_str == "\r\n"
            && received_line_.length() == 0) {
        // skip
        return false;
    }
    if (received_line_.length() == 0) {
        std::string::size_type pos = buf_str.find("\r\n");
        int status_code = validate_received_header_line_(buf_str.substr(0, pos));
        if (status_code != 200) {
            status_code_ = status_code;
            return true;
        }
    }

    received_line_ += buf_str;

    std::string crlfstr = "\r\n\r\n";
    if (received_line_.length() < crlfstr.length()) {
        // ヘッダ読み込み途中の場合
        return false;
    }
    if (received_line_.rfind(crlfstr) != std::string::npos) {
        // 最後が改行2個連続の場合
        if (received_line_.length() == 0) {
            // 通信開始直後の空Enter
            return false;
        } else {
            // ヘッダ正常読み込み終了
            return true;
        }
    } else {
        if (received_line_.length() > BUF_SIZE) {
            // 十分なサイズ分すでに読み込んでいる場合
            std::cerr << "Failed to recognize header." << std::endl;
            status_code_ = 400;  // Bad Request
            return true;  // ヘッダ解析に失敗しただけ、400正常ルート
        } else {
            // ヘッダ読み込み途中の場合
            return false;
        }
    }
}

bool HttpRequest::is_allowed_method(std::vector<std::string> v) {
    // メソッドの制限なし
    if (v.empty()) {
        return (true);
    }
    std::string method_string;
    switch (get_http_method()) {
        case 1:
            method_string = "POST";
            break;
        case 2:
            method_string = "GET";
            break;
        case 3:
            method_string = "DELETE";
            break;
        default:
            method_string = "NOT_DEFINED";
    }
    std::vector<std::string>::iterator begin = v.begin();
    std::vector<std::string>::iterator end = v.end();
    for (std::vector<std::string>::iterator itr = begin; itr != end; itr++) {
        if (*itr == method_string) {
            return (true);
        }
    }
    return (false);
}

int HttpRequest::validate_received_header_line_(const std::string &buf) {
    std::vector<std::string> split_str;
    std::stringstream        ss(buf);

    std::string              str;
    while (getline(ss, str, ' ')){
        split_str.push_back(str);
    }
    if (split_str.size() != 3) {
        // 空白の数が間違ってる
        return 400;
    }
    if (split_str[1][0] != '/') {
        // 対応しているメソッド "/で始まらないパス" 対応しているプロトコル(不正なフォーマット)
        return 400;
    }
    if (split_str[2] != std::string("HTTP/1.1")) {
        // 対応しているメソッド "/で始まるパス" 対応してないプロトコル
        return 505;
    }
    return 200;
}


void HttpRequest::analyze_request(int port) {
    // リクエストのパース
    status_code_ = parser_.parse();
    // virtual_host_index_の設定
    this->virtual_host_index_ =
         Config::getVirtualHostIndex(parser_.get_host_name(),
            StringConverter::itos(port));
    // client_max_body_sizeの決定
    this->client_max_body_size =
     Config::getSingleInt(get_virtual_host_index(), "client_max_body_size");
    // Locationの決定
    std::string path = get_request_target();
    std::vector<std::string> v =
     Config::getAllLocation(virtual_host_index_);

    // リダイレクト確認
    std::string redirectLocation
     = Config::isReturn(virtual_host_index_, path, v);
    if (redirectLocation.size()) {
        location_ = redirectLocation;
        check_redirect_();
        return;
    }
    location_ = Config::findLongestMatchLocation(path, v);
    if (!location_.size()) {
        status_code_ = 403;
        return;
    }
    std::string root =
        Config::getLocationString(virtual_host_index_, location_, "root");
    // もしrootが見つからなかった場合
    if (!root.size()) {
        root = "/";
    }

    std::vector<std::string> method =
     Config::getLocationVector(virtual_host_index_, location_, "limit_except");
    upload_dir =
     Config::getLocationString(virtual_host_index_, location_, "upload_store");
    if (upload_dir.size() && upload_dir[upload_dir.size() - 1] != '/') {
        // /で終わってなかったら追加する
        upload_dir += "/";
    }
    if (!is_allowed_method(method)) {
        status_code_ = 405;
        is_header_analyzed_ = true;
        return;
    }

    // デフォルトパスの設定
    parser_.setIndexHtmlFileName
        (Config::getLocationVector(virtual_host_index_, location_, "index"));
    parser_.setBaseHtmlPath(root);

    std::string path_not_autocompleted = replacePathToLocation_(location_, path, root);
    parser_.setPathToFile(path_not_autocompleted);
    // パスの補完(末尾にindex.htmlをつけるなど)
    parser_.autocomplete_path();

    // パースした情報からQUERY_STRING、PATH_INFOを切り出し
    std::vector<std::string> cgi_extension =
     Config::getLocationVector(virtual_host_index_, location_, "cgi_extension");
    parser_.separate_querystring_pathinfo(cgi_extension);

    // ファイル存在チェック
    if (get_http_method() != METHOD_POST
            && !PathUtil::is_file_exists(get_path_to_file())) {
        std::cerr << "File not found: " << get_path_to_file() << std::endl;
        status_code_ = 404;  // Not Found
    }
    bool is_folder_existes = PathUtil::is_folder_exists(path_not_autocompleted);
    if (path_not_autocompleted[path_not_autocompleted.size() - 1] != '/' && is_folder_existes) {
        // ディレクトリ指定で最後のスラッシュがない場合は末尾にスラッシュをつけて返す
        status_code_ = 301;  // Moved Permanently
        redirect_pair_.first = 301;
        redirect_pair_.second = "http://"
                 + get_header_field("Host") + get_request_target() + "/";
        std::cout << redirect_pair_.second << std::endl;
        is_header_analyzed_ = true;
        return;
    }

    // 認証の確認
    check_authorization_();

    // autoindex
    is_folder_existes = PathUtil::is_folder_exists(get_path_to_file());
    bool autoindex = Config::getAutoIndex(virtual_host_index_, location_);
    if (get_http_method() != METHOD_POST) {
        if (!autoindex && is_folder_existes) {
            status_code_ = 403;
        } else if (autoindex == true && is_folder_existes) {
            is_autoindex_ = true;
        }
    }

    // ファイルタイプの判定
    const std::string file_extension
            = PathUtil::get_file_extension(get_path_to_file());
    if ((file_extension == "cgi" || file_extension == "py") &&
        PathUtil::is_set_cgi_extension(cgi_extension, file_extension))
        file_type_ = FILETYPE_SCRIPT;
    else if (file_extension == "out" &&
        PathUtil::is_set_cgi_extension(cgi_extension, file_extension))
        file_type_ = FILETYPE_BINARY;
    else
        file_type_ = FILETYPE_STATIC_HTML;

    // DELETEの場合ファイルを削除する
    if (status_code_ == 200 && get_http_method() == METHOD_DELETE) {
        status_code_ = delete_file_();
    }

#ifdef DEBUG
    std::cout << "Header analyzed." << std::endl;
#endif
    is_header_analyzed_ = true;
}

std::string HttpRequest::replacePathToLocation_(std::string &location,
                                              std::string &path,
                                              std::string &root) {
    std::string newUrl;
    if (!location.size()) {
        return ("");
    }
    if (location.size() > path.size()) {
        return (root);
    }
    if (location[location.size() - 1] == '/') {
        newUrl = root + path.substr(location.size() - 1, path.size());
    } else {
        newUrl = root + path.substr(location.size(), path.size());
    }
    return (newUrl);
}

void HttpRequest::print_debug() {
#ifdef DEBUG
    typedef std::map<std::string, std::string>::const_iterator map_iter;
    std::cout << "//-----received_line_ start-----" << std::endl;
    std::cout << received_line_ << std::endl;
    std::cout << "\\\\-----received_line_ end-----" << std::endl;
    std::cout << std::endl;

    std::cout << "[request data]" << std::endl;
    std::cout << "  http_method_      : " << parser_.get_http_method();
    if (parser_.get_http_method() == METHOD_NOT_DEFINED)
        std::cout << " [not defined]" << std::endl;
    else if (parser_.get_http_method() == METHOD_POST)
        std::cout << " [POST]" << std::endl;
    else if (parser_.get_http_method() == METHOD_GET)
        std::cout << " [GET]" << std::endl;
    else if (parser_.get_http_method() == METHOD_DELETE)
        std::cout << " [DELETE]" << std::endl;
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
#endif
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

const std::string& HttpRequest::get_location() const {
    return (this->location_);
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
    return client_addr_;
}

const std::pair<int , std::string> HttpRequest::get_redirect_pair() const {
    return (this->redirect_pair_);
}

void HttpRequest::set_file_type(FileType file_type) {
    file_type_ = file_type;
}

void HttpRequest::set_client_addr(struct sockaddr_in  client_addr) {
    client_addr_ = client_addr;
}

bool HttpRequest::get_is_header_analyzed() {
    return is_header_analyzed_;
}

const std::string& HttpRequest::get_upload_dir() {
    return upload_dir;
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
        // int status_code, std::string リダイレクト先
        this->redirect_pair_ =
            Config::getRedirectPair(virtual_host_index_, location_);
        status_code_ = this->redirect_pair_.first;
}

bool HttpRequest::receive_and_store_to_file(bool is_not_readed_header, const char *buf) {
    // ヘッダ読み込み時にバッファに残っている分をコピー
    if (upload_data_ == "") {
        upload_data_ = parser_.get_remain_buffer();
    }

    // データ受信
    bool receive_ret = false;
    if (parser_.get_header_field("Transfer-Encoding").compare("chunked") == 0) {
        receive_ret = receive_chunked_data_(is_not_readed_header, buf);
        if (receive_ret == false)
            return false;  // 継続読み込み
    } else  {
        // Content-Lengthがあってもなくても通常のデータ受信モード
        receive_ret = receive_plain_data_(is_not_readed_header, buf);
        if (receive_ret == false)
            return false;  // 継続読み込み
        // receive_plain_data_()内でステータスコードがエラーになった場合
        if (status_code_ != 200) {
            return true;  // 読み込み終了
        }
        // ファイル書き出し
        if (write_to_file_() == false) {
            status_code_ = 500;
            return true;  // 読み込み終了
        }
        status_code_ = 201;  // Created
    }

    return true;  // 読み込み終了
}

bool HttpRequest::receive_chunked_data_(bool is_not_readed_header, const char *buf) {
    enum ReadMode {
        READMODE_CHUNKSIZE = 0,
        READMODE_DATA = 1
    };
    enum ReadMode read_mode = READMODE_CHUNKSIZE;
    std::pair<int, int> chunk_size;

    if (is_not_readed_header) {
        // 初回実行時
        // 前のファイルを削除する
        std::remove((upload_dir + TMP_POST_DATA_FILE).c_str());

    } else {
        // 2回目以降実行時
        // データ受信
        upload_data_ += buf;
    }

    while (true) {
        if (read_mode == READMODE_CHUNKSIZE) {
            // チャンクサイズ読み込み
            if (upload_data_.find("\r\n") == std::string::npos) {
                // 解析不能のため抜ける
                return false;  // 継続読み込み
            }
            chunk_size = split_chunk_size_();
            if (chunk_size.first == -1) {
                std::remove((upload_dir + TMP_POST_DATA_FILE).c_str());
                status_code_ = chunk_size.second;
                return true;  // 読み込み終了
            } else if (chunk_size.first == 0) {
                // チャンクサイズが0の場合、データの終了を意味する
                status_code_ = 201;  // Created
                return true;  // 読み込み終了
            } else if (client_max_body_size != -1
                    && static_cast<int>(upload_data_.length())
                    + chunk_size.first
                    > client_max_body_size) {
                // デフォルト値1MB以上なら413
                std::remove((upload_dir + TMP_POST_DATA_FILE).c_str());
                status_code_ = 413;  // Payload Too Large
                return true;  // 読み込み終了
            }

#ifdef DEBUG
            std::cout << "  chunk_size: " << chunk_size.first << std::endl;
#endif

            // モード変更
            read_mode = READMODE_DATA;
        }

        if (read_mode == READMODE_DATA) {
            // チャンクサイズ分すでに読み込まれていない場合
            if (static_cast<int>(upload_data_.length()) - chunk_size.second
                    < chunk_size.first) {
                return false;
            }

            // チャンクサイズ部分をバッファから削除
            // +2は"\r\n"分
            upload_data_.erase(0, chunk_size.second + 2);

            // チャンクサイズ分のデータを読み込んでファイルに書き出し
            if (write_to_file_append_(chunk_size.first) == false) {
                status_code_ = 400;  // Bad Request
                return true;  // 読み込み終了
            }
#ifdef DEBUG
            std::cout << "  write data size:" << chunk_size.first << std::endl;
#endif

            // 書き出した分をバッファから削除
            // +2は"\r\n"分
            upload_data_.erase(0, chunk_size.first + 2);

            // モード変更
            read_mode = READMODE_CHUNKSIZE;
        }
    }
}

// 戻り値 first   エラーの場合-1、正常の場合チャンクサイズ
//       second エラーの場合ステータスコード、正常の場合チャンクサイズ部分の文字数
std::pair<int, int> HttpRequest::split_chunk_size_() {
    int chunk_size = 0;

    // チャンクサイズ部分を読み込み
    int i = 0;
    while (i < static_cast<int>(upload_data_.length())
             && upload_data_.at(i) != '\r') {

        if ('a' <= upload_data_.at(i) && upload_data_.at(i) <= 'f') {
            chunk_size = chunk_size * 16 + upload_data_.at(i) - 'a' + 10;
        } else if ('A' <= upload_data_.at(i) && upload_data_.at(i) <= 'F') {
            chunk_size = chunk_size * 16 + upload_data_.at(i) - 'A' + 10;
        } else if ('0' <= upload_data_.at(i) && upload_data_.at(i) <= '9') {
            chunk_size = chunk_size * 16 + upload_data_.at(i) - '0';
        } else {
            std::cerr << "Failed to recognize chunk size." << std::endl;
            return std::pair<int, int>(-1, 400);  // Bad Request
        }
        // チャンクサイズが極端に大きい数値の場合、無限ループにならないよう早めに判定
        if (client_max_body_size != -1
                && static_cast<int>(upload_data_.length()) - i + chunk_size
                > client_max_body_size) {
            std::cerr << "Chunk size overflow." << std::endl;
            return std::pair<int, int>(-1, 413);  // Payload Too Large
        }
        i++;
    }
#ifdef DEBUG
    std::cout << "  chunk_size:" << chunk_size << std::endl;
#endif

    return std::pair<int, int>(chunk_size, i);
}

// 戻り値 true  読み込み完了
//       false 読み込み継続
bool HttpRequest::receive_plain_data_(bool is_not_readed_header, const char *buf) {
    std::size_t content_length
        = StringConverter::stoi(parser_.get_header_field("Content-Length"));

    if (!is_not_readed_header) {
        // データ受信
        upload_data_ += buf;
        return false;  // 継続読み込み
    }

    // 終了条件チェック
    if (client_max_body_size != -1
            && static_cast<int>(upload_data_.length()) > client_max_body_size) {
        // client_max_body_size以上のデータ受信時はエラー413
        status_code_ = 413;
        return true;
    } else if (content_length != 0
            && upload_data_.length() >= content_length) {
        // Content-Length分の読み込みが終わった
        return true;  // 読み込み完了
    }

    return false;  // 継続読み込み
}

bool HttpRequest::write_to_file_() {
    // ディレクトリがなければ作成
    if (PathUtil::is_folder_exists(upload_dir.c_str()) == false) {
        if (mkdir(upload_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
            std::cerr << "Could not create dirctory: "
                << upload_dir << std::endl;
            return false;
        }
    }

    // ファイルのオープン
    std::ofstream ofs_outfile;
    ofs_outfile.open((upload_dir + TMP_POST_DATA_FILE).c_str(),
            std::ios::out | std::ios::binary | std::ios::trunc);
    if (!ofs_outfile) {
        std::cerr << "Could not open file during receiving the file: "
            << get_path_to_file() << std::endl;
        return false;
    }

    // 書き込み
    try {
        ofs_outfile.write(upload_data_.c_str(), upload_data_.length());
    } catch (const std::exception& e) {
        std::cerr << "Could not write file during receiving the file: "
            << get_path_to_file() << std::endl;
        return (false);
    }

    // クローズ
    ofs_outfile.close();

#ifdef DEBUG
            std::cout << "write_to_file_() done: "
                << upload_dir + TMP_POST_DATA_FILE << std::endl;
#endif
    return true;
}

bool HttpRequest::write_to_file_append_(std::size_t append_length) {
    // ディレクトリがなければ作成
    if (PathUtil::is_folder_exists(upload_dir.c_str()) == false) {
        if (mkdir(upload_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
            std::cerr << "Could not create dirctory: "
                << upload_dir << std::endl;
            return false;
        }
    }

    // ファイルのオープン
    std::ofstream ofs_outfile;
    ofs_outfile.open((upload_dir + TMP_POST_DATA_FILE).c_str(),
            std::ios::app | std::ios::binary);
    if (!ofs_outfile) {
        std::cerr << "Could not open file during receiving the file: "
            << get_path_to_file() << std::endl;
        return false;
    }

    // 追加書き込み
    try {
        ofs_outfile.write(upload_data_.c_str(), append_length);
    } catch (const std::exception& e) {
        std::cerr << "Could not write file during receiving the file: "
            << get_path_to_file() << std::endl;
            return (false);
    }

    // クローズ
    ofs_outfile.close();

#ifdef DEBUG
            std::cout << "write_to_file_append_() done: "
                << upload_dir + TMP_POST_DATA_FILE << std::endl;
#endif
    return true;
}

int HttpRequest::delete_file_() {
    if (std::remove(get_path_to_file().c_str()) != 0) {
        std::cerr << "Failed to delete file: "
            << get_path_to_file() << std::endl;
        return 204;  // No Content
    }
    return 204;  // No Content
}
