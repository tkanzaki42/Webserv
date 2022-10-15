#include "Config.hpp"

Config::Config() {
}

Config::~Config() {
}

const char* Config::ConfigFormatException::what() const throw() {
    return ("Config file is not valid format.");
}

void Config::init(const std::string &path) {
    if (path.size() < 5
     || path.substr(path.size() - 5).compare(".conf")) {
        std::cerr << "Invalid extention." << std::endl;
        exit(EXIT_FAILURE);
     }
    try {
        parseConfig(path);
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        exit(EXIT_FAILURE);
    }
}

std::map<std::string, string_map> Config::_config;
void    Config::parseConfig(const std::string &path) {
    std::ifstream ifs(path);
    if (!ifs) {
        std::cerr << "Can not open file" << std::endl;
        exit(EXIT_FAILURE);
    }
    while (!ifs.eof()) {
        std::string buf;
        std::string hostname;
        std::string key;
        std::string value;
        std::getline(ifs, buf, '\n');
        if (!buf.size())
            continue;
        // インデントから始まっていない
        if (buf[0] != ' ') {
            // hostnameの中身が設定されているかのフラグ
            bool isKeySet = false;
            hostname = buf;
            // hostname の設定を読みにいく
            while (!ifs.eof()) {
                std::getline(ifs, buf, '\n');
                // インデントのチェック（インデントは１つが正しい）
                if ((ifs.eof() || buf.size() < 2
                 || buf[0] != ' ' || buf[1] == ' ')) {
                    if (isKeySet)
                        break;
                    ifs.close();
                    throw(Config::ConfigFormatException());
                }
                // インデントの次から格納する
                key = buf.substr(1);
                std::getline(ifs, buf, '\n');
                // インデントのチェック（インデントは２つが正しい）
                if (ifs.eof() || buf.size() < 3 || buf[0] != ' '
                 || buf[1] != ' ' || buf[2] == ' ') {
                    ifs.close();
                    throw(Config::ConfigFormatException());
                }
                // インデントの次から格納する
                value = buf.substr(2);
                isKeySet = true;
                // map に格納する
                _config[hostname][key] = value;
            }
        } else {
            throw(Config::ConfigFormatException());
        }
    }
    // デバッグ用 : コンフィグの中身を全て出力する
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
