//
// Created by Vaclav Samec on 11/11/14 AD.
//


#ifndef __UserData_H_
#define __UserData_H_

#include <string>
#include <tr1/unordered_map>

#include "Settings.h"

//
// dictionary with hash map
//
template <typename T>
class Dictionary
{
private:

    std::tr1::unordered_map<std::string, T> map;

public:

    T    get(std::string pKey, T defaultValue) const
    {
        auto found = map.find(pKey);
        if (found != map.end())
        {
            return found->second;
        }

        return defaultValue;
    }

    void    set(std::string key, T value)
    {
        map[key] = value;
    }

    void     clear()
    {
        map.clear();
    }

    void  dump()
    {
        CCLog_("UserData::dump");
        for (auto& item : map)
        {
            CCLog_("%s -> %f", item.first.c_str(), item.second);
        }
    }
};


#endif //__UserData_H_
