#pragma once

#include <string>
#include <ranges>
#include <cctype>
#include <algorithm>

namespace sz_string
{
    bool IsOnlyWhitespace(const std::string& s) 
    {
        return std::ranges::all_of(s, [](unsigned char c) {
            return std::isspace(c);
        });
    }
}