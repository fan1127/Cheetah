//
//  DeviceInfo.m
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/24/2557 BE.
//
//


#import "DeviceInfo.h"

#import <Foundation/Foundation.h>
#import <sys/utsname.h>

std::string DeviceInfo::getName() {
    
    struct utsname systemInfo;
    uname(&systemInfo);
    
    return [[NSString stringWithCString:systemInfo.machine
                               encoding:NSUTF8StringEncoding] UTF8String];
    
}

std::string DeviceInfo::getModel() {
    
    return [[[UIDevice currentDevice] model] UTF8String];
    
}

std::string DeviceInfo::getSystemName() {
    
    return [[[UIDevice currentDevice] systemName] UTF8String];
    
}

std::string DeviceInfo::getSystemVersion() {
    
    return [[[UIDevice currentDevice] systemVersion] UTF8String];
    
}

std::string DeviceInfo::getLanguage() {
    
    return [[[[NSBundle mainBundle] preferredLocalizations] objectAtIndex:0] UTF8String];
    
}