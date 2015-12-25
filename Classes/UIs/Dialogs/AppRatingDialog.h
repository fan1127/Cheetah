//
//  ModalDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/7/2557 BE.
//
//

#ifndef __Cheetah__AppRatingDialog__
#define __Cheetah__AppRatingDialog__


#include "NotificationDialog.h"


class AppRating;

class AppRatingsDialog : public NotificationDialog
{
protected:

    AppRating*                  m_ratings;

public:

    AppRatingsDialog();

    virtual ~AppRatingsDialog();

    static AppRatingsDialog* create(CCModalController* parent);

    static AppRatingsDialog *create(CCModalController* parent, cocos2d::CCSize dialogSize);

    void setRatings(AppRating* notes) { m_ratings = notes; }

    virtual void onButtonYes(cocos2d::CCObject *sender);

    virtual void onButtonNo(cocos2d::CCObject *sender);
};

#endif /* defined(__Cheetah__StandardDialog__) */
