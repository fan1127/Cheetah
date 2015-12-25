//
//  APIAchievementClaim.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/10/2557 BE.
//
//

#ifndef __Cheetah__APIAchievementClaim__
#define __Cheetah__APIAchievementClaim__

#include "APIRootClass.h"

class APIAchievementClaim : public APIRootClass {
    
    std::string m_checkkey;
    
public:
    
    static APIAchievementClaim *create(std::string checkkey,
                                       void *sender,
                                       void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIAchievementClaim__) */
