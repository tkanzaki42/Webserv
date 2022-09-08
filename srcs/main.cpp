#include "../includes/webserv.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <errno.h>
#include <sstream>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>

const int BUF_SIZE = 1024;
const int HTTP1_PORT = 5000;
// const int HTTP_VERSION = 1;
const std::string HTML_FILE = "index.html";

#include "util_network/Socket.hpp"

class HTTP1_Parser {
public:
    static char *get_filename(const char *excutive_file);
    static std::string argv_path_analyzer(std::string request_path, const char *path, const char *executive_file);
    static std::string get_requestline_path(char _pbuf[BUF_SIZE]);
};
char *HTTP1_Parser::get_filename(const char *excutive_file)
{
    int length = strlen(excutive_file);
    static char buf[1024]={};
    readlink( "/proc/self/exe", buf, sizeof(buf)-1 );
    int buf_length = strlen(buf);
    for(int i = 0; i <= length; i++){
       buf[buf_length-i-1] = 0;
    }
    return buf;
}
std::string HTTP1_Parser::argv_path_analyzer(std::string request_path, const char *path, const char *executive_file) {
    std::string path_string;
    if(strcmp(path,"") == 0){
        if (request_path == std::string("/")) {
           path_string = std::string(HTTP1_Parser::get_filename(executive_file)) + std::string("index.html");
        }
        else {
            int judge = request_path.rfind(".",20);
            if (judge >= 0 ) {
                path_string = std::string(HTTP1_Parser::get_filename(executive_file)) + request_path;
            } else {
                path_string = std::string(HTTP1_Parser::get_filename(executive_file)) + request_path + std::string("index.html");
            }
        }
    }
    else {
         int judge = request_path.rfind(".",20);
            if (judge >= 0 ) {
                path_string = std::string(path) + request_path;
            } else {
                path_string = std::string(path) + request_path + std::string("index.html");
            }

    }
    return path_string;
}
std::string HTTP1_Parser::get_requestline_path(char _pbuf[BUF_SIZE]) {
    int space_num = 0;
    char path_buffer[32];
    int current_point = 0;
    std::string path_string;
    for (int i = 0; i < BUF_SIZE; i++) {
        if(space_num == 2){
            path_buffer[current_point] = 0;
            path_string = std::string(path_buffer);
            return path_string;
        }
        if(space_num == 1){
            if(_pbuf[i] != ' '){
                path_buffer[current_point++] = _pbuf[i];
            }
        }
        if(_pbuf[i] == ' '){
            ++space_num;
        }
    }
    return "";
}

class HTTP1_Header {
public:
    // static std::vector<std::string>& make_response404();
    static std::vector<std::string>& make_response200(int body_length);
    // static std::vector<std::string>& make_response302(std::string path);
    // static std::vector<std::string>& make_responseUpgrade();
};
// std::vector<std::string>& HTTP1_Header::make_response404() {
//     static std::vector<std::string> header;
//     header.push_back("HTTP/1.1 404 Not Found\r\n");
//     header.push_back("Content-Type: text/html; charset=UTF-8\r\n");
//     header.push_back("Connection: close\r\n");
//     header.push_back("\r\n");
//     header.push_back("<html><body><h1>404 Not found</h1><p>The requested URL was not found on this server.</p><hr><address>Original Server</address></body></html>\r\n");

//     return header;
// }
std::vector<std::string>& HTTP1_Header::make_response200(int body_length) {
    std::ostringstream oss;
    oss << "Content-Length: " << body_length << "\r\n";

    static std::vector<std::string> header;
    header.clear();
    header.push_back("HTTP/1.1 200 OK\r\n");
    header.push_back("Content-Type: text/html; charset=UTF-8\r\n");
    header.push_back(oss.str());
    header.push_back("Connection: Keep-Alive\r\n");
    header.push_back("\r\n");

    return header;
}
// std::vector<std::string>& HTTP1_Header::make_response302(std::string path) {
//     std::ostringstream oss;
//     if (path != "") {
//         oss << "Location: https://127.0.0.1:5001" << path.c_str() << "\r\n";
//     }
//     else {
//         oss << "Location: https://127.0.0.1:5001\r\n";
//     }
//     static std::vector<std::string> header;
//     header.push_back("HTTP/1.1 302 Found\r\n");
//     header.push_back(oss.str());
//     header.push_back("\r\n");
//     return header;
// }
// std::vector<std::string>& HTTP1_Header::make_responseUpgrade() {
//     static std::vector<std::string> header;
//     header.push_back("HTTP/1.1 101 Switching Protocols\r\n");
//     header.push_back("Connection: Upgrade\r\n");
//     header.push_back("Upgrade: h2c\r\n");
//     header.push_back("\r\n");

//     return header;
// }

class HTTP1_Response {
public:
    static std::string make_response(std::vector<std::string> &header, std::vector<std::string> &message_body);
    static std::vector<std::string>& make_header(int version, int body_length, int is_file_exist, std::string path);
    static void make_body (std::vector<std::string> &body_content, int &body_content_length, std::ifstream &output_file);
};
std::string HTTP1_Response::make_response(std::vector<std::string> &header, std::vector<std::string> &message_body) {
    std::string server_response;
    int header_size = header.size();
    int body_size = message_body.size();

    std::string tmp;

    for (int i = 0; i < header_size; i++){
        server_response.append(header[i].c_str());
    }

    for (int i = 0; i < body_size; i++){
        server_response.append(message_body[i].c_str());
    }
    return server_response;
}
std::vector<std::string>& HTTP1_Response::make_header(int version, int body_length, int is_file_exist, std::string path) {
    (void)version;
    (void)is_file_exist;
    (void)path;

    // if (HTTP_VERSION == 1 && path != "") {
    //     return HTTP1_Header::make_response302(path);
    // }
    // else if (HTTP_VERSION == 2) {
    //     return HTTP1_Header::make_responseUpgrade();
    // }
    // else if (is_file_exist == 1) {
    //     return HTTP1_Header::make_response404();
    // }
    // else {
        return HTTP1_Header::make_response200(body_length);
    // }

    static std::vector<std::string> response_header;
    return response_header;
}
void HTTP1_Response::make_body(std::vector<std::string> &body_content, int &body_content_length, std::ifstream &output_file){

    if (output_file.fail() != 0) {
        std::cout << "File was not found." << std::endl;
        return;
    }

    char read_file_buf[BUF_SIZE];
    output_file.read(read_file_buf, BUF_SIZE);
    body_content.push_back(read_file_buf);
    body_content_length = output_file.gcount();
}

int run_webserver() {

    std::string executive_file = "/";
    Socket sock = Socket(HTTP1_PORT);
    sock.prepare();

    int body_length = 0;
    int is_file_exist;

    int accfd = -1;
    char buf[BUF_SIZE];

    while(1) {
        accfd = accept(sock.get_listen_fd(), (struct sockaddr*)NULL, NULL);

        //初期化
        if(accfd == -1) {
            continue;
        }
        memset(buf, 0, sizeof(buf));
        std::string recv_str = "";
        ssize_t read_size = 0;

        // \r\n\r\nが来るまでメッセージ受信
        do {
            read_size = recv(accfd, buf, sizeof(buf)-1, 0);
            if(read_size == -1) {
                std::cout << "read() failed." << std::endl;
                std::cout << "ERROR: " << errno << std::endl;
                close(accfd);
                accfd = -1;
                break;
            }
            if(read_size > 0) {
                recv_str.append(buf);
            }
            if( (recv_str[recv_str.length()-4] == 0x0d) &&
            (recv_str[recv_str.length()-3] == 0x0a) &&
            (recv_str[recv_str.length()-2] == 0x0d) &&
            (recv_str[recv_str.length()-1] == 0x0a)
            ){
                break;
            }
        } while (read_size > 0); 

        //リクエストされたパスを取得する
        std::string path = "", path_string = "";
        // std::string exe = executive_file;
        // std::size_t pos = exe.rfind('/');
        // if (pos != std::string::npos) {
        //     exe = exe.substr(pos + 1);
        // }
        path_string.clear();
        path = "";//TODO remove
        // path = HTTP1_Parser::get_requestline_path(buf);
        // path_string = HTTP1_Parser::argv_path_analyzer(path, executive_file.c_str(), exe.c_str());
        // std::cout << "path_string : " << path_string << std::endl;

        //取得したパスのファイルを開いて内容を取得する
        // std::ifstream output_file(path_string.c_str());
        // char line[256];
        // is_file_exist = output_file.fail();
        is_file_exist = -1;//TODO remove
        body_length = 0;
        std::vector<std::string> message_body;
        message_body.clear();
        // while (output_file.getline(line, 256-1)) {
        //     body_length += strlen(line);
        //     message_body.push_back(std::string(line));
        // }
        message_body.push_back("hello world");//TODO remove
        body_length = strlen("hello world");//TODO remove

        //HTTPレスポンスを作成する
        std::string server_response;
        std::vector<std::string> header = HTTP1_Response::make_header(3, body_length, is_file_exist, path);
        server_response = HTTP1_Response::make_response(header, message_body);
        std::cout << server_response << std::endl;

        //ソケットディスクリプタにレスポンス内容を書き込む
        if(send(accfd, server_response.c_str(), server_response.length(), 0) == -1){
            std::cout << "write() failed." << std::endl;
        }

        //使い終わったファイルのクローズ
        // output_file.close();
        close(accfd);
        accfd = -1;
   }
    sock.cleanup();
    return 0;
}

int main() {
    run_webserver();
    return 0;
}
