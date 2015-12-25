#include "CCAlert.h"
#include "CCAlertIOS.h"

void CCAlert::Show(const char* title, const char* text, const char* firstButtonTitle, const char* secondButtonTitle, CCObject* target, SEL_Alert selector)
{
    CCAlertIOS *alert = [[CCAlertIOS alloc] init];
    [alert showWithTitle:title Text:text FirstButtonTitle:firstButtonTitle SecondButtonTitle:secondButtonTitle Target:target Selector:selector];
//    [alert autorelease];
}