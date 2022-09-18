#include "srcs/server/HttpHeader.hpp"

std::vector<std::string>& HttpHeader::make_response200(int body_length) {
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

// std::vector<std::string>& HttpHeader::make_response404() {
//     static std::vector<std::string> header;
//     header.push_back("HTTP/1.1 404 Not Found\r\n");
//     header.push_back("Content-Type: text/html; charset=UTF-8\r\n");
//     header.push_back("Connection: close\r\n");
//     header.push_back("\r\n");
//     header.push_back("<html><body><h1>404 Not found</h1><p>The requested URL was not found on this server.</p><hr><address>Original Server</address></body></html>\r\n");

//     return header;
// }

// std::vector<std::string>& HttpHeader::make_response302(std::string path) {
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

// std::vector<std::string>& HttpHeader::make_responseUpgrade() {
//     static std::vector<std::string> header;
//     header.push_back("HTTP/1.1 101 Switching Protocols\r\n");
//     header.push_back("Connection: Upgrade\r\n");
//     header.push_back("Upgrade: h2c\r\n");
//     header.push_back("\r\n");

//     return header;
// }
