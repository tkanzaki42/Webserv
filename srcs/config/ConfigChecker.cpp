#include "ConfigChecker.hpp"

ConfigChecker::ConfigChecker() {
}

ConfigChecker::~ConfigChecker() {
}

ConfigChecker::ConfigChecker(ConfigChecker const &other) {
    *this = other;
}

ConfigChecker &ConfigChecker::operator=(ConfigChecker const &other) {
    if (this != &other) {
    }
    return *this;
}
