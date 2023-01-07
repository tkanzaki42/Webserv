#ifndef SRCS_CONFIG_CONFIGCHECKER_HPP_
#define SRCS_CONFIG_CONFIGCHECKER_HPP_

#include <map>
#include <string>
#include <vector>
#include <set>
#include <limits.h>
#include "srcs/config/ConfigChecker.hpp"
#include "srcs/config/Config.hpp"

enum locationKey {
    ROOT,
    INDEX,
    CGI_EXTENSION,
    HTTP_METHOD,
    REDIRECTOIN,
    AUTO_INDEX,
    UPLOAD_DIR,
    LOCATION_KEY_UNKNOWN
};

enum configKey {
    LOCATION,
    SERVER_NAME,
    LISTEN,
    CLIENT_MAX_BODY_SIZE,
    ERROR_PAGE,
    CONFIG_KEY_UNKNOWN
};

namespace ConfigChecker {
    bool isValidConfig();
    bool isValidListen(const std::vector<std::string> &v);
    bool isValidClientMaxBodySize(const std::vector<std::string> &v);
    bool isValidLocation(const std::string &s, const std::vector<std::string> &v);
    bool isValidRedirection(const std::string &value);
    bool isValidErrorPage(const std::vector<std::string> &v);
    bool isValidStatusCode(const std::string &status_code);
    bool isAllNum(const std::string s);
    bool isValidPort(const std::string s);
}

#endif /* SRCS_CONFIG_CONFIGCHECKER_HPP_ */
