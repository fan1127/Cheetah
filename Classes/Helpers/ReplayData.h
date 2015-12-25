#ifndef __ReplayData_H_
#define __ReplayData_H_

#include "cocos2d.h"
#include "ccMacros.h"
#include <vector>

#define kReplayDataCompressionEnabled

class ReplayData
{
public:

    static const int MAX_STATE_TYPES = 6;
    static const int MAX_FLOATS_DATA = 6;

    template <class T> struct Data
    {
        T data;
        float time;
        Data(T d, float t): data(d), time(t) {}
    };

    template <class T> struct DataArray
    {
        bool getDataAtTime(float t, Data<T>& before, Data<T>& after) const
        {
            auto iter = items.begin();
            if(iter->time >= t)
            {
                const Data<T>& data = *iter;
                before = data;
                after = data;
                return false;
            }
            const Data<T>& back = items.back();
            if(back.time <= t)
            {
                before = back;
                after = back;
                return false;
            }

            for(auto iterEnd = items.end(); iter != iterEnd; ++iter)
            {
                const Data<T>& data = *iter;
                if(data.time < t)
                    continue;
                after = data;
                before = *(iter-1);
                return true;
            }
            before = back;
            after = back;
            return false;
        }

        bool getDataInBetween(float gteTime, float ltTime, Data<T>& result) const
        {
            for(auto iter = items.begin(), iterEnd = items.end(); iter != iterEnd; ++iter)
            {
                const Data<T>& data = *iter;
                if(data.time >= ltTime)
                    return false;
                if(data.time < gteTime)
                    continue;
                result = data;
                return true;
            }
            return false;
        }

        float getNormalizedTime(float t, const Data<T>& before, const Data<T>& after) const
        {
            float timeRange = after.time - before.time;
            float currentTime = t - before.time;
            return currentTime / timeRange;
        }

        float getStartTime() const
        {
            return items.front().time;
        }

        float getEndTime() const
        {
            return items.back().time;
        }

        std::vector<Data<T> > items;
    };

    void addData(cocos2d::CCPoint position, float time);
    void addData(int stateType, int state, float time);
    void addData(int floatType, float value, float time);

    void clearData();

    cocos2d::CCPoint getPositionAtTime(float time) const;
    cocos2d::CCPoint getLastAddedPosition() const;

    int getStateAtTime(int stateType, float time, float& timeInState) const;
    bool getStateBetweenTime(int stateType, float gteTime, float ltTime, int& outState, float& outTime) const;
    int getLastAddedState(int stateType) const;

    float getFloatAtTime(int floatType, float time, bool interpolate = true) const;
    float getLastAddedFloat(int floatType) const;

    float getPositionStartTime() const;
    float getPositionEndTime() const;
    float getStateStartTime(int stateType) const;
    float getStateEndTime(int stateType) const;
    float getFloatStartTime(int floatType) const;
    float getFloatEndTime(int floatType) const;

    bool isBeyondEndTime(float time) const;
    float getDuration() const;

    std::string serialize();
    bool deserialize(const std::string& data);

private:

    DataArray<cocos2d::CCPoint> positions;
    DataArray<int> states[MAX_STATE_TYPES];
    DataArray<float> floats[MAX_FLOATS_DATA];
};

#endif //__ReplayData_H_
