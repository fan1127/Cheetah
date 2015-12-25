//
//  DataCache.cpp
//  Cheetah
//
//  Created by Vaclav Samec on 11/26/2557 BE.
//
//

#include "DataCache.h"
#include <tr1/functional>
#include "../Services/BackendService.h"
#include "../Services/APIs/APILeaderboardGet.h"
#include "../Services/APIs/APIHelperPetGet.h"
#include "../Services/APIs/APIRaceHistoryList.h"

#include "../Entities/PlayerState.h"
extern PlayerState* g_playerState;
extern PlayerState* g_opponentState;

extern double currentTimeClient;

DataCache& DataCache::Instance()
{
    static DataCache instance;
    return instance;
}

DataCache::DataCache()
{
    resetCallback();
}

void DataCache::fetchLeaderboardFriendsData()
{
    BackendService::getInstance()->send(APILeaderboardGet::create(g_playerState->fbFriendsRawData,
                                                                  NULL,
                                                                  DataCache::onLeaderboardFriendsDataRetrieved));
}

void DataCache::onLeaderboardFriendsDataRetrieved(void* sender_, std::string resp_json_str_)
{
    DataCache::Instance().onDataRetrieved(DataCache::LeaderboardFriend, resp_json_str_);
}

void DataCache::fetchLeaderboardData()
{
    BackendService::getInstance()->send(APILeaderboardGet::create("",
                                                                  NULL,
                                                                  DataCache::onLeaderboardDataRetrieved));
}

void DataCache::onLeaderboardDataRetrieved(void* sender_, std::string resp_json_str_)
{
    DataCache::Instance().onDataRetrieved(DataCache::Leaderboard, resp_json_str_);
}

void DataCache::fetchInboxData()
{
    BackendService::getInstance()->send(APIRaceHistoryList::create(NULL, DataCache::onInboxDataRetrieved));
}

void DataCache::onInboxDataRetrieved(void* sender, std::string resp_str)
{
    PlayerState *_playerState = PlayerState::getInstance();
    
    //JsonAllocator allocator;
    Json*_root = Json_create(resp_str.c_str());
    _playerState->parseHistoryItemData(_root);
    Json_dispose(_root);
    
    DataCache::Instance().onDataRetrieved(DataCache::Inbox, resp_str);
}

void DataCache::fetchHelperData()
{
    BackendService::getInstance()->send(APIHelperPetGet::create(g_playerState->fbFriendsRawData,
                                                                g_opponentState->player.uuid,
                                                                NULL,
                                                                DataCache::onHelperDataRetrieved));
}

void DataCache::onHelperDataRetrieved(void* sender_, std::string resp_json_str_)
{
    DataCache::Instance().onDataRetrieved(DataCache::Helper, resp_json_str_);
}

size_t DataCache::calculateHash(std::string value) const
{
    auto h = std::tr1::hash<std::string>();
    return h(value);
}

void DataCache::invalidateData(DataType type)
{
    cache[type].timeStamp = 0;
    cache[type].data = "";
}

void DataCache::fetchData(DataType type)
{
    unsigned timeCurr = (unsigned)currentTimeClient;

    //! wait for a while for the next url fetch
    if (timeCurr - cache[type].timeStamp < FETCH_TIMEOUT)
    {
        return;
    }

    switch (type)
    {
        case Leaderboard:
        {
            fetchLeaderboardData();
        }
        break;

        case Inbox:
        {
            fetchInboxData();
        }
        break;

        case Helper:
        {
            fetchHelperData();
        }
        break;

        case LeaderboardFriend:
        {
            fetchLeaderboardFriendsData();
        }
        break;

        case End:
        {
            CCAssert(false, "Invalid data cache type!");
        }
        break;
    }
}

void DataCache::onDataRetrieved(DataType type, std::string data)
{
    CCAssert(type != End, "Invalid data cache type!");

    CCLog_("DataCache::onDataRetrieved: %d", type);

    unsigned timeCurr = (unsigned)currentTimeClient;

    Cache& c = cache[type];

    if (c.data.empty())
    {
        c.data = data;
        c.timeStamp = timeCurr;
        onDataChanged(type);

        CCLog_("DataCache::onDataChanged NEW: %d", type);
    }
    else
    {
        CCLog_("DataCache::onDataChanged UPDATE: %d %d", type, (timeCurr - c.timeStamp));

        c.data = data;
        c.timeStamp = timeCurr;
        onDataChanged(type);
    }
}

void DataCache::onDataChanged(DataType type)
{
    if (callbackType == type && callbackMethod)
    {
        callbackMethod(callbackSender, cache[type].data);
    }
}

void DataCache::getDataAsync(DataType type, void *sender, Callback callback)
{
    callbackType = type;
    callbackSender = sender;
    callbackMethod = callback;

    if (isDataValid(type))
    {
        CCLog_("DataCache::getDataAsync: CACHE HIT %d", type);
        onDataChanged(type);
    }

    fetchData(type);
}

void DataCache::resetCallback()
{
    callbackType = End;
    callbackSender = NULL;
    callbackMethod = NULL;
}
