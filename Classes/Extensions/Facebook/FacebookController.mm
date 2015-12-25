//
//  FacebookController.mm
//  Cheetah
//
//  Created by Plus Pingya Avalable on 5/8/2557 BE.
//
//

#import "Settings.h"

#include "../../Extensions/CCAlert/CCAlert.h"

#include "Flurry.h"

#import "FacebookController.h"
using namespace std;

#import <FacebookSDK/FacebookSDK.h>

#import "AppController.h"

#include "../../Helpers/AnalyticsClientWrapper.h"
#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

static NSDictionary* parseURLParams(NSString *query) {
    NSArray *pairs = [query componentsSeparatedByString:@"&"];
    NSMutableDictionary *params = [[NSMutableDictionary alloc] init];
    for (NSString *pair in pairs) {
        NSArray *kv = [pair componentsSeparatedByString:@"="];
        NSString *val =
        [kv[1] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
        params[kv[0]] = val;
    }
    return params;
}

// #10: Shared error handler
void printError(NSString* message, NSError*error) {
    
    if(message) {
        NSLog_(@"%@", message);
    }
    
    // works for 1 FBRequest per FBRequestConnection
    int userInfoCode = [error.userInfo[@"com.facebook.FBiOSSDK:ParsedJSONResponseKey"][0][@"body"][@"error"][@"code"] integerValue];
    NSString* userInfoMessage = error.userInfo[@"com.facebook.FBiOSSDK:ParsedJSONResponseKey"][0][@"body"][@"error"][@"message"];
    
    // outer error
    NSLog_(@"Error: %@", error);
    NSLog_(@"Error code: %d", error.code);
    NSLog_(@"Error message: %@", error.localizedDescription);
    
    // inner error
    NSLog_(@"Error code: %d", userInfoCode);
    NSLog_(@"Error message: %@", userInfoMessage);
    
    if(userInfoCode == 2500) {
        UIAlertView* view = [[UIAlertView alloc]initWithTitle:@"Facebook"
                                                      message:@"You're not logged in."
                                                     delegate:nil
                                            cancelButtonTitle:@"OK"
                                            otherButtonTitles:nil];
        dispatch_async(dispatch_get_main_queue(), ^{
            [view show];
        });
    }
}

FacebookUser::FacebookUser()  {

    fbid = "unknown";
    name = "unknown";
    email = "unknown";
    gender = "";
    gender_full = "unknown";
    age_range_min = "unknown";

}

FacebookController::FacebookController() {
    
    sessionStateChangedSender = NULL;
    sessionStateChangedCallBack = NULL;
    
    session = NULL;
    user = FacebookUser();
    installed_friends.clear();
    
    AppController *app = (AppController*)[[UIApplication sharedApplication] delegate];
    m_bIsLoggedIn = app.fb_logged_in;
}


void FacebookController::createNewSession() {
    
    if (session) {
        FBSession* _session = (FBSession*)session;
        [_session release];
        _session = NULL;
    }
    
    FBSession* _session = [[FBSession alloc] init];
    [FBSession setActiveSession:_session];
    session = _session;
    
    if (FBSession.activeSession.isOpen) {
        m_bIsLoggedIn = true;
        NSLog_(@"Some one logged in!");
    }else {
        m_bIsLoggedIn = false;
        NSLog_(@"No one logged in!");
    }
    
}

void FacebookController::publishOpenGraphStoryStep1(const char* actionType,
                                                    const char* objectType,
                                                    const char* title,
                                                    const char* description,
                                                    const char* url,
                                                    const char* imageUrl,
                                                    bool explicitlyShared) {
    
    //FASTER VERSION from https://github.com/facebook/facebook-ios-sdk/blob/master/samples/HelloFacebookSample/HelloFacebookSample/HFViewController.m
    if ([FBSession.activeSession.permissions indexOfObject:@"publish_actions"] == NSNotFound) {
        
        NSLog_(@"Permission hasn't been granted, asking for publish_actions");
        
        // Permission hasn't been granted, so ask for publish_actions
        [FBSession.activeSession requestNewPublishPermissions:[NSArray arrayWithObject:@"publish_actions"]
                                              defaultAudience:FBSessionDefaultAudienceFriends
                                            completionHandler:^(FBSession *session, NSError *error) {
                                                if (!error) {
                                                    if ([FBSession.activeSession.permissions indexOfObject:@"publish_actions"] == NSNotFound){
                                                        // Permission not granted, tell the user we will not share to Facebook
                                                        NSLog_(@"Permission not granted, we will not share to Facebook.");
                                                        
                                                    } else {
                                                        // Permission granted, publish the OG story
                                                        this->publishOpenGraphStoryStep2(actionType,objectType,
                                                                                         title,
                                                                                         description,
                                                                                         url,
                                                                                         imageUrl,
                                                                                         explicitlyShared);
                                                    }
                                                    
                                                } else {
                                                    // An error occurred, we need to handle the error
                                                    // See: https://developers.facebook.com/docs/ios/errors
                                                    NSLog_(@"Encountered an error requesting permissions: %@", error.description);
                                                }
                                            }];
        
    } else {
        // Permissions present, publish the OG story
        this->publishOpenGraphStoryStep2(actionType,objectType,
                                         title,
                                         description,
                                         url,
                                         imageUrl,
                                         explicitlyShared);
    }

}


void FacebookController::publishOpenGraphStoryStep2(const char* actionType,
                                                    const char* objectType,
                                                    const char* title,
                                                    const char* description,
                                                    const char* url,
                                                    const char* imageUrl,
                                                    bool explicitlyShared) {
    
    // instantiate a Facebook Open Graph object
    NSMutableDictionary<FBOpenGraphObject> *object = [FBGraphObject openGraphObjectForPost];
    
    // specify that this Open Graph object will be posted to Facebook
    object.provisionedForPost = YES;
    
    object[@"title"] = [NSString stringWithUTF8String:title];
    object[@"type"] = [NSString stringWithFormat:@"%@:%@", FBAppNameSpace, [NSString stringWithUTF8String:objectType]];
    object[@"description"] = [NSString stringWithUTF8String:description];
    object[@"url"] = [NSString stringWithUTF8String:url];
    object[@"image"] = @[@{                @"url": [NSString stringWithUTF8String:imageUrl],
                               @"user_generated" : @"false"
                                           }];
    if (explicitlyShared) {
        object[@"explicitly_shared"] = @"true";
    }
    
    
    //The URL of an image which is used in stories published about this object. We suggest that you give us an image of at least 600x315 pixels. However, bigger is better, so if you have a 1200x630 or larger image that you can use, please give it to us. Also, we recommend that you keep images as close to a 1.91:1 aspect ratio as possible to avoid cropping. (Note: image sizes must be no more than 5MB in size.) For games, images should be square and at least 600x600 pixels. Note that you can include more than that one og:image in your object if you have more than one resolution available.
    
    
    // Post custom object
    [FBRequestConnection startForPostOpenGraphObject:object completionHandler:^(FBRequestConnection *connection, id result, NSError *error) {
        if(!error) {
            // get the object ID for the Open Graph object that is now stored in the Object API
            NSString *objectId = [result objectForKey:@"id"];
            NSLog_(@"object id: %@", objectId);
            
            // create an Open Graph action
            id<FBOpenGraphAction> action = (id<FBOpenGraphAction>)[FBGraphObject graphObject];
            [action setObject:objectId
                       forKey:[NSString stringWithUTF8String:objectType]];
            
            // create action referencing user owned object
            [FBRequestConnection startForPostWithGraphPath:[NSString stringWithFormat:@"/me/%@:%@", FBAppNameSpace, [NSString stringWithUTF8String:actionType]]
                                               graphObject:action
                                         completionHandler:^(FBRequestConnection *connection, id result, NSError *error) {
                if(!error) {
                    NSLog_(@"OG story posted, story id: %@", [result objectForKey:@"id"]);
                    
                    CCAlert::Show("OG story posted", "Check your Facebook profile or activity log to see the story.", "OK", NULL, NULL, NULL);
                    
                } else {
                    // An error occurred, we need to handle the error
                    // See: https://developers.facebook.com/docs/ios/errors
                    NSLog_(@"Encountered an error 2 posting to Open Graph: %@", error.description);
                }
            }];
            
        } else {
            // An error occurred, we need to handle the error
            // See: https://developers.facebook.com/docs/ios/errors
            NSLog_(@"Encountered an error 1 posting to Open Graph: %@", error.description);
        }
    }];
    
    
}

void FacebookController::tryLoginIfThereIsTokenLoaded()
{
    if ([FBSession activeSession].state == FBSessionStateCreatedTokenLoaded) {
        [FBSession openActiveSessionWithReadPermissions:[[NSArray alloc] initWithObjects:
                                                         @"public_profile", @"user_friends", @"email",
                                                         nil]
                                           allowLoginUI:NO
                                      completionHandler:^(FBSession *session, FBSessionState state, NSError *error) {
                                          
                                          AppController *app = (AppController*)[[UIApplication sharedApplication] delegate];
                                          [app sessionStateChanged:session state:state error:error];
                                      }];
    }
    
}

void FacebookController::login() {
    
    login(NULL, NULL);
    
}

void FacebookController::login(void* sender, void (*callback)(void*, bool)) {
    
    FBSession* _session = [[FBSession alloc] init];
    [FBSession setActiveSession:_session];
    session = _session;
    
    sessionStateChangedSender = sender;
    sessionStateChangedCallBack = callback;
    
    [FBSession openActiveSessionWithReadPermissions:[[NSArray alloc] initWithObjects:
                                                     @"public_profile", @"user_friends", @"email",
                                                     nil]
                                       allowLoginUI:YES
                                  completionHandler:NULL];
}

void FacebookController::logout(void* sender, void (*callback)(void*, bool)) {
    
    g_playerState->HelperPets.clear();
    
    // Log out of Facebook and reset our session
    [[FBSession activeSession] closeAndClearTokenInformation];
    [FBSession setActiveSession:nil];
    session = NULL;
    
    m_bIsLoggedIn = false;
    if (callback) {
        callback(sender, true);
    }
    
}

void FacebookController::fetchUserDetails(bool rewardGems, void* sender, void (*callback)(void*, bool, bool)) {
    
    
    [FBRequestConnection startWithGraphPath:@"/me?fields=id,name,gender,age_range,email"
                          completionHandler:^(FBRequestConnection *connection, id result, NSError *error)
    {
        
        
        //SAMPLE RESPONSE FOR @"/me"
//        {
//            email = "toptanapon@gmail.com";
//            "first_name" = Tanapon;
//            gender = male;
//            id = 10152038797270974;
//            "last_name" = Petapanpiboon;
//            link = "https://www.facebook.com/app_scoped_user_id/10152038797270974/";
//            locale = "en_US";
//            name = "Tanapon Petapanpiboon";
//            timezone = 7;
//            "updated_time" = "2013-10-04T04:23:30+0000";
//            verified = 1;
//        }
        
        //SAMPLE RESPONSE FOR @"/me?fields=id,name,gender,age_range,email"
//        {
//            "id": "722730973",
//            "name": "Tanapon Petapanpiboon",
//            "gender": "male",
//            "age_range": {
//                "min": 21
//            }, 
//            "email": "toptanapon@gmail.com"
//        }
        
         if (!error && result) {
             
             NSDictionary<FBGraphUser> *dict = (NSDictionary<FBGraphUser>*)result;

             NSLog_(@"%@", dict);
             
             
             // If so we can extract out the player's Facebook ID and first name
             user.fbid = [[dict objectForKey:@"id"] UTF8String];
             user.name = [[dict objectForKey:@"name"] UTF8String];
             
             if ([dict objectForKey:@"email"] != nil) {
                 user.email = [[dict objectForKey:@"email"] UTF8String];
             }
             
             NSString *gender = [dict objectForKey:@"gender"];
             
             NSString *genderMF = @"";
             if ([gender isEqualToString:@"male"]) {
                 genderMF = @"m";
             } else if ([gender isEqualToString:@"female"]) {
                 genderMF = @"f";
             }
             
             int age_range_min = [[[dict objectForKey:@"age_range"] objectForKey:@"min"] intValue];
             CREATE_CHAR_BUFFER(_buffer, 64);
             snprintf(_buffer, _buffer_size, "%d", age_range_min);
             user.age_range_min = _buffer;
             
             user.gender = [genderMF UTF8String];
             user.gender_full = [gender UTF8String];
             
             [Flurry setGender:genderMF];
             AnalyticsClient::getInstance()->setUserGender(user.gender);
             AnalyticsClient::getInstance()->setUserAgeRangeMinimum(user.age_range_min);
             AnalyticsClient::getInstance()->setUserEmail(user.email);
             AnalyticsClient::getInstance()->setFacebookID(user.fbid);
             
             if (callback) {
                 callback(sender, true, rewardGems);
             }
             
         } else {
             if (callback) {
                 callback(sender, false, rewardGems);
             }
         }
        
     }];
    
}



void FacebookController::inviteFriends(string title, string message, void* sender, void (*callback)(void*, int numberOfInvites, string fbids)) {
    
    [FBWebDialogs presentRequestsDialogModallyWithSession:nil
                                                  message:[NSString stringWithFormat:@"%s", title.c_str()]
                                                    title:[NSString stringWithFormat:@"%s", message.c_str()]
                                               parameters:nil
                                                  handler:^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
         if (error) {
             // Error launching the dialog or sending the request.
//             NSLog_(@"Error sending request.");
         } else {
             if (result == FBWebDialogResultDialogNotCompleted) {
                 // User clicked the "x" icon
//                 NSLog_(@"User canceled request.");
             } else {
//                 NSLog_(@"Handle the send request callback");
                 // Handle the send request callback
                 NSDictionary *urlParams = parseURLParams([resultURL query]);//[self parseURLParams:[resultURL query]];
                 if (![urlParams valueForKey:@"request"]) {
                     // User clicked the Cancel button
//                     NSLog_(@"User canceled request.");
                 } else {
                     // User clicked the Send button
//                     NSString *requestID = [urlParams valueForKey:@"request"];
//                     NSLog_(@"Request ID: %@", requestID);
                     
//                     NSDictionary *sent_to = [urlParams objectForKey:@"to"];
//                     NSLog_(@"invite %d people", [urlParams count]-1);
                     
                     string connector = "[";
                     string output = "";
                     
                     int count = 0;
                     
                     for( NSString *aKey in [urlParams allKeys] ) {
                         NSLog_(@"NSString *aKey in [urlParams allKeys]: %@", aKey);
                         if (![aKey isEqualToString:@"request"]) {
                             output.append(connector);
                             output.append([[urlParams objectForKey:aKey] UTF8String]);
                             connector = ",";
                             count++;
                         }
                     }
                     
                     connector = "]";
                     output.append(connector);
                     
                     if (callback) {
                         callback(sender, count, output);
                     }
                     return;
                 }
                 
             }
             if (callback) {
                 callback(sender, 0, "");
             }
         }
                                                      
     }];
    
}

void FacebookController::getInstalledFriends(void* sender, void (*callback)(void*, bool)) {
    
    FBRequest* friendsRequest = [FBRequest requestWithGraphPath:@"me/friends?fields=id,name,installed" parameters:nil HTTPMethod:@"GET"];
    [friendsRequest startWithCompletionHandler: ^(FBRequestConnection *connection,
                                                  NSDictionary* result,
                                                  NSError *error) {
        
        NSLog_(@"getInstalledFriends: %@", result);
        
        NSArray* friends = [result objectForKey:@"data"];
        int n = [friends count];
        NSLog_(@"Found: %i friends", n);
        installed_friends.clear();
        
        for (int i = 0; i < n; i++) {
            if (NSDictionary<FBGraphUser>* _friend = [friends objectAtIndex:i]) {
                NSLog_(@"%@ -- %@", _friend.name, [_friend objectForKey:@"id"]);
                FacebookUser user = FacebookUser();
                user.fbid = [[_friend objectForKey:@"id"] UTF8String];
                user.name = [_friend.name UTF8String];
                installed_friends.push_back(user);
            }
        }
        
        NSDictionary *summary = [result objectForKey:@"summary"];
        total_friends = [[summary objectForKey:@"total_count"] intValue];
        
        if (callback) {
            callback(sender, true);
        }
        
    }];
    
}
