#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include "CCAlert.h"

@interface CCAlertIOS : NSObject <UIAlertViewDelegate>
{
    CCObject* _target;
    SEL_Alert _selector;
}

-(void)showWithTitle:(const char *)title Text:(const char *)text FirstButtonTitle:(const char *)firstButtonTitle SecondButtonTitle:(const char *)secondButtonTitle Target:(CCObject*)target Selector:(SEL_Alert)selector;

@end