#ifndef SRCS_CONFIG_CONFIG_HPP_
#define SRCS_CONFIG_CONFIG_HPP_

#include <map>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <set>
#include <utility>
#include "srcs/config/ConfigChecker.hpp"
#include "srcs/util/split.hpp"
#include "srcs/util/StringConverter.hpp"

typedef std::map<std::string, std::vector<std::string> > string_vector_map;

class Config {
 public:
    static std::map<int, string_vector_map> _config;
    Config();
    ~Config();
    static void parseConfig(const std::string &path);
    static void init(const std::string &path);
    static void printConfig();
    static void testConfig();
    static std::vector<std::string> parseValue(const std::string &valueStr);
    static std::string getSingleStr(int, const std::string&);
    static int getSingleInt(int, const std::string&);
    static std::set<int> getAllListen();
    static std::vector<int>
         getVectorInt(int, const std::string&);
    static std::vector<std::string>
         getVectorStr(int, const std::string&);
    static std::map<int, std::string>
         getMapIntStr(int, const std::string&);
    static std::map<std::string, std::string> getLocation(int hostkey, const std::string& url);
    static std::vector<std::string> getLocationVector(int hostkey, const std::string& url, const std::string &key);
    static std::string getLocationString(int hostkey, const std::string& url, const std::string &key);
    static std::string findLongestMatchLocation(std::string& url, std::vector<std::string> locationVector);
    static std::vector<std::string> getAllLocation(int hostkey);
    static std::map<int, string_vector_map>::iterator getDefaultServer();
    static std::pair<int, std::string> getRedirectPair(int hostkey, const std::string& url);
    static int getVirtualHostIndex(const std::string &hostname,
                                    const std::string &port);
    static int isReturn(int virtualHostIndex);
    static bool getAutoIndex(int virtualHostIndex, const std::string& url);

    class ConfigFormatException : public std::exception {
     public:
        virtual const char* what() const throw();
    };
};

#endif /* SRCS_CONFIG_CONFIG_HPP_ */
