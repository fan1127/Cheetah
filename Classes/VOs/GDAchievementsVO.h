//
//  GDAchievementsVO.h
//  Cheetah
//
//  Created by Teerapat Avalable on 5/9/57 BE.
//
//

#ifndef __Cheetah__GDAchievementsVO__
#define __Cheetah__GDAchievementsVO__

#include "cocos2d.h"
#include "GDAchievementsVO.h"

struct GDAchievementsVO {
    
    std::string         type;
    std::string         group;
    unsigned int        gems_reward;
    unsigned int        param_1;
    unsigned int        param_2;
    std::string         title;
    std::string         description;
    
    GDAchievementsVO();
    
};

#endif /* defined(__Cheetah__GDAchievementsVO__) */
