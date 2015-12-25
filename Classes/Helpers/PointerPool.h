//
//  PointerPool.h
//  Cheetah
//
//  Created by Vaclav Samec on 12/1/2557 BE.
//
//
// PURPOSE:
//
// global manager of pointers used for async calls, pointers can be invalidated to prevent crashes


#ifndef __Cheetah__RefPointers__
#define __Cheetah__RefPointers__

#include <tr1/unordered_set>


static class PointerPool
{
private:

    static std::tr1::unordered_set<unsigned long> m_set;

public:

    static void* set(void* ptr);

    static void invalidate(void* ptr);

    static bool isValid(void* ptr);

    static void  dump();
};


#endif /* defined(__Cheetah__RefPointers__) */
