//
//  APIRaceRewardClaim.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIRaceRewardClaim__
#define __Cheetah__APIRaceRewardClaim__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIRaceRewardClaim : public APIRootClass {
    
    std::string m_race_id;
    
public:
    
    static APIRaceRewardClaim *create(std::string race_id,
                                      void *sender,
                                      void (*callback)(void*,std::string));
    
    virtual std::string name() { return "race_reward_claim"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIRaceRewardClaim__) */
