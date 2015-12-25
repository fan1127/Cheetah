/*
 * FacebookControllerAnd.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: patrick.pamplona
 */

#include "FacebookController.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include <android/log.h>

#include "platform/android/jni/JniHelper.h"
#include "../InAppPurchase/InAppPurchase.h"

#include "../../Settings.h"
using namespace cocos2d;

static FacebookController* s_FacebookInstance;
static bool isOnTop;

//#region Android Equivalent of Facebook implementation
FacebookUser::FacebookUser()  {

    fbid = "unknown";
    name = "unknown";
    email = "unknown";
    gender = "";
    gender_full = "unknown";
    age_range_min = "unknown";
}

FacebookController* FacebookController::sharedInstance()
{
	return s_FacebookInstance;
}

void FacebookController::assignSharedInstance(FacebookController* controller)
{
	s_FacebookInstance = controller;
}

void FacebookController::assignSessionStateCallback(void* sender, void (*callback)(void*, bool))
{
	fb_sceneManager = sender;
	fb_sceneManagerCallback = callback;
}

bool FacebookController::isFacebookOnTop()
{
	return isOnTop;
}

FacebookController::FacebookController()
{
	sessionStateChangedSender = NULL;
	sessionStateChangedCallBack = NULL;

	userFetchCallBack = NULL;
	userFetchSender = NULL;

	inviteFriendsSender = NULL;
	inviteFriendsCallback = NULL;

	appFriendSender = NULL;
	appFriendCallback = NULL;

	user = FacebookUser();

	userFetchReward = false;

	isOnTop = false;

	checkIfLoggedIn();
	initialize();
}

void FacebookController::checkIfLoggedIn()
{
	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo, "ppl/cocos2dx/ranchrun/facebook/FacebookHelper", "isLoggedIn_", "()Z"))
	{
		m_bIsLoggedIn = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}

void FacebookController::initialize()
{
	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo, "ppl/cocos2dx/ranchrun/facebook/FacebookHelper", "initialize_", "(Ljava/lang/String;)V"))
	{
		jstring stringArg = methodInfo.env->NewStringUTF(kFacebookAppIDChar);

		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringArg);
		methodInfo.env->DeleteLocalRef(stringArg);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}

void FacebookController::publishOpenGraphStoryStep1(const char* actionType,
								const char* objectType,
								const char* title,
								const char* description,
								const char* url,
								const char* imageUrl,
								bool explicitlyShared)
{
	//TODO: Add Publish Open Graph Story Step 1 functionality for Android
}

void FacebookController::publishOpenGraphStoryStep2(const char* actionType,
								const char* objectType,
								const char* title,
								const char* description,
								const char* url,
								const char* imageUrl,
								bool explicitlyShared)
{
	//TODO: Add Publish Open Graph Story Step 2 functionality for Android
}

void FacebookController::tryLoginIfThereIsTokenLoaded()
{
	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo, "ppl/cocos2dx/ranchrun/facebook/FacebookHelper", "tryToLoginFromToken_", "()V"))
	{
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}

void FacebookController::login()
{
	login(NULL, NULL);
}

void FacebookController::login(void* sender, void (*callback)(void*, bool))
{
	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo, "ppl/cocos2dx/ranchrun/facebook/FacebookHelper", "login_", "()V"))
	{
		sessionStateChangedCallBack = callback;
		sessionStateChangedSender = sender;

		isOnTop = true;

		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}

void FacebookController::logout(void* sender, void (*callback)(void*, bool))
{
	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo, "ppl/cocos2dx/ranchrun/facebook/FacebookHelper", "logout_", "()V"))
	{
		m_bIsLoggedIn = false;
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);

		if (callback)
		{
			callback(sender, true);
		}
	}
}

void FacebookController::fetchUserDetails(bool rewardGems, void* sender, void (*callback)(void*, bool, bool))
{
	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo, "ppl/cocos2dx/ranchrun/facebook/FacebookHelper", "fetchUserDetails_", "()V"))
	{
		userFetchSender = sender;
		userFetchCallBack = callback;
		userFetchReward = rewardGems;

		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}

void FacebookController::inviteFriends(std::string title, std::string message, void* sender, void (*callback)(void*, int numberOfInvites, std::string fbids))
{
	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo, "ppl/cocos2dx/ranchrun/facebook/FacebookHelper", "inviteFriends_", "(Ljava/lang/String;Ljava/lang/String;)V"))
	{
		jstring titleArg = methodInfo.env->NewStringUTF(title.c_str());
		jstring messageArg = methodInfo.env->NewStringUTF(message.c_str());

		inviteFriendsCallback = callback;
		inviteFriendsSender = sender;

		isOnTop = true;

		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, titleArg, messageArg);
		methodInfo.env->DeleteLocalRef(titleArg);
		methodInfo.env->DeleteLocalRef(messageArg);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}

void FacebookController::getInstalledFriends(void* sender, void (*callback)(void*, bool))
{
	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo, "ppl/cocos2dx/ranchrun/facebook/FacebookHelper", "getInstalledFriends_", "()V"))
	{
		appFriendSender = sender;
		appFriendCallback = callback;

		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}
//#endregion //End of Android equivalent of Facebook implementation


//#region C++ Callbacks
void FacebookController::loginSuccessful()
{
	m_bIsLoggedIn = true;
}

void FacebookController::loginCallback(bool success)
{
	if(sessionStateChangedSender && sessionStateChangedCallBack)
	{
		sessionStateChangedCallBack(sessionStateChangedSender, success);
	}

	//isOnTop = false;

	sessionStateChangedSender = NULL;
	sessionStateChangedCallBack = NULL;
}

void FacebookController::userDetailsCallback(const char* fbid, const char* fname, const char* email, const char* gender_full, const char* gender, const char* ageMin)
{
	user.fbid = fbid;
	user.name = fname;
	user.email = email;
	user.gender_full = gender_full;
	user.gender = gender;
	user.age_range_min = ageMin;

	__android_log_print(ANDROID_LOG_INFO, "CUSTOM_TAG", "User id - name : %s - %s", user.fbid.c_str(), user.name.c_str());

	if(userFetchCallBack)
	{
		userFetchCallBack(userFetchSender, true, userFetchReward);
	}

	userFetchCallBack = NULL;
	userFetchSender = NULL;
	userFetchReward = false;
}

void FacebookController::onFriendsInvited(int count, const char* output)
{
	if(inviteFriendsCallback)
	{
		inviteFriendsCallback(inviteFriendsSender, count, output);
	}

	//isOnTop = false;

	inviteFriendsSender = NULL;
	inviteFriendsCallback = NULL;
}

void FacebookController::onAppFriendsRetrieved(int count)
{
	total_friends = count;

	if(appFriendCallback)
	{
		appFriendCallback(appFriendSender, true);
	}

	appFriendSender = NULL;
	appFriendCallback = NULL;
}

void FacebookController::registerAppFriends(const char* fbid, const char* fname, int count)
{
	installed_friends.clear();

	std::vector<std::string> idList = InAppPurchase::customSplit(fbid, '|');
	std::vector<std::string> nameList = InAppPurchase::customSplit(fname, '|');

	for(int i = 0; i < count; i++)
	{
		std::string strId = idList[i];
		std::string strName = nameList[i];

		FacebookUser user = FacebookUser();
		user.fbid = strId;
		user.name = strName;
		installed_friends.push_back(user);

		const char* sName = strName.c_str();
		const char* sId   = strId.c_str();
		__android_log_print(ANDROID_LOG_INFO, "CUSTOM_TAG", "App friend name - id : %s - %s", sName, sId);
	}

	__android_log_print(ANDROID_LOG_INFO, "CUSTOM_TAG", "App friends : %d", count);
	onAppFriendsRetrieved(count);
}

void FacebookController::onFacebookCancelled()
{
	isOnTop = false;
	m_bIsLoggedIn = false;
}

void FacebookController::onApplicationEnterForeground()
{
	isOnTop = false;
}

void FacebookController::checkSceneManagerFromSession(bool success)
{
	m_bIsLoggedIn = success;

	if(sessionStateChangedSender && sessionStateChangedCallBack)
	{
		__android_log_print(ANDROID_LOG_INFO, "CUSTOM_TAG", "Session State Change Callback");

		sessionStateChangedCallBack(sessionStateChangedSender, m_bIsLoggedIn);

		sessionStateChangedSender = NULL;
		sessionStateChangedCallBack = NULL;
	}
	else
	{
		if(m_bIsLoggedIn && fb_sceneManagerCallback != NULL)
		{
			__android_log_print(ANDROID_LOG_INFO, "CUSTOM_TAG", "Facebook is logged in and will call scene manager callback");
			fb_sceneManagerCallback(fb_sceneManager, true);
		}
	}
}
//#endregion //End of C++ Callbacks

//#region JNI Callbacks from Java
JNIEXPORT void JNICALL Java_ppl_cocos2dx_ranchrun_facebook_FacebookHelper_loginResultCallback (JNIEnv *env, jobject obj, jboolean success)
{
	if(FacebookController::sharedInstance() != NULL)
	{
		FacebookController::sharedInstance()->loginCallback((bool)success);
	}
}

JNIEXPORT void JNICALL Java_ppl_cocos2dx_ranchrun_facebook_FacebookHelper_userDetailsCallback (JNIEnv *env, jobject obj, jstring fbid, jstring fname, jstring email, jstring gender_full, jstring gender, jstring ageMin)
{
	if(FacebookController::sharedInstance() != NULL)
	{
		const char* _fbid = env->GetStringUTFChars(fbid, NULL);
		const char* _fname = env->GetStringUTFChars(fname, NULL);
		const char* _email = env->GetStringUTFChars(email, NULL);
		const char* _gender_full = env->GetStringUTFChars(gender_full, NULL);
		const char* _gender = env->GetStringUTFChars(gender, NULL);
		const char* _ageMin = env->GetStringUTFChars(ageMin, NULL);

		FacebookController::sharedInstance()->userDetailsCallback(_fbid, _fname, _email, _gender_full, _gender, _ageMin);

		env->ReleaseStringUTFChars(fbid, _fbid);
		env->ReleaseStringUTFChars(fname, _fname);
		env->ReleaseStringUTFChars(email, _email);
		env->ReleaseStringUTFChars(gender_full, _gender_full);
		env->ReleaseStringUTFChars(gender, _gender);
		env->ReleaseStringUTFChars(ageMin, _ageMin);
	}
}

JNIEXPORT void JNICALL Java_ppl_cocos2dx_ranchrun_facebook_FacebookHelper_inviteFriendsCallback(JNIEnv *env, jobject obj, int count, jstring output)
{
	if(FacebookController::sharedInstance() != NULL)
	{
		const char* _output = env->GetStringUTFChars(output, NULL);

		FacebookController::sharedInstance()->onFriendsInvited(count, _output);

		env->ReleaseStringUTFChars(output, _output);
	}
}

JNIEXPORT void JNICALL Java_ppl_cocos2dx_ranchrun_facebook_FacebookHelper_friendsWithAppCallback(JNIEnv *env, jobject obj, int count, jstring userIds, jstring userNames)
{
	if(FacebookController::sharedInstance() != NULL)
	{
		const char* _ids = env->GetStringUTFChars(userIds, NULL);
		const char* _names = env->GetStringUTFChars(userNames, NULL);

		FacebookController::sharedInstance()->registerAppFriends(_ids, _names, count);

		env->ReleaseStringUTFChars(userIds, _ids);
		env->ReleaseStringUTFChars(userNames, _names);
	}
}

JNIEXPORT void JNICALL Java_ppl_cocos2dx_ranchrun_facebook_FacebookHelper_onActionCancelled(JNIEnv *env, jobject obj)
{
	if(FacebookController::sharedInstance() != NULL)
	{
		FacebookController::sharedInstance()->onFacebookCancelled();
	}
}

JNIEXPORT void JNICALL Java_ppl_cocos2dx_ranchrun_facebook_FacebookHelper_tryLoginCompleted(JNIEnv *env, jobject obj)
{
	if(FacebookController::sharedInstance() != NULL)
	{
		FacebookController::sharedInstance()->loginSuccessful();
	}
}

JNIEXPORT void JNICALL Java_ppl_cocos2dx_ranchrun_facebook_FacebookHelper_checkSceneManager(JNIEnv *env, jobject obj, jboolean success)
{
	if(FacebookController::sharedInstance() != NULL)
	{
		FacebookController::sharedInstance()->checkSceneManagerFromSession((bool)success);
	}
}
//#endregion //End of JNI Callbacks from Java

#endif //End if of (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
