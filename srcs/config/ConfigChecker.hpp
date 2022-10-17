#ifndef SRCS_CONFIG_CONFIGCHECKER_HPP_
#define SRCS_CONFIG_CONFIGCHECKER_HPP_

#include "srcs/config/ConfigChecker.hpp"

class ConfigChecker {
 private:

 public:
    ConfigChecker();
    ~ConfigChecker();
    ConfigChecker(ConfigChecker const &other);
    ConfigChecker &operator=(ConfigChecker const &other);
};

#endif /* SRCS_CONFIG_CONFIGCHECKER_HPP_ */
