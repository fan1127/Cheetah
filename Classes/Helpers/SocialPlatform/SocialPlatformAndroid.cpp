//
//  SocialPlatform.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/6/2557 BE.
//
//

#include "SocialPlatform.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"
using namespace cocos2d;

bool willLoginAgain = true;

SocialPlatform *s_soialplatform = NULL;
SocialPlatform *SocialPlatform::getInstance() {
    
    if (!s_soialplatform) {
        s_soialplatform = new SocialPlatform();
    }
    
    return s_soialplatform;
    
}

SocialPlatform::SocialPlatform() {
    
    m_currentPlayerID = "";
    m_currentPlayerAlias = "";
    m_authenticationComplete = false;
}

SocialPlatform::~SocialPlatform() {
    
}

#pragma mark - Authentication

bool isGameCenterAPIAvailable() {
    
    bool isAvailable = false;

    JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo,
			"com/avalable/cheetah/GPGSHelper",
			"isGoogleServicesAvailable",
			"()Z")) {

		isAvailable = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}

    return isAvailable;
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
    
    if (!isGameCenterAPIAvailable() || (willLoginAgain == false)) {
        doAuthenticationCallback();
        return;
    }

    JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo,
			"com/avalable/cheetah/GPGSHelper",
			"startSignIn",
			"()V")) {

		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}

void SocialPlatform::signInFromSettings() {

	if (!isGameCenterAPIAvailable()) {
		doAuthenticationCallback();
		return;
	}

	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo,
			"com/avalable/cheetah/GPGSHelper",
			"startSignIn",
			"()V")) {

		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}

void SocialPlatform::signOut() {

	if (!isGameCenterAPIAvailable()) {
		return;
	}

	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo,
			"com/avalable/cheetah/GPGSHelper",
			"signOut",
			"()V")) {

		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}

	m_authenticationComplete = false;
	m_currentPlayerID = "";
	m_currentPlayerID = "";

	willLoginAgain = false;
}

bool SocialPlatform::isLoggedIn() {

	return m_authenticationComplete;
}

std::string SocialPlatform::getPlayerID() {
    
    return m_currentPlayerID;
    
}

std::string SocialPlatform::getDisplayAlias() {

	return m_currentPlayerAlias;
}

#pragma mark - Achievement

bool SocialPlatform::openAchievement() {

	JniMethodInfo methodInfo;
    
    if (!isLoggedIn())
        return false;

	if (JniHelper::getStaticMethodInfo(methodInfo,
			"com/avalable/cheetah/GPGSHelper",
			"showAchievementScreen",
			"()V")) {

		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);

		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
    
    return true;
}

void SocialPlatform::postAchievement(std::string name, float percent) {

	if(percent <= 0)
		return;

	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo,
			"com/avalable/cheetah/GPGSHelper",
			"postAchievement",
			"(Ljava/lang/String;F)V")) {

		jstring stringArg = methodInfo.env->NewStringUTF(name.c_str());
		jfloat floatPercent = (jfloat)percent;

		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringArg, floatPercent);

		methodInfo.env->DeleteLocalRef(stringArg);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}


#pragma mark - JNI callbacks

void SocialPlatform::onSignInSucceeded(std::string playerID, std::string playerAlias) {

	CCLog("#######################");
	CCLog("onSignInSucceeded");
	CCLog("%s", playerID.c_str());
	CCLog("#######################");

	m_authenticationComplete = true;
	if (m_currentPlayerID == "" || m_currentPlayerID != playerID) {
		m_currentPlayerID = playerID;
	}

	if (m_currentPlayerAlias == "" || m_currentPlayerAlias != playerAlias) {
		m_currentPlayerAlias = playerAlias;
	}

	doAuthenticationCallback();
}

void SocialPlatform::onSignInFailed() {

	m_authenticationComplete = false;
	doAuthenticationCallback();
}


JNIEXPORT void JNICALL
Java_com_avalable_cheetah_GPGSHelper_onSignInSucceeded(JNIEnv * env, jobject  obj, jstring playerID, jstring playerAlias)
{
	const char *tmpPlayerID = env->GetStringUTFChars(playerID, 0);
	const char *tmpPlayerAlias = env->GetStringUTFChars(playerAlias, 0);

	std::string nativePlayerID = (tmpPlayerID);
	std::string nativePlayerAlias = (tmpPlayerAlias);

	SocialPlatform::getInstance()->onSignInSucceeded(nativePlayerID, nativePlayerAlias);

	env->ReleaseStringUTFChars(playerID, tmpPlayerID);
	env->ReleaseStringUTFChars(playerAlias, tmpPlayerAlias);
}

JNIEXPORT void JNICALL
Java_com_avalable_cheetah_GPGSHelper_onSignInFailed(JNIEnv * env, jobject  obj)
{
	CCLog("#######################");
	CCLOG("QQQ Sign In Failed!");
	CCLog("#######################");

	SocialPlatform::getInstance()->onSignInFailed();
	willLoginAgain = false;
}

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
