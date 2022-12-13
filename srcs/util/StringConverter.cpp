// Copyright 2022 tkanzaki
#include <string>
#include "srcs/util/StringConverter.hpp"

int StringConverter::stoi(const std::string &s) {
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
