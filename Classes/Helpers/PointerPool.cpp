//
//  PointerPool.cpp
//  Cheetah
//
//  Created by Vaclav Samec on 12/1/2557 BE.
//
//

#include "PointerPool.h"
#include "Settings.h"


std::tr1::unordered_set<unsigned long> PointerPool::m_set;

void* PointerPool::set(void* ptr)
{
    m_set.insert(reinterpret_cast<unsigned long>(ptr));
    return ptr;
}

void PointerPool::invalidate(void* ptr)
{
    m_set.erase(reinterpret_cast<unsigned long>(ptr));
}

bool PointerPool::isValid(void* ptr)
{
    return m_set.find(reinterpret_cast<unsigned long>(ptr)) != m_set.end();
}

void  PointerPool::dump()
{
    CCLog_("UserData::dump");
    for (auto& item : m_set)
    {
        //CCLog_("%s -> %f", item.first.c_str(), item.second);
    }
}
