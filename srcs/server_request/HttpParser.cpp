#include "srcs/server_request/HttpParser.hpp"

HttpParser::HttpParser(const std::string& received_line)
        : read_idx_(0), received_line_(received_line) {
}

HttpParser::~HttpParser() {}

HttpParser::HttpParser(const HttpParser &obj)
        : received_line_(obj.received_line_) {
    *this = obj;
}

HttpParser& HttpParser::operator=(const HttpParser &obj) {
    read_idx_       = obj.read_idx_;
    http_method_    = obj.http_method_;
    request_target_ = obj.request_target_;
    query_string_   = obj.query_string_;
    path_info_      = obj.path_info_;
    path_to_file_   = obj.path_to_file_;
    http_ver_       = obj.http_ver_;
    header_field_   = std::map<std::string, std::string>(obj.header_field_);

    return *this;
}

int HttpParser::parse() {
    parse_method_();
    parse_request_target_();
    parse_http_ver_();
    parse_header_field_();
    int status_code = validate_parsed_data_();

    return status_code;
}

HttpMethod HttpParser::get_http_method() const {
    return http_method_;
}

const std::string& HttpParser::get_request_target() const {
    return request_target_;
}

const std::string& HttpParser::get_query_string() const {
    return query_string_;
}

const std::string& HttpParser::get_path_info() const {
    return path_info_;
}

const std::string& HttpParser::get_path_to_file() const {
    return path_to_file_;
}

const std::string& HttpParser::get_http_ver() const {
    return http_ver_;
}

const std::map<std::string, std::string>&
        HttpParser::get_header_field_map() const {
    return header_field_;
}

const std::string& HttpParser::get_header_field(const std::string& key) {
    return header_field_[key];
}

const std::string HttpParser::get_host_name() {
    std::vector<std::string> field_host = split(header_field_["Host"], ':');
    return (field_host[0]);
}

const std::string HttpParser::get_remain_buffer() {
    return received_line_.substr(
            read_idx_, received_line_.length() - read_idx_);
}

void HttpParser::setIndexHtmlFileName(const std::vector<std::string> &filename) {
    this->indexHtmlFileName = filename;
}

void HttpParser::setBaseHtmlPath(const std::string &path) {
    this->baseHtmlPath = path;
}

const std::vector<std::string>& HttpParser::getIndexHtmlFileName() const {
    return (this->indexHtmlFileName);
}

const std::string& HttpParser::getBaseHtmlPath() const {
    return (this->baseHtmlPath);
}

void HttpParser::parse_method_() {
    if (received_line_.compare(read_idx_, 4, "POST") == 0) {
        http_method_ = METHOD_POST;
        read_idx_ += 4;
    } else if (received_line_.compare(read_idx_, 3, "GET") == 0) {
        http_method_ = METHOD_GET;
        read_idx_ += 3;
    } else if (received_line_.compare(read_idx_, 6, "DELETE") == 0) {
        http_method_ = METHOD_DELETE;
        read_idx_ += 6;
    }
    skip_space_();
}

void HttpParser::parse_request_target_() {
    char buffer[BUF_SIZE];
    int buffer_idx = 0;

    while (received_line_[read_idx_] != ' '
            && read_idx_ < received_line_.length()
            && received_line_.length() < BUF_SIZE - 1) {
        buffer[buffer_idx++] = received_line_[read_idx_];
        read_idx_++;
    }
    buffer[buffer_idx] = '\0';
    request_target_ = std::string(buffer);
    skip_space_();
}

void HttpParser::separate_querystring_pathinfo() {
    // パスからQUERY_STRINGを切り出す
    std::string remaining_path = split_query_string_(request_target_);

    // path_to_file_を仮で設定
    path_to_file_ = this->baseHtmlPath + remaining_path;

    // パスからPATH_INFOを切り出す
    if (get_http_method() != METHOD_POST)
        split_path_info_();
}

std::string HttpParser::split_query_string_(
        std::string &request_target) {
    std::string remaining_path;

    // パスのうち"?"以降をQUERY_STRINGとして切り出す
    std::string::size_type question_pos = request_target.find("?");
    if (question_pos == std::string::npos) {
        // "?"が見つからなければ、QUERY_STRINGはない
        remaining_path = request_target;
        query_string_ = "";
    } else {
        // "?"が見つかれば、QUERY_STRINGがあるのでquery_string_に格納
        remaining_path = request_target.substr(0, question_pos);
        query_string_ = request_target.substr(question_pos + 1,
            request_target.size() - question_pos - 1);
    }
    return remaining_path;
}

void HttpParser::autocomplete_path() {
    // パスが正しければ対応不要
    if (PathUtil::is_file_exists(path_to_file_))
        return;

    if (PathUtil::is_folder_exists(path_to_file_)) {
        std::vector<std::string> autocomp_file = getIndexHtmlFileName();

        for (std::vector<std::string>::iterator it
                    = autocomp_file.begin();
                it != autocomp_file.end();
                it++) {
            std::string temp_path;
            if (path_to_file_[path_to_file_.length() - 1] == '/') {
                temp_path = path_to_file_ + (*it);
            } else {
                temp_path = path_to_file_ + "/";
                temp_path += (*it);
            }
            if (PathUtil::is_file_exists(temp_path)) {
                path_to_file_ = temp_path;
                break;
            }
        }
    }
}

// 指定パスの無効部分をPATH_INFOとして切り出す
// 例) /valid/valid.html/invalid
//     -> path_to_file_ : /valid/valid.html
//     -> path_info_    : /invalid
// 例) /valid/invalid.html/invalid
//     -> path_to_file_ : /valid/invalid.html/invalid
//     -> path_info_    : (empty)
// 例) /invalid/invalid.html/invalid
//     -> path_to_file_ : /invalid/invalid.html/invalid
//     -> path_info_    : (empty)
void HttpParser::split_path_info_() {
    // path_to_file_の末尾からPATH_INFOを切り出す
    std::string::size_type slash_pos_prev = 0;
    while (true) {
        std::string::size_type slash_pos
            = path_to_file_.find("/", slash_pos_prev);
        if (slash_pos == std::string::npos) {
            // スラッシュが見つからなかった場合、PATH_INFOは指定されていなかった
            path_info_ = "";
            break;
        } else {
            // スラッシュが見つかった場合、スラッシュまでのパスが有効か判定
            std::string path_candidate = path_to_file_.substr(0, slash_pos);
            if (PathUtil::is_folder_exists(path_candidate)) {
                // ディレクトリが有効な場合、下層のディレクトリチェックに進む
                {}
            } else if (PathUtil::is_file_exists(path_candidate)) {
                // ファイルとして有効な場合、パスはそこまでで完了、残りをPATH_INFOに格納
                path_info_ = path_to_file_.substr(slash_pos,
                    path_to_file_.size() - slash_pos);
                path_to_file_ = path_to_file_.substr(0, slash_pos);
                break;
            } else {
                // 有効でない場合、PATH_INFOは指定されていなかった
                path_info_ = "";
                break;
            }
        }
        slash_pos_prev = slash_pos + 1;
    }
}

void HttpParser::parse_http_ver_() {
    char buffer[BUF_SIZE];
    int buffer_idx = 0;

    if (received_line_.compare(read_idx_, 5, "HTTP/") == 0) {
        read_idx_ += 5;
    }
    while (received_line_[read_idx_] != '\r'
            && read_idx_ < received_line_.length()
            && received_line_.length() < BUF_SIZE - 1) {
        buffer[buffer_idx++] = received_line_[read_idx_];
        read_idx_++;
    }
    buffer[buffer_idx] = '\0';
    http_ver_ = std::string(buffer);

    skip_crlf_();
}

void HttpParser::parse_header_field_() {
    while (received_line_[read_idx_] != '\r'
            && read_idx_ < received_line_.length()) {
        header_field_.insert(parse_one_header_field_());
        skip_crlf_();
    }
    skip_crlf_();
}

std::pair<std::string, std::string> HttpParser::parse_one_header_field_() {
    char buffer[BUF_SIZE];
    int buffer_idx = 0;

    while (received_line_[read_idx_] != ':'
            && read_idx_ < received_line_.length()
            && received_line_.length() < BUF_SIZE - 1) {
        buffer[buffer_idx++] = received_line_[read_idx_];
        read_idx_++;
    }
    buffer[buffer_idx] = '\0';
    std::string field_name = std::string(buffer);

    ++read_idx_;  // skip ':'
    skip_space_();

    buffer_idx = 0;
    while (received_line_[read_idx_] != '\r'
            && read_idx_ < received_line_.length()) {
        buffer[buffer_idx++] = received_line_[read_idx_];
        read_idx_++;
    }
    buffer[buffer_idx] = '\0';
    std::string field_value = std::string(buffer);
    rtrim_(field_value);

    return std::make_pair(field_name, field_value);
}

void HttpParser::skip_space_() {
    while (received_line_[read_idx_] == ' '
            && read_idx_ < received_line_.length()) {
        ++read_idx_;
    }
}

void HttpParser::skip_crlf_() {
    if (received_line_[read_idx_] == '\r'
            && read_idx_ < received_line_.length())
        ++read_idx_;
    if (received_line_[read_idx_] == '\n'
            && read_idx_ < received_line_.length())
        ++read_idx_;
}

void HttpParser::rtrim_(std::string &str) {
    std::string const &whitespace = " \r\n\t\v\f";
    str.erase(str.find_last_not_of(whitespace) + 1);
}

int HttpParser::validate_parsed_data_() {
    if (http_method_ == METHOD_NOT_DEFINED)
        return 405;  // Bad Request
    if (request_target_ == "")
        return 400;
    if (http_ver_ == "")
        return 400;
    else if (http_ver_ != "1.1")
        return 505;  // HTTP Version Not Supported
    if (header_field_.empty())
        return 400;
    return 200;
}
