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
    static std::map<std::string, string_vector_map> _config;
    Config();
    ~Config();
    static void parseConfig(const std::string &path);
    static void init(const std::string &path);
    static void printConfig();
    static void testConfig();
    static std::vector<std::string> parseValue(const std::string &valueStr);
    static std::string getSingleStr(const std::string&, const std::string&);
    static int getSingleInt(const std::string&, const std::string&);
    static std::set<int> getAllListen();
    static std::vector<int>
         getVectorInt(const std::string&, const std::string&);
    static std::vector<std::string>
         getVectorStr(const std::string&, const std::string&);

    class ConfigFormatException : public std::exception {
     public:
        virtual const char* what() const throw();
    };
};

#endif /* SRCS_CONFIG_CONFIG_HPP_ */
