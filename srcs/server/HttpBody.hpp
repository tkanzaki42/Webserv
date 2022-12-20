#ifndef SRCS_SERVER_HTTPBODY_HPP_
#define SRCS_SERVER_HTTPBODY_HPP_

#include <dirent.h>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "srcs/server_request/HttpRequest.hpp"
#include "srcs/server/HttpHeader.hpp"
#include "srcs/util/UTF8Util.hpp"
#include "srcs/util/Hash.hpp"

class HttpBody {
 private :
    static const int         BUFFER_SIZE = 1024;

    const HttpRequest&       request_;
    std::vector<std::string> content_;
    std::ifstream            output_file_;
    size_t                   content_length_;
    size_t                   content_head_;
    size_t                   content_tail_;
    unsigned int             hash_value_;          // Etag用
    unsigned int             hash_len_;
    bool                     is_compressed_; // Accept-Ranges用

    void                     count_content_length_();
    int                      compress_to_range_();
    int                      read_contents_from_file_();
    void                     make_status_response_(int status_code);
    void                     generate_hash_();

 public:
    explicit HttpBody(const HttpRequest& request);
    ~HttpBody();
    HttpBody(const HttpBody &obj);
    HttpBody &operator=(const HttpBody &obj);

    int                            make_response(int status_code);
    void                           make_autoindex_response();
    const std::vector<std::string> &get_content() const;
    std::size_t                    get_content_length() const;
    std::size_t                    get_content_head() const;
    std::size_t                    get_content_tail() const;
    unsigned int                   get_hash_value_() const;
    unsigned int                   get_hash_len_() const;
    bool                           is_compressed() const;
    bool                           has_hash() const;
    void                           clear_contents();
};

#endif  // SRCS_SERVER_HTTPBODY_HPP_
