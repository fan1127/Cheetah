//
//  SocialPlatform.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/6/2557 BE.
//
//

#ifndef __Cheetah__SocialPlatform__
#define __Cheetah__SocialPlatform__

#include <stdio.h>
#include <string>
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

class SocialPlatform {
    
    std::string m_currentPlayerID;
    std::string m_currentPlayerAlias; // used in android implementation
    bool        m_authenticationComplete;
    
    void doAuthenticationCallback();
    
public:
    
    static SocialPlatform *getInstance();
    
    SocialPlatform();
    ~SocialPlatform();
    
#pragma mark - Authentication
    void *onAuthenticatedSender;
    void (*onAuthenticatedCallback)(void *sender, bool result);
    void setAuthenticationCallback(void *sender, void (*callback)(void*,bool));
    void unsetAuthenticationCallback();
    void authenticate();
    bool isLoggedIn();
    
    std::string getPlayerID();
    std::string getDisplayAlias();
    
#pragma mark - Achievement
    bool openAchievement();
    void postAchievement(std::string name, float percent);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    void signInFromSettings();
    void signOut();
    void onSignInSucceeded(std::string playerID, std::string playerAlias);
    void onSignInFailed();
#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
};

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

extern "C" {

JNIEXPORT void JNICALL
Java_com_avalable_cheetah_GPGSHelper_onSignInSucceeded(JNIEnv * env, jobject  obj, jstring playerID, jstring playerAlias);

JNIEXPORT void JNICALL
Java_com_avalable_cheetah_GPGSHelper_onSignInFailed(JNIEnv * env, jobject  obj);

}
#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#endif /* defined(__Cheetah__SocialPlatform__) */
