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

std::map<std::string, string_vector_map> Config::_config;
void    Config::parseConfig(const std::string &path) {
    std::ifstream ifs(path.c_str());
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
                std::vector<std::string> valueVecotr = parseValue(value);
                _config[hostname][key] = valueVecotr;
            }
        } else {
            throw(Config::ConfigFormatException());
        }
    }
    // デバッグ用 : コンフィグの中身を全て出力する
    Config::printConfig();
    ifs.close();
}

std::vector<std::string> Config::parseValue(const std::string &valueStr) {
    std::vector<std::string> valueVector;
    if (valueStr.size() < 3) {
        throw(Config::ConfigFormatException());
    }
    if (valueStr[0] != '[') {
        valueVector.push_back(valueStr);
    } else if (valueStr.at(valueStr.size() - 1) != ']') {
        throw(Config::ConfigFormatException());
    } else {
        valueVector = split(valueStr.substr(1, valueStr.size() - 2), ',');
    }
    return (valueVector);
}


void    Config::printConfig() {
    std::map<std::string, string_vector_map>::iterator begin = _config.begin();
    std::map<std::string, string_vector_map >::iterator end = _config.end();
    for (std::map<std::string, string_vector_map>
            ::iterator itr = begin; itr != end; itr++) {
        std::cout << itr->first << std::endl;
        string_vector_map::iterator key_begin = itr->second.begin();
        string_vector_map::iterator key_end = itr->second.end();
        for (string_vector_map::iterator iter = key_begin;
             iter != key_end; iter++) {
            std::cout << " " << iter->first << std::endl;
            std::vector<std::string>::iterator beginV = iter->second.begin();
            std::vector<std::string>::iterator endV = iter->second.end();
            std::cout << "  ";
            for (std::vector<std::string>::iterator iterV = beginV;
                 iterV != endV; iterV++) {
                std::cout << *iterV << " ";
            }
            std::cout << std::endl;
        }
    }
}

std::string Config::getSingleStr(const std::string& hostname,
                                 const std::string& key) {
    return (_config[hostname][key][0]);
}

int Config::getSingleInt(const std::string& hostname, const std::string& key) {
    return (StringConverter::stoi(_config[hostname][key][0]));
}

std::vector<int> Config::getVectorInt(const std::string& hostname,
                                      const std::string& key) {
    std::vector<int> intVector;
    std::vector<std::string>::iterator begin = _config[hostname][key].begin();
    std::vector<std::string>::iterator end = _config[hostname][key].end();
    for (std::vector<std::string>::iterator iter = begin;
         iter != end; iter++) {
        intVector.push_back(StringConverter::stoi(*iter));
    }
    return (intVector);
}

std::vector<std::string> Config::getVectorStr(const std::string& hostname,
                                              const std::string& key) {
    return (_config[hostname][key]);
}

// test for getter
void Config::testConfig() {
    std::cout << "host_1 server_name : "
     << getSingleStr("host_1", "server_name") << std::endl;
    std::cout << "host_1 root : "
     << getSingleStr("host_1", "root") << std::endl;
    std::cout <<  "host_1 index : "
     <<getSingleStr("host_1", "index") << std::endl;
    std::vector<int> intVector = getVectorInt("host_1", "listen");
    std::vector<int>::iterator begin = intVector.begin();
    std::vector<int>::iterator end = intVector.end();
    for (std::vector<int>::iterator iter = begin; iter != end; iter++) {
        std::cout << "host_1 listen : " << *iter << std::endl;
    }
}