//
//  AnalyticsClientWrapper.cpp
//  Cheetah
//
//  Created by Venca
//
//

#include "../AppMacros.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "cocos-ext.h"
#include "AnalyticsClientWrapper.h"
#include "../Settings.h"
#include "../Entities/PlayerState.h"
//#include "support/zip_support/Zipper.h"
#include "../Helpers/StringHelper.h"
#include "platform/android/jni/JniHelper.h"

using namespace cocos2d;

#define ANALYTICS_URL "http://analytics.pocketplaylab.com/events"
#define ANALYTICS_URL_DEBUG "http://analytics.pocketplaylab.com/events?debug=true"
#define ANALYTICS_HTTP_RETRIES (1)

bool AnalyticsClient::usingWifi()
{
    JniMethodInfo methodInfo;
    bool ret = false;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/utils/AndroidHelper",  // fully qualified class name
            "usingWIFI",                                  // method name
            "()Z")) {                                     // data type of argument

        ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);

        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }

    CCLog_("AnalyticsClient::usingWifi: %d", ret);

    return ret;
}

std::string AnalyticsClient::generateUUID()
{
    JniMethodInfo methodInfo;
    std::string ret;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/utils/AndroidHelper",   // fully qualified class name
            "generateUUID",                                // method name
            "()Ljava/lang/String;")) {                     // data type of argument

        jstring str = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);

        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        ret = JniHelper::jstring2string(str);
        methodInfo.env->DeleteLocalRef(str);
    }

    CCLog_("AnalyticsClient::generateUUID: %s", ret.c_str());

    return ret;
}

void AnalyticsClient::sendData()
{
    std::string compressedJson = getCompressedData();

    if (compressedJson.empty())
    {
        return;
    }

    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/utils/AndroidHelper",    // fully qualified class name
            "postAnalyticsData",                            // method name
            "(Ljava/lang/String;[B)V")) {   // data type of argument

        //jstring - C++ representation of Java String
        jstring arg0 = methodInfo.env->NewStringUTF(ANALYTICS_URL_DEBUG);
        //jbyteArray arg1 = JniHelper::makeByteArray(compressedJson);

        // call the method, with arguments
        //methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, arg0, arg1);

        //methodInfo.env->DeleteLocalRef(arg0);
        //methodInfo.env->DeleteLocalRef(arg1);
        //methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void AnalyticsClient::appDidEnterBackground()
{
    sendData();
}

#endif
