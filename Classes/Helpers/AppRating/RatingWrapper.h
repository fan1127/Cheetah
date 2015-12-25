//
//  RatingWrapper.h
//  Cheetah
//
//  Created by Vaclav Samec on 11/12/2557 BE.
//
//

#ifndef __Cheetah__RatingWrapper__
#define __Cheetah__RatingWrapper__

class RatingWrapper
{
public:

    //! init the native rating library
    void init();

    //! returns true if the rating conditions have been met
    bool canRate();

    //! directly rate the app without any check, use this if user clicks YES on rating dialog
    void rateApp();

    //! user doesn't want to rate the app, use this if user clicks NO on rating dialog
    void dontRateApp();

    //! call this when the application entered foreground
    void onEnterForeground();
};

#endif /* defined(__Cheetah__RatingWrapper__) */
