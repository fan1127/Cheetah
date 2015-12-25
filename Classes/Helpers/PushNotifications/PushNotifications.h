//
// Created by Vaclav Samec on 11/10/14 AD.
//


#ifndef __PushNotifications_H_
#define __PushNotifications_H_

#include <string>
class PushNotificationsDialog;
class PushNotificationsWrapper;

class PushNotifications
{
public:

    static const int upgradeCountdownMin;
    static const float firstTimerMin;
    static const float secondTimerMin;

    static PushNotifications& Instance();

    bool isEnabled() { return m_allowNotifications; }

    enum NotificationEvents
    {
        None,
        FirstTimeInVille,
        FirstTimeAnimalWakeUp,
        FirstTimeBuildingUpgrade,
    };

    bool notify(NotificationEvents event, std::string param0 = "");

    void onButtonYes();

    void onButtonNo();
    
    std::string getStringFromEvent(NotificationEvents event);

    void onRegisteredRemoteNotifications(std::string token);
    void onRegisteredRemoteNotificationsAndroid(std::string regID);

    void invokeNativeConfirmationDialog();

private:

    PushNotificationsWrapper* m_wrapper;

    //
    // user data keys
    //
    static const char* KEY_LAST_NOTIFICATION;
    static const char* KEY_NOTIFICATION_EVENT_NUMBER;
    static const char* KEY_NOTIFICATION_ALLOW;
    static const char* KEY_NOTIFICATION_PET_WAKE_UP;
    static const char* KEY_NOTIFICATION_BUILDING_UPGRADE;
    static const char* KEY_NOTIFICATION_SCENE_VILLE;

    unsigned int m_lastNotification;

    int m_notificationEventNumber;
    bool m_allowNotifications;
    bool m_wakeUpEvent;
    bool m_upgradeEvent;
    bool m_sceneVilleEvent;
    NotificationEvents m_currEvent;

    PushNotifications();
    virtual ~PushNotifications();

    float getFirstTimeout() const;

    float getSecondTimeout() const;

    void loadData();
    void saveData();

    PushNotificationsDialog* createNotificationDialog();

};


#endif //__PushNotifications_H_
