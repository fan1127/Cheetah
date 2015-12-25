//
// Created by Vaclav Samec on 10/13/14 AD.
//

#include <sstream>
#include "Versioning.h"
#include "BackendService.h"
#include "StringHelper.h"

std::string Versioning::GetBundleVersionShort()
{
    NSString * version = [[NSBundle mainBundle] objectForInfoDictionaryKey: @"CFBundleShortVersionString"];
    
    return std::string([version UTF8String]);
}

std::string Versioning::GetBundleVersion()
{
    NSString * build = [[NSBundle mainBundle] objectForInfoDictionaryKey:(NSString*)kCFBundleVersionKey];

    return std::string([build UTF8String]);
}

#if !(IS_RELEASE_BUILD)
std::string Versioning::GetBundleVersionWithGit()
{
    std::string backendURL = DEFAULT_BACKEND_URL;
    StringHelper::replaceString(backendURL, "http://", "");
    StringHelper::replaceString(backendURL, "/api/", "");
    StringHelper::replaceString(backendURL, "api.", "");
    StringHelper::replaceString(backendURL, ".com", "");
    
    std::ostringstream ss;
    ss << GetGitVersion() << " _ " << backendURL << " _ " << GetBundleVersion();
    return ss.str();
}
#endif
