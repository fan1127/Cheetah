//
//  CCFacebookSDK.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 5/8/2557 BE.
//
//

#include <iostream>
#include <vector>

#include "../../AppMacros.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#endif//End of (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

struct FacebookUser {
    
    std::string fbid;
    std::string name;
    std::string email;
    std::string gender;
    std::string gender_full;
    std::string age_range_min;
    
    FacebookUser();
    
};

class FacebookController {
    
//    void* sender;
//    void *

private:
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	void initialize();

#endif //End of (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

public:
    
    void* sessionStateChangedSender;
    void (*sessionStateChangedCallBack)(void*, bool);
    
    bool m_bIsLoggedIn;
    
    void*                       session; //FBSession* session
    
    FacebookUser                user;
    int                         total_friends;
    std::vector<FacebookUser>   installed_friends;
    
    FacebookController();
//    FacebookController(void* sender, void (*callback)(void*, bool));
//    ~FacebookController();å
    
    void createNewSession();
    void publishOpenGraphStoryStep1(const char* actionType,
                                    const char* objectType,
                                    const char* title,
                                    const char* description,
                                    const char* url,
                                    const char* imageUrl,
                                    bool explicitlyShared);
    void publishOpenGraphStoryStep2(const char* actionType,
                                    const char* objectType,
                                    const char* title,
                                    const char* description,
                                    const char* url,
                                    const char* imageUrl,
                                    bool explicitlyShared);
    
    void tryLoginIfThereIsTokenLoaded();
    void login();
    void login(void* sender, void (*callback)(void*, bool));
    void logout(void* sender, void (*callback)(void*, bool));
    
    void fetchUserDetails(bool rewardGems, void* sender, void (*callback)(void*, bool, bool));
    void inviteFriends(std::string title, std::string message, void* sender, void (*callback)(void*, int numberOfInvites, std::string fbids));
    void getInstalledFriends(void* sender, void (*callback)(void*, bool));
    void getInvitedFriends(void* sender, void (*callback)(void*, bool));
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    void* userFetchSender;
    void (*userFetchCallBack)(void*, bool, bool);
    bool userFetchReward;

    void* inviteFriendsSender;
    void (*inviteFriendsCallback)(void*, int, std::string);

    void* appFriendSender;
    void (*appFriendCallback)(void*,bool);

    void* fb_sceneManager;
    void (*fb_sceneManagerCallback)(void*,bool);
    void assignSessionStateCallback(void* sender, void (*callback)(void*, bool));

    static FacebookController* sharedInstance();
    static bool isFacebookOnTop();

    void assignSharedInstance(FacebookController* controller);

    void checkIfLoggedIn();
    void loginSuccessful();

    void loginCallback(bool success);
    void userDetailsCallback(const char* fbid, const char* fname, const char* email, const char* gender_full, const char* gender, const char* ageMin);
    void onFriendsInvited(int count, const char* output);
    void onAppFriendsRetrieved(int count);
    void registerAppFriends(const char* fbid, const char* fname, int count);
    void onFacebookCancelled();
    void onApplicationEnterForeground();

    void checkSceneManagerFromSession(bool success);

#endif //End of (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

    bool isLoggedIn() {
        return m_bIsLoggedIn;
    }
    
//    static void onFBLoggedIn(void* sender_, bool success_);
    
};

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"

extern "C"
{
	JNIEXPORT void JNICALL Java_ppl_cocos2dx_ranchrun_facebook_FacebookHelper_loginResultCallback (JNIEnv *env, jobject obj, jboolean success);
	JNIEXPORT void JNICALL Java_ppl_cocos2dx_ranchrun_facebook_FacebookHelper_userDetailsCallback (JNIEnv *env, jobject obj, jstring fbid, jstring fname, jstring email, jstring gender_full, jstring gender, jstring ageMin);
	JNIEXPORT void JNICALL Java_ppl_cocos2dx_ranchrun_facebook_FacebookHelper_inviteFriendsCallback(JNIEnv *env, jobject obj, int count, jstring output);
	JNIEXPORT void JNICALL Java_ppl_cocos2dx_ranchrun_facebook_FacebookHelper_friendsWithAppCallback(JNIEnv *env, jobject obj, int count, jstring userIds, jstring userNames);
	JNIEXPORT void JNICALL Java_ppl_cocos2dx_ranchrun_facebook_FacebookHelper_onActionCancelled(JNIEnv *env, jobject obj);
	JNIEXPORT void JNICALL Java_ppl_cocos2dx_ranchrun_facebook_FacebookHelper_tryLoginCompleted(JNIEnv *env, jobject obj);
	JNIEXPORT void JNICALL Java_ppl_cocos2dx_ranchrun_facebook_FacebookHelper_checkSceneManager(JNIEnv *env, jobject obj, jboolean success);
}

#endif //End of (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
