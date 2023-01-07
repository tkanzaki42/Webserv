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

bool PathUtil::is_set_cgi_extension(const std::vector<std::string> &v,
                         const std::string &extension) {
    // CGIの設定がない
    if (v.empty()) {
        return (false);
    }
    std::vector<std::string>::const_iterator begin = v.begin();
    std::vector<std::string>::const_iterator end = v.end();
    for (std::vector<std::string>::const_iterator
         itr = begin; itr != end; itr++) {
        if (*itr == extension) {
            return (true);
        }
    }
    // 見つからなかった
    return (false);
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

bool PathUtil::is_folder_exists(const char *path) {
    struct stat st;

    if (stat(path, &st) != 0)
        return false;
    if (S_ISDIR(st.st_mode)) {
        return true;
    } else {
        return false;
    }
}

bool PathUtil::is_file_or_folder_exists(const std::string& path) {
    struct stat st;

    if (stat(path.c_str(), &st) != 0)
        return false;
    if (S_ISDIR(st.st_mode) || S_ISREG(st.st_mode)) {
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

const std::string PathUtil::get_full_path(const std::string& relative_path) {
    char        *resolved_path = NULL;
    std::string resolved_path_str = "";

    resolved_path = realpath(relative_path.c_str(), NULL);
    if (resolved_path != NULL) {
        resolved_path_str = std::string(resolved_path);
        free(resolved_path);
    } else {
        std::cerr << "Failed to resolve relative path "
            << "to absolute path in get_full_path(), relative_path = "
            << relative_path
            << ", errno = "
            << errno
            << std::endl;
    }
    return resolved_path_str;
}

const std::string PathUtil::get_last_modified_date(
        const std::string& relative_path) {
    // ファイルの情報を取得する
    struct stat st;
    if (stat(get_full_path(relative_path).c_str(), &st) != 0)
        return "";

    // 日付フォーマット変換(time_t -> struct tm)
    struct tm tm_last_modified_date = {};
    gmtime_r(&st.st_mtime, &tm_last_modified_date);
    // ローカル時間にする場合は以下
    // localtime_r(&st.st_mtime, &tm_last_modified_date);

    // 文字列に変換
    char last_modified_date[256];
    strftime(last_modified_date, 255, "%d-%b-%Y %H:%M", &tm_last_modified_date);

    return std::string(last_modified_date);
}

const std::string PathUtil::get_last_modified_datetime_full(
        const std::string& relative_path) {
    // ファイルの情報を取得する
    struct stat st;
    if (stat(get_full_path(relative_path).c_str(), &st) != 0)
        return "";

    // 日付フォーマット変換(time_t -> struct tm)
    struct tm tm_last_modified_date = {};
    gmtime_r(&st.st_mtime, &tm_last_modified_date);

    // 文字列に変換
    char buf_datetime[256];
    strftime(buf_datetime, sizeof(buf_datetime),
        "%a, %d %b %Y %H:%M:%S %Z", &tm_last_modified_date);

    return std::string(buf_datetime);
}

const std::string PathUtil::get_current_datetime() {
    // 現在日時を取得
    time_t t_datetime = time(NULL);

    // 日付フォーマット変換(time_t -> struct tm)
    struct tm tm_datetime;
    gmtime_r(&t_datetime, &tm_datetime);

    // 文字列に変換
    char buf_datetime[256];
    strftime(buf_datetime, sizeof(buf_datetime),
        "%a, %d %b %Y %H:%M:%S %Z", &tm_datetime);
    return std::string(buf_datetime);
}

const std::string PathUtil::get_filesize(const std::string& relative_path) {
    // ファイルの情報を取得する
    struct stat st;
    if (stat(get_full_path(relative_path).c_str(), &st) != 0)
        return "";

    // 文字列に変換
    std::ostringstream oss;
    oss << st.st_size;

    return oss.str();
}
