#include "srcs/server/HttpResponse.hpp"

std::string HttpResponse::make_response(
        std::vector<std::string> &header,
        std::vector<std::string> &message_body) {
    std::string server_response;
    int header_size = header.size();
    int body_size = message_body.size();

    std::string tmp;

    for (int i = 0; i < header_size; i++) {
        server_response.append(header[i].c_str());
    }

    for (int i = 0; i < body_size; i++) {
        server_response.append(message_body[i].c_str());
    }
    return server_response;
}
std::vector<std::string>& HttpResponse::make_header(
        int version, int body_length, int is_file_exist, std::string path) {
    (void)version;
    (void)is_file_exist;
    (void)path;

    // if (HTTP_VERSION == 1 && path != "") {
    //     return HttpHeader::make_response302(path);
    // }
    // else if (HTTP_VERSION == 2) {
    //     return HttpHeader::make_responseUpgrade();
    // }
    // else if (is_file_exist == 1) {
    //     return HttpHeader::make_response404();
    // }
    // else {
        return HttpHeader::make_response200(body_length);
    // }

    static std::vector<std::string> response_header;
    return response_header;
}
void HttpResponse::make_body(
        std::vector<std::string> &body_content,
        int &body_content_length,
        std::ifstream &output_file) {
    if (output_file.fail() != 0) {
        std::cerr << "File was not found." << std::endl;
        return;
    }

    char read_file_buf[BUF_SIZE];
    output_file.read(read_file_buf, BUF_SIZE);
    body_content.push_back(read_file_buf);
    body_content_length = output_file.gcount();
}
