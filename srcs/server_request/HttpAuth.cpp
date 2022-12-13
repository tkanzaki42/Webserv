#include "srcs/server_request/HttpAuth.hpp"
#include "srcs/util/StringConverter.hpp"

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
    std::string htpasswd = BASIC_USER_PASS;
    if (StringConverter::base64_encode(htpasswd) == encoded_info) {
        return true;
    } else {
        return false;
    }
}
