// Copyright 2022 tkanzaki
#include <string>
#include "srcs/util/StringConverter.hpp"

int StringConverter::stoi(const std::string &s) {
    int num = 0;
    std::istringstream ss(s);
    ss >> num;
    return num;
}

unsigned int StringConverter::stoui(const std::string &s) {
    unsigned int num = 0;
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

std::string StringConverter::uitos(unsigned int i) {
    std::ostringstream oss;
    oss << i;
    return (oss.str());
}

std::string StringConverter::base64_encode(const std::string &src) {
    std::string                dst;
    std::vector<unsigned char> src_vector;
    for (size_t i = 0; i < src.length(); i++) {
        src_vector.push_back(src[i]);
    }
    encode_base64_(src_vector, dst);
    return (dst);
}

// base64 エンコード
bool StringConverter::encode_base64_(const std::vector<unsigned char>& src, std::string& dst)
{
    const std::string table("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
    std::string       cdst;

    for (std::size_t i = 0; i < src.size(); ++i) {
        switch (i % 3) {
        case 0:
            cdst.push_back(table[(src[i] & 0xFC) >> 2]);
            if (i + 1 == src.size()) {
                cdst.push_back(table[(src[i] & 0x03) << 4]);
                cdst.push_back('=');
                cdst.push_back('=');
            }

            break;
        case 1:
            cdst.push_back(table[((src[i - 1] & 0x03) << 4) | ((src[i + 0] & 0xF0) >> 4)]);
            if (i + 1 == src.size()) {
                cdst.push_back(table[(src[i] & 0x0F) << 2]);
                cdst.push_back('=');
            }

            break;
        case 2:
            cdst.push_back(table[((src[i - 1] & 0x0F) << 2) | ((src[i + 0] & 0xC0) >> 6)]);
            cdst.push_back(table[src[i] & 0x3F]);

            break;
        }
    }

    dst.swap(cdst);

    return true;
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
    if (!(t = new char[sizeof(char) * (s1len + s2len + 1)]))
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

void	*StringConverter::ft_memset(void *b, int c, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len)
		((unsigned char*)b)[i++] = (unsigned char)c;
	return (b);
}

char	*StringConverter::ft_strdup(char *src)
{
	char	*dest;
	int		i;

	i = 0;
	while (src[i] != '\0')
		i++;
	dest = new char[(i + 1) * sizeof(char)];
	if (dest == NULL)
		return (NULL);
	i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

char *StringConverter::ft_strncpy(char *dst, const char *src, size_t n)
{
	if (n != 0) {
		char *d = dst;
		const char *s = src;
		do {
			if ((*d++ = *s++) == 0) {
				while (--n != 0)
					*d++ = 0;
				break;
			}
		} while (--n != 0);
	}
	return (dst);
}
