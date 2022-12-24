#ifndef SRCS_UTIL_HASH_HPP_
#define SRCS_UTIL_HASH_HPP_

#include <iostream>
#include <vector>

class Hash {
 public:
     static unsigned int generate_crc32(const std::vector<std::string> &src);
	 static std::string  convert_to_base16(unsigned int base10);
 private:
     const static unsigned int crc32_table256_[];
};

#endif  // SRCS_UTIL_HASH_HPP_
