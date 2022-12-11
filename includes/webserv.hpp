#ifndef INCLUDES_WEBSERV_HPP_
#define INCLUDES_WEBSERV_HPP_

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <map>

const int BUF_SIZE           = 9001;
const int CGI_TIMEOUT_SEC    = 5;
const int MAX_URL_LENGTH     = 8177;

# define TMP_POST_DATA_DIR  "./file/"
# define TMP_POST_DATA_FILE "./file/upload_file"

// const std::string kBaseHtmlPath = "./public_html";
// const std::string kIndexHtmlFileName = "index.html";
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
