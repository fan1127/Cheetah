//
// Created by Vaclav Samec on 10/13/14 AD.
//

#ifndef _BUNDLE_VERSION_H_
#define _BUNDLE_VERSION_H_

#include <string>

#if !(IS_RELEASE_BUILD)
#include "../version.h"
#endif

class Versioning
{
public:
    
    //
    // returns the Bundle versions string, short in the info.plist
    //
    static std::string GetBundleVersionShort();

    //
    // returns the Bundle version in the info.plist
    //
    static std::string GetBundleVersion();
    
#if !(IS_RELEASE_BUILD)
    //
    // returns current GIT commit hash
    //
    static inline std::string GetGitVersion()
    {
        return GIT_VERSION;
    }

    //
    // returns version number together with GIT commit hash
    //
    static std::string GetBundleVersionWithGit();
#endif

//#define BUNDLE_VERSION_EQUAL_TO(v)                  ([[[NSBundle mainBundle] objectForInfoDictionaryKey:(NSString*) kCFBundleVersionKey] compare:v  options:NSNumericSearch] == NSOrderedSame)
//#define BUNDLE_VERSION_GREATER_THAN(v)              ([[[NSBundle mainBundle] objectForInfoDictionaryKey:(NSString*) kCFBundleVersionKey] compare:v options:NSNumericSearch] == NSOrderedDescending)
//#define BUNDLE_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[NSBundle mainBundle] objectForInfoDictionaryKey:(NSString*) kCFBundleVersionKey] compare:v options:NSNumericSearch] != NSOrderedAscending)
//#define BUNDLE_VERSION_LESS_THAN(v)                 ([[[NSBundle mainBundle] objectForInfoDictionaryKey:(NSString*) kCFBundleVersionKey]  compare:v options:NSNumericSearch] == NSOrderedAscending)
//#define BUNDLE_VERSION_LESS_THAN_OR_EQUAL_TO(v)     ([[[NSBundle mainBundle] objectForInfoDictionaryKey:(NSString*) kCFBundleVersionKey] compare:v options:NSNumericSearch] != NSOrderedDescending)
};

#endif
