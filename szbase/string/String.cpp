#include "String.h"

namespace sz_string
{
    bool IsOnlyWhitespace(const std::string& s) 
    {
        return std::ranges::all_of(s, [](unsigned char c) {
            return std::isspace(c);
        });
    }

    std::tuple<bool,std::vector<int32_t>> UTF8Decode(std::string_view utf8)
    {
        std::vector<int32_t> result;
        size_t i = 0;

        while (i < utf8.size())
        {
            uint8_t b1 = utf8[i];
            int32_t cp = 0;
            size_t len = 0;

            if ((b1 & 0x80) == 0x00) 
            {
                len = 1;
                cp = b1;
            }
            else if ((b1 & 0xE0) == 0xC0) 
            {
                len = 2;
                cp = b1 & 0x1F;
            }
            else if ((b1 & 0xF0) == 0xE0) 
            {
                len = 3;
                cp = b1 & 0x0F;
            }
            else if ((b1 & 0xF8) == 0xF0) 
            {
                len = 4;
                cp = b1 & 0x07;
            }
            else 
            {
                return { false, std::move(result) };
            }

            if (i + len > utf8.size()) 
            {
                return { false, std::move(result) };
            }

            for (size_t j = 1; j < len; ++j) 
            {
                uint8_t b_cont = utf8[i + j];

                if ((b_cont & 0xC0) != 0x80) 
                {
                    return { false, std::move(result) };
                }

                cp = (cp << 6) | (b_cont & 0x3F);
            }

            result.push_back(cp);
            i += len;
        }

        return { true, std::move(result)};
    }
}