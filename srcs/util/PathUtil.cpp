#include "srcs/util/PathUtil.hpp"

bool PathUtil::is_file_exists(const std::string& path) {
    struct stat st;

    if (stat(path.c_str(), &st) != 0)
        return false;
    if (S_ISREG(st.st_mode)) {
        return true;
    } else {
        return false;
    }
}

bool PathUtil::is_folder_exists(const std::string& path) {
    struct stat st;

    if (stat(path.c_str(), &st) != 0)
        return false;
    if (S_ISDIR(st.st_mode)) {
        return true;
    } else {
        return false;
    }
}

const std::string PathUtil::get_file_extension(const std::string& path) {
    std::string::size_type extension_pos = path.rfind(".");
    if (extension_pos == std::string::npos) {
        // 見つからなかった場合
        return "";
    }
    std::string extension_str = path.substr(extension_pos + 1);
    return extension_str;
}
