#ifndef SRCS_UTIL_PATHUTIL_HPP_
#define SRCS_UTIL_PATHUTIL_HPP_

#include <sys/stat.h>
#include <time.h>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <string>
#include <iostream>
#include <sstream>

class PathUtil {
 public:
     static bool                is_file_exists(const std::string& path);
     static bool                is_folder_exists(const std::string& path);
     static bool                is_file_or_folder_exists(
                                    const std::string& path);
     static bool                is_folder_exists(
                                    const char *path);
     static const std::string   get_file_extension(const std::string& path);
     static const std::string   get_full_path(const std::string& relative_path);
     static const std::string   get_last_modified_date(
                                    const std::string& relative_path);
     static const std::string   get_last_modified_datetime_full(
                                    const std::string& relative_path);
     static const std::string   get_current_datetime();
     static const std::string   get_filesize(const std::string& relative_path);
};

#endif  // SRCS_UTIL_PATHUTIL_HPP_
