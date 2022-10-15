#include "Config.hpp"

Config::Config() {
}

Config::~Config() {
}

const char* Config::ConfigFormatException::what() const throw() {
    return ("Config file is not valid format.");
}

std::map<std::string, string_map> Config::_config;
void    Config::parseConfig(const std::string &path) {
    std::ifstream ifs(path);
    if (!ifs) {
        std::cout << "Can not open file" << std::endl;
        exit(EXIT_FAILURE);
    }
    while (!ifs.eof()) {
        std::string buf;
        std::string hostname;
        std::string key;
        std::string value;
        std::getline(ifs, buf, '\n');
        if (!buf.size() || buf[0] == '#')
            continue;
        if (buf[0] != ' ') {
            bool isKeySet = false;
            hostname = buf;
            while (!ifs.eof()) {
                std::getline(ifs, buf, '\n');
                if ((ifs.eof() || buf.size() < 2
                 || buf[0] != ' ' || buf[1] == ' ')) {
                    if (isKeySet)
                        break;
                    ifs.close();
                    std::cout << "key error" << std::endl;
                    throw(Config::ConfigFormatException());
                }
                key = buf.substr(1);
                std::getline(ifs, buf, '\n');
                if (ifs.eof() || buf.size() < 3 || buf[0] != ' '
                 || buf[1] != ' ' || buf[2] == ' ') {
                    ifs.close();
                    throw(Config::ConfigFormatException());
                }
                value = buf.substr(2);
                isKeySet = true;
                _config[hostname][key] = value;
            }
        }
    }
    Config::printConfig();
    ifs.close();
}

void    Config::printConfig() {
    std::map<std::string, string_map>::iterator begin = _config.begin();
    std::map<std::string, string_map >::iterator end = _config.end();
    for (std::map<std::string, string_map>
            ::iterator itr = begin; itr != end; itr++) {
        std::cout << itr->first << std::endl;
        string_map::iterator key_begin = itr->second.begin();
        string_map::iterator key_end = itr->second.end();
        for (string_map::iterator iter = key_begin; iter != key_end; iter++) {
            std::cout << " " << iter->first << std::endl;
            std::cout << "  " << iter->second << std::endl;
        }
    }
}
