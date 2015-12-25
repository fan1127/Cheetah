//
//  IsoMapMatch.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/9/56 BE.
//
//

#include "IsoMapMatch.h"
USING_NS_CC;

#include "../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "PlayerState.h"
extern PlayerState* g_opponentState;

bool IsoMapMatch::init() {
    
    if (!IsoMap::init()) {
        return false;
    }
    
    if (!g_opponentState) {
        CCLog_("(IsoMapMatch::init) Error: opponent is uninitialized!");
        return false;
    }
    
    if (!g_opponentState->didParseData()) {
        CCLog_("(IsoMapMatch::init) Error: opponent is empty!");
        return false;
    }
    
    setTouchEnabled(true);
    
    entityAction = EntityAction::create();
    entityAction->viewMode = IMV_VIEWONLY;
    addChild(entityAction, int(getContentSize().height)+3);
    
    m_drag_begin = CCPointZero;
    m_ui_is_closing = false;
    
    composeMap(g_opponentState, (IsoMapView)entityAction->viewMode, this);
    
    scheduleUpdate();
    
    return true;
    
}

void IsoMapMatch::closeAllUIs() {
    
    if (mapToScreenUI != NULL && mapToScreenUI->getChildrenCount() > 0) {
        mapToScreenUI->closeAllUIs();
    }
    
    if (entityAction != NULL) {
        entityAction->finish();
    }
    
}

#pragma mark - Touch Callback Functions

void IsoMapMatch::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent) {

    if (pTouches->count() != 1) return;
    
    m_hide_entity_action = true;
    m_hide_entity_dialog = true;
    
}

void IsoMapMatch::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent) {
    
    if (pTouches->count() != 1) return;
    
    m_hide_entity_action = false;
    m_hide_entity_dialog = false;
    
}

void IsoMapMatch::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent) {
    
    _touchEneded(pTouches, pEvent);
    
}

void IsoMapMatch::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent) {
    
    _touchEneded(pTouches, pEvent);
    
}

void IsoMapMatch::_touchEneded(CCSet *pTouches, CCEvent *pEvent) {
    
    if (pTouches->count() != 1) return;
    
    CCPoint location = ((CCTouch*)(*pTouches->begin()))->getLocation();
    location = this->convertToNodeSpace(location);
    
    CCPoint coord = IsoMap::IsoToCoord(location,
                                       g_gameContent->tileSize,
                                       g_gameContent->tileShift);
    int t_tileIndex = int(coord.y)*ISO_TILE_COL+int(coord.x);
    
    if (m_hide_entity_action) {
        Building *building = (Building*)tilesMap[t_tileIndex];
        if (building && typeid(*building) == typeid(Building) &&
            ((building->pbVo.status != BUILDING_STATUS_CONSTRUCT &&
              building->pbVo.status != BUILDING_STATUS_COMPLETE &&
              building->pbVo.status != BUILDING_STATUS_OPEN))) {
                if (!entityAction->start(building)) {
                    entityAction->finish(building);
                }
                m_hide_entity_action = false;
        }
    }
    
    if (m_hide_entity_action) {
        if (entityAction) {
            entityAction->finish();
        }
    }
    
    if (m_hide_entity_dialog) {
        if (mapToScreenUI) {
            mapToScreenUI->closeAllUIs();
        }
    }
    
}

#pragma mark - CCNikScrollViewDelegate functions
void IsoMapMatch::scrollViewDidScroll(CCNikScrollView *view) {
    
    float _dist = m_drag_begin.getDistance(view->getContentOffset());
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    if (_dist > _screenSize.height*0.2f && !m_ui_is_closing) {
        m_ui_is_closing = true;
        closeAllUIs();
    }
    
}

void IsoMapMatch::scrollViewWillBeginDragging(CCNikScrollView* view) {
    
    m_drag_begin = view->getContentOffset();
    m_ui_is_closing = false;
    
}