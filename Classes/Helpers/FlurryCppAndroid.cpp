/*
 * FlurryCppAndroid.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: patrick.pamplona
 */

#include "FlurryCpp.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)


const char* FlurryEvent::RACE_START               = "001_RaceStart_v2";
const char* FlurryEvent::RACE_END                 = "002_RaceEnd_v2";
const char* FlurryEvent::RACE_END_PLAYER_WON      = "003_RaceEnd_PlayerWon_v2";
const char* FlurryEvent::RACE_END_PLAYER_GAVEUP   = "004_RaceEnd_PlayerGaveUp_v2";
const char* FlurryEvent::RACE_END_AI_WON          = "005_RaceEnd_AIWon_v2";
const char* FlurryEvent::PLAYER_LEVEL_UP          = "101_PlayerLevelUp_v2";
const char* FlurryEvent::PET_LEVEL_UP             = "102_PetLevelUp_v2";

void FlurryCpp::setUserID(const char* userID)
{
	//TODO: Add FlurryCpp::set User ID functionality for android
}

void FlurryCpp::setUserAge(int age)
{
	//TODO: Add Set User Age functionality for Android
}

void FlurryCpp::setUserGender(const char* gender)
{
	//TODO: Add Set User Gender functionality for Android
}

void FlurryCpp::setAppVersion(const char* version)
{

}

const char* FlurryCpp::getFlurryAgentVersion()
{
	//TODO: Add Get Flurry Agent Version functionality for Android
}

void FlurryCpp::setShowErrorInLogEnabled(bool value)
{
	//TODO: Add Set Show Error In Log Enabled functionality for Android
}

void FlurryCpp::setDebugLogEnabled(bool value)
{
	//TODO: Add Set Debug Log Enabled functionality for Android
}

void FlurryCpp::setLogLevel(FlurryLogLevelCpp value)
{
	//TODO: Add Set Log Level functionality for Android
}

void FlurryCpp::setSessionContinueSeconds(int seconds)
{
	//TODO: Add Set Session Continue Seconds functionality for Android
}

//void FlurryCpp::setSecureTransportEnabled(bool value)
//{
//	//TODO: Add Set Secure Transport Enabled functionality for Android
//}

void FlurryCpp::setCrashReportingEnabled(bool value)
{
	//TODO: Add Set Crash Reporting Enabled functionality for Android
}

void FlurryCpp::startSession(const char* apiKey)
{
	//TODO: Add Start Session functionality for Android
}

void FlurryCpp::pauseBackgroundSession()
{
	//TODO: Add Pause Background Session functionality for Android
}

void FlurryCpp::addOrigin(const char* originName, const char* originVersion)
{
	//TODO: Add Add Origin functionality for Android
}

void FlurryCpp::logEvent(const char* eventName)
{
	//TODO: Add Log Event functionality for Android
}

void FlurryCpp::logEvent(const char* eventName, cocos2d::CCDictionary* parameters)
{
	//TODO: Add Log Event overload functionality for Android
}

void FlurryCpp::logEvent(const char* eventName, bool timed)
{
	//TODO: Add Log Event overload functionality for Android
}

void FlurryCpp::logEvent(const char* eventName, cocos2d::CCDictionary* parameters, bool timed)
{
	//TODO: Add Log Event overload functionality for Android
}

void FlurryCpp::endTimedEvent(const char* eventName, cocos2d::CCDictionary* parameters)
{
	//TODO: Add End Timed Event functionality for Android
}

void FlurryCpp::setEventLoggingEnabled(bool value)
{
	//TODO: Add Set Event Logging Enabled functionality for Android
}

#endif //End of if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
