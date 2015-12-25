//
//  CCStringHelper.h
//  Cheetah
//
//  Created by Teerapat Avalable on 4/2/57 BE.
//
//

#include "CCStringHelper.h"
#include <sstream>
#include <iostream>
//#include "cocos2d.h"

std::string CCStringHelper::createWithNumberFormatWithSuffix(long long n, char const* _suffix)
{
    std::string s;
    s = _suffix;
    
    bool negative = (n < 0);
    if(negative) n *= -1;
    
    int cnt = 0;
    do
    {
        s.insert(0, 1, char('0' + n % 10));
        n /= 10;
        if (++cnt == 3 && n)
        {
            s.insert(0, 1, SEPARATOR);
            cnt = 0;
        }
    } while (n);
    
    if(negative) s.insert(0, 1, char('-'));
    
    return s;
}

std::string CCStringHelper::createWithNumberFormat(unsigned int n)
{
    std::string s;

    int cnt = 0;
    do
    {
        s.insert(0, 1, char('0' + n % 10));
        n /= 10;
        if (++cnt == 3 && n)
        {
            s.insert(0, 1, SEPARATOR);
            cnt = 0;
        }
    } while (n);
    
    return s;
}

std::string CCStringHelper::createWithNumberFormat(int n)
{
    std::string s;
    bool negative = (n < 0);
    if(negative) n *= -1;
    
    int cnt = 0;
    do
    {
        s.insert(0, 1, char('0' + n % 10));
        n /= 10;
        if (++cnt == 3 && n)
        {
            s.insert(0, 1, SEPARATOR);
            cnt = 0;
        }
    } while (n);
    
    if(negative) s.insert(0, 1, char('-'));
    
    return s;
}

std::string CCStringHelper::createWithNumberFormat(long long n)
{
    std::string s;
    bool negative = (n < 0);
    if(negative) n *= -1;
    
    int cnt = 0;
    do
    {
        s.insert(0, 1, char('0' + n % 10));
        n /= 10;
        if (++cnt == 3 && n)
        {
            s.insert(0, 1, SEPARATOR);
            cnt = 0;
        }
    } while (n);
    
    if(negative) s.insert(0, 1, char('-'));
    
    return s;
}

std::string CCStringHelper::createWithNumberFormat(char const* n_char)
{
    std::stringstream strValue;
    strValue << n_char;
    long long n;
    strValue >> n;
    
    std::string s;
    bool negative = (n < 0);
    if(negative) n *= -1;
    
    int cnt = 0;
    do
    {
        s.insert(0, 1, char('0' + n % 10));
        n /= 10;
        if (++cnt == 3 && n)
        {
            s.insert(0, 1, SEPARATOR);
            cnt = 0;
        }
    } while (n);
    
    if(negative) s.insert(0, 1, char('-'));
    
    return s;
}

std::string CCStringHelper::createWithLimitText(std::string string, int max) {
    
    bool pop = false;
    while (string.size() > max) {
        string.erase(string.end());
        pop = true;
    }
    if (pop) {
        string.append("..");
    }
    return string;
    
}
