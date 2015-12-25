//
//  IsoMap.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/29/56 BE.
//
//

#include "IsoMap.h"
USING_NS_CC;
using namespace std;
using namespace cocos2d::extension;

#include <spine/Json.h>

#include "AppMacros.h"
extern TargetDevice targetDevice;

//extern float scaleFactor;
#include "../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

#include "../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../Services/GameAudioService.h"

#include "../Services/BackendService.h"
#include "../Services/APIs/APIBuildingNew.h"
#include "../Services/APIs/APIBuildingUpdate.h"
#include "../Entities/PlayerState.h"
#include "../Services/APIs/APIDecorationNew.h"
#include "../Services/APIs/APIDecorationUpdate.h"

extern float retinaMultiplier;
extern unsigned int currentTime;

#include "../Helpers/ArrayScale.h"

#include "../Extensions/CCLabelHelper.h"

#include "../Helpers/AnalyticsHelper.h"
#include "../Helpers/FlurryCpp.h"
#include "../Helpers/CustomEventsHelper.h"
#include "../Helpers/PushNotifications/PushNotifications.h"

SearchPath::SearchPath(CCPoint coord_, int count_, int trackno_)  {
    coord = coord_;
    count = count_;
    trackno = trackno_;
}

bool IsoMap::init() {
    
    return initWithPreferenceRootParent(this);
    
}

bool IsoMap::initWithPreferenceRootParent(void *preferenceRootParent_) {
    
    if (!CCLayer::init()) {
        return false;
    }
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    m_viewType          = IMV_PLAYABLE;
    m_trackStart        = NULL;
    m_trackDirection    = NULL;
    
    m_layerTrack = CCLayer::create();
    this->addChild(m_layerTrack, 1);
    
    CCSprite *_tileSprite = CCSprite::createWithSpriteFrameName("tile_grass_1.png");
    CCSize _tileSize = _tileSprite->getContentSize();
    CCSize _mapSize = CCSize(_tileSize.width*ISO_TILE_COL, _tileSize.height*ISO_TILE_ROW);
    CCPoint _tileShift = CCPoint(_mapSize.width*0.5f, -(_mapSize.height*0.0f) + (_tileSize.height*0.5f));
    
    _tileSprite = NULL;
    
    reservedDummy = new Building();
    reservedDummy->retain();
    reservedDummyForTrack = new Building();
    reservedDummyForTrack->retain();
    reservedDummyForTrack->setTag(101);
    
    for (int k=0; k<ISO_TILE_NUM; k++) {
        tilesMap[k] = reservedDummy;
        tilesLegal[k] = ISO_UNAVAILABLE_I;
        if (k < ISO_TRACK_NUM) {
            tilesTrack[k] = 0;
        }
    }
    
    this->setContentSize(_mapSize);
    this->setAnchorPoint(CCPointZero);
    this->setPosition(-(_mapSize.width-_screenSize.width)/2, -(_mapSize.height-_screenSize.height)/2);
    
    CREATE_CHAR_BUFFER(_buffer, 64);

    for (int j=0; j<4; j++) {
        for (int i=0; i<4; i++) {
            int k = (j*4)+i;
            snprintf(_buffer, _buffer_size, "bg_%.2d.png", k+1);
            m_bgSprite[j][i] = CCSprite::createWithSpriteFrameName(_buffer);
            if (targetDevice == TD_IPAD_RETINA) {
                m_bgSprite[j][i]->setScale(retinaMultiplier*2.0f);
            }
            this->addChild(m_bgSprite[j][i], 0);
        }
    }

    m_bgSprite[0][0]->setPosition(ccp(4480.0f, 8160.0f) * retinaMultiplier);
    m_bgSprite[0][1]->setPosition(ccp(6400.0f, 8160.0f) * retinaMultiplier);
    m_bgSprite[0][2]->setPosition(ccp(8960.0f, 8160.0f) * retinaMultiplier);
    m_bgSprite[0][3]->setPosition(ccp(10880.0f, 8160.0f) * retinaMultiplier);
    m_bgSprite[1][0]->setPosition(ccp(4480.0f, 6720.0f) * retinaMultiplier);
    m_bgSprite[1][1]->setPosition(ccp(6400.0f, 6720.0f) * retinaMultiplier);
    m_bgSprite[1][2]->setPosition(ccp(8960.0f, 6720.0f) * retinaMultiplier);
    m_bgSprite[1][3]->setPosition(ccp(10880.0f, 6720.0f) * retinaMultiplier);
    m_bgSprite[2][0]->setPosition(ccp(4480.0f, 4800.0f) * retinaMultiplier);
    m_bgSprite[2][1]->setPosition(ccp(6400.0f, 4800.0f) * retinaMultiplier);
    m_bgSprite[2][2]->setPosition(ccp(8960.0f, 4800.0f) * retinaMultiplier);
    m_bgSprite[2][3]->setPosition(ccp(10880.0f, 4800.0f) * retinaMultiplier);
    m_bgSprite[3][0]->setPosition(ccp(4480.0f, 3360.0f) * retinaMultiplier);
    m_bgSprite[3][1]->setPosition(ccp(6400.0f, 3360.0f) * retinaMultiplier);
    m_bgSprite[3][2]->setPosition(ccp(8960.0f, 3360.0f) * retinaMultiplier);
    m_bgSprite[3][3]->setPosition(ccp(10880.0f, 3360.0f) * retinaMultiplier);
    
    if (g_gameContent) {
        g_gameContent->tileSize = _tileSize;
        g_gameContent->tileShift = _tileShift;
    }else CCLog_("(IsoMap::init) Warning: gameContent is uninitialized!\n");

#ifdef IMPLEMENT_LOCK_ZONE
    m_shadowGrid = ShadowInactiveArea::create();
    m_shadowGrid->setPosition(this->getContentSize()*0.5f);
    addChild(m_shadowGrid, 0);
#endif
    
    scrollView = NULL;
    
    schedule(schedule_selector(IsoMap::updateBuildings), kBuildingUpdateRoutine);
    
    CCLog_("tileSize = %fx%f", _tileSize.width, _tileSize.height);

    m_petsAreSleepingCheck = !CustomEventsHelper::Instance().isSleepingCheckedFinished() && !PushNotifications::Instance().isEnabled();

    return true;
}

IsoMap::~IsoMap()
{
    disposeMap();
    
    unschedule(schedule_selector(IsoMap::updateBuildings));
    
    delete reservedDummy;
    reservedDummy = NULL;
    
    delete reservedDummyForTrack;
    reservedDummyForTrack = NULL;
}

bool IsoMap::composeMap(void *playerState_, IsoMapView viewType_, void *preferenceRootParent_)
{
    mapPlayerState = playerState_;
    m_viewType = viewType_;
    preferenceRootParent = preferenceRootParent_;
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;
        
    _setLegalTile(40);
    
    // Adding all tracks
    for (vector<Track*>::iterator it=_mapPlayerState->Tracks.begin(); it!=_mapPlayerState->Tracks.end(); ++it) {
        Track* _track = (Track*)(*it);
        _track->preferenceRootParent = preferenceRootParent;
        addTrack(_track);
    }
   
    Track* _startTrack = _mapPlayerState->getTrackToStart(_mapPlayerState->player.racing.start);
    setAsStartTrack(_startTrack);
    
    // Adding all obstacles
    for (vector<Track*>::iterator it=_mapPlayerState->Obstacles.begin(); it!=_mapPlayerState->Obstacles.end(); ++it) {
        Track *_obstacle = (Track*)(*it);
        _obstacle->preferenceRootParent = preferenceRootParent;
        addTrack(_obstacle);
    }
    
    // Adding all buildings
    for (vector<Building*>::iterator it=_mapPlayerState->Buildings.begin(); it!=_mapPlayerState->Buildings.end(); ++it) {
        Building *_building = (Building*)(*it);
        _building->setPosition(getPositionByMapIndexes(_building->pbVo.map, g_gameContent->tileSize, g_gameContent->tileShift));
        _building->preferenceRootParent = preferenceRootParent;
        addBuilding(_building);
    }
    
    // Adding all decorations
    for (vector<Decoration*>::iterator it=_mapPlayerState->Decorations.begin(); it!=_mapPlayerState->Decorations.end(); ++it) {
        Decoration *_decoration = (Decoration*)(*it);
        if (_decoration->pdVo.status != kDecorationStatusUsed) {
            CCLog_("%s kept!", _decoration->dVo.title.c_str());
            continue;
        }
        _decoration->setPosition(getPositionByMapIndexes(_decoration->pdVo.map, g_gameContent->tileSize, g_gameContent->tileShift));
        _decoration->preferenceRootParent = preferenceRootParent;
        addDecoration(_decoration);
    }
    
    // Adding all pets
    if (m_viewType == IMV_PLAYABLE || m_viewType == IMV_VIEWONLY) {
        this->addPetToItsOwnHabitat();
    }else {
        this->purgeAllPetsFromHabitat();
    }
    
    _mapPlayerState->calculateTrackpointsUsed();
    
    return true;
    
}

void IsoMap::disposeMap() {
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;
    for (vector<Building*>::iterator it=_mapPlayerState->Buildings.begin(); it!=_mapPlayerState->Buildings.end(); ++it) {
        Building *_building = (Building*)(*it);
        _building->willRemoveFromIsoMap();
    }
    
}

void IsoMap::addPetToItsOwnHabitat() {
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;

    // Adding all pets
    for (vector<Pet*>::iterator it=_mapPlayerState->Pets.begin(); it!=_mapPlayerState->Pets.end(); ++it) {
        Pet *_pet = (Pet*)((*it));
        if (_pet->ppVo.located_in == "") continue;
        _pet->updateHunger();
        addPetToBuilding(_pet->ppVo.puid, _pet->ppVo.located_in);
    }
    
}

void IsoMap::purgeAllPetsFromHabitat() {
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;
    if (!_mapPlayerState) {
        CCLog_("(IsoMap::addPetToItsOwnHabitat) Error: mapPlayerState is unitialized!");
        return;
    }
    
    // Adding all pets
    for (vector<Building*>::iterator it=_mapPlayerState->Buildings.begin(); it!=_mapPlayerState->Buildings.end(); ++it) {
        if (Building *_building = (Building*)(*it)) {
            if (_building->pet) {
                _building->pet->removeFromParent();
                _building->pet = NULL;
            }
        }
    }
    
}

#pragma mark - Buildings

int IsoMap::calZOrderForBuilding(Building *building) {

    int z = 0;
    if (building->bVo.stack == kBuildingStackOnTop) {
        z = this->getContentSize().height;
    }else if (building->bVo.stack == kBuildingStackGround) {
        z = 0;
    }else { //kBuildingStackNormal
        z = int(getContentSize().height - building->getPosition().y);
    }
    //z-order for special cases
    if (building->pbVo.type == kBuildingGate) {
        z += int(building->bVo.src_height * retinaMultiplier);
    }
    
    return z;
    
}

bool IsoMap::addBuilding(Building *building) {

    return addBuilding(building, false);

}

bool IsoMap::addBuilding(Building *building, bool update_in_backend) {
    
    return addBuilding(building, BuildingBuildTypeNormal, update_in_backend);
    
}

bool IsoMap::addBuilding(Building* building, BuildingBuildType buildType, bool update_in_backend) {
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;

    for (int i=0; i<GRID_HOLDER_MAX; i++) {
        if (building->pbVo.map[i] < 0) continue;
        tilesMap[building->pbVo.map[i]] = building;
    }
    
    if (building->bVo.type == kHQBuilding) {
        if (!_mapPlayerState->HQBuilding) {
            _mapPlayerState->HQBuilding = building;
        }else {
            CCLog_("Warning: there are more than one HQ Building!");
        }
    }

    building->removeFromParent();
    building->parent = this;
    building->setActive(m_viewType == IMV_PLAYABLE);
    building->newState(buildType, update_in_backend);
    
    _mapPlayerState->addBuilding(building);
    addChild(building, calZOrderForBuilding(building));
    
    return true;
    
}

bool IsoMap::purgeBuilding(Building *building) {
    
    if (!building) {
        CCLog_("(IsoMap::hideBuilding) Warning: building is uninitialized!");
        return false;
    }
    
    for (int i=0; i<GRID_HOLDER_MAX; i++) {
        if (building->pbVo.map[i] > 0) {
            tilesMap[building->pbVo.map[i]] = NULL;
        }
    }

    return true;
    
}

void IsoMap::updateBuildings() {
    
    PlayerState *_mapPlayerState = (PlayerState*)mapPlayerState;
    if (_mapPlayerState || typeid(*_mapPlayerState) == typeid(PlayerState)) {
        if (m_viewType == IMV_PLAYABLE) {
            for (vector<Building*>::iterator it=_mapPlayerState->Buildings.begin(); it!=_mapPlayerState->Buildings.end(); ++it) {
                Building *_building = (Building*)(*it);
                _building->update(kBuildingUpdateRoutine);
            }
        }else {
            for (vector<Pet*>::iterator it=_mapPlayerState->Pets.begin(); it!=_mapPlayerState->Pets.end(); ++it) {
                Pet* _pet = (Pet*)(*it);
                _pet->setHungerTimer(0);
            }
        }
    }
    
    if (m_petsAreSleepingCheck
        && areAllPetsSleeping()
        && CustomEventsHelper::Instance().isTutorialFinished())
    {
        if (PushNotifications::Instance().notify(PushNotifications::FirstTimeAnimalWakeUp)) {
            m_petsAreSleepingCheck = false;
            CustomEventsHelper::Instance().sleepingCheck();
        }
    }
    
}

void IsoMap::focuzOnBuilding(const char *building_type) {
    
    focuzOnBuilding(building_type, 0);
    
}

void IsoMap::focuzOnBuilding(const char *building_type, int index) {
    
    PlayerState *_mapPlayerState = (PlayerState*)mapPlayerState;
    if (!_mapPlayerState) return;
    
    Building *_building = _mapPlayerState->getBuilding(building_type, index);
    if (!_building) return;
    
    CCAssert(scrollView != NULL, "scrollView must not be NULL!");
    
    scrollView->zoomOnPosition(_building->getPosition()
                               - getContentSize()*0.25f,
                               1.0f, 1.0f, true);
    
}

#pragma mark - Tracks

bool IsoMap::addTrack(Track *track) {
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;

//    if(_mapPlayerState->player.racing._track_points >= _mapPlayerState->player.racing._track_points_limit) {
//        g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, "All Trackpoints have been used.");
//        return false;
//    }
    
    if (track->ptVo.index >= 0) {
        
        if (track->tVo.track_type == PT_OBJECT_TRACK) {
            
            if (_mapPlayerState->player.racing.start < 0) {
                setAsStartTrackIfLegal(track);
            }
            
            int t_trackMapper[SPACE_TRACK_GRID_NUM];
            
            getTrackMapper(track->ptVo.index, t_trackMapper);
            
            for (int i=0; i<SPACE_TRACK_GRID_NUM; i++) {
                tilesMap[t_trackMapper[i]] = reservedDummyForTrack;
            }
            tilesTrack[track->ptVo.index] = track->ptVo.no;
            
        }else if (track->tVo.track_type == PT_OBJECT_OBSTACLE) {
            
            tilesObstacle[track->ptVo.index] = 1;
            
        }

    }
    
    track->removeFromParent();
    
    track->updateSprite();
    track->updatePosition();
    track->updateFlip();
    
    m_layerTrack->addChild(track);
    
    _mapPlayerState->addTrack(track);
    _mapPlayerState->calculateTrackpointsUsed();
//    _mapPlayerState->player.racing._track_points += track->tVo.point_used;
   
    updateTrackGlue();

    return true;
    
}

bool IsoMap::applyTrack(Track *track) {

    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;
    
    if (track->ptVo.index < 0) {
        removeTrack(track);
        return false;
    }
    
    // update tiles map
    if (track->ptVo.index >= 0) {
        
        if (track->tVo.track_type == PT_OBJECT_TRACK) {
            
            int t_trackMapper[SPACE_TRACK_GRID_NUM];
            getTrackMapper(track->ptVo.index, t_trackMapper);
            
            for (int i=0; i<SPACE_TRACK_GRID_NUM; i++) {
                tilesMap[t_trackMapper[i]] = reservedDummyForTrack;
            }
            
            tilesTrack[track->ptVo.index] = track->ptVo.no;
            if (track->trackStart) { // also tell mapPlayerState that this track is the start track
                _mapPlayerState->player.racing.start = track->ptVo.index;
            }
            
        }else if (track->tVo.track_type == PT_OBJECT_OBSTACLE) {
            
            tilesObstacle[track->ptVo.index] = 1;
            
        }
        
    }
    
    track->removeFromParent();
    m_layerTrack->addChild(track);
    track->scheduleUpdate();
    
    track->updatePosition();
    
    _mapPlayerState->addTrack(track);
    _mapPlayerState->calculateTrackpointsUsed();
//    _mapPlayerState->player.racing._track_points += track->tVo.point_used;
    
    updateTrackGlue();
    
    return true;
    
}

bool IsoMap::purgeTrack(PlayerTrackVO ptvo) {
    
    GDTrackVO vo = g_gameContent->getTrackVO(ptvo.type);
    if (vo.type == "") {
        CCLog_("(IsoMap::purgeTrack) Warning: track=%s doesn't exist!", ptvo.type.c_str());
        return false;
    }
    
    if (vo.track_type == PT_OBJECT_TRACK) {
        
        if (ptvo.index >= 0) {
            int t_trackMapper[SPACE_TRACK_GRID_NUM];
            getTrackMapper(ptvo.index, t_trackMapper);
            for (int i=0; i<SPACE_TRACK_GRID_NUM; i++) {
                tilesMap[t_trackMapper[i]] = NULL;
            }
            tilesTrack[ptvo.index] = 0;
        }
        
    }else if (vo.track_type == PT_OBJECT_OBSTACLE) {
        
        tilesObstacle[ptvo.index] = 0;
        
    }
    
    removeTrackGlue();
    
    return true;
    
}

bool IsoMap::purgeTrack(Track *track) {
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;
    
    if (!purgeTrack(track->ptVo)) {
        return false;
    }
    
    if (_mapPlayerState->removeTrack(track)) {
        if (track->trackStart && track->tVo.track_type == PT_OBJECT_TRACK) {
            _mapPlayerState->player.racing.start = -1;
        }
//        _mapPlayerState->player.racing._track_points -= track->tVo.point_used;
    }
    
    return true;
    
}

bool IsoMap::removeTrack(Track *track) {
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;
    purgeTrack(track->ptVo);
    if (track->tVo.track_type == PT_OBJECT_TRACK) {
        if (track->trackStart) {
            _mapPlayerState->player.racing.start = -1;
        }
    }

    track->removeFromParentAndCleanup(true);
//    track->release();
    track = NULL;
    
    removeTrackGlue();

    return true;

}

bool IsoMap::addTrackGlue(Track *track1, Track *track2) {
    
    TrackGlue* exising_glue = NULL;
    
    for (vector<TrackGlue*>::iterator it=trackGlues.begin(); it!=trackGlues.end(); ++it) {
        if (TrackGlue* trackGlue = (TrackGlue*)(*it)) {
            if ((trackGlue->track[0]->ptVo.index == track1->ptVo.index && trackGlue->track[1]->ptVo.index == track2->ptVo.index) ||
                (trackGlue->track[0]->ptVo.index == track2->ptVo.index && trackGlue->track[1]->ptVo.index == track1->ptVo.index)) {
                exising_glue = trackGlue;
                break;
            }
        }
    }
    
    if (exising_glue) {
        
        //exising_glue->up
        
    }else {
        
        int d = 0;
        if (track1->getPosition().x < track2->getPosition().x) {
            if (track1->getPosition().y < track2->getPosition().y) {
                d = 1;
            }else if (track1->getPosition().y > track2->getPosition().y) {
                d = 2;
            }
        }else if (track1->getPosition().x > track2->getPosition().x) {
            if (track1->getPosition().y > track2->getPosition().y) {
                d = 3;
            }else if (track1->getPosition().y < track2->getPosition().y) {
                d = 4;
            }
        }

        bool _track_connected = false;

        switch (track1->ptVo.no) {
            case 1:
                switch (d) {
                    case 1:
                        switch (track2->ptVo.no) {
                            case 1:
                            case 2:
                            case 4: _track_connected = true; break;
                        }
                        break;
                    case 2:
                        break;
                    case 3:
                        switch (track2->ptVo.no) {
                            case 1:
                            case 5:
                            case 6: _track_connected = true; break;
                        }
                        break;
                    case 4:
                        break;
                }
                
                break;
            case 2:
                switch (d) {
                    case 1:
                        break;
                    case 2:
                        switch (track2->ptVo.no) {
                            case 3:
                            case 4:
                            case 6: _track_connected = true; break;
                        }
                        break;
                    case 3:
                        switch (track2->ptVo.no) {
                            case 1:
                            case 5:
                            case 6: _track_connected = true; break;
                        }
                        break;
                    case 4:
                        break;
                }
                break;
            case 3:
                switch (d) {
                    case 1:
                        break;
                    case 2:
                        switch (track2->ptVo.no) {
                            case 3:
                            case 4:
                            case 6: _track_connected = true; break;
                        }
                        break;
                    case 3:
                        break;
                    case 4:
                        switch (track2->ptVo.no) {
                            case 2:
                            case 3:
                            case 5: _track_connected = true; break;
                        }
                        break;
                }
                break;
            case 4:
                switch (d) {
                    case 1:
                        break;
                    case 2:
                        break;
                    case 3:
                        switch (track2->ptVo.no) {
                            case 1:
                            case 5:
                            case 6: _track_connected = true; break;
                        }
                        break;
                    case 4:
                        switch (track2->ptVo.no) {
                            case 2:
                            case 3:
                            case 5: _track_connected = true; break;
                        }
                        break;
                }
                break;
            case 5:
                switch (d) {
                    case 1:
                        switch (track2->ptVo.no) {
                            case 1:
                            case 2:
                            case 4: _track_connected = true; break;
                        }
                        break;
                    case 2:
                        switch (track2->ptVo.no) {
                            case 3:
                            case 4:
                            case 6: _track_connected = true; break;
                        }
                        break;
                    case 3:
                        break;
                    case 4:
                        break;
                }
                break;
            case 6:
                switch (d) {
                    case 1:
                        switch (track2->ptVo.no) {
                            case 1:
                            case 2:
                            case 4: _track_connected = true; break;
                        }
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                    case 4:
                        switch (track2->ptVo.no) {
                            case 2:
                            case 3:
                            case 5: _track_connected = true; break;
                        }
                        break;
                }
                break;
        }
        
        if (track1->ptVo.type == track2->ptVo.type  && _track_connected) { //&& track1->ptVo.type != "track_normal" && track1->ptVo.type != "track_dirt"
            exising_glue = TrackGlue::createWithTracks(track1, track2);
            exising_glue->preferenceRootParent = preferenceRootParent;
            exising_glue->updateTrackGlueSprite();
            if (exising_glue) {
                m_layerTrack->addChild(exising_glue, 1);
                trackGlues.push_back(exising_glue);
            }
        }
        
    }
    
    return true;
    
}

bool IsoMap::addDecoration(Decoration *decoration_) {
    
    return addDecoration(decoration_, false, false, false);
    
}

bool IsoMap::addDecoration(Decoration *decoration_, bool fixed_) {
    
    return addDecoration(decoration_, fixed_, false, false);
    
}

bool IsoMap::addDecoration(Decoration *decoration, bool fixed, bool update_backend, bool instant) {
    
    bool success = false;
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;

    for (int i=0; i<GRID_HOLDER_MAX; i++) {
        if (decoration->pdVo.map[i] >= 0) {
            if (!tilesMap[decoration->pdVo.map[i]]) {
                tilesMap[decoration->pdVo.map[i]] = decoration;
                success = true;
            }
        }
    }
    
    if (!success) {
        return false;
    }
    
    decoration->removeFromParent();
    decoration->createDecoration(false);
    decoration->updateFlip();
    
    int z = int(this->getContentSize().height-decoration->getPosition().y);
    if (decoration->dVo.ground) {
        z = 0;
    }
    this->addChild(decoration, z);
    
    if (fixed) {
        
//        fixedDecorations.push_back(decoration_);
    
    }else {
    
        _mapPlayerState->addDecoration(decoration);
        
        if (decoration->pdVo.duid == "") {
            
            if (update_backend) {
                _mapPlayerState->player.balance.gold -= decoration->dVo.cost_coins;
                BackendService::getInstance()->send(APIDecorationNew::create(decoration,
                                                                             instant,
                                                                             this,
                                                                             IsoMap::onDecorationUpdated));
            }
        
        }else {
            
            if (update_backend && m_viewType == IMV_PLAYABLE) {
                _mapPlayerState->moveDecoration(decoration);
                if (decoration->pdVo.status == kDecorationStatusKept) {
                    BackendService::getInstance()->send(APIDecorationUpdate::create(decoration,
                                                                                    this,
                                                                                    IsoMap::onDecorationUpdated));
                }
            }
            
        }
        
        decoration->pdVo.status = kDecorationStatusUsed;
        
    }
    
    return true;
    
}

bool IsoMap::purgeDecoration(Decoration *decoration_) {
    
    for (int i=0; i<GRID_HOLDER_MAX; i++) {
        if (decoration_->pdVo.map[i] > 0) {
            tilesMap[decoration_->pdVo.map[i]] = NULL;
        }
    }
    
    return true;
}

bool IsoMap::areAllPetsSleeping()
{

    PlayerState *_mapPlayerState = (PlayerState*)mapPlayerState;
    std::vector<Building*> buildings = _mapPlayerState->Buildings;

    bool sleeping = true;
    int pets = 0;

    for (auto& building : buildings)
    {
        if (building->pbVo.type == kBuildingHabitat || building->pbVo.type == kBuildingHabitatPremium)
        {
            if (building->isPetContained())
            {
                sleeping &= building->pet->currPetActionIsSleeping();
                pets++;
            }
        }
    }

    return sleeping && pets > 0;
}

bool IsoMap::addPetToBuilding(string puid, string buid) {
    
    CCLog_("put pet:%s >> habitat:%s", puid.c_str(), buid.c_str());
    
    PlayerState *_mapPlayerState = (PlayerState*)mapPlayerState;
    
    Pet *_pet = _mapPlayerState->getPet(puid);
    if (!_pet) {
        CCLog_("(IsoMapBuilding::addPetToBuilding) Error: Given puid=%s doesn't exist!", puid.c_str());
        return false;
    }
    
    _pet->setActive(m_viewType==IMV_PLAYABLE);
    
    Building *_building = _mapPlayerState->getBuilding(buid);
    if (!_building) {
        CCLog_("(IsoMapBuilding::addPetToBuilding) Error: Given buid=%s doesn't exist!", buid.c_str());
        _pet->ppVo.located_in = "";
        return false;
    }
    
    _pet->updateCoinsProduced();
    if (!_building->addPetToBuilding(_pet)) {
        CCLog_("(IsoMapBuilding::addPetToBuilding) Error: Given buid=%s is unavailable!", buid.c_str());
        _pet->ppVo.located_in = "";
        return false;
    }
    
    // if the pet is host then set the habitat
    if (_mapPlayerState->player.racing.host == puid ||
        _mapPlayerState->player.racing.host == "") {
        _mapPlayerState->player.racing.host = puid;
        _building->setPetAsHost(true);
        _mapPlayerState->HostHabitat = _building;
    }
    
    return true;
    
}

Track* IsoMap::getTrackAt(int index_) {
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;
    if (!_mapPlayerState) return NULL;
    
    if (index_ >= 0 &&
        index_ < _mapPlayerState->Tracks.size()) {
        return _mapPlayerState->Tracks[index_];
    }
    
    return NULL;
    
}

Track* IsoMap::getTrackAtIndex(int index) {
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;
    if (!_mapPlayerState) return NULL;
    
    for (vector<Track*>::iterator it=_mapPlayerState->Tracks.begin(); it!=_mapPlayerState->Tracks.end(); it++) {
        Track* track = (Track*)(*it);
        if (track->ptVo.index != index) continue;
        return track;
    }
    
    return NULL;
    
}

Track* IsoMap::getObstacleAtIndex(int index) {
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;
    if (!_mapPlayerState) return NULL;
        
    for (vector<Track*>::iterator it=_mapPlayerState->Obstacles.begin(); it!=_mapPlayerState->Obstacles.end(); it++) {
        Track* obstacle = (Track*)(*it);
        if (obstacle->ptVo.index != index) continue;
        return obstacle;
    }
    
    return NULL;
    
}

int IsoMap::getNBuilding() {
    
    if (PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState) {
        
        return _mapPlayerState->Buildings.size();
        
    }
    
    return 0;
    
}

int IsoMap::getNTrack() {
    
    if (PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState) {
        
        return _mapPlayerState->Tracks.size();
        
    }
    
    return 0;

    
}

int IsoMap::getNObstacle() {
    
    if (PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState) {
        
        return _mapPlayerState->Obstacles.size();
        
    }
    
    return 0;

}

Track* IsoMap::getNearestTrack(CCPoint coord, int no, int avoidIndex) {
    
    return _findNearestTrack(coord, no, avoidIndex);
    
}

void IsoMap::getTrackMapper(int trackIndex, int *trackMapper) {
    
    for (int i=0; i<SPACE_TRACK_GRID_NUM; i++) {

        CCPoint pos = getPositionByTrackIndex(trackIndex, g_gameContent->tileSize, g_gameContent->tileShift);
        
        switch (i) {
            case 0:
                pos = pos + CCPoint(0, g_gameContent->tileSize.height/2);
                break;
            case 1:
                pos = pos + CCPoint(g_gameContent->tileSize.width/2, 0);
                break;
            case 2:
                pos = pos + CCPoint(0,-g_gameContent->tileSize.height/2);
                break;
            case 3:
                pos = pos + CCPoint(-g_gameContent->tileSize.width/2, 0);
                break;
        }
        
        CCPoint coord = IsoToCoord(pos, g_gameContent->tileSize, g_gameContent->tileShift);
        trackMapper[i] = getTileIndexByCoord(coord);
        
    }
    
}

int IsoMap::analystTrackDirection() {
    
    int _start_track_index = getStartTrackIndex();
    int _avoid_track_index = -1;
    Track* _curr_track = getTrackAtIndex(_start_track_index);
    
    PlayerState *_playerState = (PlayerState*)mapPlayerState;
    
    switch (tilesTrack[_curr_track->ptVo.index]) {
        case 1:
            if (_playerState->player.racing.direction) {
                _avoid_track_index = _curr_track->ptVo.index-1;
            }else {
                _avoid_track_index = _curr_track->ptVo.index+1;
            }
            break;
        default:
        case 3:
            if (_playerState->player.racing.direction) {
                _avoid_track_index = _curr_track->ptVo.index+(ISO_TILE_COL/2);
            }else {
                _avoid_track_index = _curr_track->ptVo.index-(ISO_TILE_COL/2);
            }
            break;
    }

    int n = getNTrack();
    int count = 0;
    while (Track *_next_track = getNearestTrack(_curr_track->getPosition(), tilesTrack[_curr_track->ptVo.index], _avoid_track_index)) {
//        CCLabelBMFont *label = CCLabelHelper::createBMFont(CCString::createWithFormat("%d",count)->getCString(), 50, ccWHITE);
//        _curr_track->addChild(label);
        _curr_track->trackIndex = count;
        _curr_track->nextTrack = _next_track;
        _next_track->prevTrack = _curr_track;
        _avoid_track_index = _curr_track->ptVo.index;// tilesTrack[_curr_track->ptVo.index];
        _curr_track = _next_track;
        count++;
        CCLog_("trackIndex : %d",count);
        if (count >= n) {
            break;
        }
    }
    
//    Track *_next_track = getNearestTrack(_curr_track->getPosition(), tilesTrack[_curr_track->ptVo.index], _avoid_track_index);
//    do {
//        _curr_track->trackIndex = count;
//        _curr_track->nextTrack = _next_track;
//        _next_track->prevTrack = _curr_track;
//        _avoid_track_index = _curr_track->ptVo.index;
//        _curr_track = _next_track;
//        
//        count++;
//        CCLog_("%d",count);
//        _next_track = getNearestTrack(_curr_track->getPosition(), tilesTrack[_curr_track->ptVo.index], _avoid_track_index);
//    } while (_start_track_index != _next_track->ptVo.index);
    
    return count;
}

CCPoint IsoMap::getPositionOfAvailableSpaceFor(void *entity) {
    
    if (typeid(*entity) == typeid(Building)) {
        Building* _building = (Building*)entity;
        return getPositionOfAvailableSpaceFor(_building->bVo);
    }
    
    return ccp(-1.0f , -1.0f);
    
}

CCPoint IsoMap::getPositionOfAvailableSpaceFor(GDBuildingVO vo) {
    
    PlayerState *_playerState = (PlayerState*)mapPlayerState;
    if (!_playerState) return ccp(-1.0f , -1.0f);
    
    Building *_attachToBuilding = _playerState->getBuilding(kBuildingHQ, 0);
    if (!_attachToBuilding) return ccp(-1.0f, -1.0f);
    
    int tileIndexes[BUILDING_SPACE_GRID_NUM];
    if (getAvailableSpaceFor(vo, _attachToBuilding->bVo, _attachToBuilding->pbVo.map[0], tileIndexes)) {
        return getPositionByMapIndexes(tileIndexes,
                                       g_gameContent->tileSize,
                                       g_gameContent->tileShift);
    }
    
    return ccp(-1.0f, -1.0f);
    
}

CCPoint IsoMap::getSnapGridPosition(CCPoint pt, CCSize size, CCPoint shift, CCSize gridsize) {
    
    CCPoint coord = IsoToCoord(pt, size, shift);
    
    CCPoint ext = CCPointZero;
    if (int(gridsize.width)%2 == 0 || int(gridsize.height)%2 == 0) {
        ext.y = size.height/2;
    }
    
    return IsoMap::CoordToIso(coord, size, shift)+ext;
    
}

CCPoint IsoMap::getSnapQuadPosition(cocos2d::CCPoint pt, cocos2d::CCSize size, cocos2d::CCPoint shift, cocos2d::CCSize gridsize) {
    
    CCPoint coord = IsoMap::IsoToCoord(pt, size*2, shift);
    
    CCPoint ext = CCPointZero;
    if (int(gridsize.width*2)%2 == 0 || int(gridsize.height*2)%2 == 0) {
        ext.y = size.height/2;
    }
    
    return IsoMap::CoordToIso(coord, size*2, shift)+ext;
    
}


////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
bool IsoMap::_validateTrackCompletion() {
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;
    if (!_mapPlayerState) {
        return false;
    }
    
    bool valid = false;

    for (int i=0; i<ISO_TRACK_NUM; i++) {
        
        if (tilesTrack[i] > 0) {
            
            int map_[ISO_TRACK_NUM] = {0};
            
            int j = i-1;
            switch (tilesTrack[i]) {
                default:
                case 1:
                    j = i - 1;
                    break;
                case 2:
                    j = i - 1;
                    break;
                case 3:
                    j = i - int(ISO_TILE_COL/2);
                    break;
                case 4:
                    j = i - 1;
                    break;
                case 5:
                    j = i - int(ISO_TILE_COL/2);
                    break;
                case 6:
                    j = i + 1;
                    break;
            }
            
            int count = _travelMap(map_, j, i, 0);
            if (count >= getNTrack()) {
                valid = true;
            }
            break;
            
        }
        
    }
    
    if (_mapPlayerState->player.racing.host == "") {
        valid = false;
    }
    
    if (_mapPlayerState->player.racing.start == -1) {
        valid = false;
    }
    
    _mapPlayerState->player.racing.status = valid;
    
    return valid;
    
}

int IsoMap::_travelMap(int map[], int prepos, int curpos, int count) {
    
    bool debug_ = false;
    
    int n = getNTrack();
    
    if (debug_) {
        CCLog_("---------------------------------");
        for (int j=0; j<ISO_TILE_ROW/2; j++) {
            for (int i=0; i<ISO_TILE_COL/2; i++) {
                int k = j*(ISO_TILE_COL/2)+i;
                CCLog_("%d,", map[k]);
            }
            
        }
        CCLog_("count=%d < n=%d, pre:%d=%d, cur:%d=%d", count, n, prepos, tilesTrack[prepos], curpos, tilesTrack[curpos]);
    }
    
    if (map[curpos] == 0) {
        
        switch (tilesTrack[curpos]) {
            case 1: {
                
                bool ok = false;
                
                int nxtpos[] = {
                    curpos + 1,
                    curpos - 1
                };
                
                int k=0;
                for (int i=0; i<2; i++) {
                    if(nxtpos[i] == prepos) {
                        ok = true;
                        k = i;
                    }
                }
                
                if (ok) {
                    
                    if (k==0) {
                        if (debug_) CCLog_(", nxt: %d=%d", nxtpos[!k], tilesTrack[nxtpos[!k]]);
                        if ((tilesTrack[prepos] == 1 ||
                             tilesTrack[prepos] == 2 ||
                             tilesTrack[prepos] == 4) && (count < n)) {
                            count++;
                            map[curpos] = 1;
                            return _travelMap(map, curpos, nxtpos[1], count);
                        }
                    }else {
                        if (debug_) CCLog_(", nxt: %d=%d", nxtpos[!k], tilesTrack[nxtpos[!k]]);
                        if ((tilesTrack[prepos] == 1 ||
                             tilesTrack[prepos] == 5 ||
                             tilesTrack[prepos] == 6 ) && (count < n)) {
                            count++;
                            map[curpos] = 1;
                            return _travelMap(map, curpos, nxtpos[0], count);
                        }
                    }
                    
                }
                
            }break;
            case 2: {
                
                bool ok = false;
                
                int nxtpos[] = {
                    curpos - int(ISO_TILE_COL/2),
                    curpos - 1
                };
                
                int k=0;
                for (int i=0; i<2; i++) {
                    if(nxtpos[i] == prepos) {
                        ok = true;
                        k = i;
                        
                    }
                }
                
                if (ok) {
                    if (k==0) {
                        if (debug_) CCLog_(", nxt: %d=%d", nxtpos[!k], tilesTrack[nxtpos[!k]]);
                        if ((tilesTrack[prepos] == 3 ||
                             tilesTrack[prepos] == 4 ||
                             tilesTrack[prepos] == 6 ) && (count < n)) {
                            count++;
                            map[curpos] = 1;
                            return _travelMap(map, curpos, nxtpos[!k], count);
                        }
                        
                    }else {
                        if (debug_) CCLog_(", nxt: %d=%d", nxtpos[!k], tilesTrack[nxtpos[!k]]);
                        
                        if ((tilesTrack[prepos] == 1 ||
                             tilesTrack[prepos] == 5 ||
                             tilesTrack[prepos] == 6 ) && (count < n)) {
                            count++;
                            map[curpos] = 1;
                            return _travelMap(map, curpos, nxtpos[!k], count);
                        }
                        
                    }
                    
                }
                
            }break;
            case 3: {
                
                bool ok = false;
                
                int nxtpos[] = {
                    curpos - int(ISO_TILE_COL/2),
                    curpos + int(ISO_TILE_COL/2)
                };
                
                int k=0;
                for (int i=0; i<2; i++) {
                    if(nxtpos[i] == prepos) {
                        ok = true;
                        k = i;
                    }
                }
                
                if (ok) {
                    
                    if (k==0) {
                        if (debug_) CCLog_(", nxt: %d=%d", nxtpos[!k], tilesTrack[nxtpos[!k]]);
                        if ((tilesTrack[prepos] == 3 ||
                             tilesTrack[prepos] == 4 ||
                             tilesTrack[prepos] == 6 )  && (count < n)) {
                            count++;
                            map[curpos] = 1;
                            return _travelMap(map, curpos, nxtpos[1], count);
                        }
                    }else {
                        if (debug_) CCLog_(", nxt: %d=%d", nxtpos[!k], tilesTrack[nxtpos[!k]]);
                        if ((tilesTrack[prepos] == 2 ||
                             tilesTrack[prepos] == 3 ||
                             tilesTrack[prepos] == 5 ) && (count < n)) {
                            count++;
                            map[curpos] = 1;
                            return _travelMap(map, curpos, nxtpos[0], count);
                        }
                        
                    }
                    
                }
                
            }break;
            case 4: {
                
                bool ok = false;
                
                int nxtpos[] = {
                    curpos + int(ISO_TILE_COL/2),
                    curpos - 1
                };
                
                int k=0;
                for (int i=0; i<2; i++) {
                    if(nxtpos[i] == prepos) {
                        ok = true;
                        k = i;
                    }
                }
                
                if (ok) {
                    
                    if (k==0) {
                        
                        if (debug_) CCLog_(", nxt: %d=%d", nxtpos[!k], tilesTrack[nxtpos[!k]]);
                        if ((tilesTrack[prepos] == 2 ||
                             tilesTrack[prepos] == 3 ||
                             tilesTrack[prepos] == 5 ) && (count < n)) {
                            count++;
                            map[curpos] = 1;
                            return _travelMap(map, curpos, nxtpos[1], count);
                        }
                        
                    }else {
                        
                        if (debug_) CCLog_(", nxt: %d=%d", nxtpos[!k], tilesTrack[nxtpos[!k]]);
                        if ((tilesTrack[prepos] == 1 ||
                             tilesTrack[prepos] == 5 ||
                             tilesTrack[prepos] == 6 ) && (count < n)) {
                            count++;
                            map[curpos] = 1;
                            return _travelMap(map, curpos, nxtpos[0], count);
                        }
                        
                    }
                    
                }
                
            }break;
            case 5: {
                
                bool ok = false;
                
                int nxtpos[] = {
                    curpos - int(ISO_TILE_COL/2),
                    curpos + 1
                };
                
                int k=0;
                for (int i=0; i<2; i++) {
                    if(nxtpos[i] == prepos) {
                        ok = true;
                        k = i;
                    }
                }
                
                if (ok) {
                    
                    if (k==0) {
                        if (debug_) CCLog_(", nxt: %d=%d", nxtpos[!k], tilesTrack[nxtpos[!k]]);
                        if ((tilesTrack[prepos] == 3 ||
                             tilesTrack[prepos] == 4 ||
                             tilesTrack[prepos] == 6 ) && (count < n)) {
                            count++;
                            map[curpos] = 1;
                            return _travelMap(map, curpos, nxtpos[1], count);
                        }
                    }else {
                        if (debug_) CCLog_(", nxt: %d=%d", nxtpos[!k], tilesTrack[nxtpos[!k]]);
                        if ((tilesTrack[prepos] == 1 ||
                             tilesTrack[prepos] == 2 ||
                             tilesTrack[prepos] == 4 ) && (count < n)) {
                            count++;
                            map[curpos] = 1;
                            return _travelMap(map, curpos, nxtpos[0], count);
                        }
                    }
                    
                }
            }break;
            case 6: {
                
                bool ok = false;
                
                int nxtpos[] = {
                    curpos + 1,
                    curpos + int(ISO_TILE_COL/2)
                };
                
                int k=0;
                for (int i=0; i<2; i++) {
                    if(nxtpos[i] == prepos) {
                        ok = true;
                        k = i;
                    }
                }
                
                if (ok) {
                    
                    if (k==0) {
                        if (debug_) CCLog_(", nxt: %d=%d", nxtpos[!k], tilesTrack[nxtpos[!k]]);
                        if ((tilesTrack[prepos] == 1 ||
                             tilesTrack[prepos] == 2 ||
                             tilesTrack[prepos] == 4 ) && (count < n)) {
                            count++;
                            map[curpos] = 1;
                            return _travelMap(map, curpos, nxtpos[1], count);
                        }
                        
                    }else {
                        if (debug_) CCLog_(", nxt: %d=%d", nxtpos[!k], tilesTrack[nxtpos[!k]]);
                        if ((tilesTrack[prepos] == 2 ||
                             tilesTrack[prepos] == 3 ||
                             tilesTrack[prepos] == 5 ) && (count < n)) {
                            count++;
                            map[curpos] = 1;
                            return _travelMap(map, curpos, nxtpos[0], count);
                        }
                    }
                    
                }
                
            }break;
        }
        
    }
    
    return count;
}

Track* IsoMap::_findNearestTrack(CCPoint location, int no) {
    return _findNearestTrack(location, no, -1);
}

Track* IsoMap::_findNearestTrack(CCPoint location, int no, int avoidIndex) {
    
    int neighbor_trackIndex[] = {
        getTrackIndexByPosition(location+CCPoint( g_gameContent->tileSize.width, g_gameContent->tileSize.height), g_gameContent->tileSize, g_gameContent->tileShift),
        getTrackIndexByPosition(location+CCPoint( g_gameContent->tileSize.width,-g_gameContent->tileSize.height), g_gameContent->tileSize, g_gameContent->tileShift),
        getTrackIndexByPosition(location+CCPoint(-g_gameContent->tileSize.width,-g_gameContent->tileSize.height), g_gameContent->tileSize, g_gameContent->tileShift),
        getTrackIndexByPosition(location+CCPoint(-g_gameContent->tileSize.width, g_gameContent->tileSize.height), g_gameContent->tileSize, g_gameContent->tileShift),
    };
    
    for (int i=0; i<4; i++) {
        if (neighbor_trackIndex[i] != avoidIndex) {
            if (Track* track = getTrackAtIndex(neighbor_trackIndex[i])) {
                switch (i) {
                    case 0: {
                        switch (no) {
                            case 1:
                            case 5:
                            case 6: {
                                switch (track->ptVo.no) {
                                    case 1:
                                    case 2:
                                    case 4: {
                                        return track;
                                    }break;
                                }
                            }break;
                        }
                    }break;
                    case 1: {
                        switch (no) {
                            case 2:
                            case 3:
                            case 5: {
                                switch (track->ptVo.no) {
                                    case 3:
                                    case 4:
                                    case 6: {
                                        return track;
                                    }break;
                                }
                            }break;
                        }
                    }break;
                    case 2: {
                        switch (no) {
                            case 1:
                            case 2:
                            case 4: {
                                switch (track->ptVo.no) {
                                    case 1:
                                    case 5:
                                    case 6: {
                                        return track;
                                    }break;
                                }
                            }break;
                        }
                    }break;
                    case 3: {
                        switch (no) {
                            case 3:
                            case 4:
                            case 6: {
                                switch (track->ptVo.no) {
                                    case 2:
                                    case 3:
                                    case 5: {
                                        return track;
                                    }break;
                                }
                            }break;
                        }
                    }break;
                }
            }
            
        }
    }
    
    return NULL;
    
}

void IsoMap::_setLegalTile(int legalTileSize_) {
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    m_legalTileSize = legalTileSize_;
    
    m_allLegalTileSize  = CCSize(g_gameContent->tileSize.width*m_legalTileSize,
                                 g_gameContent->tileSize.height*m_legalTileSize);
    m_allLegalTilePos   = CCPoint(-(m_allLegalTileSize.width-_screenSize.width)/2,
                                  -(m_allLegalTileSize.height-_screenSize.height)/2);
    
    int _start = round((ISO_TILE_SIZE - m_legalTileSize)/2);
    int _end = _start + m_legalTileSize;

#ifdef IMPLEMENT_LOCK_ZONE
    int exactSizeBlockZone[N_TILE_PLAY] = {-1};
    ArrayScale(g_gameContent->BlockZone, N_ZONE, exactSizeBlockZoon, N_TILE_PLAY, 4, sqrt(N_ZONE));
#endif
    
    int l=0;
    for (int j=0; j<ISO_TILE_ROW; j++) {
        for (int i=0; i<ISO_TILE_COL; i++) {
            int k = j*ISO_TILE_COL+i;
            if ((i >= _start && i < _end) &&
                (j >= _start && j < _end) &&
                (l < N_TILE_PLAY)) {
#ifdef IMPLEMENT_LOCK_ZONE
                tilesLegal[k] = exactSizeBlockZone[l];
#else
                tilesLegal[k] = 1;
#endif
                tilesMap[k] = NULL;
                l++;
            }
        }
    }
    
//    m_shadowGrid->setup(int current_level)
    
}

////////////////////////////////////////////////////////////////////////////////////////////
// STATIC FUNCTIONS
CCPoint IsoMap::CoordToIso(CCPoint pt, CCSize size, CCPoint shift) {
    
    float x = (pt.x - pt.y) * (size.width/2) + shift.x;
    float y = (pt.x + pt.y) * (size.height/2) + shift.y;
    
    return CCPoint(x, y);
    
}

CCPoint IsoMap::IsoToCoord(CCPoint pt, CCSize size, CCPoint shift) {
    
    float ScreenX = pt.x - (shift.x);
    float ScreenY = pt.y - (shift.y) + (size.height/2);
    
    float TileX = truncf((ScreenY / size.height) + (ScreenX / size.width));
    float TileY = truncf((ScreenY / size.height) - (ScreenX / size.width));

    return CCPoint(TileX, TileY);
    
}

CCPoint IsoMap::IsoToCoord2(CCPoint pt, CCSize size, CCPoint shift) {
    
    float ScreenX = pt.x - (shift.x);
    float ScreenY = pt.y - (shift.y) + (size.height/2);

    float TileX = roundf((ScreenY / size.height) + (ScreenX / size.width));
    float TileY = roundf((ScreenY / size.height) - (ScreenX / size.width));
    
    return CCPoint(TileX, TileY);
    
}

CCPoint IsoMap::getCoordByMapIndex(int mapIndex_) {

    return CCPoint(mapIndex_ % ISO_TILE_COL, mapIndex_ / ISO_TILE_COL);
/*
    int row = 0;
    int col = 0;
    int count = 0;
    int index = mapIndex_;
    
    while (index > 0) {
        count++;
        if (count >= ISO_TILE_COL) {
            count = 0;
            row++;
        }
        index--;
    }
    col = count;
    
    return CCPoint(col, row);
*/
}

CCPoint IsoMap::getPositionByMapIndex(int mapIndex_, cocos2d::CCSize size_, cocos2d::CCPoint shift_)
{
    CCPoint coord = getCoordByMapIndex(mapIndex_);
    CCPoint pos = CoordToIso(coord, size_, shift_);
    return IsoMap::getSnapGridPosition(pos, size_, shift_, CCPoint(1,1));
}

CCPoint IsoMap::getPositionByMapIndexes(int *mapIndexes_, cocos2d::CCSize size_, cocos2d::CCPoint shift_)
{
    CCPoint _left_top = getPositionByMapIndex(mapIndexes_[0], size_, shift_);
    CCPoint _right_bottom = getPositionByMapIndex(mapIndexes_[0], size_, shift_);
    for (int i = 0; i<BUILDING_SPACE_GRID_NUM; i++) {
        if (mapIndexes_[i] < 0) continue;
        CCPoint _pos = getPositionByMapIndex(mapIndexes_[i], size_, shift_);
        if (_left_top.x > _pos.x) {
            _left_top.x = _pos.x;
        }
        if (_right_bottom.x < _pos.x) {
            _right_bottom.x = _pos.x;
        }
        if (_left_top.y > _pos.y) {
            _left_top.y = _pos.y;
        }
        if (_right_bottom.y < _pos.y) {
            _right_bottom.y = _pos.y;
        }
    }
    return (_left_top + _right_bottom)/2;
}

int IsoMap::getTileLeftFrom(int tileIndex)
{
    int nextIndex = tileIndex - 1;
    if (nextIndex < (int)floor((float)tileIndex/ISO_TILE_COL)*ISO_TILE_COL) {
        return -1;
    }
    return nextIndex;
}
int IsoMap::getTileRightFrom(int tileIndex)
{
    int nextIndex = tileIndex + 1;
    if (nextIndex > (int)ceil((float)tileIndex/ISO_TILE_COL)*ISO_TILE_COL-1) {
        return -1;
    }
    return nextIndex;
}
int IsoMap::getTileUpFrom(int tileIndex)
{
    int nextIndex = tileIndex + ISO_TILE_COL;
    if (nextIndex > ISO_TILE_COL*ISO_TILE_NUM) {
        return -1;
    }
    return nextIndex;
}
int IsoMap::getTileDownFrom(int tileIndex)
{
    int nextIndex = tileIndex - ISO_TILE_COL;
    if (nextIndex < 0) {
        return -1;
    }
    return nextIndex;
}


CCPoint IsoMap::getCoordByTrackIndex(int trackIndex) {
    
    int row = 0;
    int col = 0;
    int count = 0;
    int index = trackIndex;
    
    while (index > 0) {
        count++;
        if (count >= ISO_TILE_COL/2) {
            count = 0;
            row++;
        }
        index--;
    }
    col = count;
    
    return CCPoint(col, row);
    
}

CCPoint IsoMap::getPositionByTrackIndex(int trackIndex, cocos2d::CCSize size, cocos2d::CCPoint shift) {
    
    CCPoint coord = getCoordByTrackIndex(trackIndex);
    CCPoint pos = CoordToIso(coord, size*2, shift);
    //printf("coord:%d,%d, pos:%f,%f\n", (int)coord.x, (int)coord.y, pos.x, pos.y);
    return IsoMap::getSnapQuadPosition(pos, size, shift, CCPoint(2,2));
    
}

int IsoMap::getTileIndexByCoord(CCPoint coord) {
    
    return int(coord.y*ISO_TILE_COL+coord.x);
    
}

int IsoMap::getTileIndexByPosition(cocos2d::CCPoint position_, CCSize size_, CCPoint shift_) {
    
    CCSize _coord = IsoToCoord(position_, size_, shift_);
    return getTileIndexByCoord(_coord);
    
}

bool IsoMap::getAvailableSpaceFor(GDBuildingVO buildingVo,
                                  GDBuildingVO attachToBuildingVo,
                                  int firstTileOfBuildingAttachTo,
                                  int *tileIndexes)
{
    return getAvailableSpaceFor(buildingVo, attachToBuildingVo, firstTileOfBuildingAttachTo, tileIndexes, 0);
}

bool IsoMap::getAvailableSpaceFor(GDBuildingVO buildingVo,
                                  GDBuildingVO attachToBuildingVo,
                                  int firstTileOfBuildingAttachTo,
                                  int *tileIndexes,
                                  int recursiveLevel)
{
    //TODO: Optimize
    vector<int> potentialFirstTiles;
    
    int count = 0;
    int currTile = firstTileOfBuildingAttachTo;
    int tile = -1;
    int forkPointTile = -1;
    
    // TOP-LEFT
    currTile = firstTileOfBuildingAttachTo;
    for (count=0; count<buildingVo.grid_row; count++) {
        tile = getTileUpFrom(currTile);
        if (tile < 0) break;
        currTile = tile;
    }
    if (count == buildingVo.grid_row) {
        forkPointTile = currTile;
        vector<int> tmp;
        for (count=0; count<buildingVo.grid_col; count++) {
            tile = getTileLeftFrom(currTile);
            if (tile < 0) break;
            currTile = tile;
            tmp.push_back(currTile);
        }
        while (tmp.size() > 0) {
            int t = tmp[tmp.size()-1];
            potentialFirstTiles.push_back(t);
            tmp.pop_back();
        }
        currTile = forkPointTile;
        potentialFirstTiles.push_back(currTile);
        for (count=0; count<attachToBuildingVo.grid_col-1; count++) {
            tile = getTileRightFrom(currTile);
            if (tile < 0) break;
            currTile = tile;
            potentialFirstTiles.push_back(currTile);
        }
    }
    
    // TOP-RIGHT
    currTile = firstTileOfBuildingAttachTo;
    for (count=0; count<attachToBuildingVo.grid_col; count++) {
        tile = getTileRightFrom(currTile);
        if (tile < 0) break;
        currTile = tile;
    }
    if (count == attachToBuildingVo.grid_col) {
        forkPointTile = currTile;
        vector<int> tmp;
        for (count=0; count<attachToBuildingVo.grid_row; count++) {
            tile = getTileUpFrom(currTile);
            if (tile < 0) break;
            currTile = tile;
            tmp.push_back(currTile);
        }
        while (tmp.size() > 0) {
            int t = tmp[tmp.size()-1];
            potentialFirstTiles.push_back(t);
            tmp.pop_back();
        }
        currTile = forkPointTile;
        potentialFirstTiles.push_back(currTile);
        for (count=0; count<buildingVo.grid_row-1; count++) {
            tile = getTileDownFrom(currTile);
            if (tile < 0) break;
            currTile = tile;
            potentialFirstTiles.push_back(currTile);
        }
    }
    
    // BOTTOM-RIGHT
    currTile = firstTileOfBuildingAttachTo;
    for (count=0; count<attachToBuildingVo.grid_row; count++) {
        tile = getTileDownFrom(currTile);
        if (tile < 0) break;
        currTile = tile;
    }
    if (count == attachToBuildingVo.grid_row) {
        forkPointTile = currTile;
        vector<int> tmp;
        for (count=0; count<attachToBuildingVo.grid_col; count++) {
            tile = getTileRightFrom(currTile);
            if (tile < 0) break;
            currTile = tile;
            tmp.push_back(currTile);
        }
        while (tmp.size() > 0) {
            int t = tmp[tmp.size()-1];
            potentialFirstTiles.push_back(t);
            tmp.pop_back();
        }
        currTile = forkPointTile;
        potentialFirstTiles.push_back(currTile);
        for (count=0; count<buildingVo.grid_col-1; count++) {
            tile = getTileLeftFrom(currTile);
            if (tile < 0) break;
            currTile = tile;
            potentialFirstTiles.push_back(currTile);
        }
    }
    
    // BOTTOM-LEFT
    for (count=0; count<buildingVo.grid_col; count++) {
        tile = getTileLeftFrom(currTile);
        if (tile < 0) break;
        currTile = tile;
    }
    if (count == buildingVo.grid_col) {
        forkPointTile = currTile;
        vector<int> tmp;
        for (count=0; count<attachToBuildingVo.grid_row; count++) {
            tile = getTileDownFrom(currTile);
            if (tile < 0) break;
            currTile = tile;
            tmp.push_back(currTile);
        }
        while (tmp.size() > 0) {
            int t = tmp[tmp.size()-1];
            potentialFirstTiles.push_back(t);
            tmp.pop_back();
        }
        currTile = forkPointTile;
        potentialFirstTiles.push_back(currTile);
        for (count=0; count<buildingVo.grid_row-1; count++) {
            tile = getTileUpFrom(currTile);
            if (tile < 0) break;
            currTile = tile;
            potentialFirstTiles.push_back(currTile);
        }
    }
    
    for (int i=0; i<potentialFirstTiles.size(); i++) {
        if (checkAndAssignAvailableSpace(buildingVo, potentialFirstTiles[i], tileIndexes)) {
            return tileIndexes;
        }
    }

    recursiveLevel ++;
    if (recursiveLevel >= FIND_AVAILABLE_SPACE_RECURSIVE_MAX) return false;
    
    for (int i=0; i<potentialFirstTiles.size(); i++) {
        if (getAvailableSpaceFor(buildingVo, buildingVo, potentialFirstTiles[i], tileIndexes, recursiveLevel)) {
            return true;
        }
    }
    
    return false;
    
}

bool IsoMap::checkAndAssignAvailableSpace(GDBuildingVO buildingVo,
                                          int firstTile,
                                          int *tileIndexes) {
    
    vector<int> _tileIndexesOfCurr;
    vector<int> _tileIndexes;
    
    int tile = -1;
    int currTile = firstTile;
    
    for (int j=0; j<buildingVo.grid_col; j++) {
        if (j == 0) {
            _tileIndexesOfCurr.push_back(currTile);
            continue;
        }
        tile = getTileRightFrom(currTile);
        currTile = tile;
        _tileIndexesOfCurr.push_back(currTile);
    }
    
    for (int j=0; j<buildingVo.grid_row; j++) {
        for (int i=0; i<_tileIndexesOfCurr.size(); i++) {
            if (j == 0) {
                int t = _tileIndexesOfCurr[i];
                _tileIndexes.push_back(t);
                continue;
            }
            int t = getTileDownFrom(_tileIndexesOfCurr[i]);
            _tileIndexes.push_back(t);
            _tileIndexesOfCurr[i] = t;
        }
    }
    
    if (_tileIndexes.size() != buildingVo.grid_row*buildingVo.grid_col) return false;
    
    for (int i=0; i<_tileIndexes.size(); i++) {
        if (isTileFree(_tileIndexes[i])) continue;
        return false;
    }
    
    int i=0;
    for (i=0; i<_tileIndexes.size(); i++) {
        tileIndexes[i] = _tileIndexes[i];
    }
    
    while (i<BUILDING_SPACE_GRID_NUM) {
        tileIndexes[i] = -1;
        i++;
    }
    
    return true;
    
}

bool IsoMap::isTileFree(int tileIndex) {
    
    if (tileIndex < 0 || tileIndex >= ISO_TILE_NUM) return false;
    return tilesMap[tileIndex] == NULL;
    
}

int IsoMap::getTrackIndexByCoord(CCPoint coord) {
    
    return int(coord.y*(ISO_TILE_COL/2)+coord.x);
    
}

int IsoMap::getTrackIndexByPosition(CCPoint position, CCSize size, CCPoint shift) {
    
    CCPoint coord = IsoToCoord(position, size*2, shift);
    //printf("%d,%d\n", (int)coord.x, (int)coord.y);
    return getTrackIndexByCoord(coord);
    
}

void IsoMap::_printTrackMap() {
    
    int i, j, k, n = 0, m = 0;
    for (j=0; j<ISO_TILE_ROW/2; j++) {
        for (i=0; i<ISO_TILE_COL/2; i++) {
            k = j*(ISO_TILE_COL/2)+i;
            //printf("[%d|%d],", tilesTrack[k], tilesObstacle[k]);
            CCLog_("%d,", tilesTrack[k]);
            if (tilesTrack[k] > 0) {
                n++;
            }
            if (tilesObstacle[k] > 0) {
                m++;
            }
        }
    }
    
        
    CCLog_("nTrackInMap: %d, nTrackInVector: %d", n+m, getNTrack());
    
}

bool IsoMap::isTrackComplete() {
    
    bool result  = _validateTrackCompletion();
    if (PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState) {
        if (Building* _gate = _mapPlayerState->getBuilding(kBuildingGate, 0)) {
            if (result) {
                _gate->spineBuilding->setAnimation(kBuildingGateAniOpen, true);
            }else {
                _gate->spineBuilding->setAnimation(kBuildingGateAniClose, true);
            }
        }
    }
    return result;
    
}

bool IsoMap::setAsStartTrack(Track *track_) {
    
    if (!track_) return false;
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;
    if (!_mapPlayerState) return false;
    
    
    if (track_->ptVo.no != 1 &&
        track_->ptVo.no != 3) return false;
    
    if (!track_) {
        track_ = _mapPlayerState->getTrackToStart(0);
    }
    
    // make sure that all tracks are not track start
    for (vector<Track*>::iterator it=_mapPlayerState->Tracks.begin(); it!=_mapPlayerState->Tracks.end(); ++it) {
        Track* _track = (Track*)(*it);
        if (_track == track_) continue;
        _track->trackStart = false;
        _track->updateSprite();
    }
    
    // set the track in holder as a start track
    track_->trackStart = true;
    track_->trackDirection = _mapPlayerState->player.racing.direction;
    track_->updateSprite();
    
    // indicate mapPlayerState
    _mapPlayerState->player.racing.start = track_->ptVo.index;
    
    return true;
    
}

bool IsoMap::setAsStartTrackIfLegal(Track *track) {
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;

    Track* start_track = NULL;
    
    for (vector<Track*>::iterator it=_mapPlayerState->Tracks.begin(); it!=_mapPlayerState->Tracks.end(); ++it) {
        Track* _track = (Track*)(*it);
        if (_track->trackStart) {
            start_track = _track;
            break;
        }
    }
    
    if (start_track) {
        _mapPlayerState->player.racing.start = start_track->ptVo.index;
        return false;
    }
    
    if (track->ptVo.no != 1 && track->ptVo.no != 3) {
        return false;
    }
    
    GDTrackVO vo = g_gameContent->getTrackVO(track->ptVo.type);
    if (vo.type == "") {
        return false;
    }
    
    if (vo.track_type != PT_OBJECT_TRACK) {
        return false;
    }
    
    track->trackStart = true;
    _mapPlayerState->player.racing.start = track->ptVo.index;

    return true;

}

bool IsoMap::updateTrackGlue() {
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;
    
    for (vector<Track*>::iterator it=_mapPlayerState->Tracks.begin(); it!=_mapPlayerState->Tracks.end(); ++it) {
        if (Track* track = (Track*)(*it)) {
            
            int _track_all = int(ISO_TILE_NUM/2);
            int _track_col = int(ISO_TILE_COL/2);
            int _trackIndex1 = track->ptVo.index;
            
            int _trackNeigborIndex[] = {
                _trackIndex1+1,
                _trackIndex1+_track_col,
                _trackIndex1-1,
                _trackIndex1-_track_col
            };
            
            if (_trackIndex1 % _track_col == _track_col) {
                _trackNeigborIndex[0] = -1;
            }
            
            if (_trackIndex1 > _track_all-_track_col) {
                _trackNeigborIndex[1] = -1;
            }
            
            if (_trackIndex1 % _track_col == 0) {
                _trackNeigborIndex[2] = -1;
            }
            
            if (_trackIndex1 < _track_col) {
                _trackNeigborIndex[3] = -1;
            }
            
            for (int i=0; i<4; i++) {
                if (_trackNeigborIndex[i] >= 0) {
                    
                    if (Track* track2 = getTrackAtIndex(_trackNeigborIndex[i])) {
                        
                        addTrackGlue(track, track2);
                        
                    }
                    
                }
            }
            
        }
    }
    
    removeTrackGlue();
    
    return true;
    
}

void IsoMap::switchTrackGlue() {
    
    for (vector<TrackGlue*>::iterator it = trackGlues.begin(); it != trackGlues.end(); ++it) {
        if (TrackGlue* _glue = (TrackGlue*)(*it)) {
            _glue->updateTrackGlueSprite();
        }
    }
    
}

void IsoMap::removeTrackGlue() {
    
    //remove glue that
    int n = trackGlues.size();
    
    for (int i=0; i<n; i++) {
        
        int j = 0;
        for (vector<TrackGlue*>::iterator it=trackGlues.begin(); it!=trackGlues.end(); ++it) {
            if (TrackGlue* trackGlue = (TrackGlue*)(*it)) {
                int k = 0;
                for (int l=0; l<2; l++) {
                    if (getTrackAtIndex(trackGlue->index[l])) {
                        k++;
                    }
                }
                if (k < 2) {
                    trackGlues[j] = trackGlues.back();
                    trackGlues.pop_back();
                    trackGlue->removeFromParentAndCleanup(true);
                    trackGlue = NULL;
                    break;
                }
            }
            j++;
        }
    
    }
    
}

int IsoMap::getStartTrackIndex() {
    
    PlayerState* _mapPlayerState = (PlayerState*)mapPlayerState;
    return _mapPlayerState->player.racing.start;
    
}

int IsoMap::getLegalTileSize() {
    
    return m_legalTileSize;
    
}

CCSize IsoMap::getAllLegalTileSize() {
    
    return m_allLegalTileSize;
    
}

CCPoint IsoMap::getAllLegalTilePos() {
    
    return m_allLegalTilePos;
    
}

void *IsoMap::getMapPlayerState() {
    
    return mapPlayerState;

}


void IsoMap::setLockCamera(bool lock) {

    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*g_currentScene) != typeid(SceneVille)) return;
    if (!_scene->scrollView) return;
    _scene->scrollView->setScrollEnabled(!lock);
    
}

#pragma mark - callback funtions

void IsoMap::onBuildingUpdated(void *sender, std::string respn_str) {
    
    PlayerState *_playerState = PlayerState::getInstance();
    //JsonAllocator allocator;
    Json*root = Json_create(respn_str.c_str());
    _playerState->parseChecklistData(root);
    _playerState->parseItemData(root);
    _playerState->parseBuildingData(root);
    _playerState->parseUserBalance(root);
    _playerState->parseUserRaceData(root);
    _playerState->parseAbilityData(root);
    Json_dispose(root);

    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*g_currentScene) != typeid(SceneVille)) return;
    _scene->hudPanel->updateHUD();
    _scene->hudPanel->updateHUDTrack(_scene->isoMap->isTrackComplete());
    _scene->hudPanel->updateAllResourceBars();
    _scene->hudPanel->updateBadgeAchievement();
    
    if (!_scene->shopPanel) return;
    _scene->shopPanel->setItemCount();
    
}

void IsoMap::onDecorationUpdated(void *sender, std::string respn_str) {
    
    PlayerState *_playerState = PlayerState::getInstance();
    //JsonAllocator allocator;
    Json*root = Json_create(respn_str.c_str());
    _playerState->parseDecorationData(root);
    _playerState->parseUserBalance(root);
    Json_dispose(root);
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*g_currentScene) != typeid(SceneVille)) return;
    _scene->hudPanel->updateHUD();
    _scene->hudPanel->updateAllResourceBars();
    
}
