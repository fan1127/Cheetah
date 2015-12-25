//
//  IsoMapBuilding.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/30/56 BE.
//
//

#include "IsoMapBuilding.h"

USING_NS_CC;
using namespace std;
using namespace cocos2d::extension;

#include <spine/Json.h>

#include "Building.h"

#include "../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "PlayerState.h"
extern PlayerState *g_playerState;

#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../Services/BackendService.h"
#include "../Services/APIs/APITrackUpdate.h"

#include "../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../Helpers/CCStringHelper.h"
#include "../Settings.h"
#include "../Helpers/CustomEventsHelper.h"
#include "../Helpers/PushNotifications/PushNotifications.h"

extern float retinaMultiplier;
extern float sizeMultiplier;
extern unsigned int currentTime;

bool IsoMapBuilding::init() {
    
    if (!IsoMap::init()) {
        return false;
    }
    
    mapToScreenUI           = NULL;
    
    m_grabBuilding         = NULL;
    m_grabBuildingEnabled   = true;
    m_grabDidStartCount    = false;
    m_grabCounting         = 0.0f;
    m_grabTrack            = NULL;
    m_grabTrackEnabled     = true;
    m_grabObstacle         = NULL;
    m_decorationHolder      = NULL;
    m_grabDecorationEnabled = true;
    
    m_track_changed         = false;
    m_pending_save_call     = false;
    
    placeHolder             = NULL;
    
    hudPanel                = NULL;
    shopPanel               = NULL;
    merchant                = NULL;
    
    // collectableFoods & collectableCoins
    // are intialized in the parent view, (SceneVille)
    collectableFoods        = NULL;
    collectableCoins        = NULL;
    collectableCoins        = NULL;
    
    m_cancelBuildingPlacementEnabled = true;
    
    revokeBuilding = CCSkeletonAnimation::createWithFile("common/ui/shift_building.json", "common/ui/shift_building.atlas");
    revokeBuilding->timeScale = 4.0f;
    revokeBuilding->setScale(retinaMultiplier);
    revokeBuilding->setVisible(false);
    addChild(revokeBuilding, int(getContentSize().height)+1);

    entityAction = EntityAction::create();
    addChild(entityAction, int(getContentSize().height)+3);
    
    composeMap(g_playerState, IMV_PLAYABLE, this);
    plotChestGems(true);
    
#ifdef IMPLEMENT_LOCK_ZONE
    m_shadowGrid->setup(g_playerState->player.racing.level);
#endif
    
    if (isTrackComplete()) {
//        autoAddStartTrack();
    }
    
    envManager = EnvManager::createWithParent(this);
    envManager->scheduleUpdate();
    addChild(envManager);

    m_makeFirstTouch = !CustomEventsHelper::Instance().isTutorialFinished();
//    g_sceneManager->phatTouches->enable();
    
    setTouchEnabled(true);
    setTouchMode(kCCTouchesAllAtOnce);
    
    // update this in scene's update function
    // so we don't have to call scheduleUpdate(); any more!

    return true;
    
}

//IsoMapBuilding::~IsoMapBuilding() {
    
//    entityAction->release();
//    entityAction = NULL;
    
//}

void IsoMapBuilding::update(float delta) {

    if (placeHolder) {
        placeHolder->verifyMap(this);
        FadeBuildings(delta);
    } else {
        UnfadeBuildings(delta);
    }
    
    switch ((int)m_touch_locations.size()) {
        case 0: {
            m_grabBuilding = NULL;
            m_grabObstacle = NULL;
            m_grabTrack = NULL;
            m_decorationHolder = NULL;
            m_grabDidStartCount = false;
            m_grabCounting = 0.0f;
            revokeBuilding->setVisible(false);
        }break;
        case 1: {
            
            if (m_grabCounting >= kIsoMapBuildingGrabbingCountMax)
                return;
            
            CCPoint location = m_touch_locations[0];
            CCPoint coord = IsoMap::IsoToCoord(location, g_gameContent->tileSize, g_gameContent->tileShift);
            int t_tileIndex = int(coord.y)*ISO_TILE_COL+int(coord.x);
            int t_trackIndex = getTrackIndexByPosition(location, g_gameContent->tileSize, g_gameContent->tileShift);
            
            m_grabCounting += delta;
            
            PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;
            if (!_mapPlayerState) return;
            
            switch (m_touch_state) {
                    
                case 1: { //TouchBegan
                    
                    m_hide_entity_action = true;
                    m_hide_entity_dialog = true;
                    
                    if (!m_grabBuilding && !placeHolder) {
                        
                        Building *building = (Building *)tilesMap[t_tileIndex];
                        if (building &&
                            typeid(*building) == typeid(Building) &&
                            building->getTag() != 101) {
                            
                            if (building->spine_pet_running) {
                                return;
                            }
                            
                            if (m_grabBuildingEnabled &&
                                (!m_grabDidStartCount) &&
                                (m_grabCounting >= 0.2f) &&
                                (tilesLegal[t_tileIndex] >= 0) &&
                                (tilesLegal[t_tileIndex] <= _mapPlayerState->player.racing.level) &&
                                building->isMovable()) {
                                
                                m_grabDidStartCount = true;
                                revokeBuilding->setPosition(building->getPosition()+CCPoint(0.0f, g_gameContent->tileSize.height));
                                revokeBuilding->setVisible(true);
                                revokeBuilding->setAnimation("shift", false);
                            }
                            
                            if (building->pbVo.status == BUILDING_STATUS_COMPLETE) {
                                g_gameAudio->playEffect(AUDIO_FINISH_BUILDING);
                                building->openState();
                                return;
                            }else {
                                
                                if (m_grabBuildingEnabled &&
                                    (m_grabCounting >= 0.2f) &&
                                    (tilesLegal[t_tileIndex] >= 0) &&
                                    (tilesLegal[t_tileIndex] <= _mapPlayerState->player.racing.level) &&
                                    (building->isMovable()) &&
                                    revokeBuilding->states[0]->time >= revokeBuilding->states[0]->animation->duration) {
                                    
                                    m_grabDidStartCount = false;
                                    revokeBuilding->setVisible(false);
                                    addBuildingWithPlaceHolder(building);
                                    m_grabBuilding = building;
                                    entityAction->finish();
                                    return;
                                }
                                
                            }
                        }
                        
                    }
                    
                    
                    if (!m_grabBuilding && !m_decorationHolder && !placeHolder && m_grabDecorationEnabled) {
                        
                        Decoration *_decoration = (Decoration*)tilesMap[t_tileIndex];
                        if (_decoration && typeid(*_decoration) == typeid(Decoration)) {
                            
                            if ((!m_grabDidStartCount) &&
                                (m_grabCounting >= 0.2f) &&
                                (tilesLegal[t_tileIndex] >= 0) &&
                                (tilesLegal[t_tileIndex] <= _mapPlayerState->player.racing.level) &&
#ifndef EDIT_MODE
                                (_decoration->dVo.movable))
#else
                                (_decoration->dVo.movable || DECORATION_MOVABLE))
#endif
                            {
                                m_grabDidStartCount = true;
                                
                                revokeBuilding->setPosition(_decoration->getPosition()+CCPoint(0.0f, g_gameContent->tileSize.height));
                                revokeBuilding->setVisible(true);
                                revokeBuilding->setAnimation("shift", false);
                            }
                            
                            if ((m_grabCounting >= 0.2f) &&
                                (tilesLegal[t_tileIndex] >= 0) &&
                                (tilesLegal[t_tileIndex] <= _mapPlayerState->player.racing.level) &&
#ifndef EDIT_MODE
                                (_decoration->dVo.movable))
#else
                                (_decoration->dVo.movable || DECORATION_MOVABLE))
#endif
                            {
                                if (revokeBuilding->states[0]->time >= revokeBuilding->states[0]->animation->duration) {
                                    m_grabDidStartCount = false;
                                    revokeBuilding->setVisible(false);
                                    addDecorationOnPlaceHolder(_decoration);
                                    m_decorationHolder = _decoration;
                                    entityAction->finish();
                                    return;
                                }
                            }
                        }
                        
                    }
                    
                    if (!m_grabBuilding && !m_decorationHolder && !m_grabObstacle && !m_grabTrack && !placeHolder && m_grabTrackEnabled) {
                        Track* track = getTrackAtIndex(t_trackIndex);
                        if (track && m_grabCounting >= 0.2f) {
                            addTrackWithPlaceHolder(track);
                            return;
                        }
                    }
                    
                    if (merchant &&
                        ((Merchant*)merchant)->isTouched(t_tileIndex) &&
                        ((Merchant*)merchant)->isHere()) {
                        
                        ((Merchant*)merchant)->onBubbleTouched(((Merchant*)merchant)->bubble, merchant);
                        return;
                    }
                    
                }break;
                default: {
                    
                }break;
            }

        }break;
        default:
            break;
    }

    
}

void IsoMapBuilding::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent) {

    if (pTouches->count() != 1) return;
    
    if (m_makeFirstTouch)
    {
        if (CustomEventsHelper::Instance().isTutorialFinished())
        {
            m_makeFirstTouch = false;
            PushNotifications::Instance().notify(PushNotifications::FirstTimeInVille);
        }
    }
    
    CCPoint location = ((CCTouch*)(*pTouches->begin()))->getLocation();
    location = this->convertToNodeSpace(location);
    
    m_touch_locations.clear();
    m_touch_locations.push_back(location);

    m_touch_state = 1;
    
}

void IsoMapBuilding::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent) {

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

        CCTouch *touch = (CCTouch*)pTouches->anyObject();

        float distance2 = touch->getStartLocationInView().getDistanceSq(touch->getLocationInView());
        CCLog_("TouchableItem::ccTouchesEnded %f", sqrtf(distance2));

        if (distance2 < ANDROID_TOUCH_MOVE_THRESHOLD*ANDROID_TOUCH_MOVE_THRESHOLD)
        {
            return;
        }
#endif

    m_hide_entity_action = false;
    m_hide_entity_dialog = false;
    m_grabBuilding = NULL;
    m_grabObstacle = NULL;
    m_decorationHolder = NULL;
    m_grabTrack = NULL;
    m_grabDidStartCount = false;
    m_grabCounting = kIsoMapBuildingGrabbingCountMax;
    revokeBuilding->setVisible(false);
}

void IsoMapBuilding::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
    _touchEnded(pTouches, pEvent);
}

void IsoMapBuilding::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{
    _touchEnded(pTouches, pEvent);
}

void IsoMapBuilding::_touchEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent) {
    
    if ((int)m_touch_locations.size() <= 0)
        return;
    
    CCPoint location = m_touch_locations[0];
    CCPoint coord = IsoMap::IsoToCoord(location, g_gameContent->tileSize, g_gameContent->tileShift);
    int t_tileIndex = int(coord.y)*ISO_TILE_COL+int(coord.x);

    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;
    if (!_mapPlayerState) return;
    
    m_touch_locations.clear();
    m_touch_state = 0;
    
    bool _did_start_entity_action = false;
    if (!placeHolder) {
        if (!m_grabBuilding) {
            
            Building *building = (Building*)tilesMap[t_tileIndex];
            if (building &&
                typeid(*building) == typeid(Building) &&
                m_grabCounting < 0.2f) {
                
                m_grabDidStartCount = false;
                revokeBuilding->setVisible(false);
                
                if ((tilesLegal[t_tileIndex] >= 0) &&
                    (tilesLegal[t_tileIndex] <= _mapPlayerState->player.racing.level)) {
                    
                    if (building->pbVo.status != BUILDING_STATUS_COMPLETE &&
                        building->pbVo.status != BUILDING_STATUS_OPEN) {
                        
                        if (!entityAction->start(building)) {
                            entityAction->finish(building);
                        }
                        
                        m_hide_entity_action = false;
                        _did_start_entity_action = true;
                        
                    }
                    
                } else {
                    
                    if (building->pbVo.status != BUILDING_STATUS_CONSTRUCT &&
                        building->pbVo.status != BUILDING_STATUS_COMPLETE &&
                        building->pbVo.status != BUILDING_STATUS_OPEN) {
                        
                        if (!entityAction->start(building)) {
                            entityAction->finish(building);
                        }
                        m_hide_entity_action = false;
                        _did_start_entity_action = true;
                        
                    }
                    
                }
                
            }
            
        }
        
        if (!m_grabBuilding && !m_decorationHolder && m_grabDecorationEnabled) { // && !g_gameTutorial->tutorialMode
            
            Decoration *decoration = (Decoration*)tilesMap[t_tileIndex];
            if (decoration &&
                typeid(*decoration) == typeid(Decoration) &&
                decoration->pdVo.status == kDecorationStatusUsed) {
                
                if ((m_grabCounting < 0.2f) &&
                    (tilesLegal[t_tileIndex] >= 0) &&
                    (tilesLegal[t_tileIndex] <= _mapPlayerState->player.racing.level)) {
                    
                    m_grabDidStartCount = false;
                    revokeBuilding->setVisible(false);
                    
                    if (!entityAction->start(decoration)) {
                        entityAction->finish(decoration);
                    }
                    
                    m_hide_entity_action = false;
                    _did_start_entity_action = true;
                    
                }
                
            }
            
        }
        
    }
    
    m_grabBuilding = NULL;
    m_grabObstacle = NULL;
    m_grabTrack = NULL;
    m_grabDidStartCount = false;
    
    if (m_hide_entity_action) {
        if (placeHolder) {
            if (placeHolder->life > 0 &&
                placeHolder->state > 0 &&
                placeHolder->type() != PHT_NEW_BUILDING  &&
                placeHolder->type() != PHT_NEW_DECORATION
                )
            {
                //new buildings shouldn't be removed from placeholder when player taps the screen
                placingStuffFromPlaceHolder(false, BuildingBuildTypeNormal);
            }
        }
        if (entityAction) {
            entityAction->finish();
        }
    }
    
    if (m_hide_entity_dialog && mapToScreenUI) {
        mapToScreenUI->closeAllUIs();
    }
    
    if (placeHolder && entityAction && _did_start_entity_action) {
        if (placingStuffFromPlaceHolder()) {
            
        }
    }
    
}


void IsoMapBuilding::addDroppedItemToBarn() {
    
    if (PlayerState::getInstance()->itemsToAddToBarn.size() == 0) return;

    Building *_itemBarn = PlayerState::getInstance()->getBuilding(kBuildingItem, 0);
    if (_itemBarn) {
        _itemBarn->addItemToBarn(PlayerState::getInstance()->itemsToAddToBarn);
        focuzOnBuilding(_itemBarn->bVo.type.c_str(), 0);
        PlayerState::getInstance()->itemsToAddToBarn.clear();
    }
    
}

void IsoMapBuilding::FadeBuildings(float delta)
{
    PlayerState* playerState = (PlayerState*)mapPlayerState;
    for(auto iter = playerState->Buildings.begin(), iterEnd = playerState->Buildings.end(); iter != iterEnd; ++iter)
    {
        Building* building = *iter;
        CCRect bounds = building->spineBuilding->boundingBox();
        CCPoint pos = building->getPosition();
        if(IsOverlappedWithTrack(bounds, pos))
            FadeOutBuilding(building, delta, 128);
        else
            FadeInBuilding(building, delta, 255);
    }
}

bool IsoMapBuilding::IsOverlappedWithTrack(CCRect bounds, CCPoint pos) const
{
    PlayerState* playerState = (PlayerState*)mapPlayerState;
    for(auto iter = playerState->Tracks.begin(), iterEnd = playerState->Tracks.end(); iter != iterEnd; ++iter)
    {
        Track* track = *iter;
        CCRect trackBounds = track->getTrackBoundingBox();
        CCPoint trackPos = track->getPosition();

        if(trackPos.y < pos.y )
            continue;

        bounds.setRect(pos.x, pos.y, bounds.size.width, bounds.size.height);
        trackBounds.setRect(trackPos.x, trackPos.y, trackBounds.size.width, trackBounds.size.height);

        if(bounds.intersectsRect(trackBounds))
            return true;
    }
    return false;
}

void IsoMapBuilding::UnfadeBuildings(float delta)
{
    PlayerState* playerState = (PlayerState*)mapPlayerState;
    for(auto iter = playerState->Buildings.begin(), iterEnd = playerState->Buildings.end(); iter != iterEnd; ++iter)
    {
        Building* building = *iter;
        FadeInBuilding(building, delta, 255);
    }
}

void IsoMapBuilding::FadeInBuilding(Building* building, float delta, GLubyte maxOpacity)
{
    GLubyte opac = building->spineBuilding->getOpacity();
    if(opac >= maxOpacity)
        return;
    if(delta >= 1.f)
        building->spineBuilding->setOpacity(GLubyte(maxOpacity));
    else
    {
        int toAdd = static_cast<int>(255 * delta);
        if((opac + toAdd) > maxOpacity)
            opac = maxOpacity;
        else
            opac += toAdd;
        building->spineBuilding->setOpacity(GLubyte(opac));
    }
}

void IsoMapBuilding::FadeOutBuilding(Building* building, float delta, GLubyte minOpacity)
{
    GLubyte opac = building->spineBuilding->getOpacity();
    if(opac <= minOpacity)
        return;
    if(delta >= 1.f)
        building->spineBuilding->setOpacity(GLubyte(minOpacity));
    else
    {
        int toSub = static_cast<int>(255 * delta);
        if((opac - toSub) < minOpacity)
            opac = minOpacity;
        else
            opac -= toSub;
        building->spineBuilding->setOpacity(GLubyte(opac));
    }
}

void IsoMapBuilding::saveInBackend(float delta) { // this function get called every given time (eg 15 secs)
    
    if (PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState) {
        
        /////////////////////////////////////////////////////////////////////////////////////////
        // m_track_changed watches racing-track, whether it has been moved, removed or added
        // while _mapPlayerState->didMovingBuilding() watches buildings
        // if any of these changed g_gameBackend->track_update() would be called
        // g_gameBackend->track_update() send all track and changed building to server, in order to save the player state
        if ((m_track_changed || _mapPlayerState->didMoveBuilding() || _mapPlayerState->didMoveDecoration())) {
            if (!isTrackComplete()) {
                if (!((SceneVille*)g_currentScene)->tutMgr->getCurrentTutUnit()) {
                    g_sceneManager->notification->addMessage(MSGN_TYPE_WARNING, g_gameContent->Word["complete_track_required"].c_str());
                }
            }else {
                if (placeHolder) {
                    m_pending_save_call = true;
                }else {
                    m_track_changed = false;
                    BackendService::getInstance()->send(APITrackUpdate::create(NULL, NULL));
                    m_pending_save_call = false;
                }
            }
        }
        
    }

}

////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
SearchPath IsoMapBuilding::_searchPath1(CCPoint coord, int count, int no) {
    
    int k = int(coord.y*(ISO_TILE_COL/2)+coord.x);
    int t = IsoMap::tilesTrack[k];
    
    count ++;
    int n = getNTrack();//IsoMap::Tracks.size();
    if (count < n+1) {
        
        switch (no) {
            case 1:
                switch (t) {
                    case 1: // straigh /
                        return _searchPath1(CCPoint(coord.x+1, coord.y), count, t);
                    case 2: // curve up ^
                        return _searchPath1(CCPoint(coord.x, coord.y-1), count, t);
                    case 4: // curve >
                        return _searchPath2(CCPoint(coord.x, coord.y+1), count, t);
                    case 3:
                    case 5:
                    case 6:
                        count = n;
                        break;
                }
                break;
            case 2:
                switch (t) {
                    case 3: /* straigh \ */
                        return _searchPath1(CCPoint(coord.x, coord.y-1), count, t);
                    case 4: // curve >
                        return _searchPath2(CCPoint(coord.x, coord.y+1), count, t);
                    case 6: // curve V
                        return _searchPath1(CCPoint(coord.x+1, coord.y), count, t);
                        
                    case 2: // curve up ^
                        if (count == 1) {
                            return _searchPath1(CCPoint(coord.x, coord.y-1), count, t);
                        }
                    case 1:
                    case 5:
                        count = n;
                        break;
                }
                break;
            case 3:
                switch (t) {
                    case 3: /* straigh \ */
                        return _searchPath1(CCPoint(coord.x, coord.y-1), count, t);
                    case 4: // curve >
                        return _searchPath2(CCPoint(coord.x, coord.y+1), count, t);
                    case 6: // curve V
                        return _searchPath1(CCPoint(coord.x+1, coord.y), count, t);
                    case 1:
                    case 2:
                    case 5:
                        count = n;
                        break;
                }
                break;
            case 4:
                switch (t) {
                    case 1: // straigh /
                        return _searchPath1(CCPoint(coord.x+1, coord.y), count, t);
                    case 5: // curve <
                        return _searchPath1(CCPoint(coord.x+1, coord.y), count, t);
                    case 6: // curve V
                        return _searchPath1(CCPoint(coord.x+1, coord.y), count, t);
                    case 4: // curve >
                        if (count == 1) {
                            return _searchPath2(CCPoint(coord.x, coord.y+1), count, t);
                        }
                    case 2:
                    case 3:
                        count = n;
                        break;
                }
                break;
            case 5:
                switch (t) {
                    case 1: // straigh /
                        return _searchPath1(CCPoint(coord.x+1, coord.y), count, t);
                    case 2: // curve up ^
                        return _searchPath1(CCPoint(coord.x, coord.y-1), count, t);
                    case 4: // curve >
                        return _searchPath2(CCPoint(coord.x, coord.y+1), count, t);
                    case 5: // curve <
                        if (count == 1) {
                            return _searchPath1(CCPoint(coord.x+1, coord.y), count, t);
                        }
                    case 3:
                    case 6:
                        count = n;
                        break;
                }
                break;
            case 6:
                switch (t) {
                    case 1: // straigh /
                        return _searchPath1(CCPoint(coord.x+1, coord.y), count, t);
                    case 2: // curve up ^
                        return _searchPath1(CCPoint(coord.x, coord.y-1), count, t);
                    case 4: // curve >
                        return _searchPath2(CCPoint(coord.x, coord.y+1), count, t);
                    case 6: // curve V
                        if (count == 1) {
                            return _searchPath1(CCPoint(coord.x+1, coord.y), count, t);
                        }
                    case 3:
                    case 5:
                        count = n;
                        break;
                }
                break;
        }
        
    }
    
    return SearchPath(coord, count, no);
    
}

SearchPath IsoMapBuilding::_searchPath2(CCPoint coord, int count, int no) {
    
    int k = int(coord.y*ISO_TILE_COL/2+coord.x);
    int t = IsoMap::tilesTrack[k];
    
    count ++;
    
    int n = getNTrack();//IsoMap::Tracks.size();
    if (count < n+1) {
        
        switch (no) {
            case 1:
                switch (t) {
                    case 1: // straigh /
                        return _searchPath2(CCPoint(coord.x-1, coord.y), count, t);
                    case 5: // curve <
                        return _searchPath1(CCPoint(coord.x, coord.y-1), count, t);
                    case 6: // curve V
                        return _searchPath2(CCPoint(coord.x, coord.y+1), count, t);
                    case 2:
                    case 3:
                    case 4:
                        count = n;
                        break;
                }
                break;
            case 2:
                switch (t) {
                    case 1: // straigh /
                        return _searchPath2(CCPoint(coord.x-1, coord.y), count, t);
                    case 5: // curve <
                        return _searchPath1(CCPoint(coord.x, coord.y-1), count, t);
                    case 6: // curve V
                        return _searchPath2(CCPoint(coord.x, coord.y+1), count, t);
                    case 2: // curve up ^
                        if (count == 1) {
                            return _searchPath2(CCPoint(coord.x-1, coord.y), count, t);
                        }
                    case 3:
                    case 4:
                        count = n;
                        break;
                        
                }
                break;
            case 3:
                switch (t) {
                    case 2: // curve up ^
                        return _searchPath2(CCPoint(coord.x-1, coord.y), count, t);
                    case 3: /* straigh \*/
                        return _searchPath2(CCPoint(coord.x, coord.y+1), count, t);
                    case 5: // curve <
                        return _searchPath1(CCPoint(coord.x, coord.y-1), count, t);
                    case 1:
                    case 4:
                    case 6:
                        count = n;
                        break;
                }
                break;
            case 4:
                switch (t) {
                    case 1: // straigh /
                        return _searchPath2(CCPoint(coord.x-1, coord.y), count, t);
                    case 5: // curve <
                        return _searchPath1(CCPoint(coord.x, coord.y-1), count, t);
                    case 6: // curve V
                        return _searchPath2(CCPoint(coord.x, coord.y+1), count, t);
                    case 4: // curve >
                        if (count == 1) {
                            return _searchPath2(CCPoint(coord.x-1, coord.y), count, t);
                        }
                    case 2:
                    case 3:
                        count = n;
                        break;
                }
                break;
            case 5:
                switch (t) {
                    case 3: /* straigh \*/
                        return _searchPath2(CCPoint(coord.x, coord.y+1), count, t);
                    case 4: // curve >
                        return _searchPath2(CCPoint(coord.x-1, coord.y), count, t);
                    case 6: // curve V
                        return _searchPath2(CCPoint(coord.x, coord.y+1), count, t);
                    case 5: // curve <
                        if (count == 1) {
                            return _searchPath1(CCPoint(coord.x, coord.y-1), count, t);
                        }
                    case 1:
                    case 2:
                        count = n;
                        break;
                }
                break;
            case 6:
                switch (t) {
                    case 1: // straigh /
                        return _searchPath2(CCPoint(coord.x-1, coord.y), count, t);
                    case 2: // curve up ^
                        return _searchPath2(CCPoint(coord.x-1, coord.y), count, t);
                    case 4: // curve >
                        return _searchPath2(CCPoint(coord.x-1, coord.y), count, t);
                    case 6: // curve V
                        if (count == 1) {
                            return _searchPath2(CCPoint(coord.x, coord.y+1), count, t);
                        }
                    case 3:
                    case 5:
                        count = n;
                        break;
                }
                break;
        }
    }
    
    return SearchPath(coord, count, no);
}

bool IsoMapBuilding::addBuildingWithPlaceHolder(GDBuildingVO vo) {
    
    return addBuildingWithPlaceHolder(vo, false);
    
}

bool IsoMapBuilding::addBuildingWithPlaceHolder(GDBuildingVO vo, bool validate_coin_) {
    
    if (validate_coin_ && g_playerState->player.balance.gold < vo.cost_coins) return false;
    
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    Building* building = Building::createWithBuildingVO(vo);
    
    if (!building) return false;
    building->retain();
    
    if (placeHolder != NULL && placingStuffFromPlaceHolder()) {
        placeHolder->removeFromParent();
        placeHolder = NULL;
    }
    
    placeHolder = PlaceHolder::createWithBuilding(building, hudPanel, PHT_NEW_BUILDING);
    placeHolder->setSender(this);
    placeHolder->onBegin = lockCamera;
    placeHolder->onDone = unlockCamera;
    placeHolder->parent = this;
    placeHolder->state = 1;
    placeHolder->setHolder(g_gameContent->tileSize, g_gameContent->tileShift);
    placeHolder->snapWithGrid(this->convertToNodeSpace(screenSize/2));
    this->addChild(placeHolder, int(this->getContentSize().height));
    
    m_keepShopOpen = shopPanel->isPanelOpen();
    shopPanel->closePanel();
    hudPanel->showAllMainMenuButtons(false);

    return true;

}

bool IsoMapBuilding::addBuildingWithPlaceHolder(Building* building) {
    
    if (!building) {
        return false;
    }
    
    IsoMap::purgeBuilding(building);
    
    if (placeHolder) {
        if (this->placingStuffFromPlaceHolder()) {
            placeHolder->removeFromParentAndCleanup(true);
            placeHolder = NULL;
        }
    }
    
    placeHolder = PlaceHolder::createWithBuilding(building, hudPanel, PHT_EDT_BUILDING);
    placeHolder->setSender(this);
    placeHolder->onBegin = lockCamera;
    placeHolder->onDone = unlockCamera;
    placeHolder->parent = this;
    placeHolder->setHolder(g_gameContent->tileSize, g_gameContent->tileShift);
    placeHolder->setMeAGoodPosition();
    this->addChild(placeHolder, int(this->getContentSize().height));
    
    if (shopPanel) {
        m_keepShopOpen = shopPanel->isPanelOpen();
    }
    
    hudPanel->showAllMainMenuButtons(false);
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    return true;
    
}

bool IsoMapBuilding::addTrackWithPlaceHolder(GDTrackVO vo) {
    
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    
    Track* track = Track::createWithTrackVO(vo, PlayerTrackVO());
    track->retain();
    
    setAsStartTrackIfLegal(track);
    
    if(!IsoMap::addTrack(track)) {
        track->release();
        delete track;
        track = NULL;
        return false;
    }
    hudPanel->updateHUD();
    
    IsoMap::purgeTrack(track);
    
    if (placeHolder) {
        if (this->placingStuffFromPlaceHolder()) {
            placeHolder->removeFromParentAndCleanup(true);
            placeHolder = NULL;
        }
    }
    
    if (vo.track_type == PT_OBJECT_OBSTACLE) {
        placeHolder = PlaceHolder::createWithTrack(track, hudPanel, PHT_NEW_OBSTACLE);
    }else { // PT_OBJECT_TRACK
        placeHolder = PlaceHolder::createWithTrack(track, hudPanel, PHT_NEW_TRACK);
    }
    placeHolder->setSender(this);
    placeHolder->onBegin = lockCamera;
    placeHolder->onDone = unlockCamera;
    placeHolder->parent = this;
    placeHolder->setHolder(g_gameContent->tileSize, g_gameContent->tileShift);
    placeHolder->snapWithQuadGrid(this->convertToNodeSpace(screenSize/2));
    if (vo.track_type == PT_OBJECT_OBSTACLE) {
        placeHolder->flip(placeHolder);
    }
    this->addChild(placeHolder, int(this->getContentSize().height));
    
    m_keepShopOpen = shopPanel->isPanelOpen();
    shopPanel->closePanel();
    hudPanel->showAllMainMenuButtons(false);
    
    hudPanel->updateHUDTrack(isTrackComplete());

    return true;
    
}

bool IsoMapBuilding::addTrackWithPlaceHolder(Track* track) {
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    return addTrackWithPlaceHolder(track, false);
    
}

bool IsoMapBuilding::addTrackWithPlaceHolder(Track* track, bool finishbtn) {

    if (placeHolder && placeHolder->canPlace())
    {
        return false;
    }

    if (placeHolder) {
        if (placingStuffFromPlaceHolder(true, BuildingBuildTypeNormal)) {
            placeHolder->removeFromParentAndCleanup(true);
            placeHolder = NULL;
        }
    }
    
    setAsStartTrackIfLegal(track);
    track->updateSprite();
    
    purgeTrack(track);
    
    if (track->tVo.track_type == PT_OBJECT_OBSTACLE) {
        placeHolder = PlaceHolder::createWithTrack(track, hudPanel, PHT_EDT_OBSTACLE, finishbtn);
    }else if (track->tVo.track_type == PT_OBJECT_TRACK) {
        placeHolder = PlaceHolder::createWithTrack(track, hudPanel, PHT_EDT_TRACK, finishbtn);
    }
    placeHolder->setSender(this);
    placeHolder->onBegin = lockCamera;
    placeHolder->onDone = unlockCamera;
    placeHolder->parent = this;
    placeHolder->setHolder(g_gameContent->tileSize, g_gameContent->tileShift);
    placeHolder->setPosition(getPositionByTrackIndex(track->ptVo.index, g_gameContent->tileSize, g_gameContent->tileShift));
    if (track->tVo.track_type == PT_OBJECT_OBSTACLE) {
        placeHolder->flip(placeHolder);
    }
    this->addChild(placeHolder, int(this->getContentSize().height));

    m_keepShopOpen = shopPanel->isPanelOpen();
    shopPanel->closePanel();
    hudPanel->showAllMainMenuButtons(false);
    
    m_grabTrack = track;
    
    return true;
    
}

bool IsoMapBuilding::addMoreTrackWithPlaceHolder(GDTrackVO vo, PlayerTrackVO pptvo) {
    
    CCPoint coord = IsoMap::IsoToCoord(placeHolder->getPosition(), g_gameContent->tileSize*2, g_gameContent->tileShift);
    SearchPath p1 = _searchPath1(coord, 0, pptvo.no);
    SearchPath p2 = _searchPath2(coord, 0, pptvo.no);
    int n = IsoMap::getNTrack();
    
    if (p1.count == p2.count && p1.count > 2 && p1.count == n) {
        return false;
    }
    
    if (p1.count <= p2.count) {
        placeHolder->moveBy(p1.coord-coord);
    }else {
        placeHolder->moveBy(p2.coord-coord);
    }
    
    if (placeHolder->isOnTrack()) {
        return false;
    }
    
    Track* track = Track::createWithTrackVO(vo, PlayerTrackVO());
    track->retain();
    
    setAsStartTrackIfLegal(track);
    
    if(!IsoMap::addTrack(track)) {
        CCLog_("(IsoMapBuilding::addTrackWithPlaceHolder) Warning: addTrack() failed!");
        return false;
    }
    hudPanel->updateHUD();
    hudPanel->updateHUDTrack(isTrackComplete());

    IsoMap::purgeTrack(track);
    
    track->ptVo.index = IsoMap::getTrackIndexByPosition(placeHolder->getPosition(), g_gameContent->tileSize, g_gameContent->tileShift);
    placeHolder->addMoreTrack(track, hudPanel);
    placeHolder->switchTrack(true);
    
    placeHolder->updateMenu(hudPanel, false);
    hudPanel->showAllMainMenuButtons(false);
    
    return true;
}

bool IsoMapBuilding::addDecorationOnPlaceHolder(GDDecorationVO vo_) {

    return addDecorationOnPlaceHolder(vo_, true);
    
}

bool IsoMapBuilding::addDecorationOnPlaceHolder(GDDecorationVO vo_, bool valid_coin_) {
    
    Decoration* _decoration = Decoration::createWithDecorationVO(vo_);
    if (!_decoration) {
        return false;
    }
    
    _decoration->preferenceRootParent = this;
    _decoration->createDecoration();
    _decoration->retain();
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    if (placeHolder) {
        this->placingStuffFromPlaceHolder();
        placeHolder->removeFromParentAndCleanup(true);
        placeHolder = NULL;
    }
    
    placeHolder = PlaceHolder::createWithDecoration(_decoration, hudPanel, PHT_NEW_DECORATION);
    placeHolder->setSender(this);
    placeHolder->onBegin = lockCamera;
    placeHolder->onDone = unlockCamera;
    placeHolder->parent = this;
    placeHolder->setHolder(g_gameContent->tileSize, g_gameContent->tileShift);
    placeHolder->snapWithGrid(this->convertToNodeSpace(_screenSize*0.5f));
    this->addChild(placeHolder, int(this->getContentSize().height));
    
    m_keepShopOpen = shopPanel->isPanelOpen();
    shopPanel->closePanel();
    hudPanel->showAllMainMenuButtons(false);
    
    return true;
    
}

bool IsoMapBuilding::addDecorationOnPlaceHolder(Decoration *decoration_) {
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    return addDecorationOnPlaceHolder(decoration_, false);
    
}

bool IsoMapBuilding::addDecorationOnPlaceHolder(Decoration* decoration_, bool finishbtn) {
    
    if (!decoration_) {
        CCLog_("(IsoMapBuilding::addDecorationOnPlaceHolder) Error: Given decoration is uninitialized!");
        return false;
    }
    
    purgeDecoration(decoration_);
    
    if (placeHolder) {
        this->placingStuffFromPlaceHolder();
        placeHolder->removeFromParentAndCleanup(true);
        placeHolder = NULL;
    }
    
    placeHolder = PlaceHolder::createWithDecoration(decoration_, hudPanel, PHT_EDT_DECORATION);
    placeHolder->setSender(this);
    placeHolder->onBegin = lockCamera;
    placeHolder->onDone = unlockCamera;
    placeHolder->parent = this;
    placeHolder->setHolder(g_gameContent->tileSize, g_gameContent->tileShift);
    placeHolder->setMeAGoodPosition();
    if (decoration_->pdVo.status == kDecorationStatusKept) {
        CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
        placeHolder->setPosition(this->convertToNodeSpace(_screenSize*0.5f));
    }
    this->addChild(placeHolder, int(this->getContentSize().height));
 
    m_keepShopOpen = shopPanel->isPanelOpen();
    shopPanel->closePanel();
    hudPanel->showAllMainMenuButtons(false);
    
    return true;
    
}

void IsoMapBuilding::plotChestGems(bool reset) {
    
    for (map<string, ChestGems*>::iterator it=g_playerState->ChestGemsDict.begin(); it!=g_playerState->ChestGemsDict.end(); ++it) {
        if (ChestGems* _chestgems = (ChestGems*)it->second) {
            if (_chestgems->cgVo.locate == "ville") {
                _chestgems->removeFromParentAndCleanup(false);
                _chestgems->setPosition(ccp(_chestgems->cgVo.posx, _chestgems->cgVo.posy));
                _chestgems->reload();
                addChild(_chestgems, this->getContentSize().height-_chestgems->cgVo.posy);
                _chestgems->showup(reset);
            }
        }
    }
    
}

void IsoMapBuilding::onGemsShouldBeating(void *sender_, std::string json_str) {
    
    PlayerState* _playerState = PlayerState::getInstance();
    
    //JsonAllocator allocator;
    Json *_root = Json_create(json_str.c_str());
    _playerState->parseUserBalance(_root);
    _playerState->parseAbilityData(_root);
    Json_dispose(_root);
    
    if (typeid(*g_currentScene) != typeid(SceneVille)) return;
    
    if (IsoMapBuilding* _this = (IsoMapBuilding*)sender_) {
        _this->hudPanel->resourceBars[HUDPanelBarGems]->addTargetAmount(_playerState->player.balance.gem);
        _this->hudPanel->resourceBars[HUDPanelBarGems]->processTargetAmount();
    }
    
}

void IsoMapBuilding::onCoinShouldBeating(void *sender_, std::string json_str) {
    
    PlayerState* _playerState = PlayerState::getInstance();
    
    //JsonAllocator allocator;
    Json *_root = Json_create(json_str.c_str());
    _playerState->parseUserBalance(_root);
    _playerState->parseAbilityData(_root);
    Json_dispose(_root);
    
    if (typeid(*g_currentScene) != typeid(SceneVille)) return;
    
    if (IsoMapBuilding* _this = (IsoMapBuilding*)sender_) {
        _this->hudPanel->resourceBars[HUDPanelBarCoins]->addTargetAmount(_playerState->player.balance.gold);
        _this->hudPanel->resourceBars[HUDPanelBarCoins]->processTargetAmount();
    }
    
}


bool IsoMapBuilding::hidingPlaceHolder() {
    
    if (!placeHolder) {
        return false;
    }
    
    placeHolder->hideButton();
    if (m_keepShopOpen) shopPanel->openPanel();
    
    return true;
    
}

// @june remove placeHolder & show Hudpanel
bool IsoMapBuilding::removePlaceHolder() {
    
    if (!placeHolder) {
        return false;
    }
    
    if (placeHolder->bgMenuSprite) {
        placeHolder->bgMenuSprite->removeFromParentAndCleanup(true);
        placeHolder->bgMenuSprite = NULL;
    }
    if (hudPanel) {
        hudPanel->menuPlaceHolderLayer->removeAllChildrenWithCleanup(true);
    }
    placeHolder->removeFromParentAndCleanup(true);
    placeHolder = NULL;
    m_grabBuilding = NULL;
    m_grabObstacle = NULL;
    m_grabTrack = NULL;
    
    hudPanel->showAllMainMenuButtons(true);
    
    if (m_pending_save_call) {
        saveInBackend(0.0f);
    }
    
    return true;

}

bool IsoMapBuilding::placingStuffFromPlaceHolder() {
    
    return placingStuffFromPlaceHolder(false, BuildingBuildTypeNormal);
    
}

bool IsoMapBuilding::placingStuffFromPlaceHolder(bool cancel, int type) {
    
    if (!placeHolder) {
        return false;
    }
    
    if (placeHolder->building) {
        if (!placeHolder->isActive() || cancel) {
            if (placeHolder->building->pbVo.buid != "") { //ignore if the building hasn't built
                placeHolder->revert();
                IsoMap::addBuilding(placeHolder->building, false);
            }else {
                placeHolder->building->removeFromParent();
            }
        }else {
            IsoMap::addBuilding(placeHolder->building, (BuildingBuildType)type, true);
            //open entityaction as soon as building is added
            if (placeHolder->building->pbVo.buid == "" &&
                (!BUILD_HABITAT_WITH_GEMS || placeHolder->building->bVo.building_type != BUILDING_TYPE_HABITAT)) {
                if (!entityAction->start(placeHolder->building)) {
                    entityAction->finish(placeHolder->building);
                }
            }
        }
        unlockCamera(this);
        placeHolder->building = NULL;
        removePlaceHolder();
        return true;
    }
    
    if (placeHolder->track) {
        if (cancel) placeHolder->revert();
        bool finish = true;
        if (placeHolder->bgMenuSprite->numberOfRunningActions() > 0) {
            return false;
        }
        if (placeHolder->canPlace()) {
            if(IsoMap::applyTrack(placeHolder->track)) {
                m_track_changed = true;
                if (placeHolder->didBtnPressed) {
                    GDTrackVO vo = g_gameContent->getTrackVO(placeHolder->track->ptVo.type);
                    PlayerTrackVO pptvo = placeHolder->track->ptVo;
                    placeHolder->track = NULL;
                    if (g_playerState->player.racing._track_points + vo.point_used <= g_playerState->player.racing._track_points_limit) {
                        if (vo.track_type == PT_OBJECT_TRACK) {
                            if(addMoreTrackWithPlaceHolder(vo, pptvo)) {
                                finish = false;
                            }
                        }
                    }
                }else {
                    placeHolder->track = NULL;
                }
            }
        }
        if (finish || cancel) {
            unlockCamera(this);
            placeHolder->track = NULL;
            hudPanel->updateHUDTrack(isTrackComplete());
            placeHolder->hideButton();
        }
        return true;
    }
    
    if (placeHolder->decoration) {
        if (!placeHolder->isActive()) {
            if (placeHolder->decoration->pdVo.duid != "") { //ignore if the decoration hasn't built
                placeHolder->revert();
                IsoMap::addDecoration(placeHolder->decoration, false, false, type);
            }
        }else {
            if (!cancel) {
                IsoMap::addDecoration(placeHolder->decoration, false, true, type);
            }else {
                placeHolder->revert();
                IsoMap::addDecoration(placeHolder->decoration, false, false, type);
            }
        }
        unlockCamera(this);
        placeHolder->decoration = NULL;
        removePlaceHolder();
        return true;
    }
    
    return false;
    
}

bool IsoMapBuilding::removingStuffInPlaceHolder() {
    
    if (!placeHolder) {
        CCLog_("(IsoMapBuilding::removingStuffInPlaceHolder) Warning: placeHolder is uninitialized!");
        return false;
    }

    if (placeHolder->track) {
        
        PlayerState* _playerState = (PlayerState*)mapPlayerState;
        if (_playerState->Tracks.size() == 0) return false;
        
        Track* anotherTrack = NULL;
        
        if (placeHolder->track->tVo.track_type == PT_OBJECT_TRACK) {
            placeHolder->revert();
            anotherTrack = IsoMap::getNearestTrack(getPositionByTrackIndex(placeHolder->track->ptVo.index,
                                                                    g_gameContent->tileSize,
                                                                    g_gameContent->tileShift),
                                            placeHolder->track->ptVo.no, -1);
        }
        
        IsoMap::removeTrack(placeHolder->track);
        placeHolder->track = NULL;
        
        if (anotherTrack) {
            placeHolder->removeFromParentAndCleanup(true);
            placeHolder = NULL;
            addTrackWithPlaceHolder(anotherTrack, true);
        }else {
            placeHolder->hideButton();
        }
        
        m_track_changed = true;
        
    }

    hudPanel->updateHUD();
    hudPanel->updateHUDTrack(isTrackComplete());
    
    return true;
    
}


void IsoMapBuilding::onFoodIconHitHUD(void *inst_, void *sender_) {
    
    if (typeid(*g_currentScene) != typeid(SceneVille)) return;
    
    if (IsoMapBuilding* _this = (IsoMapBuilding*)sender_) {
        if (CollectableItem *_item = (CollectableItem*)inst_) {
            _this->hudPanel->resourceBars[HUDPanelBarFoods]->addUp(_item->collectingAmount, g_playerState->getTotalFoodStorageCapacity(), AUDIO_FOOD_HIT);
        }
    }
    
}
        
void IsoMapBuilding::onCoinIconHitHUD(void *inst_, void *sender_) {
    
    if (typeid(*g_currentScene) != typeid(SceneVille)) return;
    
    if (IsoMapBuilding* _this = (IsoMapBuilding*)sender_) {
        if (CollectableItem *_item = (CollectableItem*)inst_) {
            _this->hudPanel->resourceBars[HUDPanelBarCoins]->addUp(_item->collectingAmount, g_playerState->getTotalCoinStorageCapacity(), AUDIO_COIN_HIT);
        }
    }
    
}

bool IsoMapBuilding::collectGems(ChestGems* chestgems_) {
    
    // collect coins
    if (collectableGems != NULL && chestgems_ != NULL) {
        
        //Gem Icon fly to HUD
        int _collect_amount = chestgems_->cgVo.gems;
        collectableGems->add(chestgems_->getParent()->convertToWorldSpace(chestgems_->getPosition()), getParent()->getScale(), _collect_amount);
        
        // Text Effect
        CREATE_CHAR_BUFFER(_buffer, 64);
        snprintf(_buffer, _buffer_size, "%d", _collect_amount);
        float totalAnimationTime = kCollectAnimTotalAnimationTime;
        float fadeTime = kCollectAnimFadeTime;
        CCLabelBMFont *_collectedAmountLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeVeryLarge, true);
        _collectedAmountLabel->setColor(kTextColorGem);
        _collectedAmountLabel->setPosition(ccp(0.0f, chestgems_->getContentSize().height*0.7f*retinaMultiplier));
        chestgems_->addChild(_collectedAmountLabel, 10);
        _collectedAmountLabel->
        runAction(CCSequence::create(CCSpawn::create(CCSequence::create(CCDelayTime::create(totalAnimationTime-fadeTime),
                                                                        CCFadeOut::create(fadeTime),
                                                                        NULL),
                                                     CCMoveBy::create(totalAnimationTime, ccp(0.0f, kCollectAnimDistanceMoved*retinaMultiplier))
                                                     ,
                                                     NULL)
                                     ,
                                     CCRemoveSelf::create(),
                                     NULL));
        
    }
    
    return false;
    
}
        
void IsoMapBuilding::onGemsIconHitHUD(void *inst_, void *sender_) {
    
    if (typeid(*g_currentScene) != typeid(SceneVille)) return;
    
    if (IsoMapBuilding* _this = (IsoMapBuilding*)sender_) {
        if (CollectableItem *_item = (CollectableItem*)inst_) {
            _this->hudPanel->resourceBars[HUDPanelBarGems]->addUp(_item->collectingAmount, AUDIO_FUSE_EVOLVE_2);
        }
    }
    
}

void IsoMapBuilding::onGemsCollected(void *sender, string json_str) {
    
    PlayerState* _playerState = PlayerState::getInstance();
    
    //JsonAllocator allocator;
    Json *_root = Json_create(json_str.c_str());
    _playerState->parseUserBalance(_root);
    string collected_from_guid = Json_getString(_root, "collected_from_guid", "");
    Json_dispose(_root);
    
    if (collected_from_guid == "")
        return;
    
    ChestGems* _chestgems = (ChestGems*)_playerState->ChestGemsDict[collected_from_guid];
    if (!_chestgems)
        return;
    
    _playerState->removeChestGems(_chestgems);
    _chestgems->openup();
  
}

bool IsoMapBuilding::updateMenu() {
    
    if (!placeHolder) {
    
        return false;
    }
    
    placeHolder->updateMenu(hudPanel, true);
    
    return true;
    
}

bool IsoMapBuilding::setStartOrChangeDirection() {
    
    if (!placeHolder) {
        return false;
    }
    
    if (!placeHolder->track) {
        return false;
    }
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;
    if (!_mapPlayerState) {
        CCLog_("(IsoMapBuilding::setStartOrChangeDirection) Warning: mapStatePlayer is uninitialized!");
        return false;
    }
    
    // if it's not a track start then set it
    if (!placeHolder->track->trackStart) {

        IsoMap::setAsStartTrack(placeHolder->track);

        // update HUD
        placeHolder->updateMenu(hudPanel, false);
        placeHolder->trackStart_recover = true;
        
    //but if it's already a track start, then reverse its direction
    }else {
    
        placeHolder->track->trackDirection = !placeHolder->track->trackDirection;
        placeHolder->track->updateSprite();
        
        _mapPlayerState->player.racing.direction = placeHolder->track->trackDirection;
        
    }
    
    return true;
    
}

void IsoMapBuilding::closeAllMapUIs() {
    
    if (mapToScreenUI != NULL) {
        if (mapToScreenUI->getChildrenCount() > 0) {
            mapToScreenUI->closeAllUIs();
        }
    }
    
    if (entityAction != NULL) {
        entityAction->finish();
    }
    
}

void IsoMapBuilding::lockCamera(void *sender_) {
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    if (!_scene || !_scene->scrollView) return;
    
    _scene->scrollView->setScrollEnabled(false);
    
}

void IsoMapBuilding::unlockCamera(void *sender) {
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    if (!_scene
        || typeid(*_scene) != typeid(SceneVille)
        || !_scene->scrollView) return;
    
    _scene->scrollView->setScrollEnabled(true);
    
    IsoMapBuilding *_isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
    if (!_isoMapBuilding ||
        !_isoMapBuilding->placeHolder ||
        !_isoMapBuilding->placeHolder->camLock) return;
    
    CCPoint _extra = CCPointZero;
    if (_isoMapBuilding->placeHolder->menuItemYes) {
        _extra = ccp(0, _isoMapBuilding->placeHolder->menuItemYes->getContentSize().height);
    }
    _scene->scrollView->zoomOnPosition(_isoMapBuilding->placeHolder->getPosition()
                                       - _scene->isoMap->getContentSize()/4.f
                                       + _extra, 1.0f, 1.0f, true);
    
}

void IsoMapBuilding::scrollViewDidScroll(CCNikScrollView *view) {
    
    float _dist = m_drag_begin.getDistance(view->getContentOffset());
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    if (_dist > _screenSize.height*0.2f && !m_ui_is_closing) {
        
        m_ui_is_closing = true;
        
        closeAllMapUIs();

    }
}

void IsoMapBuilding::scrollViewDidZoom(CCNikScrollView *view) {

}

void IsoMapBuilding::scrollViewWillBeginDragging(CCNikScrollView* view) {

//    if (shopPanel) {
//        if (shopPanel->getManuallyCloseShopPanelEnabled()) {
//            shopPanel->m_scrollView->setTouchDelegate(NULL);
//            shopPanel->closePanel();
//        }
//    }
    
    m_drag_begin = view->getContentOffset();
    m_ui_is_closing = false;
    
}

void IsoMapBuilding::scrollViewTouchBegan(CCNikScrollView *view) {
    
//    if (!shopPanel)
//        return;
//    
//    if (shopPanel->getManuallyCloseShopPanelEnabled()) {
//        shopPanel->m_scrollView->setTouchDelegate(NULL);
//        shopPanel->closePanel();
//    }
    
}

void IsoMapBuilding::scrollViewDidEndDragging(CCNikScrollView* view, bool decelerate)
{
    if (!decelerate) {
//        CCLog_("scrollViewDidEndDragging");
    }
}

void IsoMapBuilding::scrollViewWillBeginDecelerating(CCNikScrollView* view)
{
//    CCLog_("scrollViewWillBeginDecelerating");
//    hudPanel->m_popOver->hide();
}

void IsoMapBuilding::scrollViewDidEndDecelerating(CCNikScrollView* view)
{
//    CCLog_("scrollViewDidEndDecelerating");
}

void IsoMapBuilding::scrollViewWillBeginZooming(CCNikScrollView* view)
{
//    CCLog_("scrollViewWillBeginZooming");
//    hudPanel->m_popOver->hide();
}
void IsoMapBuilding::scrollViewDidEndZooming(CCNikScrollView* view)
{
//    CCLog_("scrollViewDidEndZooming");
}


#pragma mark - Release function
        
void IsoMapBuilding::releaseDecoration(Decoration *decoration_) {
    
    if (decoration_) {
        
        g_playerState->removeDecoration(decoration_);
        
    }
    
}
