#ifndef SRCS_SERVER_REQUEST_HTTPPARSER_HPP_
#define SRCS_SERVER_REQUEST_HTTPPARSER_HPP_

#include <iostream>
#include <string>
#include <utility>
#include <map>
#include <fstream>

#include "includes/webserv.hpp"
#include "srcs/util/PathUtil.hpp"
#include "srcs/config/Config.hpp"

class HttpParser {
 public:
    explicit HttpParser(const std::string& received_line_);
    ~HttpParser();
    HttpParser(const HttpParser &obj);
    HttpParser& operator=(const HttpParser &obj);

    int                          parse();
    void                         separate_querystring_pathinfo();

    // getter
    HttpMethod                   get_http_method() const;
    const std::string&           get_request_target() const;
    const std::string&           get_query_string() const;
    const std::string&           get_path_info() const;
    const std::string&           get_path_to_file() const;
    const std::string&           get_http_ver() const;
    const std::map<std::string, std::string>&
                                 get_header_field_map() const;
    const std::string&           get_header_field(const std::string& key);
    const std::string            get_remain_buffer();

    //  setter
    void                setIndexHtmlFileName(const std::string &filename);
    void                         setBaseHtmlPath(const std::string &path);

 private:
    std::size_t                         read_idx_;
    const std::string&                  received_line_;

    // parsed data
    HttpMethod                          http_method_;
    std::string                         request_target_;
    std::string                         query_string_;
    std::string                         path_info_;
    std::string                         path_to_file_;
    std::string                         http_ver_;
    std::map<std::string, std::string>  header_field_;
   
    // get from confign file data
    std::string   baseHtmlPath;
    std::string   indexHtmlFileName;

    // parser
    void         parse_method_();
    void         parse_request_target_();
    std::string  split_query_string_(std::string &request_target);
    void         generate_path_to_file_(std::string &remaining_path);
    void         split_path_info_();
    void         parse_http_ver_();
    void         parse_header_field_();
    std::pair<std::string, std::string>
                 parse_one_header_field_();
    void         skip_space_();
    void         skip_crlf_();
    void         rtrim_(std::string &str);
    int          validate_parsed_data_();
};

#endif  // SRCS_SERVER_REQUEST_HTTPPARSER_HPP_
