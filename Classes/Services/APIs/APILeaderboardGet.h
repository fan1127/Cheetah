//
//  APILeaderboardGet.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APILeaderboardGet__
#define __Cheetah__APILeaderboardGet__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APILeaderboardGet : public APIRootClass {
    
    std::string m_ids;
    
public:
    
    static APILeaderboardGet *create(std::string ids,
                                     void *sender,
                                     void (*callback)(void*,std::string));
    
    virtual std::string name() { return "leaderboard_get"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APILeaderboardGet__) */
