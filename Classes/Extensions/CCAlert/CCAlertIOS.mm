#import "CCAlertIOS.h"

@implementation CCAlertIOS

-(void)showWithTitle:(const char *)title Text:(const char *)text FirstButtonTitle:(const char *)firstButtonTitle SecondButtonTitle:(const char *)secondButtonTitle Target:(CCObject*)target Selector:(SEL_Alert)selector
{
    _target = target;
    _selector = selector;
    if (_target != NULL)
        _target->retain();
    
    UIAlertView *alert = [[UIAlertView alloc] init];
    [alert setTitle:[NSString stringWithUTF8String:title]];
    [alert setMessage:[NSString stringWithUTF8String:text]];
    [alert setDelegate:self];
    if (firstButtonTitle) [alert addButtonWithTitle:[NSString stringWithUTF8String:firstButtonTitle]];
    if (secondButtonTitle) [alert addButtonWithTitle:[NSString stringWithUTF8String:secondButtonTitle]];
    [alert show];
    [alert release];
}

-(void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (_target != NULL)
    {
        if (buttonIndex == 0)
        {
            (_target->*_selector)(true);
        }
        else
        {
            (_target->*_selector)(false);
        }
        _target->release();
    }
    
    [self release];
}

@end