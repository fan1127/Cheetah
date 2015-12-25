//
//  CTutManager.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__CTutManager__
#define __Cheetah__CTutManager__

#include "CTutUnit.h"

class CTutManager : public cocos2d::CCLayer {
    
    std::vector<CTutUnit*> m_tutUnits;
    CTutUnit* m_tutUnitToBeRelease;
    
public:
    
    CREATE_FUNC(CTutManager);
    virtual ~CTutManager();
    
    virtual bool init();
    virtual void update(float delta_);
    
    void addTut(CTutUnit* tutUnit_);
    void addTutToFront(CTutUnit* tutUnit_);
    CTutUnit* getCurrentTutUnit();
    std::vector<CTutUnit*> getTutUnits() { return m_tutUnits; };
    void cleanAllTutUnits();
    
};

#endif /* defined(__Cheetah__TutManager__) */
