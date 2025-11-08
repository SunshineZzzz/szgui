#include "String.h"

namespace sz_string
{
    bool IsOnlyWhitespace(const std::string& s) 
    {
        return std::ranges::all_of(s, [](unsigned char c) {
            return std::isspace(c);
        });
    }

    std::vector<int32_t> UTF8Decode(std::string_view utf8)
    {
        std::vector<int32_t> result;
        for (size_t i = 0; i < utf8.size(); ) 
        {
            uint8_t b1 = utf8[i];
            size_t len = (b1 < 0x80) ? 1 :
                (b1 & 0xE0) == 0xC0 ? 2 :
                (b1 & 0xF0) == 0xE0 ? 3 : 4;

            if (i + len > utf8.size()) break;

            char32_t cp = 0;
            for (size_t j = 0; j < len; ++j) {
                uint8_t b = utf8[i + j];
                cp |= (j ? (b & 0x3F) : (b & (0xFF >> (len + 1)))) << (6 * (len - 1 - j));
            }
            result.push_back(cp);
            i += len;
        }
        return result;
    }
}