//
//  CCShareSkeletonAnimation.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 1/13/2557 BE.
//
//

#include "CCShareSkeletonAnimation.h"
#include "../Settings.h"

USING_NS_CC;
using namespace std;
using namespace cocos2d::extension;

#pragma mark - CCShareSkeletonAnimationObject

#pragma mark - CCShareSkeletonAnimation

static CCShareSkeletonAnimation* s_sharedSkeletonAnimation;


CCShareSkeletonAnimation* CCShareSkeletonAnimation::sharedSkeletonAnimation()
{
    if (s_sharedSkeletonAnimation == NULL)
    {
        s_sharedSkeletonAnimation = new CCShareSkeletonAnimation();
        s_sharedSkeletonAnimation->init();
    }
    return s_sharedSkeletonAnimation;
}

bool CCShareSkeletonAnimation::init()
{
    skeletonDictionary = CCDictionary::create();
    skeletonDictionary->retain();
    return true;
}


CCShareSkeletonAnimation::~CCShareSkeletonAnimation() {
    
    if (skeletonDictionary) {
        skeletonDictionary->release();
        skeletonDictionary = NULL;
    }
    
}

CCSkeletonAnimation* CCShareSkeletonAnimation::getSkeleton(string path_, string spinename_) {

    string key = path_.append(spinename_);
    
    CCSkeletonAnimation* spine = (CCSkeletonAnimation*)skeletonDictionary->objectForKey(key);
    if (spine) {
        return CCSkeletonAnimation::createWithData(spine->skeleton->data);
    }
    
    CREATE_CHAR_BUFFER(buffer_json, 128);
    CREATE_CHAR_BUFFER(buffer_atlast, 128);
    snprintf(buffer_json, buffer_json_size, "%s.json", key.c_str());
    snprintf(buffer_atlast, buffer_atlast_size, "%s.atlas", key.c_str());
    spine = cocos2d::extension::CCSkeletonAnimation::createWithFile(buffer_json, buffer_atlast);
    skeletonDictionary->setObject(spine, key);
    
    return CCSkeletonAnimation::createWithData(spine->skeleton->data);
}

void CCShareSkeletonAnimation::clear() {
    
    skeletonDictionary->removeAllObjects();
    
}