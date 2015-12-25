//
//  RatingWrapper.cpp
//  Cheetah
//
//  Created by Vaclav Samec on 11/12/2557 BE.
//
//

#include "FBAppEventsWrapper.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"
#include "../Settings.h"
#include "../Helpers/StringHelper.h"

using namespace cocos2d;

void FBAppEventsWrapper::logEvent(const char* customEventName)
{
    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/ads/Facebook",           // fully qualified class name
            "logEvent",                                     // method name
            "(Ljava/lang/String;)V")) {                     // data type of argument

        //jstring - C++ representation of Java String
        jstring arg0 = methodInfo.env->NewStringUTF(customEventName);

        // call the method, with arguments
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, arg0);

        methodInfo.env->DeleteLocalRef(arg0);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void FBAppEventsWrapper::logEvent(const char* customEventName, double valueToSum)
{
    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/ads/Facebook",           // fully qualified class name
            "logEvent",                                     // method name
            "(Ljava/lang/String;D)V")) {                    // data type of argument

        //jstring - C++ representation of Java String
        jstring arg0 = methodInfo.env->NewStringUTF(customEventName);

        // call the method, with arguments
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, arg0, valueToSum);

        methodInfo.env->DeleteLocalRef(arg0);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void FBAppEventsWrapper::logEvent(const char* customEventName, cocos2d::CCDictionary* customParameters)
{
    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/ads/Facebook",
            "logEvent",
            "(Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/String;)V"))
    {
        const int count = customParameters->count();

        std::string *keys = new std::string[count];
        std::string *values = new std::string[count];

        CCDictElement* pElement = NULL;
        int i=0;
        CCDICT_FOREACH(customParameters, pElement)
        {
            std::string value = ((CCString*)pElement->getObject())->getCString();
            std::string key = pElement->getStrKey();

            keys[i] = key;
            values[i] = value;
            i++;
        }

        jstring arg0 = methodInfo.env->NewStringUTF(customEventName);
        //jobjectArray arg1 = JniHelper::makeStringArray(count, keys);
        //jobjectArray arg2 = JniHelper::makeStringArray(count, values);

        //methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, arg0, arg1, arg2);

        //methodInfo.env->DeleteLocalRef(arg0);
        //methodInfo.env->DeleteLocalRef(arg1);
        //methodInfo.env->DeleteLocalRef(arg2);
        //methodInfo.env->DeleteLocalRef(methodInfo.classID);

        delete [] keys;
        delete [] values;
    }
}

void FBAppEventsWrapper::logEvent(const char* customEventName, double valueToSum, cocos2d::CCDictionary* customParameters)
{
    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/ads/Facebook",
            "logEvent",
            "(Ljava/lang/String;D[Ljava/lang/String;[Ljava/lang/String;)V"))
    {
        const int count = customParameters->count();

        std::string *keys = new std::string[count];
        std::string *values = new std::string[count];

        CCDictElement* pElement = NULL;
        int i=0;
        CCDICT_FOREACH(customParameters, pElement)
        {
            std::string value = ((CCString*)pElement->getObject())->getCString();
            std::string key = pElement->getStrKey();

            keys[i] = key;
            values[i] = value;
            i++;
        }

        jstring arg0 = methodInfo.env->NewStringUTF(customEventName);
        //jobjectArray arg2 = JniHelper::makeStringArray(count, keys);
        //jobjectArray arg3 = JniHelper::makeStringArray(count, values);

        //methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, arg0, valueToSum, arg2, arg3);

        //methodInfo.env->DeleteLocalRef(arg0);
        //methodInfo.env->DeleteLocalRef(arg2);
        //methodInfo.env->DeleteLocalRef(arg3);
        //methodInfo.env->DeleteLocalRef(methodInfo.classID);

        delete [] keys;
        delete [] values;
    }
}

void FBAppEventsWrapper::logEvent(_FBAppEventName eventName, double valueToSum)
{
    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/ads/Facebook",           // fully qualified class name
            "logEvent",                                     // method name
            "(I;D;)V")) {                                   // data type of argument

        // call the method, with arguments
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (int)eventName, valueToSum);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void FBAppEventsWrapper::logEvent(_FBAppEventName eventName, cocos2d::CCDictionary* predefinedParameters)
{
//public static void logEvent(int eventIndex, int[] keys, String[] values)

    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/ads/Facebook",
            "logEvent",
            "(I[I[Ljava/lang/String;)V"))
    {
        const int count = predefinedParameters->count();

        int *keys = new int[count];
        std::string *values = new std::string[count];

        CCDictElement* pElement = NULL;
        int i=0;
        CCDICT_FOREACH(predefinedParameters, pElement)
        {
            std::string value = ((CCString*)pElement->getObject())->getCString();
            int key = StringHelper::toInt(pElement->getStrKey());

            keys[i] = key;
            values[i] = value;
            i++;
        }

        //jintArray arg1 = JniHelper::makeIntArray(count, keys);
        //jobjectArray arg2 = JniHelper::makeStringArray(count, values);

        //methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (int)eventName, arg1, arg2);

        //methodInfo.env->DeleteLocalRef(arg1);
        //methodInfo.env->DeleteLocalRef(arg2);
        //methodInfo.env->DeleteLocalRef(methodInfo.classID);

        delete [] keys;
        delete [] values;
    }
}

void FBAppEventsWrapper::logEvent(_FBAppEventName eventName, double valueToSum, cocos2d::CCDictionary* predefinedParameters)
{
//public static void logEvent(int eventIndex, double valueToSum, int[] keys, String[] values)

    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/ads/Facebook",
            "logEvent",
            "(ID[I[Ljava/lang/String;)V"))
    {
        const int count = predefinedParameters->count();

        int *keys = new int[count];
        std::string *values = new std::string[count];

        CCDictElement* pElement = NULL;
        int i=0;
        CCDICT_FOREACH(predefinedParameters, pElement)
        {
            std::string value = ((CCString*)pElement->getObject())->getCString();
            int key = StringHelper::toInt(pElement->getStrKey());

            keys[i] = key;
            CCLog_("logEvent: intarray: %d %s", keys[i], pElement->getStrKey());
            values[i] = value;
            i++;
        }

        //jintArray arg2 = JniHelper::makeIntArray(count, keys);
        //jobjectArray arg3 = JniHelper::makeStringArray(count, values);

        //methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (int)eventName, valueToSum, valueToSum, arg2, arg3);

        //methodInfo.env->DeleteLocalRef(arg2);
        //methodInfo.env->DeleteLocalRef(arg3);
        //methodInfo.env->DeleteLocalRef(methodInfo.classID);

        delete [] keys;
        delete [] values;
    }
}

void FBAppEventsWrapper::logPurchase(double amount, const char* currency)
{
    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/ads/Facebook",           // fully qualified class name
            "logPurchase",                                  // method name
            "(DLjava/lang/String;)V")) {                    // data type of argument

        //jstring - C++ representation of Java String
        jstring arg0 = methodInfo.env->NewStringUTF(currency);

        // call the method, with arguments
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, amount, arg0);

        methodInfo.env->DeleteLocalRef(arg0);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}


#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
