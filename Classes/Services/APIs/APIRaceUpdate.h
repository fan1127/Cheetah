//
//  APIRaceUpdate.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIRaceUpdate__
#define __Cheetah__APIRaceUpdate__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIRaceUpdate : public APIRootClass {
    
    float m_player_time;
    std::string m_player_replay;
    float m_opponent_time;
    std::string m_opponent_replay;
    std::string m_helper_pet_type;
    std::string m_helper_pet_replay;
    bool m_timeup;
    float m_ai_difficulty;
    int m_garage_level;
    
public:
    
    static APIRaceUpdate *create(float player_time,
                                 std::string player_replay,
                                 float opponent_time,
                                 std::string opponent_replay,
                                 std::string helper_pet_type,
                                 std::string helper_pet_replay,
                                 bool timeup,
                                 float ai_difficulty,
                                 int garage_level,
                                 void *sender,
                                 void (*callback)(void*,std::string));
    
    virtual std::string name() { return "race_update"; }
    virtual std::string param();
    virtual std::string param2();
    
};

#endif /* defined(__Cheetah__APIRaceUpdate__) */
