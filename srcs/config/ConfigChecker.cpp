#include "ConfigChecker.hpp"

bool isAllNum(const std::string s);
bool isValidIP(const std::string s);
bool isValidPort(const std::string s);

bool ConfigChecker::isValidConfig() {
    std::map<int, string_vector_map>::iterator begin
         = Config::_config.begin();
    std::map<int, string_vector_map>::iterator end
         = Config::_config.end();
    bool    is_set_location = false;
    for (std::map<int, string_vector_map>
        ::iterator itr = begin; itr != end; itr++) {
        string_vector_map::iterator key_begin = itr->second.begin();
        string_vector_map::iterator key_end = itr->second.end();
        for (string_vector_map::iterator iter = key_begin;
             iter != key_end; iter++) {
            if (!iter->first.compare("listen")) {
                if (!isValidListen(iter->second)) {
                    return (false);
                }
                continue;
            } else if (!iter->first.compare("client_max_body_size")) {
                if (!isValidClientMaxBodySize(iter->second)) {
                    return (false);
                }
                continue;
            } else if (!iter->first.compare("error_page")) {
                if (!isValidErrorPage(iter->second)) {
                    return (false);
                }
                continue;
            } else if(!iter->first.compare("server_name")) {
                continue;
            } else if (iter->first.size() > std::string("location ").size()) {
                if (!iter->first.substr
                    (0, std::string("location ").size()).
                        compare("location ")) {
                    if (!isValidLocation(iter->first, iter->second)) {
                        return (false);
                    }
                    is_set_location = true;
                    continue;
                }
            }
            std::cout << iter->first << std::endl;
            return (false);
        }
    }
    // Location項目がない場合はエラー
    return (is_set_location);
}

int convertKeyToInt(const std::string &key) {
    if (!key.compare("autoindex")) {
        return (AUTO_INDEX);
    } else if (!key.compare("index")) {
        return (INDEX);
    } else if (!key.compare("cgi_extension")) {
        return (CGI_EXTENSION);
    } else if (!key.compare("http_method")) {
        return (HTTP_METHOD);
    } else if (!key.compare("return")) {
        return (REDIRECTOIN);
    } else if (!key.compare("root")) {
        return (ROOT);
    } else if (!key.compare("upload_dir")) {
        return (UPLOAD_DIR);
    }
    return (LOCATION_KEY_UNKNOWN);
}

bool ConfigChecker::isValidStatusCode(const std::string &status_code) {
    if (status_code.size() != 3 || !isAllNum(status_code)) {
        return (false);
    }
    std::istringstream iss(status_code);
    int n;
    iss >> n;
    return (100 <= n && n <= 599);
}

bool ConfigChecker::isValidErrorPage(const std::vector<std::string> &v) {
    std::vector<std::string>::const_iterator begin = v.begin();
    std::vector<std::string>::const_iterator end = v.end();
    // 重複確認のためのstd::set<std::string>
    std::set<std::string> status_code;
    for (std::vector<std::string>::const_iterator iter = begin;
         iter != end; iter++) {
        if (iter->find('|') == std::string::npos) {
            return (false);
        }
        int sep_position = iter->find('|');
        std::string key = iter->substr(0, sep_position);
        std::string value = iter->substr(sep_position + 1, iter->length());
        if (!value.size()) {
            // エラーページのURLが空
            return (false);
        } else if (!status_code.insert(key).second || !isValidStatusCode(key)) {
            // 同一error_page内でstatus_codeが重複している または ステータスコードが不正
            return (false);
        }
    }
    return (true);
}

bool ConfigChecker::isValidRedirection(const std::string &value) {
    if (value.find('|') == std::string::npos) {
        return (false);
    }
    int sep_position = value.find('|');
    std::string status_code = value.substr(0, sep_position);
    std::string url = value.substr(sep_position + 1, value.length());
    if (!url.size()) {
        return (false);
    }
    return (isValidStatusCode(status_code));
}

bool ConfigChecker::isValidLocation(const std::string &s,
                                 const std::vector<std::string> &v) {
    std::string locationArg =
         s.substr(std::string("location ").size(), s.size());
    for (size_t i = 0; i < locationArg.size(); i++) {
        if (!isspace(locationArg[i])) {
            break;
        }
        return (false);
    }

    std::vector<std::string>::const_iterator begin = v.begin();
    std::vector<std::string>::const_iterator end = v.end();
    // 重複確認のためのstd::set<std::string>
    std::set<std::string> key_set;
    for (std::vector<std::string>::const_iterator iter = begin;
         iter != end; iter++) {
        int sep_position = iter->find('|');
        std::string key = iter->substr(0, sep_position);
        std::string value = iter->substr(sep_position + 1, iter->length());
        if (!key_set.insert(key).second) {
            // 同一Location内でキーが重複している
            return (false);
        }

        int key_num = convertKeyToInt(key);
        switch (key_num) {
        case AUTO_INDEX:
            if (value.compare("on") && value.compare("off")) {
                return (false);
            }
            break;
        case REDIRECTOIN:
            if (!isValidRedirection(value)) {
                return (false);
            }
            break;
        case LOCATION_KEY_UNKNOWN:
            return (false);
        default:
            break;
        }
    }
    return (true);
}

bool ConfigChecker::isValidListen(const std::vector<std::string> &v) {
    std::vector<std::string>::const_iterator begin = v.begin();
    std::vector<std::string>::const_iterator end = v.end();
    for (std::vector<std::string>::const_iterator iter = begin;
         iter != end; iter++) {
        if (!isValidPort(*iter) && !isValidIP(*iter)) {
            return (false);
        }
        for (std::vector<std::string>::const_iterator itr = iter + 1;
         itr != end; itr++) {
            if (*itr == *iter) {
                return (false);
            }
        }
    }
    return (true);
}

bool ConfigChecker::isValidIP(const std::string s) {
    std::vector<std::string> v = split(s, '.');
    if (v.size() != 4)
        return (false);
    std::vector<std::string>::const_iterator begin = v.begin();
    std::vector<std::string>::const_iterator end = v.end();
    for (std::vector<std::string>::const_iterator iter = begin;
            iter != end; iter++) {
        if (!isAllNum(*iter)) {
            return (false);
        }
        std::stringstream ss(*iter);
        long n;
        ss >> n;
        if (!(0 <= n && n <= 255)) {
            return false;
        }
    }
    return (true);
}

bool ConfigChecker::isValidPort(const std::string s) {
    if (!isAllNum(s)) {
        return (false);
    }
    std::stringstream ss(s);
    long n;
    ss >> n;
    if (!(0 <= n && n <= 65535)) {
        return (false);
    }
    return (true);
}

bool ConfigChecker::isAllNum(const std::string s) {
    std::string::const_iterator begin = s.begin();
    std::string::const_iterator end = s.end();
    for (std::string::const_iterator iter = begin; iter != end; iter++) {
        if (!std::isdigit(*iter)) {
            return (false);
        }
    }
    return (true);
}

bool ConfigChecker
    ::isValidClientMaxBodySize(const std::vector<std::string> &v) {
    if (v.size() != 1 || !isAllNum(*v.begin())) {
        return (false);
    }
    return (true);
}
