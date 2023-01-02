#ifndef SRCS_SERVER_REQUEST_HTTPREQUEST_HPP_
#define SRCS_SERVER_REQUEST_HTTPREQUEST_HPP_

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <vector>
#include <netinet/in.h>

#include "includes/webserv.hpp"
#include "srcs/server_request/HttpParser.hpp"
#include "srcs/server_request/HttpAuth.hpp"
#include "srcs/server_request/HttpAuthType.hpp"
#include "srcs/util/PathUtil.hpp"
#include "srcs/util/StringConverter.hpp"

# define REQUEST_ENTITY_MAX 1000000

class HttpRequest {
 public:
    HttpRequest();
    ~HttpRequest();
    HttpRequest(const HttpRequest &obj);
    HttpRequest& operator=(const HttpRequest &obj);

    void                reset();
    bool                receive_header();
    void                analyze_request(int port);
    void                print_debug();
    bool                receive_and_store_to_file(bool is_not_readed_header);

    // getter(HttpParser)
    HttpMethod          get_http_method() const;
    const std::string&  get_request_target() const;
    const std::string&  get_query_string() const;
    const std::string&  get_path_info() const;
    const std::string&  get_path_to_file() const;
    const std::string&  get_http_ver() const;
    const std::string&  get_location() const;
    const std::string&  get_header_field(const std::string& key);
    const std::map<std::string, std::string>&
                        get_header_field_map() const;
    // getter(HttpRequest)
    FileType            get_file_type() const;
    int                 get_status_code() const;
    int                 get_virtual_host_index() const;
    bool                get_is_autoindex() const;
    struct sockaddr_in  get_client_addr();
    bool                get_is_header_analyzed();
    //  getter(HttpResponse)
    const std::pair<int, std::string>
                        get_redirect_pair() const;
    // setter
    void                set_file_type(FileType file_type);
    void                set_readpipe(int pp);
    void                set_client_addr(struct sockaddr_in  client_addr);

 private:
    HttpAuth            auth_;
    HttpParser          parser_;
    std::string         received_line_;
    std::string         location_;
    FileType            file_type_;
    int                 status_code_;
    int                 virtual_host_index_;
    bool                is_autoindex_;
    int                 client_max_body_size;
    std::pair<int, std::string>
                        redirect_pair_;
    std::string         upload_dir;
    int                 readpipe_;
    struct sockaddr_in  client_addr_;
    bool                is_header_analyzed_;
    std::string         upload_data_;

    void                check_redirect_();
    void                check_authorization_();
    bool                receive_chunked_data_(bool is_not_readed_header);
    bool                recv_and_join_data_();
    std::pair<int, int> split_chunk_size_();
    bool                receive_plain_data_(bool is_not_readed_header);
    bool                write_to_file_();
    bool                write_to_file_append_(std::size_t append_length);
    int                 delete_file_();
    bool                is_allowed_method(std::vector<std::string> method,
                                          const std::string &upload_dir);
    bool                is_set_cgi_extension(std::vector<std::string> v,
                                              const std::string &extention);
    std::string         replacePathToLocation_(std::string &location,
                                              std::string &path, 
                                              std::string &root);
    int                 validate_received_header_line_(const std::string &buf);
};

#endif  // SRCS_SERVER_REQUEST_HTTPREQUEST_HPP_
