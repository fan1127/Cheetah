//
//  APIUserStatSet.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/6/2557 BE.
//
//

#ifndef __Cheetah__APIUserStatSet__
#define __Cheetah__APIUserStatSet__

#include "APIRootClass.h"

class APIUserStatSet : public APIRootClass {
    
    std::string m_stats;
    bool m_return_me_purchase_stat;
    
public:
    
    static APIUserStatSet *create(std::string stats);
    static APIUserStatSet *create(std::string stats, bool return_me_purchase_stat);
    static APIUserStatSet *create(std::string stats, bool return_me_purchase_stat, void *sender, void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIUserStatSet__) */
