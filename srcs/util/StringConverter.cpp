// Copyright 2022 tkanzaki
#include <string>
#include "srcs/util/StringConverter.hpp"

int StringConverter::stoi(const std::string &s) {
    int num;
    std::istringstream ss(s);
    ss >> num;
    return num;
}

int StringConverter::cstrtoi(const char *s) {
    int num;
    std::istringstream ss(s);
    ss >> num;
    return num;
}

std::string StringConverter::itos(int i) {
    std::ostringstream oss;
    oss << i;
    return (oss.str());
}

std::size_t StringConverter::ft_strlen(const char *str) {
    std::size_t i;

    if (!str)
        return (0);
    i = 0;
    while (str[i] != '\0')
        i++;
    return (i);
}

std::size_t StringConverter::ft_strlcpy(
        char *dst, const char *src, std::size_t dstsize) {
    std::size_t  i;

    i = 0;
    while (i < dstsize - 1) {
        dst[i] = src[i];
        if (src[i] == '\0')
            break;
        i++;
    }
    if (dstsize > 0)
        dst[i] = '\0';
    return (ft_strlen(src));
}

char* StringConverter::ft_strjoin(char const *s1, char const *s2) {
    int     s1len;
    int     s2len;
    int     i;
    char    *t;

    s1len = ft_strlen(s1);
    s2len = ft_strlen(s2);
    if (!(t = (char *)malloc(sizeof(char) * (s1len + s2len + 1))))
        return (NULL);
    i = 0;
    while (i < s1len) {
        t[i] = s1[i];
        i++;
    }
    while (i < (s1len + s2len)) {
        t[i] = s2[i - s1len];
        i++;
    }
    t[i] = '\0';
    return (t);
}
