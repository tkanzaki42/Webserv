#ifndef SRCS_CONFIG_CONFIG_HPP_
#define SRCS_CONFIG_CONFIG_HPP_

#include <map>
#include <iostream>
#include <string>
#include <fstream>

class Config {
 private:
    static std::map<std::string, std::map<std::string, std::string> > _config;
 public:
    Config();
    ~Config();
    static void parseConfig(const std::string &path);
    static void printConfig();

    class ConfigFormatException : public std::exception {
     public:
        virtual const char* what() const throw();
    };
};

#endif /* SRCS_CONFIG_CONFIG_HPP_ */
