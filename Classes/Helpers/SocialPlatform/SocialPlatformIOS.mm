//
//  SocialPlatform.m
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/6/2557 BE.
//
//

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#import "AppController.h"
#import <Foundation/Foundation.h>

#include "SocialPlatform.h"
using namespace std;

#import <UIKit/UIKit.h>
#import <GameKit/GameKit.h>

#include <sys/types.h>
#include <sys/sysctl.h>

SocialPlatform *s_soialplatform = NULL;
SocialPlatform *SocialPlatform::getInstance() {
    
    if (!s_soialplatform) {
        s_soialplatform = new SocialPlatform();
    }
    
    return s_soialplatform;
    
}

SocialPlatform::SocialPlatform() {
    
    m_currentPlayerID = "";
    m_authenticationComplete = false;
    
}

SocialPlatform::~SocialPlatform() {
    
}

#pragma mark - Authentication

bool isGameCenterAPIAvailable() {
    
    Class gcClass = (NSClassFromString(@"GKLocalPlayer"));
    
    NSString *reqSysVer = @"4.1";
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
    
    bool osVersionSupported = ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending);
 
    return (gcClass && osVersionSupported);
    
}

void SocialPlatform::setAuthenticationCallback(void *sender, void (*callback)(void *, bool)) {
    
    onAuthenticatedSender = sender;
    onAuthenticatedCallback = callback;
    
}

void SocialPlatform::unsetAuthenticationCallback() {
    
    onAuthenticatedSender = NULL;
    onAuthenticatedCallback = NULL;
    
}

void SocialPlatform::doAuthenticationCallback() {
    
    if (onAuthenticatedCallback) {
        onAuthenticatedCallback(onAuthenticatedSender, m_authenticationComplete);
    }
    
}

void SocialPlatform::authenticate() {
    
    if (!isGameCenterAPIAvailable()) {
        doAuthenticationCallback();
        return;
    }
    
    GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
    
    [[GKLocalPlayer localPlayer] authenticateWithCompletionHandler:^(NSError *error) {
        if (error) {
            NSLog(@"%@", error);
        }
        if (localPlayer.authenticated) {
            m_authenticationComplete = true;
            if (m_currentPlayerID == "" ||
                m_currentPlayerID != [localPlayer.playerID UTF8String]) {
                m_currentPlayerID = [localPlayer.playerID UTF8String];
            }
        }else {
            m_authenticationComplete = false;
        }
        doAuthenticationCallback();
    }];
    
}

string SocialPlatform::getPlayerID() {
    
    return [[GKLocalPlayer localPlayer].playerID UTF8String];
    
}

string SocialPlatform::getDisplayAlias() {
    
    return [[GKLocalPlayer localPlayer].alias UTF8String];
    
}

#pragma mark - Achievement

bool SocialPlatform::openAchievement() {
    
    GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
    
    if(!localPlayer.authenticated)
        return false;
    
    AppController *appController = (AppController *)[UIApplication sharedApplication].delegate;
    
    GKAchievementViewController *achievementController = [[GKAchievementViewController alloc]init];
    achievementController.achievementDelegate = appController.viewController;
    
    [appController.viewController presentModalViewController:achievementController
                                                     animated:YES];

}

void SocialPlatform::postAchievement(string name, float percent) {
    
    GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
    
    if(![localPlayer isAuthenticated]) {
        NSLog(@"Gamecenter not authenticated.");
        return;
    }
    
    GKAchievement *achievement = [[GKAchievement alloc] initWithIdentifier:[NSString stringWithUTF8String: name.c_str()]];
    achievement.percentComplete = percent;
    achievement.showsCompletionBanner = YES;
    [achievement reportAchievementWithCompletionHandler:^(NSError *error) {
        if(error) {
            NSLog(@"Error : %@",error);
        } else {
            NSLog(@"Sent Achievement.");
        }
    }];

}

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_IOS