#ifndef _CCALERT_H_
#define _CCALERT_H_

#include "cocos2d.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)


using namespace cocos2d;

typedef void (CCObject::*SEL_Alert)(bool);
#define alert_selector(_SELECTOR) (SEL_Alert)(&_SELECTOR)

class CCAlert
{
public:
    static void Show(const char* title, const char* text, const char* firstButtonTitle, const char* secondButtonTitle, CCObject* target, SEL_Alert selector);
};


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

extern "C" {

JNIEXPORT void JNICALL
Java_com_avalable_cheetah_CheetahHelper_onDialogClick(JNIEnv * env, jobject  obj, jint buttonIdx);

}
#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#endif //_CCALERT_H_
