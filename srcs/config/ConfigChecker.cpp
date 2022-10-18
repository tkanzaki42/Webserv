#include "ConfigChecker.hpp"

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
        StringConverter::stoi(*iter);
    }
    return (true);
}

bool ConfigChecker::isValidClientMaxBodySize(const std::vector<std::string> &v) {
    std::vector<std::string>::const_iterator begin = v.begin();
    std::vector<std::string>::const_iterator end = v.end();
    for (std::vector<std::string>::const_iterator iter = begin;
         iter != end; iter++) {
        StringConverter::stoi(*iter);
    }
    return (true);
}
