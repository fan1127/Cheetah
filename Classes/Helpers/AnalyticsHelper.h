//
//  AnalyticsHelper.h
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 10/29/2557 BE.
//
//

#ifndef __Cheetah__AnalyticsHelper__
#define __Cheetah__AnalyticsHelper__

#include <stdio.h>

#include "cocos2d.h"

#include "../../extensions/CCArmature/external_tool/Json/lib_json/value.h"
//#include "reader.h"
#include "../../extensions/CCArmature/external_tool/Json/lib_json/writer.h"

class AnalyticsHelper {
    
public:
    
    static CSJson::Value getPlayerInfo();
    static CSJson::Value getPetsInfo();
    static CSJson::Value getBuildingsInfo();
    static CSJson::Value getAbilitiesInfo();
    static CSJson::Value getSessionPurchasedInfo();
    
    static double convertFloatTo1DecimalDouble(float number);
    static double convertFloatTo2DecimalDouble(float number);
};

#endif /* defined(__Cheetah__AnalyticsHelper__) */
