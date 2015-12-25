//
//  CTutManualControl.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "CTutManualControl.h"
#include "CTutUnit.h"

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

extern float sizeMultiplier;

void CTutManualControl::perform(void* tutUnit_) {
    
    CTutUnit* _tutUnit = (CTutUnit*)tutUnit_;
    if (!_tutUnit) return;
    
    active = true;
    
    tutUnit = tutUnit_;
    
    _tutUnit->removeAllUIs();
    _tutUnit->removeSpotlightBG();
}

CTutManualControl::~CTutManualControl() {
    
}

void CTutManualControl::update(float delta_) {
    
    if (setToInactive) {
        active = false;
    }
    
    if (!active) {
        m_isRunningUpdate = false;
        return;
    }
    
    if (!m_isRunningUpdate) m_isRunningUpdate = true;
    
    //------
    //everything else should run here
    //------
}

void CTutManualControl::setToInActiveNow()
{
    active = false;
    m_isRunningUpdate = false;
}

bool CTutManualControl::isRunningUpdate()
{
    return m_isRunningUpdate;
}
