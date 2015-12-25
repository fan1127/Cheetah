//
//  Vehicle.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/14/2556 BE.
//
//

#include "Vehicle.h"
USING_NS_CC;
using namespace std;
using namespace cocos2d::extension;

#include "../../SceneManager.h"
extern SceneManager *g_sceneManager;

//extern CCPoint screenFactor;
extern float sizeMultiplier;


Vehicle *Vehicle::createWithSpineName(const char *spine_name_, bool auto_remove_, void *preferenceRootParent_) {
    
    Vehicle *pRet = new Vehicle();
    pRet->initWithSpineName(spine_name_, auto_remove_, preferenceRootParent_);
    pRet->autorelease();
    return pRet;
    
}

bool Vehicle::initWithSpineName(const char *spine_name_, bool auto_remove_, void *preferenceRootParent_) {
    
    if (!CCNode::init()) {
        return false;
    }
    
    autoRemove = auto_remove_;
    
    dropped = false;
    m_distPositions.clear();
    m_stopAt = -1;
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    snprintf(_buffer, _buffer_size, "%s_front", spine_name_);
    m_vehicleFront = g_sceneManager->sharedSkeletonData->getSkeleton(kVehicleSrcDir, _buffer);
    //m_vehicleFront->preferenceRootParent = preferenceRootParent_;
    m_vehicleFront->setAnimation("animation", true);
    m_vehicleFront->timeScale = 1.0f;
    m_vehicleFront->setScale(sizeMultiplier);
    this->addChild(m_vehicleFront);
    
    snprintf(_buffer, _buffer_size, "%s_back", spine_name_);
    m_vehicleBack = g_sceneManager->sharedSkeletonData->getSkeleton(kVehicleSrcDir, _buffer);
    //m_vehicleBack->preferenceRootParent = preferenceRootParent_;
    m_vehicleBack->setAnimation("animation", true);
    m_vehicleBack->timeScale = 1.0f;
    m_vehicleBack->setScale(sizeMultiplier);
    this->addChild(m_vehicleBack);
    
    setVehicleState(VS_MOVING_RIGHT_DOWN);

    return true;
    
}

bool Vehicle::updateMoving(float delta_) {
    
    if (m_distPositions.size() <= 0) {
        return false;
    }
    
    if (m_distPositions.size() == m_stopAt) {
        m_stopAt = -2;
        dropped = true;
        scheduleOnce(schedule_selector(Vehicle::willContinue), 2.0f);
        return true;
    }
    
    if (m_stopAt == -2) {
        return true;
    }
    
    CCPoint _currPosition = this->getPosition();
    
    float x = m_distPositions[m_distPositions.size()-1].x - _currPosition.x;
    float y = m_distPositions[m_distPositions.size()-1].y - _currPosition.y;
    float z = sqrtf(x*x + y*y);
    
    if (z < m_movingSpeed) {
        m_distPositions.pop_back();
    }
    
    if (_currPosition.x > m_distPositions[m_distPositions.size()-1].x) {
        if (_currPosition.y > m_distPositions[m_distPositions.size()-1].y) {
            setVehicleState(VS_MOVING_LEFT_DOWN);
        }else if (_currPosition.y < m_distPositions[m_distPositions.size()-1].y) {
            setVehicleState(VS_MOVING_LEFT_UP);
        }
    }else if (_currPosition.x < m_distPositions[m_distPositions.size()-1].x) {
        if (_currPosition.y > m_distPositions[m_distPositions.size()-1].y) {
            setVehicleState(VS_MOVING_RIGHT_DOWN);
        }else if (_currPosition.y < m_distPositions[m_distPositions.size()-1].y) {
            setVehicleState(VS_MOVING_RIGHT_UP);
        }
    }
    
    float a = atan2f(y, x);
    _currPosition.x += m_movingSpeed * cosf(a);
    _currPosition.y += m_movingSpeed * sinf(a);
    
    this->setPosition(_currPosition);

    return true;
}

void Vehicle::willContinue() {
    
    dropped = false;
    m_stopAt = -1;
    
}

void Vehicle::setVehicleState(VehicleState state_) {
    
    switch (state_) {
        default:
        case VS_STOP: {
            
        }break;
        case VS_MOVING_LEFT_DOWN: {
            m_vehicleFront->setVisible(true);
            m_vehicleBack->setVisible(false);
            _flip(false);
        }break;
        case VS_MOVING_LEFT_UP: {
            m_vehicleFront->setVisible(false);
            m_vehicleBack->setVisible(true);
            _flip(false);
        }break;
        case VS_MOVING_RIGHT_DOWN: {
            m_vehicleFront->setVisible(true);
            m_vehicleBack->setVisible(false);
            _flip(true);
        }break;
        case VS_MOVING_RIGHT_UP: {
            m_vehicleFront->setVisible(false);
            m_vehicleBack->setVisible(true);
            _flip(true);
        }break;
    }
    
    m_state = state_;
    
}

void Vehicle::setVehicleSize(CCSize size1_, CCSize size2_) {
    
    m_vehicleFront->setContentSize(size1_);
    m_vehicleBack->setContentSize(size2_);
    
}

void Vehicle::setVehicleSpeed(float moving_speed_) {
    
    m_movingSpeed = moving_speed_;
    
}

void Vehicle::setVehicleStopAt(int stop_at_) {
    
    m_stopAt = stop_at_;
    
}

void Vehicle::addDistinationPosition(CCPoint pos_) {
    
    m_distPositions.insert(m_distPositions.begin(), pos_);
    
}
/*
void Vehicle::setDriveTo(CCPoint to_pos_) {
    
    setDriveTo(this->getPosition(), to_pos_, m_movingSpeed);
    
}

void Vehicle::setDriveTo(CCPoint from_pos_, CCPoint to_pos_) {
    
    setDriveTo(from_pos_, to_pos_, m_movingSpeed);
    
}

void Vehicle::setDriveTo(CCPoint from_pos_, CCPoint to_pos_, float moving_speed_) {
    
    this->setPosition(from_pos_);
    m_distPosition = to_pos_;
    m_movingSpeed = moving_speed_;
    
    if (from_pos_.x > to_pos_.x) {
        if (from_pos_.y > to_pos_.y) {
            setVehicleState(VS_MOVING_LEFT_DOWN);
        }else if (from_pos_.y < to_pos_.y) {
            setVehicleState(VS_MOVING_LEFT_UP);
        }
    }else if (from_pos_.x < to_pos_.x) {
        if (from_pos_.y > to_pos_.y) {
            setVehicleState(VS_MOVING_RIGHT_DOWN);
        }else if (from_pos_.y < to_pos_.y) {
            setVehicleState(VS_MOVING_RIGHT_UP);
        }
    }
    
}
*/
void Vehicle::_flip(bool flip_) {
    
    float _scale_x = fabsf(this->getScaleX());
    
    if (flip_) {
        this->setScaleX(-_scale_x);
    }else {
        this->setScaleX( _scale_x);
    }
    
}
