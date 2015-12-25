//
//  MerchantItemChoose.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 6/20/2557 BE.
//
//

#ifndef __Cheetah__MerchantItemChoose__
#define __Cheetah__MerchantItemChoose__

#include "../../Entities/Tutorials/CTutManager.h"

#include "../IsoMapBuilding.h"

#define MERCHANT_SOLD_SOMETHING "portrait_track_cheer_c"
#define MERCHANT_SOLD_NOTHING "portrait_idle_bye"

#define kMerchantItemCacheTimeKey   "merchant_cache_timestamp"

class MerchantItemChoose: public CTutUnit {
    
    std::string m_chosenItem;
    bool        m_chosenItemDidCallback;
    
    CTutSpeech  *m_thankYouSpeech;
    CCMenu* m_closeMenu;
    
public:
    
    /** required instance */
    
    static std::string key();
    static bool check();
    static MerchantItemChoose* create();
    virtual bool init();
    virtual void update(float delta);
    
    static void onAfterTutChooseItemPerform(void *sender);
    
    void processItem(std::string *keys);
    void updateThankYouSpeech();
    
    static void onItemCached(void* sender, std::string resp);
    
    static void onTapTheBG(void* sender, std::string result);
    static void onItemChosen(void* sender, std::string result);
    
    void* sender;
    void (*onMerchantItemChosen)(void* sender, std::string resp_json_str);
   
    void onClose(cocos2d::CCObject *object);
    
};

#endif /* defined(__Cheetah__MerchantItemChoose__) */
