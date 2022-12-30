#include "srcs/server/HttpBody.hpp"
#include "srcs/server/StatusDescription.hpp"

HttpBody::HttpBody(const HttpRequest& request)
        : request_(request),
          content_(std::vector<std::string>()),
          content_length_(0),
          content_head_(0),
          content_tail_(0),
          hash_value_(0xffffffff),
          is_compressed_(false) {
}

HttpBody::~HttpBody() {
}

HttpBody::HttpBody(const HttpBody &obj)
        : request_(obj.request_) {
    *this = obj;
}

HttpBody &HttpBody::operator=(const HttpBody &obj) {
    this->is_compressed_   = obj.is_compressed_;
    this->content_         = obj.content_;
    this->content_length_  = obj.content_length_;
    this->content_head_    = obj.content_head_;
    this->content_tail_    = obj.content_tail_;
    this->hash_value_            = obj.hash_value_;
    return *this;
}

std::string HttpBody::get_error_page(int status_code) {
    std::map<int, std::string> errorPageMap =
        Config::getErrorPage(request_.get_virtual_host_index(), request_.get_location());
    std::map<int, std::string>::iterator it =
        errorPageMap.find(status_code);
    // std::map<int, std::string>::iterator begin = errorPageMap.begin();
    // std::map<int, std::string>::iterator end = errorPageMap.end();
    // for (std::map<int, std::string>::iterator iter = begin; iter != end; iter++) {
    //     std::cout << iter->second << std::endl;
    // }
    if (it != errorPageMap.end()) {
        return (it->second);
    }
    return ("");
}

int HttpBody::read_contents_from_error_file_(int status_code) {
    // ファイルのオープン
    std::ifstream ifs_readfile;
    std::string error_page_path = get_error_page(status_code);
    ifs_readfile.open(error_page_path);

    // エラーチェック
    if (ifs_readfile.fail()) {
        // ファイルが存在しない
        std::cerr << "File not found: "
            << error_page_path << std::endl;
        return 404;  // Not Found
    } else if (!ifs_readfile) {
        // その他のファイルオープンエラー
        std::cerr << "Could not open file: "
            << error_page_path << std::endl;
        return 500;  // Internal Server Error
    }

    // ファイル読み込み
    char          read_line[256];
    while (ifs_readfile.getline(read_line, 256 - 1)) {
        content_.push_back(std::string(read_line));
    }

    ifs_readfile.close();
    // Content-Lengthを数える
    count_content_length_();
    return status_code;
}

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
    char          read_line[BUF_SIZE];

    while (true) {
        ifs_readfile.getline(read_line, BUF_SIZE - 1);
        if (ifs_readfile.eof()) {
            content_.push_back(std::string(read_line));
            break ;
        } else {
            content_.push_back(std::string(read_line) + "\n");
        }
    }
    ifs_readfile.close();

    // Content-Lengthを数える
    count_content_length_();

    // Etag用にcontent_を元にハッシュ値を生成。必ずAccept-Rangesで圧縮する前に処理する
    if (ETAG_ENABLED) {
        hash_value_ = Hash::generate_crc32(content_);
    }

    // Accept-Ranges
    is_compressed_ = false;
    return compress_to_range_();
}

bool HttpBody::is_match_error_page(int status_code) {
    std::map<int, std::string> errorPageMap =
        Config::getErrorPage(request_.get_virtual_host_index(), request_.get_location());
    std::map<int, std::string>::iterator it =
        errorPageMap.find(status_code);
    if (it != errorPageMap.end()) {
        return (true);
    }
    return (false);
}

void HttpBody::make_status_response_(int status_code) {
    std::ostringstream oss_body;
    oss_body << "<html><body><h1>" << status_code << " "
        << StatusDescription::get_reason(status_code)
        << "</h1><p>" << StatusDescription::get_message(status_code)
        << "</p><hr><address>"
        << kServerSoftwareName
        << "</address></body></html>\r\n";

    content_.clear();
    content_.push_back(oss_body.str());

    // 401はContent-Lengthを返す
    if (400 <= status_code && status_code <= 499) {
        count_content_length_();
    }
}

int HttpBody::make_response(int status_code) {
    if ((status_code == 200 && request_.get_http_method() == METHOD_GET)
        || status_code == 206) {
        status_code = read_contents_from_file_();
    } else if (is_match_error_page(status_code)) {
        status_code = read_contents_from_error_file_(status_code);
    } else {
        make_status_response_(status_code);
    }
    return status_code;
}

void HttpBody::make_autoindex_response() {
    std::ostringstream oss_body;
    oss_body << "<html>\n"
        << "<head><title>Index of " << request_.get_request_target()
        << "</title></head>\n"
        << "<body>\n"
        << "<h1>Index of " << request_.get_request_target()
        << "</h1><hr><pre>"
        << "<a href=\"../\">../</a>\n";

    // ファイル一覧をopendr()、readdir()で取得して列挙
    DIR *dir;
    struct dirent *dir_read;
    if ((dir = opendir(request_.get_path_to_file().c_str())) != NULL) {
        while ((dir_read = readdir(dir)) != NULL) {
            std::string file_name(dir_read->d_name);

            // . および..はスキップ
            if (file_name.compare(".") == 0 || file_name.compare("..") == 0)
                continue;

            oss_body << "<a href=\"" << file_name << "\">";
            int str_width = UTF8Util::get_string_width(file_name);
            const int display_width = 50;  // 表示する幅を指定
            if (str_width > display_width) {
                // 文字幅が長い場合は省略表記
                std::string limited_str = UTF8Util::get_limited_wide_string(
                    file_name, display_width - 3);
                oss_body << limited_str <<  "..&gt;</a> ";
                str_width = UTF8Util::get_string_width(limited_str) + 3;
            } else {
                // 文字幅が短い場合はすべて表示
                oss_body << file_name <<  "</a> ";
            }
            // 文字数不足分をスペースで埋める
            while (str_width < display_width) {
                oss_body << ' ';
                str_width++;
            }

            oss_body << PathUtil::get_last_modified_date(
                        request_.get_path_to_file() + "/" + file_name)
                << std::setw(20)
                << PathUtil::get_filesize(
                        request_.get_path_to_file() + "/" + file_name)
                << "\r\n";
        }
        closedir(dir);
    } else {
        std::cerr << "Failed to opendir(), errno = " << errno << std::endl;
    }

    oss_body << "</pre><hr></body>\n"
        << "</html>\n";

    content_.clear();
    content_.push_back(oss_body.str());
    
    // Content-Lengthを数える
    count_content_length_();
}

// Rangeの範囲にcontent_を圧縮。416エラーはfalse
int  HttpBody::compress_to_range_() {
    std::string range_of_content;
    const std::map<std::string, std::string>& map = request_.get_header_field_map();
    // 静的ページ or Rangeヘッダがなかったら圧縮しない
    if (request_.get_file_type() != FILETYPE_STATIC_HTML
        || map.count(std::string("Range")) == 0){
        return 200;
    }
    std::string range = map.at(std::string("Range"));
    // 先頭の文字数、末尾の文字数をセット
    content_head_ = (size_t)StringConverter::stoi(range.substr(std::string("bytes=").length()));
    content_tail_   = (size_t)StringConverter::stoi(range.substr(range.find("-") + 1));
    // 先頭の文字数が末尾の文字数より大きかったら416
    if (content_head_ > content_tail_) {
        return 416;
    }
    size_t len   = content_tail_ - content_head_ + 1;
    size_t total = 0;
    bool once = false;
    for (std::size_t i = 0; i < content_.size(); i++) {
        total += content_[i].length();
        if (total < content_head_){
            // Rangeの先頭までスキップ
            continue ;
        }
        // 最初の一回だけRangeの先頭までoffsetを進める
        size_t offset = 0;
        if (!once){
            offset = content_head_ - (total - content_[i].length());
            once = true;
        }
        std::cout << "Line:" << content_[i].length() - offset<< std::endl;
        if (total - content_head_ > len) {
            // Rangeの長さ分に達したらbreak
            std::cout << "Last Line:" << content_[i].length() - (total - len - content_head_) << std::endl;
            range_of_content.append(
                content_[i].substr(
                    offset, content_[i].length() - (total - content_head_ - len) - offset
                ).c_str()
            );
            break ;
        } else {
            // Rangeの長さまで格納し続ける
            range_of_content.append(
                content_[i].substr(
                    offset, content_[i].length() - offset + 1
                ).c_str()
            );
        }
        std::cout << "range_of_content:" << range_of_content << std::endl;
    }
    // content_が1文字もRangeの範囲に含まれなかったら416
    if (total < content_head_){
        return 416;
    }
    content_.clear();
    content_.push_back(range_of_content);
    is_compressed_ = true;
    return 206;
}


void HttpBody::count_content_length_() {
    int content_length = 0;

    for (std::vector<std::string>::iterator it = content_.begin();
            it != content_.end(); ++it) {
        content_length += (*it).length();
    }
    content_length_ = content_length;
}

std::size_t HttpBody::get_content_length() const {
    return this->content_length_;
}

std::size_t HttpBody::get_content_head() const {
    return this->content_head_;
}

std::size_t HttpBody::get_content_tail() const {
    return this->content_tail_;
}

unsigned int HttpBody::get_hash_value_() const {
    return this->hash_value_;
}

unsigned int HttpBody::get_hash_len_() const {
    return this->hash_len_;
}

bool HttpBody::is_compressed() const {
    return is_compressed_;
}

bool HttpBody::has_hash() const {
    if (this->hash_value_ != 0xffffffff) {
        return true;
    } else {
        return false;
    }
}

const std::vector<std::string> &HttpBody::get_content() const {
    return this->content_;
}

void HttpBody::clear_contents() {
    content_.clear();
}
