//
//  EnvManager.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/14/2556 BE.
//
//

#include "EnvManager.h"
USING_NS_CC;
using namespace std;

#include "../IsoMapBuilding.h"

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

EnvManager *EnvManager::createWithParent(void *preferentRootParent_) {
    
    EnvManager *pRet = new EnvManager();
    pRet->preferentRootParent = preferentRootParent_;
    if (pRet->init()) {
        pRet->autorelease();
        return pRet;
    }
    delete pRet;
    return NULL;
    
}

bool EnvManager::init() {
    
    IsoMapBuilding *_isoMap = (IsoMapBuilding*)preferentRootParent;
    if (!_isoMap) {
        CCLog_("EnvManager::init - isoMapBuilding is uninitialized!");
        return false;
    }
    
    if (!CCLayer::init()) {
        return false;
    }
    
    m_visitors.clear();
    
    PlayerState *_mapPlayerState = (PlayerState*)_isoMap->mapPlayerState;
    m_ratio = g_gameContent->GameConfig["MAX_VISITORS"]/g_gameContent->GameConfig["MAX_FANS"];
    m_limit_visitor = ceil(_mapPlayerState->player.balance.fan * m_ratio);
    if (m_limit_visitor < g_gameContent->GameConfig["MIN_VISITORS"]) {
        m_limit_visitor = g_gameContent->GameConfig["MIN_VISITORS"];
    }
    if (m_limit_visitor > g_gameContent->GameConfig["MAX_VISITORS"]) {
        m_limit_visitor = g_gameContent->GameConfig["MAX_VISITORS"];
    }
    
    int n = m_limit_visitor/2;
    for (int i=0; i<n; i++) {
        
        Visitor *_visitor = Visitor::createWithRandomSpineName();
        _visitor->isoMap = _isoMap;
        _visitor->setDistAndCurrPos(_visitor->getRandomPointNearBuilding(_mapPlayerState)); // ccp(2964,2206)
        m_visitors.push_back(_visitor);
        _isoMap->addChild(_visitor);
        
    }
    
    m_cloudShadow = CloudShadow::createWithContentSize(_isoMap->getContentSize());
    m_cloudShadow->setPosition(_isoMap->getContentSize()*0.5f);
    _isoMap->addChild(m_cloudShadow, int(_isoMap->getContentSize().height));
    
    return true;
    
}

//EnvManager::~EnvManager() {
//void EnvManager::cleanup() {

//    for (vector<Vehicle*>::iterator it=m_vehicleLane1.begin(); it!=m_vehicleLane1.end(); ++it) {
//        if (Vehicle *_vehicle = (Vehicle*)(*it)) {
//            _vehicle->removeFromParentAndCleanup(true);
////            _vehicle->release();
//            _vehicle = NULL;
//        }
//    }
//    m_vehicleLane1.clear();
//    
//    for (vector<Vehicle*>::iterator it=m_vehicleLane2.begin(); it!=m_vehicleLane2.end(); ++it) {
//        if (Vehicle *_vehicle = (Vehicle*)(*it)) {
//            _vehicle->removeFromParentAndCleanup(true);
////            _vehicle->release();
//            _vehicle = NULL;
//        }
//    }
//    m_vehicleLane2.clear();
//    
//    for (vector<Visitor*>::iterator it=m_visitors.begin(); it!=m_visitors.end(); ++it) {
//        if (Visitor *_visitor = (Visitor*)(*it)) {
//            _visitor->removeFromParentAndCleanup(true);
////            _visitor->release();
//            _visitor = NULL;
//        }
//    }
//    m_visitors.clear();
    
//    CCLayer::cleanup();
    
//}

void EnvManager::update(float delta_) {
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    
    IsoMapBuilding *_isoMap = (IsoMapBuilding*)preferentRootParent;
    if (!_isoMap) {
        CCLog_("EnvManager::update - isoMapBuilding is uninitialized!");
        return;
    }
    
    m_timer += delta_;
    if (m_timer > 1.0f) {
        m_timer = 0.0f;
        
        m_count ++;
        if (m_count > 5) {
            m_count = 0;
            if (rand()%2 == 0) {
                if (m_vehicleLane1.size() == 0) {
                    this->addVehicleLane1("bus");
                }
            }else {
                if (m_vehicleLane2.size() == 0) {
                    this->addVehicleLane2("bus");
                }
            }
        }
        
    }
    
    std::vector<int> _remove_list;
    _remove_list.clear();
    
    int i = 0;
    // Update Vehicle Moving
//    for (vector<Vehicle*>::reverse_iterator it=m_vehicleLane1.rbegin(); it!=m_vehicleLane1.rend(); ++it) {
    for (vector<Vehicle*>::iterator it=m_vehicleLane1.begin(); it!=m_vehicleLane1.end(); ++it) {
        if (Vehicle *_vehicle = (Vehicle*)(*it)) {
            if(!_vehicle->updateMoving(delta_)) {
                _remove_list.push_back(i);
//                _vehicle->removeFromParentAndCleanup(true);
//                _vehicle = NULL;
//                m_vehicleLane1.erase(m_vehicleLane1.begin()+i);
            }
            if (_vehicle) {
                if (_vehicle->dropped) {
                    _vehicle->dropped = false;
                    if ((m_visitors.size() < m_limit_visitor) && arc4random()%2==0 && g_playerState->player.racing.status) {
                        Visitor *_visitor = NULL;
                        snprintf(_buffer, _buffer_size, "people%d", arc4random()%4);
                        _visitor = Visitor::createWithSpineName(_buffer);
                        _visitor->isoMap = _isoMap;
                        _visitor->setDistAndCurrPos(_vehicle->getPosition()); // ccp(2964,2206)
                        m_visitors.push_back(_visitor);
                        _visitor->enterRanch();
                        _isoMap->addChild(_visitor);
                    }
                    
                }
            }
            
        }
        i++;
    }
    
    // Remove those arrived vehicle
    for (vector<int>::reverse_iterator it=_remove_list.rbegin(); it!=_remove_list.rend(); ++it) {
        int _index = (int)(*it);
        if (_index >= 0) {
            if (Vehicle *_vehicle = (Vehicle*)m_vehicleLane1[_index]) {
                if (_vehicle->autoRemove) {
                    _vehicle->removeFromParentAndCleanup(true);
                    _vehicle = NULL;
                    m_vehicleLane1.erase(m_vehicleLane1.begin()+_index);
                }
            }
        }
    }
    
    _remove_list.clear();
    i = 0;
    
    // Update Vehicle Moving
    for (vector<Vehicle*>::iterator it=m_vehicleLane2.begin(); it!=m_vehicleLane2.end(); ++it) {
//    for (vector<Vehicle*>::reverse_iterator it=m_vehicleLane2.rbegin(); it!=m_vehicleLane2.rend(); ++it) {
        if (Vehicle *_vehicle = (Vehicle*)(*it)) {
            if(!_vehicle->updateMoving(delta_)) {
                _remove_list.push_back(i);
//                _vehicle->removeFromParentAndCleanup(true);
//                _vehicle = NULL;
//                m_vehicleLane2.erase(m_vehicleLane2.begin()+i);
            }
        }
        i++;
    }
    
    // Remove those arrived vehicle
    for (vector<int>::reverse_iterator it=_remove_list.rbegin(); it!=_remove_list.rend(); ++it) {
        int _index = (int)(*it);
        if (_index >= 0) {
            if (Vehicle *_vehicle = (Vehicle*)m_vehicleLane2[_index]) {
                if (_vehicle->autoRemove) {
                    _vehicle->removeFromParentAndCleanup(true);
                    _vehicle = NULL;
                    m_vehicleLane2.erase(m_vehicleLane2.begin()+_index);
                }
            }
        }
    }

    _remove_list.clear();
    i = 0;
    for (vector<Visitor*>::iterator it=m_visitors.begin(); it!=m_visitors.end(); ++it) {
//    for (vector<Visitor*>::reverse_iterator it=m_visitors.rbegin(); it!=m_visitors.rend(); ++it) {
        if (Visitor *_visitor = (Visitor*)(*it)) {
            _visitor->setZOrder(int(_isoMap->getContentSize().height-_visitor->getPositionY()));
            if(!_visitor->updateMoving(delta_)) {
                _visitor->decisionMaking(_isoMap);
            }
            if (_visitor->pleaseReleaseMe) {
                _remove_list.push_back(i);
//                _visitor->removeFromParentAndCleanup(true);
//                _visitor = NULL;
//                m_visitors.erase(m_visitors.begin()+i);
            }
        }
        i++;
    }
    // Remove those gone home visitors
    for (vector<int>::reverse_iterator it=_remove_list.rbegin(); it!=_remove_list.rend(); ++it) {
        int _index = (int)(*it);
        if (_index >= 0) {
            if (Visitor *_visitor = (Visitor*)m_visitors[_index]) {
                _visitor->removeFromParentAndCleanup(true);
                _visitor = NULL;
                m_visitors.erase(m_visitors.begin()+_index);
            }
        }
    }
    
    
}

bool EnvManager::addVehicleLane1(const char *spine_name_) {
    
    IsoMapBuilding *_isoMap = (IsoMapBuilding*)preferentRootParent;
    if (!_isoMap) {
        CCLog_("EnvManager::addVehicleLane1 - isoMapBuilding is uninitialized!");
        return false;
    }
    
    Vehicle *_vehicle = Vehicle::createWithSpineName("bus", true, _isoMap);
    _vehicle->setVehicleSize(CCSize(441.0f, 424.0f), CCSize(423.0f, 428.0f));
    _vehicle->setVehicleSpeed(1.0f + ((arc4random()%150)/100));
    _vehicle->setPosition(_isoMap->getPositionByMapIndex(11552, g_gameContent->tileSize, g_gameContent->tileShift));
    _vehicle->addDistinationPosition(ccp(2948.0,2208.0));
    _vehicle->setVehicleStopAt(1);
    _vehicle->addDistinationPosition(_isoMap->getPositionByMapIndex(2432, g_gameContent->tileSize, g_gameContent->tileShift));
    m_vehicleLane1.push_back(_vehicle);
    _isoMap->addChild(_vehicle, _isoMap->getContentSize().height);
    
    return true;
    
}

bool EnvManager::addVehicleLane2(const char *spine_name_) {
    
    IsoMapBuilding *_isoMap = (IsoMapBuilding*)preferentRootParent;
    if (!_isoMap) {
        CCLog_("EnvManager::addVehicleLane2 - isoMapBuilding is uninitialized!");
        return false;
    }
    
    Vehicle *_vehicle = Vehicle::createWithSpineName("bus", true, _isoMap);
    _vehicle->setVehicleSize(CCSize(441.0f, 424.0f), CCSize(423.0f, 428.0f));
    _vehicle->setVehicleSpeed(1.0f + ((arc4random ()%150)/100));
    _vehicle->setPosition(_isoMap->getPositionByMapIndex(2429, g_gameContent->tileSize, g_gameContent->tileShift));
    _vehicle->addDistinationPosition(_isoMap->getPositionByMapIndex(11308, g_gameContent->tileSize, g_gameContent->tileShift));
    m_vehicleLane2.push_back(_vehicle);
    _isoMap->addChild(_vehicle, _isoMap->getContentSize().height+1);
    
    return true;
    
}

