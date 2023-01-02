#ifndef SRCS_CONFIG_CONFIGCHECKER_HPP_
#define SRCS_CONFIG_CONFIGCHECKER_HPP_

#include <map>
#include <string>
#include <vector>
#include <set>
#include "srcs/config/ConfigChecker.hpp"
#include "srcs/config/Config.hpp"

enum locationKye {
    ROOT,
    INDEX,
    CGI_EXTENSION,
    HTTP_METHOD,
    REDIRECTOIN,
    AUTO_INDEX,
    UPLOAD_DIR,
    KEY_UNKNOWN
};

namespace ConfigChecker {
    bool isValidConfig();
    bool isValidListen(const std::vector<std::string> &v);
    bool isValidClientMaxBodySize(const std::vector<std::string> &v);
    bool isValidLocation(const std::string &s, const std::vector<std::string> &v);
    bool isValidRedirection(const std::string &value);
}

#endif /* SRCS_CONFIG_CONFIGCHECKER_HPP_ */
