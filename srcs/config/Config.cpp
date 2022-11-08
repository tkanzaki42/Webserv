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

std::map<int, string_vector_map>::iterator Config::getDefaultServer() {
    return (_config.find(0));
}

std::map<int, string_vector_map>::iterator
     Config::getVirtualServer(const std::string &hostname) {
    std::map<int, string_vector_map>::iterator begin = _config.begin();
    std::map<int, string_vector_map >::iterator end = _config.end();
    string_vector_map::iterator defaultIter;
    for (std::map<int, string_vector_map>
            ::iterator itr = begin; itr != end; itr++) {
        defaultIter = itr->second.find("server_name");
        if (!defaultIter->second[0].compare(hostname)) {
            return (itr);
        }
    }
    return (getDefaultServer());
}

int Config::getVirtualServerIndex(const std::string &hostname) {
    std::map<int, string_vector_map>::iterator begin = _config.begin();
    std::map<int, string_vector_map >::iterator end = _config.end();
    string_vector_map::iterator defaultIter;
    int virtual_host_index = 0;
    for (std::map<int, string_vector_map>
            ::iterator itr = begin; itr != end; itr++) {
        defaultIter = itr->second.find("server_name");
        if (!defaultIter->second[0].compare(hostname)) {
            return (virtual_host_index);
        }
        virtual_host_index++;
    }
    // もし見つからなかったら一番最初のIndex（0）を返す
    return (0);
}


std::map<int, string_vector_map> Config::_config;
void    Config::parseConfig(const std::string &path) {
    bool isDefault = true;
    std::ifstream ifs(path.c_str());
    if (!ifs) {
        std::cerr << "Can not open file" << std::endl;
        exit(EXIT_FAILURE);
    }
    int hostKey = -1;
    while (!ifs.eof()) {
        std::string buf;
        std::string key;
        std::string value;
        std::getline(ifs, buf, '\n');
        if (!buf.size())
            continue;
        // インデントから始まっていない
        if (buf[0] != ' ') {
            // hostKeyの中身が設定されているかのフラグ
            bool isKeySet = false;
            // serverで始まっていないとエラー
            if (buf.compare("server")) {
                throw(Config::ConfigFormatException());
            }
            hostKey++;
            // hostKey の設定を読みにいく
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
                std::pair<std::string,
                     std::vector<std::string> > key_value_pair;
                key_value_pair = std::make_pair(key, valueVecotr);
                // もしinsert出来なかったらキーが重複している。その場合はフォーマットエラーを返す。
                if (!_config[hostKey].insert(key_value_pair).second) {
                    throw(Config::ConfigFormatException());
                }
                // 最初のサーバーネームの場合は　default フラグを立てる
                if (isDefault) {
                    std::vector<std::string> defaultVect;
                    defaultVect.push_back("true");
                    if (!_config[hostKey].insert(
                            std::make_pair("default", defaultVect)).second) {
                        throw(Config::ConfigFormatException());
                    }
                    isDefault = false;
                }
            }
        } else {
            throw(Config::ConfigFormatException());
        }
    }
    // デバッグ用 : コンフィグの中身を全て出力する
    // Config::printConfig();
    ifs.close();
    if (!ConfigChecker::isValidConfig())
        throw(Config::ConfigFormatException());
}

std::set<int> Config::getAllListen() {
    std::set<int> allListen;
    std::map<int, string_vector_map>::iterator begin = _config.begin();
    std::map<int, string_vector_map >::iterator end = _config.end();
    for (std::map<int, string_vector_map>
            ::iterator itr = begin; itr != end; itr++) {
        string_vector_map::iterator key_begin = itr->second.begin();
        string_vector_map::iterator key_end = itr->second.end();
        for (string_vector_map::iterator iter = key_begin;
             iter != key_end; iter++) {
            if (!iter->first.compare("listen")) {
                std::vector<std::string>::iterator beginV = iter->second.begin();
                std::vector<std::string>::iterator endV = iter->second.end();
                for (std::vector<std::string>::iterator iterV = beginV;
                    iterV != endV; iterV++) {
                    allListen.insert(StringConverter::stoi(*iterV));
                }
            }
        }
    }
    return (allListen);
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
    std::map<int, string_vector_map>::iterator begin = _config.begin();
    std::map<int, string_vector_map >::iterator end = _config.end();
    for (std::map<int, string_vector_map>
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

std::string Config::getSingleStr(int hostKey,
                                 const std::string& key) {
    return (_config[hostKey][key][0]);
}

int Config::getSingleInt(int hostKey, const std::string& key) {
    return (StringConverter::stoi(_config[hostKey][key][0]));
}

std::vector<int> Config::getVectorInt(int hostKey,
                                      const std::string& key) {
    std::vector<int> intVector;
    std::vector<std::string>::iterator begin = _config[hostKey][key].begin();
    std::vector<std::string>::iterator end = _config[hostKey][key].end();
    for (std::vector<std::string>::iterator iter = begin;
         iter != end; iter++) {
        intVector.push_back(StringConverter::stoi(*iter));
    }
    return (intVector);
}

std::vector<std::string> Config::getVectorStr(int hostKey,
                                              const std::string& key) {
    return (_config[hostKey][key]);
}

std::map<int, std::string> Config::getMapIntStr(int hostKey,
                                             const std::string& key) {
    std::map<int, std::string> map;
    std::vector<std::string> vectorStr = getVectorStr(hostKey, key);
    std::vector<std::string>::iterator begin = vectorStr.begin();
    std::vector<std::string>::iterator end = vectorStr.end();
    for (std::vector<std::string>::iterator iter = begin;
         iter != end; iter++) {
        // この時点でConfigの構成に従っていれば"|"区切りの要素二つの状態のはず TODO(kfukuta)
        std::vector<std::string> tmp = split(*iter, '|');
        // mapの挿入方法が問題ないか TODO(kfukuta)
        std::cout << "tmp[1]:" << tmp[1] << std::endl;
        map[StringConverter::stoi(tmp[0])] = tmp[1];
    }
    return (map);
}

// test for getter
void Config::testConfig() {
    std::cout << "host_1 server_name : "
     << getSingleStr(1, "server_name") << std::endl;
    std::cout << "host_1 root : "
     << getSingleStr(1, "root") << std::endl;
    std::cout <<  "host_1 index : "
     <<getSingleStr(1, "index") << std::endl;
    std::vector<int> intVector = getVectorInt(1, "listen");
    std::vector<int>::iterator begin = intVector.begin();
    std::vector<int>::iterator end = intVector.end();
    for (std::vector<int>::iterator iter = begin; iter != end; iter++) {
        std::cout << "host_1 listen : " << *iter << std::endl;
    }
}
