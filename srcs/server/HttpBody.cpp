#include "srcs/server/HttpBody.hpp"
#include "srcs/server/StatusDescription.hpp"

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
                // 文字幅が文字会場合はすべて表示
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
