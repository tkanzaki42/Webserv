#include "srcs/util/UTF8Util.hpp"

// ref. https://ja.wikipedia.org/wiki/UTF-8

// UTF-8文字の文字幅を取得する
int UTF8Util::get_string_width(const std::string& str) {
    int wide_length = 0;
    for (int i = 0; str[i] != '\0'; ) {
        std::pair<std::string, int> ret = UTF8Util::get_one_char(str, i);
        if (ret.second == 1)
            wide_length++;
        else
            wide_length += 2;
        i += ret.second;
    }
    return wide_length;
}

// 指定した長さ未満で最大の長さ分UTF-8文字を取り出す
const std::string UTF8Util::get_limited_wide_string(
        const std::string& str, const int length) {
    std::string limited_str;
    int wide_length = 0;
    for (int i = 0; str[i] != '\0'; ) {
        std::pair<std::string, int> ret = UTF8Util::get_one_char(str, i);
        if (ret.second == 1)
            wide_length++;
        else
            wide_length += 2;
        if (length < wide_length)
            break;
        i += ret.second;
        limited_str.append(ret.first);
    }
    return limited_str;
}

// strのpos位置からUTF-8文字1文字を取り出す
// 戻り値：std::string 取り出した文字1文字
//        int 取り出した文字のstd::stringの長さ
const std::pair<std::string, int>
        UTF8Util::get_one_char(const std::string& str, const int pos) {
    if ((str[pos] & (1 << 8)) == 0) {
        // 1文字1バイト：ビットパターン"0"で始まる
        std::string s;
        s.push_back(str[pos]);
        return std::pair<std::string, int>(s, 1);
    } else if ((static_cast<unsigned int>(str[pos]) & bin_to_UInt_("11100000"))
                    == bin_to_UInt_("11000000")) {
        // 1文字2バイト：ビットパターン"110"で始まる
        std::string s;
        s.push_back(str[pos]);
        s.push_back(str[pos + 1]);
        return std::pair<std::string, int>(s, 2);
    } else if ((static_cast<unsigned int>(str[pos]) & bin_to_UInt_("11110000"))
                    == bin_to_UInt_("11100000")) {
        // 1文字3バイトビットパターン"1110"で始まる
        std::string s;
        s.push_back(str[pos]);
        s.push_back(str[pos + 1]);
        s.push_back(str[pos + 2]);
        return std::pair<std::string, int>(s, 3);
    } else if ((static_cast<unsigned int>(str[pos]) & bin_to_UInt_("11111000"))
                    == bin_to_UInt_("11110000")) {
        // 1文字4バイトビットパターン"11110"で始まる
        std::string s;
        s.push_back(str[pos]);
        s.push_back(str[pos + 1]);
        s.push_back(str[pos + 2]);
        s.push_back(str[pos + 3]);
        return std::pair<std::string, int>(s, 4);
    } else {
        // 不正
        return std::pair<std::string, int>("", 0);
    }
}

// 複数のstd::stringで表現されるUTF-8の各文字の先頭位置かを判定する
// 2バイト目以降であればビットパターン"10"で始まる
bool UTF8Util::is_start_pos(const std::string& str, const int pos) {
    if ((str[pos] & bin_to_UInt_("11000000"))
                    == bin_to_UInt_("10000000"))
        return false;
    return true;
}

// 2進数表現の文字列をunsgined int型に変換する
// 例：110 -> 6
unsigned int UTF8Util::bin_to_UInt_(const std::string &str) {
    unsigned int val = 0;
    for (std::string::size_type i = 0; i < str.size(); ++i) {
        switch (str[i]) {
        case '0':
            val *= 2;
            break;
        case '1':
            val = val * 2 + 1;
            break;
        }
    }
    return val;
}
