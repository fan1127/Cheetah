//
//  CCShareSkeletonAnimation.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 1/13/2557 BE.
//
//

#ifndef __Cheetah__CCShareSkeletonAnimation__
#define __Cheetah__CCShareSkeletonAnimation__

#include "cocos2d.h"
#include <spine/CCSkeletonAnimation.h>

namespace cocos2d { namespace extension {

class CCShareSkeletonAnimationObject: public CCObject {
    
    public:
        SkeletonData *data;
    };
    
    
    class CCShareSkeletonAnimation: public CCObject {
        CCDictionary *skeletonDictionary;
    public:
        static CCShareSkeletonAnimation* sharedSkeletonAnimation();
        bool init();
        ~CCShareSkeletonAnimation();
        CCSkeletonAnimation* getSkeleton(std::string path_, std::string spinename_);
        void clear();
    };
    
}}

#endif /* defined(__Cheetah__CCShareSkeletonAnimation__) */
