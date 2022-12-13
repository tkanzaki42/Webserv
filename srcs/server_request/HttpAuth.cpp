#include "srcs/server_request/HttpAuth.hpp"
#include "srcs/util/StringConverter.hpp"
#include <fstream>

HttpAuth::HttpAuth():
    client_info_(""){
}

HttpAuth::~HttpAuth() {}

HttpAuth::HttpAuth(const HttpAuth &obj){
    *this = obj;
}

HttpAuth& HttpAuth::operator=(const HttpAuth &obj) {
    this->client_info_ = obj.client_info_;
    return *this;
}

void HttpAuth::set_client_info(const std::string &client_info){
    client_info_ = client_info;
}

HttpAuthType HttpAuth::check_auth_type(){
    if (client_info_ == ""){
        return AUTH_OTHER;
    }
    if (client_info_.substr(0, 5) == "Basic") {
        return AUTH_BASIC;
    }
    return AUTH_OTHER;
}
    
HttpAuthType HttpAuth::get_auth_type() const {
    return auth_type_;
}

// Basic認証
bool HttpAuth::do_basic() {
    std::string encoded_info = client_info_.substr(
        std::string("Basic ").length(),
        client_info_.length() - std::string("Basic ").length()
    ).c_str();
    // TODO(someone)
    // .htpasswdを読み込んで一致したらtrue
    std::vector<std::string> user_info;
    std::ifstream ifs_readfile;
    ifs_readfile.open("./configs/.htpasswd");
    if (ifs_readfile.fail()) {
        // ファイルが存在しない
        return 500;
    }
    // ファイル読み込み
    char          read_line[BUF_SIZE];
    while (ifs_readfile.getline(read_line, BUF_SIZE - 1)) {
        user_info.push_back(std::string(read_line));
    }
    ifs_readfile.close();
    for (size_t i = 0; i < user_info.size(); i++)
    {
        if (StringConverter::base64_encode(user_info[i])
            == encoded_info) {
            return true;
        }
    }
    return false;
}
