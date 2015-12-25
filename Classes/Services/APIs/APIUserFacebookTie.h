//
//  APIUserFacebookTie.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/8/2558 BE.
//
//

#ifndef __Cheetah__APIUserFacebookTie__
#define __Cheetah__APIUserFacebookTie__

#include "APIRootClass.h"

class APIUserFacebookTie : public APIRootClass {
    
    std::string m_fbid;
    std::string m_email;
    std::string m_gender;
    std::string m_age_range_min;
    bool m_reward_gems;
    
public:
    
    static APIUserFacebookTie *create(std::string fbid,
                                      std::string email,
                                      std::string gender,
                                      std::string age_range_min,
                                      bool reward_gems,
                                      void *sender,
                                      void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIUserFacebookTie__) */
