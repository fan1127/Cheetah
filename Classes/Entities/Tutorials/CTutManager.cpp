//
//  CTutManager.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "CTutManager.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"

#include "../../AppMacros.h"
extern TargetDevice targetDevice;
extern float sizeMultiplier;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#pragma mark - TutManager

bool CTutManager::init() {
    
    if (!CCLayer::init()) {
        return false;
    }
    
    cleanAllTutUnits();
    m_tutUnitToBeRelease = NULL;
    
    scheduleUpdate();
    
    return true;
    
}

CTutManager::~CTutManager() {
    
    cleanAllTutUnits();
    
}

void CTutManager::update(float delta_) {
    
    if (m_tutUnitToBeRelease) {
        m_tutUnitToBeRelease->release();
        m_tutUnitToBeRelease->removeFromParent();
        m_tutUnitToBeRelease = NULL;
    }
    
    if(m_tutUnits.empty())
        return;
    
    CTutUnit *_tutUnit = m_tutUnits[0];
    
    if (!_tutUnit->isActive() && _tutUnit->trig())
        addChild(_tutUnit);
    
    _tutUnit->update(delta_);
    
    if (_tutUnit->isFinish()) {
        m_tutUnitToBeRelease = _tutUnit;
        m_tutUnits.erase(m_tutUnits.begin());
    }
    
}

void CTutManager::addTut(CTutUnit *tutUnit_) {
    
    if (!tutUnit_) return;
    
    for (vector<CTutUnit*>::iterator it = m_tutUnits.begin(); it != m_tutUnits.end(); ++it) {
        if (typeid(*(*it)) == typeid(*tutUnit_)) {
            return;
        }
    }
    
    tutUnit_->retain();
    m_tutUnits.push_back(tutUnit_);
    
    CCLog_("tut added: %s", tutUnit_->getKey().c_str());
}

void CTutManager::addTutToFront(CTutUnit *tutUnit_) {
    
    if (!tutUnit_) return;
    
    for (vector<CTutUnit*>::iterator it = m_tutUnits.begin(); it != m_tutUnits.end(); ++it) {
        if (typeid(*(*it)) == typeid(*tutUnit_)) {
            return;
        }
    }
    
    tutUnit_->retain();
    m_tutUnits.insert(m_tutUnits.begin(), tutUnit_);
    
}


CTutUnit* CTutManager::getCurrentTutUnit() {
    
    if (m_tutUnits.size() > 0) {
        return m_tutUnits[0];
    }
    
    return NULL;
    
}

void CTutManager::cleanAllTutUnits() {
    
    for (vector<CTutUnit*>::reverse_iterator it=m_tutUnits.rbegin(); it!=m_tutUnits.rend(); ++it) {
        if (CTutUnit* _tutUnit = (CTutUnit*)(*it)) {
            _tutUnit->release();
        }
    }
    m_tutUnits.clear();
    
}