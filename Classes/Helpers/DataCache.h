//
//  DataCache.h
//  Cheetah
//
//  Created by Vaclav Samec on 11/26/2557 BE.
//
//

#ifndef __Cheetah__DataCache__
#define __Cheetah__DataCache__

#include <string>
#include <vector>
#include <tr1/functional>

class DataCache
{
public:

    static DataCache &Instance();

    //! timeout between consequent data fetch of the same type
    static const unsigned int FETCH_TIMEOUT = 10*60*60;

    typedef std::tr1::function<void (void*, std::string)> Callback;

    enum DataType
    {
        Leaderboard,
        LeaderboardFriend,
        Inbox,
        Helper,
        End,
    };

    void fetchData(DataType type);
    std::string getData(DataType type) { return cache[type].data; }
    bool isDataValid(DataType type) const { return !cache[type].data.empty(); }
    void invalidateData(DataType type);

    void getDataAsync(DataType type, void *sender, Callback callback);
    void resetCallback();

private:

    DataCache();

    struct Cache
    {
        Cache() : data(""), timeStamp(0) {}
        std::string data;
        unsigned int timeStamp;
    };

    Cache cache[End];

    DataType callbackType;
    void* callbackSender;
    Callback callbackMethod;
//    void (*callbackMethod)(void* sender_, std::string resp_json_str_);

    void fetchLeaderboardFriendsData();
    static void onLeaderboardFriendsDataRetrieved(void* sender_, std::string resp_json_str_);

    void fetchLeaderboardData();
    static void onLeaderboardDataRetrieved(void* sender_, std::string resp_json_str_);

    void fetchInboxData();
    static void onInboxDataRetrieved(void* sender_, std::string resp_json_str_);

    void fetchHelperData();
    static void onHelperDataRetrieved(void* sender_, std::string resp_json_str_);

    size_t calculateHash(std::string value) const;

    void onDataRetrieved(DataType type, std::string data);

    void onDataChanged(DataType type);
};

#endif /* defined(__Cheetah__DataCache__) */
