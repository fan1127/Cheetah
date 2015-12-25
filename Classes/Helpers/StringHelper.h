//
//  StringHelper.h
//  Cheetah
//
//  Created by Kaweerut on 4/22/57 BE.
//
//

#ifndef __Cheetah__StringHelper__
#define __Cheetah__StringHelper__

#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>


#include <string>
#include <stdexcept>
#include <iomanip>
#include <zlib.h>

#define kStringHelperBufferChunk     132072

class StringHelper {
    
public:
    
    static bool replaceString(std::string& str, const std::string& from, const std::string& to);
    
    // trim from start
    static std::string &ltrim(std::string &s);
    // trim from end
    static std::string &rtrim(std::string &s);
    
    // trim from both ends
    static std::string &trim(std::string &s);

    static void replace_all(std::string &str, const std::string& from, const std::string& to);
    
    static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
    
    static std::vector<std::string> split(const std::string &s, char delim);
    
    static void genRandomString(char *s, const int len);

    static std::string compress_string(const std::string& str, int compressionlevel = Z_BEST_COMPRESSION);

    static std::string decompress_string(const std::string& str);

    static std::string toUpper(std::string str);

    static std::string capitalize(std::string str);

    template <typename T>
    static std::string toString(T value)
    {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    static int toInt(const char* value)
    {
        if (value == NULL)
        {
            return 0;
        }
        return std::atoi(value);
    }
};

#endif /* defined(__Cheetah__StringHelper__) */
