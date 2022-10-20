#ifndef SRCS_SERVER_REQUEST_HTTPPARSER_HPP_
#define SRCS_SERVER_REQUEST_HTTPPARSER_HPP_

#include <iostream>
#include <string>
#include <utility>
#include <map>
#include <fstream>

#include "includes/webserv.hpp"

class HttpParser {
 public:
    explicit HttpParser(const std::string& received_line_);
    ~HttpParser();
    HttpParser(const HttpParser &obj);
    HttpParser& operator=(const HttpParser &obj);

    int                          parse();
    HttpMethod                   get_http_method() const;
    const std::string&           get_request_path() const;
    const std::string&           get_query_string() const;
    const std::string&           get_http_ver() const;
    const std::map<std::string, std::string>&
                                 get_header_field_map() const;
    const std::string&           get_header_field(const std::string& key);
    const std::string            get_remain_buffer();

 private:
    std::size_t                         read_idx_;
    const std::string&                  received_line_;

    // parsed data
    HttpMethod                          http_method_;
    std::string                         request_path_;
    std::string                         query_string_;
    std::string                         http_ver_;
    std::map<std::string, std::string>  header_field_;

    // parser
    void                                 parse_method_();
    void                                 parse_request_path_();
    void                                 parse_http_ver_();
    void                                 parse_header_field_();
    std::pair<std::string, std::string>  parse_one_header_field_();
    void                                 skip_space_();
    void                                 skip_crlf_();
    void                                 rtrim_(std::string &str);
    int                                  validate_parsed_data_();
};

#endif  // SRCS_SERVER_REQUEST_HTTPPARSER_HPP_
