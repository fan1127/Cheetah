/*
 * VersioningAndroid.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: kristian.mitra
 */

#include <sstream>
#include "Versioning.h"
#include "cocos2d.h"
#include <string>
 #include <algorithm>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"

using namespace cocos2d;

//! read version name from the manifest file
std::string Versioning::GetBundleVersionShort()
{
    JniMethodInfo methodInfo;
    std::string ret;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/utils/Versioning",   // fully qualified class name
            "getVersionName",                           // method name
            "()Ljava/lang/String;")) {                  // data type of argument

        jstring str = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);

        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        ret = JniHelper::jstring2string(str);
        methodInfo.env->DeleteLocalRef(str);
    }

    if (!ret.empty())
    {
        if (std::count(ret.begin(), ret.end(), '.') == 3)
        {
            ret.resize(ret.find_last_of('.'));
        }
    }

	return ret;
}

std::string Versioning::GetBundleVersion()
{
    return GetBundleVersionShort();
}

#if !(IS_RELEASE_BUILD)
std::string Versioning::GetBundleVersionWithGit()
{
    std::ostringstream ss;
    ss << GetGitVersion() << "_" << GetBundleVersion();
    return ss.str();
}
#endif //!(IS_RELEASE_BUILD)


#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
