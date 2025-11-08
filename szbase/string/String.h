#pragma once

#include <cstdint>
#include <string>
#include <ranges>
#include <cctype>
#include <vector>
#include <algorithm>

namespace sz_string
{
    // 判断字符串是否只包含空白字符
    bool IsOnlyWhitespace(const std::string& s);
    // UTF8字符串转Codepoint
    std::vector<int32_t> UTF8Decode(std::string_view utf8);
}