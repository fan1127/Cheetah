//
//  StringHelper.cpp
//  Cheetah
//
//  Created by Plus Pingya on 4/22/57 BE.
//
//

#include "StringHelper.h"
using namespace std;
//#include "cocos2d.h"
 #include <algorithm>

bool StringHelper::replaceString(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

// trim from start
string &StringHelper::ltrim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
    return s;
}

// trim from end
string &StringHelper::rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}

// trim from both ends
string &StringHelper::trim(string &s) {
//    cocos2d::CCLog_("string:%s",ltrim(rtrim(s)).c_str());
    
    return ltrim(rtrim(s));
}

vector<string> &StringHelper::split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector<string> StringHelper::split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

void StringHelper::replace_all(std::string &str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
}

void StringHelper::genRandomString(char *s, const int len) {

    ////////////////////
    // Code took from
    // http://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
    
    static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
    
    for (int i=0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    s[len-1] = 0;
}


#pragma mark - compres & decompres string

// Copyright 2007 Timo Bingmann <tb@panthema.net>
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

/** Compress a STL string using zlib with given compression level and return
 * the binary data. */
string StringHelper::compress_string(const string& str, int compressionlevel) {
    
    z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));
    
    if (deflateInit(&zs, compressionlevel) != Z_OK)
        throw(runtime_error("deflateInit failed while compressing."));
    
    zs.next_in = (Bytef*)str.data();
    zs.avail_in = str.size();           // set the z_stream's input
    
    int ret;
    char outbuffer[kStringHelperBufferChunk];
    string outstring;
    
    // retrieve the compressed bytes blockwise
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);
        
        ret = deflate(&zs, Z_FINISH);
        
        if (outstring.size() < zs.total_out) {
            // append the block to the output string
            outstring.append(outbuffer,
                             zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);
    
    deflateEnd(&zs);
    
    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        ostringstream oss;
        oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
        throw(runtime_error(oss.str()));
    }
    
    return outstring;
    
}

/** Decompress an STL string using zlib and return the original data. */

string StringHelper::decompress_string(const string& str) {

    z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));
    
    if (inflateInit(&zs) != Z_OK)
        throw(runtime_error("inflateInit failed while decompressing."));
    
    zs.next_in = (Bytef*)str.data();
    zs.avail_in = str.size();
    
    int ret;
    char outbuffer[kStringHelperBufferChunk];
    string outstring;
    
    // get the decompressed bytes blockwise using repeated calls to inflate
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);
        
        ret = inflate(&zs, 0);
        
        if (outstring.size() < zs.total_out) {
            outstring.append(outbuffer,
                             zs.total_out - outstring.size());
        }
        
    } while (ret == Z_OK);
    
    inflateEnd(&zs);
    
    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        ostringstream oss;
        oss << "Exception during zlib decompression: (" << ret << ") "
        << zs.msg;
        throw(runtime_error(oss.str()));
    }
    
    return outstring;
}

std::string StringHelper::toUpper(std::string value)
{
    std::string str(value);
    std::transform(str.begin(), str.end(),str.begin(), ::toupper);

    return str;
}

std::string StringHelper::capitalize(std::string str)
{
    std::string newStr(str);

    if (newStr != "")
    {
        newStr[0] = std::toupper(newStr[0]);
        for (int i=1; i<newStr.length(); i++)
        {
            newStr[i] = std::tolower(newStr[i]);
        }
    }

    return newStr;
}
