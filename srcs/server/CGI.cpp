#include "srcs/server/CGI.hpp"

CGI::CGI(const HttpRequest& request)
    : request_(request), path_(NULL),
      exec_envs_(NULL), file_type_(FILETYPE_NOT_DEFINED) {
}

CGI::~CGI() {
    cleanup_();
}

CGI::CGI(const CGI &obj) : request_(obj.request_) {
    *this = obj;
}

CGI &CGI::operator=(const CGI &obj) {
    (void)obj; //TODO(someone)
    return *this;
}

int CGI::exec_cgi(FileType file_type) {
    file_type_ = file_type;

    int pp[2];
    if (pipe(pp) == -1) {
        std::cerr << "Failed to pipe()" << std::endl;
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Failed to fork()" << std::endl;
        return EXIT_FAILURE;
    } else if (pid == 0) {
        if (close_pipe_(pp[0]) == EXIT_FAILURE)
            return EXIT_FAILURE;
        if (connect_pipe_(pp[1], 1) == EXIT_FAILURE)
            return EXIT_FAILURE;
        run_child_process_();
    }
    if (close_pipe_(pp[1]) == EXIT_FAILURE)
        return EXIT_FAILURE;

    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) == true && WEXITSTATUS(status) != 0) {
        std::cerr << "Child process was exited abnormally, status = "
            << WEXITSTATUS(status) << std::endl;
        return EXIT_FAILURE;
    }

    // パイプから読み込み
    std::string read_buffer;
    ssize_t     read_size = 0;
    char        buf[BUF_SIZE];
    while (true) {
        memset(buf, 0, sizeof(char) * BUF_SIZE);
        read_size = read(pp[0], buf, sizeof(char) * BUF_SIZE - 1);
        if (read_size <= 0)
            break;
        buf[read_size] = '\0';
        read_buffer += buf;
    }

    // 改行ごとに切ってヘッダとボディのvectorに入れる
    separate_to_header_and_body_(read_buffer);

    return EXIT_SUCCESS;
}

std::size_t CGI::get_content_length() {
    std::size_t content_length = 0;

    for (std::vector<std::string>::iterator it = body_content_.begin();
            it != body_content_.end(); ++it) {
        content_length += (*it).length();
    }
    return content_length;
}

const std::vector<std::string> &CGI::get_header_content() {
    return this->header_content_;
}

const std::vector<std::string> &CGI::get_body_content() {
    return this->body_content_;
}

void CGI::run_child_process_() {
    generate_exec_paths_();
    generate_env_vars_();

    if (file_type_ == FILETYPE_SCRIPT)
        std::cerr << "Executing cgi script: "
            << path_[0] << " " << path_[1] << std::endl << std::endl;
    else
        std::cerr << "Eecuting cgi binary: "
            << path_[0] << std::endl << std::endl;

    int ret = execve(path_[0], path_, exec_envs_);
    std::cerr << "Failed to execve(), ret = " << ret << std::endl;
    exit(ret);
}

int CGI::close_pipe_(int pipe_no) {
    if (close(pipe_no) == -1) {
        std::cerr << "Failed to close()" << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int CGI::connect_pipe_(int pipe_no_old, int pipe_no_new) {
    if (dup2(pipe_no_old, pipe_no_new) == -1) {
        std::cerr << "Failed to dup2()" << std::endl;
        return EXIT_FAILURE;
    }
    return close_pipe_(pipe_no_old);
}

void CGI::generate_exec_paths_() {
    size_t path_length       = request_.get_path_to_file().size();
    size_t path_size         = 2;
    std::string shebang_path = "";

    // シェバンがあるかどうかの判定
    if (file_type_ == FILETYPE_SCRIPT) {
        shebang_path = read_shebang_();
        if (shebang_path != "")
            path_size++;
    }

    path_ = new char*[path_size];

    // スクリプトの場合、シェバンを1つ目にする
    size_t path_i = 0;
    if (shebang_path != "") {
        path_[path_i] = new char[shebang_path.size() + 1];
        strncpy(path_[path_i], shebang_path.c_str(), shebang_path.size());
        path_[path_i][shebang_path.size()] = '\0';
        path_i++;
    }

    // ファイルパスを追加
    path_[path_i] = new char[path_length + 1];
    strncpy(path_[path_i], request_.get_path_to_file().c_str(), path_length);
    path_[path_i][path_length] = '\0';
    path_i++;

    path_[path_i] = NULL;
}

void CGI::generate_env_vars_() {
    std::map<std::string, std::string> env_vars_;
    env_vars_["SERVER_SOFTWARE"] = "Webserv/1.0";
    env_vars_["GATEWAY_INTERFACE"] = "CGI/1.1";

    exec_envs_ = new char*[env_vars_.size() + 1];
    size_t exec_envs_i = 0;
    for (std::map<std::string, std::string>::const_iterator env
                = env_vars_.begin(); env != env_vars_.end(); env++) {
        std::string env_str = env->first + "=" + env->second;
        exec_envs_[exec_envs_i] = duplicate_string_(env_str);
        exec_envs_i++;
    }
    exec_envs_[exec_envs_i] = NULL;
}

char* CGI::duplicate_string_(const std::string &str) {
    char *allocated_char = new char[str.size() + 1];
    strncpy(allocated_char, str.c_str(), str.size() + 1);
    allocated_char[str.size() + 1] = '\0';
    if (allocated_char == NULL) {
        std::cerr << "Failed to duplicate string" << std::endl;
        return NULL;
    }
    return allocated_char;
}

std::string CGI::read_shebang_() {
    std::string shebang_line;

    std::ifstream input_file(request_.get_path_to_file().c_str());
    if (!input_file.is_open()) {
        std::cerr << "Could not open the file : "
             << request_.get_path_to_file() << std::endl;
        return "";
    }
    getline(input_file, shebang_line);
    input_file.close();

    if (shebang_line.size() >= 2
            && shebang_line[0] == '#' && shebang_line[1] == '!') {
        // 末尾の改行を取り除く
        std::string::size_type extension_pos = shebang_line.rfind('\n');
        if (extension_pos != std::string::npos) {
            shebang_line.replace(shebang_line.rfind('\n'), 1, "");
        }
        extension_pos = shebang_line.rfind('\r');
        if (extension_pos != std::string::npos) {
            shebang_line.replace(shebang_line.rfind('\r'), 1, "");
        }
        return shebang_line.substr(2);
    }
    return "";
}

void CGI::separate_to_header_and_body_(const std::string& read_buffer) {
    bool is_reading_header = true;
    std::string::size_type newline_pos_prev = 0;
    while (true) {
        std::string::size_type newline_pos
                = read_buffer.find("\n", newline_pos_prev);
        if (newline_pos == std::string::npos) {
            // read_bufferの最後の部分を格納
            if (newline_pos != read_buffer.size()) {
                std::string str_line = read_buffer.substr(
                        newline_pos_prev,
                        read_buffer.size() - newline_pos_prev + 1);
                body_content_.push_back(str_line);
            }
            break;
        }

        // 改行が連続していたら、ヘッダとボディの境目ということ
        if (newline_pos == newline_pos_prev) {
            is_reading_header = false;
            newline_pos_prev++;
            continue;
        }

        // stringを切り出してヘッダ、ボディのvectorに格納
        std::string str_line = read_buffer.substr(
                newline_pos_prev, newline_pos - newline_pos_prev + 1);
        if (is_reading_header)
            header_content_.push_back(str_line);
        else
            body_content_.push_back(str_line);

        newline_pos_prev = newline_pos + 1;
    }
}

void CGI::cleanup_() {
    if (path_ != NULL) {
        for (size_t i = 0; path_[i] != NULL; ++i) {
            delete path_[i];
        }
        delete[] path_;
    }

    if (exec_envs_ != NULL) {
        for (size_t i = 0; exec_envs_[i] != NULL; ++i) {
            delete exec_envs_[i];
        }
        delete[] exec_envs_;
    }
}
