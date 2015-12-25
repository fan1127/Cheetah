//
// Created by Maethee Chongchitnant on 9/5/14 AD.
//

#include "ReplayData.h"
#include "Settings.h"

USING_NS_CC;
using namespace std;

#include "base64.h"
#include "StringHelper.h"

CCPoint ReplayData::getPositionAtTime(float time) const
{
    Data<CCPoint> before(CCPoint(), 0.f);
    Data<CCPoint> after(CCPoint(), 0.f);
    if(!positions.getDataAtTime(time, before, after))
        return before.data;

    float normalizedTime = positions.getNormalizedTime(time, before, after);
    CCPoint pos = before.data.lerp(after.data, normalizedTime);
    //CCLog_("before: %.2f, %.2f", before.data.x, before.data.y);
    //CCLog_("after: %.2f, %.2f", after.data.x, after.data.y);
    //CCLog_("time: %.2f", normalizedTime);
    //CCLog_("lerp: %.2f, %.2f", pos.x, pos.y);
    return pos;
}

int ReplayData::getStateAtTime(int stateType, float time, float& timeInState) const
{
    CCAssert(stateType >= 0 && stateType < MAX_STATE_TYPES, "invalid state type");
    Data<int> before(0, 0.f);
    Data<int> after(0, 0.f);
    if(!states[stateType].getDataAtTime(time, before, after))
    {
        timeInState = time - before.time;
        return before.data;
    }
    timeInState = time - before.time;
    //CCLog_("before state: %d", before.data);
    //CCLog_("after state: %d", after.data);
    //CCLog_("time in state: %.2f", timeInState);
    return before.data;
}

bool ReplayData::getStateBetweenTime(int stateType, float gteTime, float ltTime, int& outState, float& outTime) const
{
    CCAssert(stateType >= 0 && stateType < MAX_STATE_TYPES, "invalid state type");
    Data<int> result(0, 0.f);
    if(!states[stateType].getDataInBetween(gteTime, ltTime, result))
        return false;
    outState = result.data;
    outTime = result.time;
    return true;
}

float ReplayData::getFloatAtTime(int floatType, float time, bool interpolate) const
{
    CCAssert(floatType >= 0 && floatType < MAX_FLOATS_DATA, "invalid float type");
    Data<float> before(0.f, 0.f);
    Data<float> after(0.f, 0.f);
    if(!floats[floatType].getDataAtTime(time, before, after))
        return before.data;
    if(!interpolate)
        return before.data;
    float normalizedTime = floats[floatType].getNormalizedTime(time, before, after);
    float val = before.data + ((after.data - before.data) * normalizedTime);
    //CCLog_("before: %.2f", before.data);
    //CCLog_("after: %.2f", after.data.x);
    //CCLog_("time: %.2f", normalizedTime);
    //CCLog_("lerp: %.2f, %.2f", pos.x, pos.y);
    return val;
}

void ReplayData::addData(CCPoint position, float time)
{
    Data<CCPoint> posData(position, time);
    positions.items.push_back(posData);
}

void ReplayData::addData(int stateType, int state, float time)
{
    CCAssert(stateType >= 0 && stateType < MAX_STATE_TYPES, "invalid state type");
    Data<int> stateData(state, time);
    states[stateType].items.push_back(stateData);
}

void ReplayData::addData(int floatType, float value, float time)
{
    CCAssert(floatType >= 0 && floatType < MAX_FLOATS_DATA, "invalid float type");
    Data<float> floatData(value, time);
    floats[floatType].items.push_back(floatData);
}

void ReplayData::clearData()
{
    positions.items.clear();
    for(int i=0; i<MAX_STATE_TYPES; ++i)
        states[i].items.clear();
    for(int i=0; i<MAX_FLOATS_DATA; ++i)
        floats[i].items.clear();
}

CCPoint ReplayData::getLastAddedPosition() const
{
    return positions.items.back().data;
}

int ReplayData::getLastAddedState(int stateType) const
{
    CCAssert(stateType >= 0 && stateType < MAX_STATE_TYPES, "invalid state type");
    return states[stateType].items.back().data;
}

float ReplayData::getLastAddedFloat(int floatType) const
{
    CCAssert(floatType >= 0 && floatType < MAX_FLOATS_DATA, "invalid float type");
    return floats[floatType].items.back().data;
}

float ReplayData::getPositionStartTime() const
{
    return positions.getStartTime();
}

float ReplayData::getPositionEndTime() const
{
    return positions.getEndTime();
}

float ReplayData::getStateStartTime(int stateType) const
{
    CCAssert(stateType >= 0 && stateType < MAX_STATE_TYPES, "invalid state type");
    return states[stateType].getStartTime();
}

float ReplayData::getStateEndTime(int stateType) const
{
    CCAssert(stateType >= 0 && stateType < MAX_STATE_TYPES, "invalid state type");
    return states[stateType].getEndTime();
}

float ReplayData::getFloatStartTime(int floatType) const
{
    CCAssert(floatType >= 0 && floatType < MAX_FLOATS_DATA, "invalid float type");
    return floats[floatType].getStartTime();
}

float ReplayData::getFloatEndTime(int floatType) const
{
    CCAssert(floatType >= 0 && floatType < MAX_FLOATS_DATA, "invalid float type");
    return floats[floatType].getEndTime();
}

bool ReplayData::isBeyondEndTime(float time) const
{
    if(positions.items.size() > 0 && getPositionEndTime() >= time)
        return false;

    for(int i=0; i<MAX_STATE_TYPES; ++i)
    {
        if(states[i].items.size() > 0 && getStateEndTime(i) >= time)
            return false;
    }
    for(int i=0; i<MAX_FLOATS_DATA; ++i)
    {
        if(floats[i].items.size() > 0 && getFloatEndTime(i) >= time)
            return false;
    }
    return true;
}

float ReplayData::getDuration() const
{
    float duration = 0;

    if(positions.items.size() > 0)
        duration = getPositionEndTime();

    for(int i=0; i<MAX_STATE_TYPES; ++i)
    {
        if(states[i].items.size() < 1)
            continue;
        float endTime = getStateEndTime(i);
        if(endTime > duration)
            duration = endTime;
    }
    for(int i=0; i<MAX_FLOATS_DATA; ++i)
    {
        if(floats[i].items.size() < 1)
            continue;
        float endTime = getFloatEndTime(i);
        if(endTime > duration)
            duration = endTime;
    }
    return duration;
}

string ReplayData::serialize()
{
    string data;
    data.append(CCString::createWithFormat("%d\n", (int)positions.items.size())->m_sString);
    for(auto iter = positions.items.begin(), iterEnd = positions.items.end(); iter != iterEnd; ++iter)
    {
        Data<CCPoint> posData = *iter;
        CCPoint point = posData.data;
        data.append(CCString::createWithFormat("%.3f\n%.3f\n%.4f\n", point.x, point.y, posData.time)->m_sString);
    }
    for(int i=0; i<MAX_STATE_TYPES; ++i)
    {
        data.append(CCString::createWithFormat("%d\n", (int)states[i].items.size())->m_sString);
        for(auto iter = states[i].items.begin(), iterEnd = states[i].items.end(); iter != iterEnd; ++iter)
        {
            Data<int> stateData = (*iter);
            data.append(CCString::createWithFormat("%d\n%.4f\n", stateData.data, stateData.time)->m_sString);
        }
    }
    for(int i=0; i<MAX_FLOATS_DATA; ++i)
    {
        data.append(CCString::createWithFormat("%d\n", (int)floats[i].items.size())->m_sString);
        for(auto iter = floats[i].items.begin(), iterEnd = floats[i].items.end(); iter != iterEnd; ++iter)
        {
            Data<float> floatData = (*iter);
            data.append(CCString::createWithFormat("%.4f\n%.4f\n", floatData.data, floatData.time)->m_sString);
        }
    }
    
#ifndef kReplayDataCompressionEnabled
    return data;
#else
    string compressed_data = StringHelper::compress_string(data);
    string compressed_base64encoded_data = base64_encode((unsigned char const*)compressed_data.c_str(), compressed_data.size());
    CCLog_("data size: %ld", data.size());
    CCLog_("compressed size: %ld", compressed_data.size());
    CCLog_("base64encoded size: %ld", compressed_base64encoded_data.size());
    return compressed_base64encoded_data;
#endif
    
}


bool ReplayData::deserialize(const string& data)
{
#ifndef kReplayDataCompressionEnabled
    string _data = string(data);
#else
    string compressed_base64encoded_data = string(data);
    // all '+' characters have been replace by ' ' (space) while transfering data
    // so we replace it back
    StringHelper::replace_all(compressed_base64encoded_data, " ", "+");
    CCLog_("Start Decompress Data:");
    string compressed_data = base64_decode(compressed_base64encoded_data);
    string _data = StringHelper::decompress_string(compressed_data);
#endif
    CCLog_("Start Deserialize Data:");

    const int READ_POS_COUNT = 0;
    const int READ_POS_X = 1;
    const int READ_POS_Y = 2;
    const int READ_POS_TIME = 3;
    const int READ_STATE_COUNT = 4;
    const int READ_STATE = 5;
    const int READ_STATE_TIME = 6;
    const int READ_FLOAT_COUNT = 7;
    const int READ_FLOAT = 8;
    const int READ_FLOAT_TIME = 9;
#ifndef kReplayDataCompressionEnabled
    istringstream ss(data);
#else
    istringstream ss(_data);
#endif
    string line;
    int progress = 0;
    int count = 0;
    int current = 0;
    CCPoint point;
    int state = 0;
    int stateType = 0;
    float floatValue = 0.f;
    int floatType = 0;

    while (getline(ss, line))
    {
        switch(progress)
        {
        case READ_POS_COUNT:
            count = CCString::create(line)->intValue();
            if(count == 0)
                progress = READ_STATE_COUNT;
            else
                progress = READ_POS_X;
            break;

        case READ_POS_X:
            point.x = CCString::create(line)->floatValue();
            progress = READ_POS_Y;
            break;

        case READ_POS_Y:
            point.y = CCString::create(line)->floatValue();
            progress = READ_POS_TIME;
            break;

        case READ_POS_TIME:
            addData(point, CCString::create(line)->floatValue());
            ++current;
            if(current == count)
                progress = READ_STATE_COUNT;
            else
                progress = READ_POS_X;
            break;

        case READ_STATE_COUNT:
            count = CCString::create(line)->intValue();
            current = 0;
            if(count == 0)
            {
                progress = READ_STATE_COUNT;
                ++stateType;
                if(stateType == MAX_STATE_TYPES)
                    progress = READ_FLOAT_COUNT;
            }
            else
                progress = READ_STATE;
            break;

        case READ_STATE:
            state = CCString::create(line)->intValue();
            progress = READ_STATE_TIME;
            break;

        case READ_STATE_TIME:
            addData(stateType, state, CCString::create(line)->floatValue());
            ++current;
            if(current == count)
            {
                progress = READ_STATE_COUNT;
                ++stateType;
                if(stateType == MAX_STATE_TYPES)
                    progress = READ_FLOAT_COUNT;
            }
            else
                progress = READ_STATE;
            break;

        case READ_FLOAT_COUNT:
            if(floatType == MAX_FLOATS_DATA)
                break;
            count = CCString::create(line)->intValue();
            current = 0;
            if(count == 0)
            {
                progress = READ_FLOAT_COUNT;
                ++floatType;
            }
            else
                progress = READ_FLOAT;
            break;

        case READ_FLOAT:
            floatValue = CCString::create(line)->floatValue();
            progress = READ_FLOAT_TIME;
            break;

        case READ_FLOAT_TIME:
            addData(floatType, floatValue, CCString::create(line)->floatValue());
            ++current;
            if(current == count)
            {
                progress = READ_FLOAT_COUNT;
                ++floatType;
            }
            else
                progress = READ_FLOAT;
            break;

        default:
            CCAssert(false, "Should not be here!");
            break;
        }
    }

    CCLog_("Finished Deserialization");
#ifndef kReplayDataCompressionEnabled
    CCAssert(strcmp(serialize().c_str(), data.c_str()) == 0, "deserialization failed");
#else
    CCAssert(strcmp(serialize().c_str(), compressed_base64encoded_data.c_str()) == 0, "deserialization failed");
#endif
    
    return true;
}
