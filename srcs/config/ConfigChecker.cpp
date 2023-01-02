#include "ConfigChecker.hpp"

bool isAllNum(const std::string s);
bool isValidIP(const std::string s);
bool isValidPort(const std::string s);

bool ConfigChecker::isValidConfig() {
    std::map<int, string_vector_map>::iterator begin
         = Config::_config.begin();
    std::map<int, string_vector_map>::iterator end
         = Config::_config.end();
    for (std::map<int, string_vector_map>
        ::iterator itr = begin; itr != end; itr++) {
        string_vector_map::iterator key_begin = itr->second.begin();
        string_vector_map::iterator key_end = itr->second.end();
        for (string_vector_map::iterator iter = key_begin;
             iter != key_end; iter++) {
            if (!iter->first.compare("listen")) {
                if (!ConfigChecker::isValidListen(iter->second)) {
                    return (false);
                }
                continue;
            } else if (!iter->first.compare("client_max_body_size")) {
                if (!ConfigChecker::isValidClientMaxBodySize(iter->second)) {
                    return (false);
                }
                continue;
            } else if (!iter->first.compare("error_page")
                         || !iter->first.compare("server_name")) {
                continue;
            } else if (iter->first.size() > std::string("location ").size()) {
                if (!iter->first.substr
                    (0, std::string("location ").size()).
                        compare("location ")) {
                    Config::printVector(iter->second);
                    if (!isValidLocation(iter->second)) {
                        return (false);
                    }
                    continue;
                }
            }
            std::cout << iter->first << std::endl;
            return (false);
        }
    }
    return (true);
}

bool ConfigChecker::isValidLocation(const std::vector<std::string> &v) {
    std::vector<std::string>::const_iterator begin = v.begin();
    std::vector<std::string>::const_iterator end = v.end();
    for (std::vector<std::string>::const_iterator iter = begin;
         iter != end; iter++) {
        // エラーチェック処理を書く
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

bool isValidIP(const std::string s) {
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

bool isValidPort(const std::string s) {
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

bool isAllNum(const std::string s) {
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
