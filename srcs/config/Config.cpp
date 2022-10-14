#include "Config.hpp"

Config::Config() {
    // std::map<std::string, std::string> map;
    Config::_config["a"]["b"] = "hoge";
}

Config::~Config() {
}

Config::Config(Config const &other) {
    *this = other;
}

Config &Config::operator=(Config const &other) {
    if (this != &other) {
    }
    return *this;
}

const char* Config::ConfigFormatException::what() const throw() {
    return ("Config file is not valid format.");
}

std::map<std::string, std::map<std::string, std::string> > Config::_config;
void    Config::parseConfig(const std::string &path) {
    std::ifstream ifs(path);
    if (!ifs) {
        std::cout << "Can not open file" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string buf;
    std::string hostname;
    std::string key;
    std::string value;
    while (!ifs.eof()) {
        std::getline(ifs, buf, '\n');
        if (!buf.size() || buf[0] == '#')
            continue;
        if (buf[0] != ' ') {
            hostname = buf;
            std::getline(ifs, buf, '\n');
            if (ifs.eof() || buf[0] != ' ' || buf[1] == ' ') {
                ifs.close();
                std::cout << "key error" << std::endl;
                throw(Config::ConfigFormatException());
            }
            key = buf.substr(1);
            std::getline(ifs, buf, '\n');
            if (ifs.eof() || buf[0] != ' ' || buf[1] != ' ' || buf[2] == ' ') {
                ifs.close();
                std::cout << "value error" << std::endl;
                throw(Config::ConfigFormatException());
            }
            value = buf.substr(2);
        }
        std::cout << hostname << " " << key << " " << value << std::endl;
        _config[hostname][key] = value;
    }
    ifs.close();
}
