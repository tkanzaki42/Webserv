#include "srcs/server_request/HttpRequest.hpp"
#include "srcs/server/Webserv.hpp"

HttpRequest::HttpRequest(const Webserv &server)
        : server_(server),
          parser_(HttpParser(received_line_)),
          status_code_(200) {
}

HttpRequest::~HttpRequest() {
}

HttpRequest::HttpRequest(const HttpRequest &obj)
        : server_(obj.server_),
          parser_(HttpParser(obj.parser_)) {
    *this = obj;
}

HttpRequest& HttpRequest::operator=(const HttpRequest &obj) {
    parser_       = HttpParser(obj.parser_);
    status_code_  = obj.status_code_;
    return *this;
}

// void HttpRequest::set_accept_fd(int accept_fd) {
//     this->accept_fd_ = accept_fd;
// }

int HttpRequest::receive_header() {
    char            buf[BUF_SIZE];
    const FDManager &fd_manager = server_.get_fd_manager();

    if (!fd_manager.recieve(buf)) {
        std::cerr << "recv() failed." << std::endl;
        std::cerr << "ERROR: " << errno << std::endl;
        status_code_ = 400;  // Bad Request
        return false;
    }
    const char *found_empty_line = strstr(buf, "\r\n\r\n");
    if (!found_empty_line) {
        std::cerr << "Failed to recognize header." << std::endl;
        status_code_ = 400;  // Bad Request
        return false;
    }
    received_line_.append(buf);

    return true;
}

bool HttpRequest::analyze_request() {
    status_code_ = parser_.parse();

    generate_path_to_file_();

    if (status_code_ == 200 && get_http_method() == METHOD_POST) {
        if (!receive_and_store_to_file_()) {
            return false;
        }
    }
    return true;
}

void HttpRequest::print_debug() {
    typedef std::map<std::string, std::string>::const_iterator map_iter;

    std::cout << "//-----received_line_ start-----" << std::endl;
    std::cout << received_line_ << std::endl;
    std::cout << "\\\\-----received_line_ end-----" << std::endl;
    std::cout << std::endl;

    std::cout << "[request data]" << std::endl;
    std::cout << "  http_method_   : "
        << parser_.get_http_method() << std::endl;
    std::cout << "  request_path_  : "
        << parser_.get_request_path() << std::endl;
    std::cout << "  http_ver_      : " << parser_.get_http_ver() << std::endl;
    std::cout << "  base_html_path : " << kBaseHtmlPath << std::endl;
    std::cout << "  path_to_file_  : " << get_path_to_file() << std::endl;

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

const std::string& HttpRequest::get_request_path() const {
    return parser_.get_request_path();
}

const std::string& HttpRequest::get_http_ver() const {
    return parser_.get_http_ver();
}

const std::map<std::string, std::string>&
        HttpRequest::get_header_field_map() const {
    return parser_.get_header_field_map();
}

int HttpRequest::get_status_code() const {
    return status_code_;
}

const std::string& HttpRequest::get_path_to_file() const {
    return path_to_file_;
}

void HttpRequest::generate_path_to_file_() {
    if (get_request_path()[get_request_path().length() - 1] == '/') {
        path_to_file_ = kBaseHtmlPath + get_request_path() + kIndexHtmlFileName;
    } else {
        path_to_file_ = kBaseHtmlPath + get_request_path();
    }
}

bool HttpRequest::receive_and_store_to_file_() {
    // ssize_t         read_size = 0;
    // ssize_t         total_read_size = 0;
    // char            buf[BUF_SIZE];
    // const FDManager &fd_manager = server_.get_fd_manager();
    std::ofstream   ofs_outfile;

    // ディレクトリがなければ作成
    struct stat stat_dir;
    std::string dir_path = path_to_file_.substr(0, path_to_file_.rfind('/'));
    if (stat(dir_path.c_str(), &stat_dir) == -1) {
        if (mkdir(dir_path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
            std::cerr << "Could not create dirctory: " << dir_path << std::endl;
            status_code_ = 500;
            return false;  // Internal Server Error
        }
    }

    // ファイルのオープン
    ofs_outfile.open(path_to_file_.c_str(),
            std::ios::out | std::ios::binary | std::ios::trunc);
    if (!ofs_outfile) {
        std::cerr << "Could not open file: " << path_to_file_ << std::endl;
        status_code_ = 500;
        return false;  // Internal Server Error
    }

    ofs_outfile << "ヘッダ読み取ったContent";

    // do {
    //     if (!fd_manager.recieve(buf)) {
    //         std::cerr << "recv() failed in "
    //             << "receive_and_store_to_file_()." << std::endl;
    //         return false;
    //     }
    //     if (read_size > 0) {
    //         ofs_outfile.write(buf, read_size);
    //         total_read_size += read_size;
    //         std::cout << "read_size:" << read_size
    //             << ", total:" << total_read_size << std::endl;
    //         if (total_read_size
    //                 >= atoi(parser_.get_header_field("Content-Length").c_str())
    //            ) {
    //             break;
    //         }
    //     }
    // } while (read_size > 0);

    ofs_outfile.close();
    status_code_ = 201;
    return true;  // Created
}
