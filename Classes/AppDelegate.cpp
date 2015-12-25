#include "AppDelegate.h"
#include "AppMacros.h"

#include <vector>
#include <string>

#include "Settings.h"

#include "Extensions/InAppPurchase/InAppPurchase.h"
#include "Helpers/TimeHelper.h"
#include "Scenes/SceneVille.h"

USING_NS_CC;
using namespace std;

#include "Helpers/FlurryCpp.h"
#include "Helpers/GaiCpp.h"
#include "Helpers/Versioning.h"
#include "Helpers/AppsFlyerWrapper.h"

#define FLURRY_API_KEY "X2H94CN7BZQYX8GQCSSH"
#define GAI_TRACKER_ID "UA-56474375-1"

float scaleFactor = 1.0f;
float sizeMultiplier = 1.0f;
float retinaMultiplier = 1.0f;
float isIPadMultiplier = 1.0f;
float isIPhoneMultiplier = 1.0f;
float isRetinaMultiplier = 1.0f;
float isIOSMultiplier = 1.0f;
float isAndroidMultiplier = 1.0f;
TargetDevice targetDevice = TD_IPHONE_RETINA;
bool isPhone = false;

bool hasPurchasedDuringThisSession = false;
const char* purchasedWithProductIdentifier = "";

extern string uuid;

#include "SceneManager.h"
extern SceneManager *g_sceneManager;
extern CCScene* g_currentScene;

#include "Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "Services/GameAudioService.h"
#include "Helpers/AppRating/AppRating.h"
#include "Extensions/APKExpansion.h"
#include "Helpers/AdsManagerWrapper.h"
#include "Helpers/DeviceInfo.h"
#include "Helpers/AnalyticsClientWrapper.h"

extern GameAudioService* g_gameAudio;

std::string device_token = "";

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() {

}

bool AppDelegate::applicationDidFinishLaunching() {

    // initialize director
    CCDirector* pDirector = CCDirector::sharedDirector();
    CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();

    pDirector->setOpenGLView(pEGLView);
    
    // set search path for resource
    CCSize frameSize = pEGLView->getFrameSize();
	vector<string> searchPath;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (frameSize.height > largeResource.size.height) {
        searchPath.push_back(xlargeResource.directory);
        targetDevice = TD_IPAD_RETINA;
        sizeMultiplier = kXLargeMultiplier;
        retinaMultiplier = 1.0f;
        isIPadMultiplier = 1.0f;
        isIPhoneMultiplier = 0.0f;
        isRetinaMultiplier = 1.0f;
        isIOSMultiplier = 1.0f;
        isAndroidMultiplier = 0.0f;
        isPhone = false;
    }else if (frameSize.height > mediumResource.size.height) {
        searchPath.push_back(largeResource.directory);
        targetDevice = TD_IPAD;
        sizeMultiplier = kLargeMultiplier;
        retinaMultiplier = 0.5f;
        isIPadMultiplier = 1.0f;
        isIPhoneMultiplier = 0.0f;
        isRetinaMultiplier = 0.0f;
        isIOSMultiplier = 1.0f;
        isAndroidMultiplier = 0.0f;
        isPhone = false;
    }else if (frameSize.height > smallResource.size.height) {
        searchPath.push_back(mediumResource.directory);
        targetDevice = TD_IPHONE_RETINA;
        sizeMultiplier = kMediumMultiplier;
        retinaMultiplier = 0.5f;
        isIPadMultiplier = 0.0f;
        isIPhoneMultiplier = 1.0f;
        isRetinaMultiplier = 1.0f;
        isIOSMultiplier = 1.0f;
        isAndroidMultiplier = 0.0f;
        isPhone = true;
    } else {
        searchPath.push_back(smallResource.directory);
        targetDevice = TD_IPHONE;
        sizeMultiplier = kSmallMultiplier;
        retinaMultiplier = 0.5f;
        isIPadMultiplier = 0.0f;
        isIPhoneMultiplier = 1.0f;
        isRetinaMultiplier = 0.0f;
        isIOSMultiplier = 1.0f;
        isAndroidMultiplier = 0.0f;
        isPhone = true;
    }
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    //
    // Android platform is choosing resource based on DPI rather than screen resolutions,
    // since there are devices with medium resolution but smaller physical size which make the "real" resolution much bigger
    // and there are devices with high resolution but bigger physical size which make the "real" resolution much smaller
    //
    // http://stackoverflow.com/questions/6930031/how-do-i-convert-ppi-into-dpi-for-android-images
    // http://stackoverflow.com/questions/20420226/how-to-classify-android-devices-based-on-dpi-values
    //
    int dpi = DeviceInfo::getDPI();

    if (dpi >= 480)
    {
        searchPath.push_back(xlargeResource.directory);
        targetDevice = TD_IPAD_RETINA;
        sizeMultiplier = kXLargeMultiplier;
        retinaMultiplier = 1.0f;
        isIPadMultiplier = 1.0f;
        isIPhoneMultiplier = 0.0f;
        isRetinaMultiplier = 1.0f;
        isIOSMultiplier = 1.0f;
        isAndroidMultiplier = 0.0f;
        isPhone = false;
    }
    else if (dpi >= 320)
    {
        searchPath.push_back(largeResource.directory);
        targetDevice = TD_IPAD;
        sizeMultiplier = kLargeMultiplier;
        retinaMultiplier = 0.5f;
        isIPadMultiplier = 1.0f;
        isIPhoneMultiplier = 0.0f;
        isRetinaMultiplier = 0.0f;
        isIOSMultiplier = 1.0f;
        isAndroidMultiplier = 0.0f;
        isPhone = false;
    }
    else if (dpi >= 160)
    {
        searchPath.push_back(mediumResource.directory);
        targetDevice = TD_IPHONE_RETINA;
        sizeMultiplier = kMediumMultiplier;
        retinaMultiplier = 0.5f;
        isIPadMultiplier = 0.0f;
        isIPhoneMultiplier = 1.0f;
        isRetinaMultiplier = 1.0f;
        isIOSMultiplier = 1.0f;
        isAndroidMultiplier = 0.0f;
        isPhone = true;
    }
    else
    {
        searchPath.push_back(smallResource.directory);
        targetDevice = TD_IPHONE;
        sizeMultiplier = kSmallMultiplier;
        retinaMultiplier = 0.5f;
        isIPadMultiplier = 0.0f;
        isIPhoneMultiplier = 1.0f;
        isRetinaMultiplier = 0.0f;
        isIOSMultiplier = 1.0f;
        isAndroidMultiplier = 0.0f;
        isPhone = true;
    }
#endif

    CCLog_("frameSize: %f %f Type: %s", frameSize.height, frameSize.width, searchPath[0].c_str());
    searchPath.push_back(defaultResource.directory);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

    isPhone = !DeviceInfo::isTablet();
    CCLog_("IsPhone: %d", isPhone);
    CCLog_("DPI: %d", DeviceInfo::getDPI());
    isAndroidMultiplier = 1.0f;

    if (isPhone)
    {
        isIPhoneMultiplier = 1.0f;
        isIPadMultiplier = 0.0f;
    }
    else
    {
        isIPhoneMultiplier = 0.0f;
        isIPadMultiplier = 1.0f;
    }

    CCLog_("IsExpansionInstalled: %d", APKExpansion::isExpansionInstalled());
    CCLog_("Expansion directory: %s", APKExpansion::getInstallationDir().c_str());

    // append assets
    std::string expansionDir = APKExpansion::getInstallationDir();
    expansionDir.append("/assets");
    searchPath.push_back(expansionDir);

    expansionDir.append("/");

    std::string expansionImages = expansionDir;
    expansionImages.append(searchPath[0]);

    std::string expansionCommon = expansionDir;
    expansionCommon.append(defaultResource.directory);

    searchPath.push_back(expansionImages);
    searchPath.push_back(expansionCommon);
#endif

    CCFileUtils::sharedFileUtils()->setSearchPaths(searchPath);

        scaleFactor = frameSize.height/defaultResource.size.height;
//        if ( (scaleFactor*defaultResource.size.width) > frameSize.width )
//        {
//            scaleFactor = frameSize.width/defaultResource.size.width;
//        }

    // turn on display FPS
//    pDirector->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
//    pDirector->setAnimationInterval(1.0 / 60);

    // set clear colour
    //pDirector->setGLClearColor(ccc3(159, 212, 46));
    
    switch (targetDevice) {
        case TD_IPHONE: {
            CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("medium/loading_bar.plist");
        } break;
        case TD_IPHONE_RETINA: {
            CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("medium/loading_bar.plist");
        } break;
        case TD_IPAD: {
            CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("large/loading_bar.plist");
        } break;
            
        case TD_IPAD_RETINA: {
            CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("xlarge/loading_bar.plist");
        } break;
            
        default:
            break;
    }
    
    // start scene manager
    CCDirector::sharedDirector()->runWithScene(SceneManager::scene());

    InAppPurchase::sharedInAppPurchase()->getProduct();
    
#if IS_RELEASE_BUILD
    
    //note: iOS only allows one crash reporting tool per app; if using another, set to: NO
    FlurryCpp::setCrashReportingEnabled(true); //calls this before startSession
    
    FlurryCpp::setDebugLogEnabled(true);
    
    FlurryCpp::setUserID(uuid.c_str());
    FlurryCpp::setAppVersion(Versioning::GetBundleVersionShort().c_str());
    
    FlurryCpp::startSession(FLURRY_API_KEY);
    
#endif
    
    // Optional: set Google Analytics dispatch interval to e.g. 20 seconds.
//    GaiCpp::setDispatchInterval(20);
    
#if IS_DEBUG_BUILD
    GaiCpp::setLogLevelVerbose();
    GaiCpp::setDryRun(true);
#endif
    
    GaiCpp::trackerWithTrackingId(GAI_TRACKER_ID);
    
    GaiCpp::reportToACTConversionReporter();

    AppRating::Instance().initialize();
    
    AppsFlyerWrapper::init();
    
    AdsManagerWrapper::increaseSessionCount();

    AppsFlyerWrapper::startSession();

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    
    AnalyticsClient::getInstance()->appDidEnterBackground();
    
    hasPurchasedDuringThisSession = false;
    purchasedWithProductIdentifier = "";
    
    CCDirector::sharedDirector()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();

    g_gameAudio->pauseEffects();

    if (g_sceneManager) {
        g_sceneManager->scheduleLocalNotifications();
        g_sceneManager->enter_background_timestamp = TimeHelper::getCurrentLocalTime();
    }
    
    if (typeid(*g_currentScene) == typeid(SceneVille)) {
        if (SceneVille* _scene = (SceneVille*)g_currentScene) {
            _scene->saveIsoMapInBackend(0);
        }
    }
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    
    CCDirector::sharedDirector()->startAnimation();

    if (g_sceneManager) {
        g_sceneManager->unscheduleLocalNotification();

        //if(!FacebookController::isFacebookOnTop())
        //{
        	long int entered_background_second = TimeHelper::getCurrentLocalTime() - g_sceneManager->enter_background_timestamp;
			if (entered_background_second > kEnterBackgroundTimeMax)
			{
				g_sceneManager->restart(g_sceneManager);
			}
        /*}
        else
        {
        	g_sceneManager->facebook->onApplicationEnterForeground();
        } */
    }

    AppRating::Instance().onEnterForeground();
    
    AdsManagerWrapper::increaseSessionCount();

    AppsFlyerWrapper::startSession();

    AnalyticsClient::getInstance()->startSession(StartSessionOptionsAppResume);

    // if you use SimpleAudioEngine, it must resume here
    CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();

    g_gameAudio->resumeEffects();
}

void AppDelegate::applicationDidReceiveMemoryWarning()
{
    CCLog_("applicationDidReceiveMemoryWarning");
    
    CCLog_("g_gameAudio->unloadAllEffect();");
    g_gameAudio->unloadAllEffect();
    
    if (g_sceneManager) {
        CCLog_("g_sceneManager->sharedSkeletonData->clear()");
        g_sceneManager->sharedSkeletonData->clear();
    }
    
//    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
//    CCDirector::sharedDirector()->purgeCachedData();
}
