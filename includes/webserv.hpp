#ifndef INCLUDES_WEBSERV_HPP_
#define INCLUDES_WEBSERV_HPP_

#include <string>

const int BUF_SIZE           = 9001;
const int CGI_TIMEOUT_SEC    = 5;
const int MAX_URL_LENGTH     = 8177;

#define BASIC_AUTH   false
#define ETAG_ENABLED true

#define TMP_POST_DATA_FILE "upload_file"

const std::string kServerSoftwareName = "Webserv/1.0";

enum HttpMethod {
    METHOD_NOT_DEFINED = 0,
    METHOD_POST = 1,
    METHOD_GET = 2,
    METHOD_DELETE = 3
};

enum FileType {
    FILETYPE_NOT_DEFINED = 0,
    FILETYPE_STATIC_HTML = 1,
    FILETYPE_SCRIPT = 2,
    FILETYPE_BINARY = 3
};

#endif  // INCLUDES_WEBSERV_HPP_
