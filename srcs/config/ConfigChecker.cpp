#include "ConfigChecker.hpp"

bool isAllNum(const std::string s);
bool isValidIP(const std::string s);
bool isValidPort(const std::string s);

bool ConfigChecker::isValidConfig() {
    std::map<std::string, string_vector_map>::iterator begin
         = Config::_config.begin();
    std::map<std::string, string_vector_map>::iterator end
         = Config::_config.end();
    for (std::map<std::string, string_vector_map>
        ::iterator itr = begin; itr != end; itr++) {
        string_vector_map::iterator key_begin = itr->second.begin();
        string_vector_map::iterator key_end = itr->second.end();
        for (string_vector_map::iterator iter = key_begin;
             iter != key_end; iter++) {
            if (!iter->first.compare("listen")) {
                if (!ConfigChecker::isValidListen(iter->second)) {
                    return (false);
                }
            } else if (!iter->first.compare("client_max_body_size")) {
                if (!ConfigChecker::isValidClientMaxBodySize(iter->second)) {
                    return (false);
                }
            }
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
    std::vector<std::string>::const_iterator begin = v.begin();
    std::vector<std::string>::const_iterator end = v.end();
    for (std::vector<std::string>::const_iterator iter = begin;
         iter != end; iter++) {
        std::cout  << StringConverter::stoi(*iter) << std::endl;
    }
    return (true);
}
