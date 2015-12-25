//
//  DeviceInfo.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/24/2557 BE.
//
//


#include <iostream>
#include <string>

class DeviceInfo
{
public:
    
    static std::string getName();
    static std::string getModel();
    static std::string getSystemName();
    static std::string getSystemVersion();
    static std::string getLanguage();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

    //! this will returns true if the current device is tablet
    //! however this function is not accurate because in reality there is no
    //! difference between android phone and android tablet, some tablets can
    //! call and can be considered as phones, and some phones with big screens/resolutions
    //! can be considered as tablets
    //!
    //! http://stackoverflow.com/questions/5832368/tablet-or-phone-android
    static bool isTablet();

    //! get DPI of the device
    static int getDPI();

#endif
};
