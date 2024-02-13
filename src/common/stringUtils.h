#pragma once
//
// trim from here:
// https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
//

#ifndef COMMON_STRING_UTILS_H
#define COMMON_STRING_UTILS_H

#include <vector>
#include <string>

namespace stringUtils
{
    inline bool hasSpace(const std::string& s)
    {
        for (const char& c : s)
        {
            if (std::isspace(c))
            {
                return true;
            }
        }

        return false;
    }

    // trim from start (in place)
    inline void ltrim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
            }));
    }

    // trim from end (in place)
    inline void rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
            }).base(), s.end());
    }

    // trim from both ends (in place)
    inline void trim(std::string& s) {
        rtrim(s);
        ltrim(s);
    }

    // trim from start (copying)
    inline std::string ltrim_copy(std::string s) {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    inline std::string rtrim_copy(std::string s) {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    inline std::string trim_copy(std::string s) {
        trim(s);
        return s;
    }

    inline void split(const std::string& line, const char splitter, std::vector<std::string>& splitString)
    {
        size_t begin = 0;
        size_t end = 0;
        const size_t size = line.size();

        while (begin < size)
        {
            end = line.find(splitter, begin);
            if (end == std::string::npos)
            {
                splitString.push_back(line.substr(begin));
                return;
            }

            if (begin != end)
            {
                splitString.push_back(line.substr(begin, end - begin));
            }

            begin = end + 1;
        }
    }
}

#endif
