#include "Webserv.hpp"

int run_webserver() {

    std::string executive_file = "/";
    Socket sock = Socket(HTTP_PORT);
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
        std::vector<std::string> header = HttpResponse::make_header(3, body_length, is_file_exist, path);
        server_response = HttpResponse::make_response(header, message_body);
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
