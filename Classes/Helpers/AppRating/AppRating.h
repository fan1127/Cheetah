//
//  AppRating.h
//  Cheetah
//
//  Created by Vaclav Samec on 11/12/2557 BE.
//
//

#include "cocos2d.h"

#ifndef __Cheetah__AppRating__
#define __Cheetah__AppRating__

class AppRatingsDialog;
class NotificationDialog;
class RatingWrapper;

class AppRating : cocos2d::CCNode
{
private:

    // this instance is non-copyable!
    AppRating();

    AppRating(const AppRating &);

    AppRating &operator=(const AppRating &);

    void setupUpdate();

    void stopUpdate();

    AppRatingsDialog* createNotificationDialog();

    void loadData();
    void saveData();

    float m_dialogTimeout;
    bool m_levelUp;

    RatingWrapper*  m_rating;

    static const char* KEY_APP_RATING_LEVEL;

public:

    static AppRating &Instance();

    void initialize();

    void onEnterForeground();

    void showRateDialog();

    void showRateDialogDislike();

    void onRateButtonNo();

    void onRateButtonYes();

    void onRateDislikeOk();

    bool canRate();

    virtual void update(float dt);

    void onLevelUp(int level);
};

#endif /* defined(__Cheetah__AppRating__) */
