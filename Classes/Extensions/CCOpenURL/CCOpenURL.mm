//
//  CCOpenURL.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 4/12/2557 BE.
//
//

#include "CCOpenURL.h"

#import <UIKit/UiKit.h>

bool CCOpenURL::canOpenURL(std::string stringURL)
{
    NSString *nsStringURL = [NSString stringWithCString:stringURL.c_str()
                                               encoding:[NSString defaultCStringEncoding]];
    return [[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:nsStringURL]];
}

bool CCOpenURL::openURL(std::string stringURL)
{
    NSString *nsStringURL = [NSString stringWithCString:stringURL.c_str()
                                                encoding:[NSString defaultCStringEncoding]];
    return [[UIApplication sharedApplication] openURL:[NSURL URLWithString:nsStringURL]];
}