/*
 * GaiCppAndroid.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: kristian.mitra
 */
#include "GaiCpp.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

void GaiCpp::trackerWithTrackingId(const char* trackingId)
{
    //TODO: Android Implementation
}

void GaiCpp::reportToACTConversionReporter()
{
	//TODO: Android Implementation
}

void GaiCpp::reportScreenView(const char* screenName)
{
	//TODO: Android Implementation
}

void GaiCpp::setDryRun(bool dryRun)
{
	//TODO: Android Implementation
}

void GaiCpp::setLogLevelVerbose()
{
	//TODO: Android Implementation
}

void GaiCpp::setDispatchInterval(double timeInterval)
{
	//TODO: Android Implementation
}

void GaiCpp::reportPurchase(const char* productIdentifier,
                            const char* localizedTitle,
                            const char* productCategory,
                            float price,
                            const char* currencyCode)
{
    //TODO: Android Implementation
}

#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)


