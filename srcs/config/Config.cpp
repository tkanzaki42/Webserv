#include "Config.hpp"
#include <algorithm>

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

int Config::getVirtualHostIndex(const std::string &hostname,
                                 const std::string &port) {
    std::map<int, string_vector_map>::iterator begin = _config.begin();
    std::map<int, string_vector_map >::iterator end = _config.end();
    string_vector_map::iterator serverName;
    string_vector_map::iterator listenNum;
    int virtual_host_index = 0;
    for (std::map<int, string_vector_map>
            ::iterator itr = begin; itr != end; itr++) {
        serverName = itr->second.find("server_name");
        listenNum = itr->second.find("listen");
        if (!serverName->second[0].compare(hostname)
            && !listenNum->second[0].compare(port)) {
            return (virtual_host_index);
        }
        virtual_host_index++;
    }
    // もし見つからなかったら一番最初のIndex（0）を返す
    return (0);
}

// アクセスしたlocationにリダイレクトが設定されているかを返却する
bool Config::isReturn(int virtualServerIndex, std::string &location) {
    if (Config::getLocationString(virtualServerIndex,
                                    location, "return").size()) {
        return (true);
    }
    return (false);
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

// 与えられたlocationとキーからLocationの中の設定を取得する。rootなど単数の場合その設定をStringで返却する。
std::string Config::getLocationString(int hostkey, const std::string &location, const std::string &key) {
    std::map<std::string, std::string> locationMap = Config::getLocation(hostkey, location);
    std::map<std::string, std::string>::iterator iter = locationMap.find(key);
    if (iter == locationMap.end()) {
        return "";
    }
    return (iter->second);
}

// 与えられたlocationとキーからLocationの中の設定を取得する。indexなど複数ある場合にVector（スペースで分割されていた）で返却する。
std::vector<std::string> Config::getLocationVector(int hostkey, const std::string &location, const std::string &key) {
    std::map<std::string, std::string> locationMap = Config::getLocation(hostkey, location);
    std::map<std::string, std::string>::iterator iter = locationMap.find(key);
    std::vector<std::string> v;
    if (iter == locationMap.end())
        return (v);
    return (split(iter->second, '|'));
}

// 上二つの関数のためのヘルパー関数
std::map<std::string, std::string> Config::getLocation(int hostkey, const std::string& location) {
    std::map<std::string, std::string> locationMap;
    std::vector<std::string> locationVector = Config::getVectorStr(hostkey, "location " + location);
    std::vector<std::string>::iterator begin = locationVector.begin();
    std::vector<std::string>::iterator end = locationVector.end();
    for (std::vector<std::string>::iterator itr = begin; itr != end; itr++) {
        int sep_position = itr->find('|');
        std::string key = itr->substr(0, sep_position);
        std::string value = itr->substr(sep_position + 1, itr->length());
        locationMap.insert(std::make_pair(key, value));
    }
    return locationMap;
}

std::pair<int, std::string> Config::getRedirectPair(int hostkey, const std::string& location) {
    std::vector<std::string> redirectVector =
         Config::getLocationVector(hostkey, location, "return");
    std::pair<int, std::string> redirectPair =
    std::make_pair(StringConverter::stoi(redirectVector[0]),
                     redirectVector[1]);
    return (redirectPair);
}

// あたえられたURLからlocationを決定して、そのLocationのパスを返す。
std::string Config::findLongestMatchLocation(std::string& url, std::vector<std::string> locationVector) {
    int biggesttMathDepth = 0;
    std::vector<std::string> urlVector = split(url, '/');
    std::vector<std::string>::iterator begin = locationVector.begin();
    std::vector<std::string>::iterator end = locationVector.end();
    std::string longestMatchLocation;
    for (std::vector<std::string>::iterator itr = begin; itr != end; itr++) {
        // URLとlocationが完全一致したらそのまま返す
        if (url.compare(*itr) == 0) {
            return(url);
        }
        int matchDepth = 0;
        std::vector<std::string> locationUrl = split(*itr, '/');
        std::vector<std::string>::iterator url_begin = urlVector.begin();
        std::vector<std::string>::iterator locationUrl_begin = locationUrl.begin();
        std::vector<std::string>::iterator locationUrl_iter = locationUrl_begin;
        // 要素数の少ない方をループ回数として指定する。
        int loopMax = urlVector.size() > locationUrl.size() ? locationUrl.size() : urlVector.size();
        int loopCount = 0;
        for (std::vector<std::string>::iterator iter = url_begin; loopCount < loopMax; iter++) {
            loopCount++;
            if (locationUrl_iter == locationUrl.end()) {
                // locationUrlの最深部以降は比較しない
                break;
            }
            if (iter->compare(*locationUrl_iter) == 0) {
                // パスの検索結果が一致した場合
                matchDepth++;
            } else {
                if (!locationUrl_iter->size()) {
                    locationUrl_iter++;
                    continue;
                }
                // LocationのURLと一致しない階層があったらその時点で不採用
                matchDepth = 0;
                break;
            }
            locationUrl_iter++;
        }
        if (matchDepth > biggesttMathDepth) {
            biggesttMathDepth = matchDepth;
            longestMatchLocation = *itr;
        }
    }
    if (!longestMatchLocation.size()
     && std::find(locationVector.begin(),
                     locationVector.end(),
                      "/") != locationVector.end()) {
        longestMatchLocation = "/";
    }
    return (longestMatchLocation);
}

// getLocation()の引数に渡すlocationベクタの作成
std::vector<std::string> Config::getAllLocation(int hostkey) {
    std::vector<std::string> allLocation;
    string_vector_map host = _config[hostkey];
    string_vector_map::iterator begin = host.begin();
    string_vector_map::iterator end = host.end();
    for (string_vector_map::iterator itr = begin; itr != end; itr++) {
        if (itr->first.compare(0, 8, "location") == 0) {
            allLocation.push_back(itr->first.substr(9, itr->first.size()));
        }
    }
    return (allLocation);
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
    if (valueStr[0] != '{') {
        valueVector.push_back(valueStr);
    } else if (valueStr.at(valueStr.size() - 1) != '}') {
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

bool Config::getAutoIndex(int virtualHostIndex, const std::string& url) {
    std::string autoIndexString =
            getLocationString(virtualHostIndex, url, "autoindex");
    if (!autoIndexString.compare("on"))
        return (true);
    else
        return (false);
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
