//
// Created by Vaclav Samec on 12/24/14 AD.
//


#ifndef __APKExpansion_H_
#define __APKExpansion_H_


#include <string>

class APKExpansion
{
public:

    static bool isExpansionInstalled();

    static std::string getInstallationDir();

    static void hideSplashscreen();

    static int getScreenWidth();

    static int getScreenHeight();
};


#endif //__APKExpansion_H_
