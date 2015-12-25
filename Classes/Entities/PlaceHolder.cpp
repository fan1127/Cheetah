//
//  PlaceHolder.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/30/56 BE.
//
//

#include "PlaceHolder.h"
USING_NS_CC;
using namespace cocos2d::extension;
using namespace std;

extern float scaleFactor;

#include "../AppMacros.h"
extern TargetDevice targetDevice;

#include "IsoMap.h"

#include "../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "PlayerState.h"
extern PlayerState *g_playerState;

#include "../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../Services/BackendService.h"
extern BackendService *g_gameBackend;

extern float sizeMultiplier;
extern float retinaMultiplier;
extern float margin;

#include "../Extensions/CCMenuItemScaleSprite.h"
#include "../UIs/Dialogs/InstantDialog.h"

#include "../Helpers/CCSpriteHelper.h"

PlaceHolder* PlaceHolder::createWithBuilding(Building *building, HUDPanel* panel, PlaceHolderType type) {
    
    PlaceHolder *pRet = new PlaceHolder();
    pRet->initWithBuilding(building, panel, type);
    pRet->autorelease();
    return pRet;
    
}

PlaceHolder* PlaceHolder::createWithTrack(Track *track, HUDPanel* panel, PlaceHolderType type) {
    
    PlaceHolder *pRet = new PlaceHolder();
    pRet->initWithTrack(track, panel, type, false);
    pRet->autorelease();
    return pRet;
    
}

PlaceHolder* PlaceHolder::createWithTrack(Track *track, HUDPanel *panel, PlaceHolderType type, bool finishbtn) {
    
    PlaceHolder *pRet = new PlaceHolder();
    pRet->initWithTrack(track, panel, type, finishbtn);
    pRet->autorelease();
    return pRet;
    
}

PlaceHolder *PlaceHolder::createWithDecoration(Decoration *decoration_, HUDPanel *panel_, PlaceHolderType type_) {
    
    PlaceHolder *pRet = new PlaceHolder();
    pRet->initWithDecoration(decoration_, panel_, type_);
    pRet->autorelease();
    return pRet;
    
}

bool PlaceHolder::initWithBuilding(Building *building, HUDPanel* panel, PlaceHolderType type) {
    
    if (!CCNode::init()) {
        return false;
    }
    
    m_sender = NULL;
    
    m_gridLayer        = NULL;
    m_spineBuilding    = NULL;
    m_track            = NULL;
    m_trackStart       = NULL;
    m_trackDirection   = NULL;
    m_spineDecoration  = NULL;
    
    _gridSize = CCSizeZero;
    _tileSize = CCPointZero;
    _shift = CCPointZero;
    
    m_type = type;
    
    life = 0;
    state = 0;
    
    pbVo_recover = building->pbVo;
    this->building = building;
    
    ptVo_recover = PlayerTrackVO();
    this->track = NULL;
    
    pdVo_recover = PlayerDecorationVO();
    this->decoration = NULL;
    
    GDBuildingVO _bvo = g_gameContent->getBuildingVO(this->building->pbVo.type, this->building->pbVo.level);
    if (_bvo.type == "") {
        CCLog_("(PlaceHolder::initWithBuilding) Error: building=%s doesn't exist!", this->building->pbVo.type.c_str());
        return false;
    }
    
    this->building->removeFromParent();
    
    this->building->createBuilding();
    this->building->setPosition(CCPointZero);
    this->addChild(this->building, 2);
    this->building->updateProgressBarVisibility();
    
    if (this->building->pet) {
        this->building->pet->reload();
        this->building->pet->resumeAction();
        this->building->pet->scheduleUpdate();
    }
    
    if (this->building->pbVo.flip) {
        _composeGrid(_bvo.grid_row, _bvo.grid_col);
    }else {
        _composeGrid(_bvo.grid_col, _bvo.grid_row);
    }
    
    if (panel) {
        composeMenu(panel, false);
    }
    _active = true;
    
    //@june
    camLock = true;
    _diffOffset = CCPoint(0.0f, 30.0f);
    scheduleUpdate();
    
    return true;
    
}

bool PlaceHolder::initWithTrack(Track* track, HUDPanel* panel, PlaceHolderType type, bool finishbtn) {
    
    if (!CCNode::init()) {
        return false;
    }
    
    _gridSize = CCSizeZero;
    _tileSize = CCPointZero;
    _shift = CCPointZero;
    
    m_type = type;
    life = 0;
    state = 0;
    
    pbVo_recover = PlayerBuildingVO();
    this->building = NULL;
    
    ptVo_recover = track->ptVo;
    trackStart_recover = track->trackStart;
    this->track = track;
    
    GDTrackVO _tvo = g_gameContent->getTrackVO(track->ptVo.type);
    if (_tvo.type == "") {
        CCLog_("(PlaceHolder::initWithTrack) Error: track=%s doesn't exist!", track->ptVo.type.c_str());
        return false;
    }
    
    pdVo_recover = PlayerDecorationVO();
    this->decoration = NULL;
    
    this->track->removeFromParent();
    this->track->setPosition(CCPointZero);
    if (_tvo.track_type == PT_OBJECT_TRACK) {
        this->addChild(this->track, 0);
    } else {
        this->addChild(this->track, 2);
    }
    this->track->scheduleUpdate();
    
    _composeGrid(2, 2);
    composeMenu(panel, finishbtn);
    
    _active = true;
    
    //@june
    camLock = true;
    _diffOffset = CCPoint(0.0f, 50.0f);
    
    
    scheduleUpdate();
    
    return true;
    
}

bool PlaceHolder::initWithDecoration(Decoration *decoration_, HUDPanel *panel_, PlaceHolderType type_) {
    
    if (!CCNode::init()) {
        return false;
    }
    
    _gridSize = CCSizeZero;
    _tileSize = CCPointZero;
    _shift = CCPointZero;
    
    m_type = type_;
    life = 0;
    state = 0;
    
    pbVo_recover = PlayerBuildingVO();
    this->building = NULL;
    
    ptVo_recover = PlayerTrackVO();
    this->track = NULL;
    
    pdVo_recover = decoration_->pdVo;
    this->decoration = decoration_;
    
    GDDecorationVO _dvo = g_gameContent->getDecorationVO(this->decoration->pdVo.type);
    if (_dvo.type == "") {
        CCLog_("(PlaceHolder::initWithDecoration) Error: decoration=%s doesn't exist!", pdVo_recover.type.c_str());
        return false;
    }
   
    this->decoration->removeFromParent();
    this->decoration->createDecoration();
    this->decoration->setPosition(CCPointZero);
    this->addChild(this->decoration, 2);
    
    if (this->decoration->pdVo.flip) {
        _composeGrid(_dvo.grid_row, _dvo.grid_col);
    }else {
        _composeGrid(_dvo.grid_col, _dvo.grid_row);
    }
    composeMenu(panel_, false);
    
    _active = true;
    
    //@june
    camLock = true;
    _diffOffset = CCPoint(0.0f, 30.0f);
    scheduleUpdate();
    
    return true;
    
}

PlaceHolder::~PlaceHolder() {
    
    if (m_clippingLayer) {
        m_clippingLayer->removeFromParentAndCleanup(true);
        m_clippingLayer = NULL;
    }
    
}

void PlaceHolder::onEnter() {
    
    CCDirector::sharedDirector()->getTouchDispatcher()->addStandardDelegate(this, 0);
    CCNode::onEnter();
    
}

void PlaceHolder::onExit() {
    
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    CCNode::onExit();
    
}

void updateItemTrack(Building* building_association_, ItemTrack* item_track_) {
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    CREATE_CHAR_BUFFER(_buffer2, 128);
    
    if (building_association_) {
        if (building_association_->pbVo.level >= item_track_->vo.unlocks_at_level ||
            item_track_->vo.unlocks_at_level == 1) {
            item_track_->lock(false, "");
        }else {
            if (building_association_->pbVo.status != BUILDING_STATUS_DELIVERED) {
                snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["finish_building_now_to_unlock"].c_str());
                snprintf(_buffer2, _buffer2_size, _buffer, building_association_->bVo.title.c_str());
                snprintf(_buffer, _buffer_size, "%s", _buffer2);
            }else {
                snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["upgrade_building_to_unlock"].c_str());
                snprintf(_buffer2, _buffer2_size, _buffer, building_association_->bVo.title.c_str(), item_track_->vo.unlocks_at_level);
                snprintf(_buffer, _buffer_size, "%s", _buffer2);
            }
            item_track_->lock(true, _buffer);
        }
    }else {
        GDBuildingVO _bvo = g_gameContent->getBuildingVO(kBuildingAssociation, 1);
        if (_bvo.type != "") {
            if (item_track_->vo.unlocks_at_level >= 2) {
                snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["build_and_upgrade_building_to_unlock"].c_str());
                snprintf(_buffer2, _buffer2_size, _buffer, _bvo.title.c_str(), item_track_->vo.unlocks_at_level);
                snprintf(_buffer, _buffer_size, "%s", _buffer2);
                item_track_->lock(true, _buffer);
            }
        }
    }
    
    if (!item_track_->isLock())
    {
        g_playerState->calculateTrackpointsUsed();
        if (g_playerState->player.racing._track_points + item_track_->vo.point_used > g_playerState->player.racing._track_points_limit) {
            item_track_->lock(true, g_gameContent->Word["not_enough_trackpoints"].c_str());
        }
    }
}

bool PlaceHolder::composeMenu(HUDPanel* panel, bool finish)
{
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    if (m_clippingLayer) {
        m_clippingLayer->release();
        m_clippingLayer->removeFromParent();
    }
    panel->menuPlaceHolderLayer->removeAllChildrenWithCleanup(true);
    
    switch (m_type) {
            
        default:
            break;
            
        case PHT_NEW_BUILDING: {
            
            // Yes Button
            CCSprite *_btnYes = UIHelper::getSprite(UIHelper::TraceBuildingsOptionsOkButton);
            menuItemYes = CCMenuItemScaleSprite::create(_btnYes, this, menu_selector(PlaceHolder::place));
            menuYes = CCMenu::create(menuItemYes, NULL);
            menuYes->setContentSize(_btnYes->getContentSize());
            menuYes->setPosition(ccp(100*retinaMultiplier, building->bVo.src_height*retinaMultiplier*0.75f));
            addChild(menuYes, 3);
            
            // No Button
            CCSprite *_btnNo = UIHelper::getSprite(UIHelper::TraceBuildingsOptionsCancelButton);
            menuItemNo = CCMenuItemScaleSprite::create(_btnNo, this, menu_selector(PlaceHolder::cancel));
            menuItemNo->setAnchorPoint(ccp(0.5f, 0.5f));
            menuNo = CCMenu::create(menuItemNo, NULL);
            menuNo->setPosition(ccp(-100*retinaMultiplier, building->bVo.src_height*retinaMultiplier*0.75f));
            addChild(menuNo, 3);
            
            menuSet = NULL;
            
        }break;
            
        case PHT_NEW_DECORATION: {
        
            // Yes Button
            CCSprite *_btnYes = UIHelper::getSprite(UIHelper::TraceBuildingsOptionsOkButton);
            menuItemYes = CCMenuItemScaleSprite::create(_btnYes, this, menu_selector(PlaceHolder::place));
            menuYes = CCMenu::create(menuItemYes, NULL);
            menuYes->setPosition(ccp(100*retinaMultiplier, decoration->dVo.src_height*retinaMultiplier*0.75f));
            addChild(menuYes, 3);
            
            // No Button
            CCSprite *_btnNo = UIHelper::getSprite(UIHelper::TraceBuildingsOptionsCancelButton);
            menuItemNo = CCMenuItemScaleSprite::create(_btnNo, this, menu_selector(PlaceHolder::cancel));
            menuItemNo->setAnchorPoint(ccp(0.5f, 0.5f));
            menuNo = CCMenu::create(menuItemNo, NULL);
            menuNo->setPosition(ccp(-100*retinaMultiplier, decoration->dVo.src_height*retinaMultiplier*0.75f));
            addChild(menuNo, 3);
            
            menuSet = NULL;
            
        }break;
            
        case PHT_EDT_BUILDING: {
            
        }break;
            
        case PHT_NEW_TRACK: 
        case PHT_EDT_TRACK: {
            
            CCMenu* _previous_button = NULL;
            
            // Button Add
            CCSprite *_btnYes = UIHelper::getSprite(UIHelper::TraceBuildingsOptionsAddButton);
            CCMenuItemSprite *_menuItemAdd = CCMenuItemScaleSprite::create(_btnYes, this, menu_selector(PlaceHolder::place));
            menuYes = CCMenu::create(_menuItemAdd, NULL);
            menuYes->setContentSize(_btnYes->getContentSize());
            if (panel) {
                CCPoint t_init = CCPoint(_screenSize.width-(_btnYes->getContentSize().width/2)-margin,-(_btnYes->getContentSize().height/2));
                CCPoint t_dest = CCPoint(_screenSize.width-(_btnYes->getContentSize().width/2)-margin, (_btnYes->getContentSize().height/2)+margin*2);
                if (finish) {
                    menuYes->setPosition(t_dest);
                }else {
                    menuYes->stopAllActions();
                    menuYes->setPosition(t_init);
                    menuYes->runAction(CCEaseElasticOut::create(CCMoveTo::create(PLACEHOLDER_MENU_MOVING_SPEED, t_dest), PLACEHOLDER_MENU_EASING_SPEED));
                }
                panel->menuPlaceHolderLayer->addChild(menuYes, 3);
            }
            _previous_button = menuYes;

            bgMenuSprite = CCPhatTouchableSprite::createWithSpriteFrameName(UIHelper::getSpritePath(UIHelper::TrackMenuBg));
            bgMenuSprite->setScaleX(_screenSize.width);
            bgMenuSprite->setAnchorPoint(CCPointZero);
            bgMenuSprite->setPosition(CCPointMake(0.0f, -12.0f * sizeMultiplier));
            bgMenuSprite->onHit = onHit;

            if (panel)
            {
                if (finish)
                {
//                    bgMenuSprite->setOpacity(128);
                }
                else
                {
                    bgMenuSprite->stopAllActions();
                    bgMenuSprite->setOpacity(0);
                    bgMenuSprite->runAction(CCFadeTo::create(PLACEHOLDER_MENU_MOVING_SPEED, 255));
                }
                panel->menuPlaceHolderLayer->addChild(bgMenuSprite, 2);
            }
            
            // Button Remove
            CCSprite *_btnNo = UIHelper::getSprite(UIHelper::TraceBuildingsOptionsDeleteButton);
            CCMenuItemSprite *_menuItemRemove = CCMenuItemScaleSprite::create(_btnNo, this, menu_selector(PlaceHolder::remove));
            menuNo = CCMenu::create(_menuItemRemove, NULL);
            menuNo->setContentSize(_btnNo->getContentSize());
            if (panel) {
                CCPoint t_init = CCPoint(_previous_button->getPositionX()-_previous_button->getContentSize().width*0.5f-margin-(_btnNo->getContentSize().width/2),-(_btnNo->getContentSize().height/2));
                CCPoint t_dest = CCPoint(_previous_button->getPositionX()-_previous_button->getContentSize().width*0.5f-margin-(_btnNo->getContentSize().width/2), (_btnNo->getContentSize().height/2)+margin*2);
                if (finish) {
                    menuNo->setPosition(t_dest);
                }else {
                    menuNo->stopAllActions();
                    menuNo->setPosition(t_init);
                    menuNo->runAction(CCSequence::create(CCDelayTime::create(PLACEHOLDER_MENU_DELAY), CCEaseElasticOut::create(CCMoveTo::create(PLACEHOLDER_MENU_MOVING_SPEED, t_dest), PLACEHOLDER_MENU_EASING_SPEED), NULL));
                }
                panel->menuPlaceHolderLayer->addChild(menuNo, 3);
            }
            _previous_button = menuNo;
            
            // Button Switch
            CCSprite *_btnSwitch = UIHelper::getSprite(UIHelper::TraceBuildingsOptionsRotateButton);
            CCMenuItemSprite *_menuItemSwitch = CCMenuItemScaleSprite::create(_btnSwitch, this, menu_selector(PlaceHolder::flip));
            menuSwitch = CCMenu::create(_menuItemSwitch, NULL);
            menuSwitch->setContentSize(_btnSwitch->getContentSize());
            if (panel) {
                CCPoint t_init = CCPoint(_previous_button->getPositionX()-_previous_button->getContentSize().width*0.5f-margin-(_btnSwitch->getContentSize().width/2),-(_btnSwitch->getContentSize().height/2));
                CCPoint t_dest = CCPoint(_previous_button->getPositionX()-_previous_button->getContentSize().width*0.5f-margin-(_btnSwitch->getContentSize().width/2), (_btnSwitch->getContentSize().height/2)+margin*2);
                if (finish) {
                    menuSwitch->setPosition(t_dest);
                }else {
                    menuSwitch->stopAllActions();
                    menuSwitch->setPosition(t_init);
                    menuSwitch->runAction(CCSequence::create(CCDelayTime::create(PLACEHOLDER_MENU_DELAY*2), CCEaseElasticOut::create(CCMoveTo::create(PLACEHOLDER_MENU_MOVING_SPEED, t_dest), PLACEHOLDER_MENU_EASING_SPEED), NULL));
                }
                panel->menuPlaceHolderLayer->addChild(menuSwitch, 3);
            }
            _previous_button = menuSwitch;
            
            // Set Button
            CCSprite *_btnSet = NULL;
            if (track->trackStart)
            {
                _btnSet = UIHelper::getSprite(UIHelper::TraceBuildingsOptionsReverseButton);
            }
            else
            {
                _btnSet = UIHelper::getSprite(UIHelper::TraceBuildingsOptionsStartButton);
            }

            CCMenuItemSprite *_menuItemSet = CCMenuItemScaleSprite::create(_btnSet, this, menu_selector(PlaceHolder::setStartOrChangeDirection));
            menuSet = CCMenu::create(_menuItemSet, NULL);
            menuSet->setContentSize(_btnSet->getContentSize());

            if (panel) {
                CCPoint t_init = CCPoint(_previous_button->getPositionX()-_previous_button->getContentSize().width*0.5f-margin-(_btnSet->getContentSize().width/2),-(_btnSet->getContentSize().height/2));
                CCPoint t_dest = CCPoint(_previous_button->getPositionX()-_previous_button->getContentSize().width*0.5f-margin-(_btnSet->getContentSize().width/2), (_btnSet->getContentSize().height/2)+margin*2);
                if (finish) {
                    menuSet->setPosition(t_dest);
                }else {
                    menuSet->stopAllActions();
                    menuSet->setPosition(t_init);
                    menuSet->runAction(CCSequence::create(CCDelayTime::create(PLACEHOLDER_MENU_DELAY*3), CCEaseElasticOut::create(CCMoveTo::create(PLACEHOLDER_MENU_MOVING_SPEED, t_dest), PLACEHOLDER_MENU_EASING_SPEED), NULL));
                }
                panel->menuPlaceHolderLayer->addChild(menuSet, 3);
            }
            _previous_button = menuSet;
            
            
            ////////////////////////////////////////////////////
            // Track Selection
            if (!m_clippingLayer) {
                
                m_clippingLayer = CCClippingLayer::create();
                m_clippingLayer->retain();
                m_clippingLayer->setAnchorPoint(ccp(0, 0));
                m_clippingLayer->preferenceRootParent = panel->menuPlaceHolderLayer;
                m_clippingLayer->setContentSize(CCSize(_previous_button->getPositionX()-(_previous_button->getContentSize().width*0.5f)-(margin*2),
                                                       bgMenuSprite->getContentSize().height-margin*2));
                m_clippingLayer->setPosition(ccp(margin, margin));
                
                m_scrollView = CCNikScrollView::create();
                m_scrollView->setBounce(true);
                m_scrollView->setAlwaysBounceVertical(false);
                m_scrollView->setAlwaysBounceHorizontal(true);
                m_scrollView->setTouchMode(kCCTouchesOneByOne);
                m_scrollView->setContentSize(m_clippingLayer->getContentSize());
                m_clippingLayer->addChild(m_scrollView);
                
                int i=0;
                ItemTrack* _last_item = NULL;
                m_item_tracks.clear();
                
                Building* _building_association = g_playerState->getBuilding(kBuildingAssociation, 0);
                
                for (vector<GDTrackVO>::iterator it = g_gameContent->Track.begin(); it != g_gameContent->Track.end(); ++it) {
                    GDTrackVO _tvo = (GDTrackVO)(*it);
                    if (_tvo.type != "" && _tvo.track_type == "track") {
                        
                        ItemTrack* _item_track = ItemTrack::create(_tvo);
                        _item_track->setPosition(ccp(m_scrollView->getContentSize().height*0.5f + 30*sizeMultiplier, m_scrollView->getContentSize().height*0.5f) +
                                                 ccp(i*(_item_track->getContentSize().width+margin), 0.0f));
                        _item_track->setTarget(this, menu_selector(PlaceHolder::selectTrack), CCNikScrollViewButtonStateTouchUp);
                        updateItemTrack(_building_association, _item_track);
                        m_scrollView->addChild(_item_track);
                        m_item_tracks.push_back(_item_track);
                        if (track->ptVo.type == _tvo.type) {
                            _item_track->select(true);
                        }
                        
                        _last_item = _item_track;
                        i++;
                
                    }
                }
                
                if (_last_item) {
                    m_scrollView->setContentView(CCSize(_last_item->getPositionX()+_last_item->getContentSize().width*0.5f+margin, m_scrollView->getContentSize().height));
                }

            }
            
            if (!finish) {
                CCModalController::showAllChildrensAnimated(m_clippingLayer, PLACEHOLDER_MENU_MOVING_SPEED);
            }
            panel->menuPlaceHolderLayer->addChild(m_clippingLayer, 3);
            
            updateMenu(panel, true);
            
        }break;
            
        case PHT_NEW_OBSTACLE:
        case PHT_EDT_OBSTACLE: {
            
        }break;
            
        case PHT_EDT_DECORATION: {
            
        }break;
            
    }
    
    return true;
    
}

void PlaceHolder::updateMenu(HUDPanel* panel, bool ignorechange) {
    
    switch (m_type) {
        case PHT_NEW_TRACK:
        case PHT_EDT_TRACK:
            
            if (this->track) {
                
                if (this->track->trackStart) {
                    
                    if (!ignorechange) {
                        
                        CCPoint t_pos = menuSet->getPosition();
                        
                        if (menuSet) {
                            menuSet->removeFromParentAndCleanup(true);
                            menuSet = NULL;
                        }
                        
                        // Set Button
                        CCSprite *_btnSet = UIHelper::getSprite(UIHelper::TraceBuildingsOptionsReverseButton);
                        CCMenuItemSprite *_menuItemSet = CCMenuItemScaleSprite::create(_btnSet, this, menu_selector(PlaceHolder::setStartOrChangeDirection));
                        menuSet = CCMenu::create(_menuItemSet, NULL);
                        
                        if (panel) {
                            menuSet->setPosition(t_pos);
                            panel->menuPlaceHolderLayer->addChild(menuSet, 3);
                        }
                        
                    }
                    
                } else {
                        CCMenuItemSprite *_menuItemSet = (CCMenuItemSprite*)menuSet->getChildren()->objectAtIndex(0);
                        if (_menuItemSet) {
                            CCSprite *_btnSet = NULL;
                            if (this->track->ptVo.no != 1 && this->track->ptVo.no != 3) {
                            }else {
                            }
                            _btnSet = UIHelper::getSprite(UIHelper::TraceBuildingsOptionsStartButton);
                            _menuItemSet->setNormalImage(_btnSet);
                        }
                }
                
                Building* _building_association = g_playerState->getBuilding(kBuildingAssociation, 0);
                for (vector<ItemTrack*>::iterator it=m_item_tracks.begin(); it!=m_item_tracks.end(); ++it) {
                    if (ItemTrack* _item_track = (ItemTrack*)(*it)) {
                        updateItemTrack(_building_association, _item_track);
                    }
                }
                
            }
            break;
            
        default:
            break;
    }
    
}

bool PlaceHolder::addMoreTrack(Track *track, HUDPanel* panel) {
    
    if (this->track) {
        CCLog_("(PlaceHolder::addMoreTrack) Warning: Placeholder is unavailable, track exists!");
        return false;
    }
    
    ptVo_recover = PlayerTrackVO();
    trackStart_recover = false;
    
    this->track = track;
    this->track->removeFromParent();
    this->addChild(this->track, 0);
    this->track->scheduleUpdate();
    
    return true;
    
}

void PlaceHolder::update(float delta) {
    
    didBtnPressed = true;
    
    if (!building && !track && !decoration) {
        camLock = false;
        return;
    }
    
    

    // Blink The Grid -----------------------
    static float _t_runner = 0;
    _t_runner += 0.05f;
    if (_t_runner>M_PI*2) {
        _t_runner = 0;
    }

    if (this->building || this->decoration) {
        
        float _t_opacity = fabsf(sinf(_t_runner)*105) + 150;
        for (int i=0; i<GRID_HOLDER_MAX; i++) {
            if (m_grid[i]) {
                m_grid[i]->setOpacity(_t_opacity);
            }
        }
        
    } else if (this->track) {

        float _t_opacity = fabsf(sinf(_t_runner)*128) + 50;
        for (int i=0; i<GRID_HOLDER_MAX; i++) {
            if (m_grid[i]) {
                m_grid[i]->setOpacity(_t_opacity);
            }
        }
    }
    
    // ------------------------------------------
//    if (life < 100) {
        life = 1;
//    }


}

void PlaceHolder::ccTouchesBegan(CCSet* touches, CCEvent* event) {
    
    if (touches->count() != 1) return;
    
    CCPoint location = ((CCTouch*)(*touches->begin()))->getLocation();
    location = this->getParent()->convertToNodeSpace(location);
    
    CCRect itemRect = CCRect(this->getPosition().x - (this->getContentSize().width/2),
                             this->getPosition().y - (this->getContentSize().height/2),
                             this->getContentSize().width,
                             this->getContentSize().height);
    
    if(itemRect.intersectsRect(CCRect(location.x, location.y, 1, 1))) {
        camLock = true;
        if (onBegin) {
            onBegin(m_sender);
        }
        
        //@june update offset
        _diffOffset = CCPoint(this->getPosition().x-location.x, this->getPosition().y-location.y);
        
    }else {
        
        switch (m_type) {
            case PHT_EDT_BUILDING:
            case PHT_NEW_TRACK:
            case PHT_EDT_TRACK:
            case PHT_NEW_OBSTACLE:
            case PHT_EDT_OBSTACLE:
            case PHT_EDT_DECORATION:
                didBtnPressed = false;
                if (bgMenuSprite) {
                    if (bgMenuSprite->numberOfRunningActions() > 0) {
                        return;
                    }
                }
                this->place(this);
                return;
                
                break;
                
            default:
                break;
                
        }
        
    }
    
}

void PlaceHolder::ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
    
    if (touches->count() != 1) return;
    
    CCPoint location = ((CCTouch*)(*touches->begin()))->getLocation();
    location = this->getParent()->convertToNodeSpace(location);
    
    if (camLock) {
        switch (m_type) {
            default:
            case PHT_NEW_BUILDING:
            case PHT_EDT_BUILDING:
            case PHT_NEW_DECORATION:
            case PHT_EDT_DECORATION:
                this->snapWithGrid(location);
                break;
            case PHT_NEW_TRACK:
            case PHT_EDT_TRACK:
                this->snapWithQuadGrid(location);
                break;
            case PHT_NEW_OBSTACLE:
            case PHT_EDT_OBSTACLE:
                this->snapWithQuadGrid(location);
                this->flip(this);
                break;
        }
        
    }
    
    state = 1;
    
}

void PlaceHolder::ccTouchesEnded(CCSet* touches, CCEvent* event) {
    
    _touchEnded(touches, event);
    
}

void PlaceHolder::ccTouchesCancelled(CCSet* touches, CCEvent* event) {
    
     _touchEnded(touches, event);
    
}

void PlaceHolder::_touchEnded(CCSet* touches, CCEvent* event) {
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*_scene) != typeid(SceneVille))
        return;
    
    if (_scene->shopPanel) {
        _scene->shopPanel->removeCCTouchDelegateFromScrollView();
    }
    
    done();

}

void PlaceHolder::done() {
    
    if (onDone) {
        onDone(m_sender);
    }
    
    camLock = false;
    
}

void PlaceHolder::_composeGrid(int col, int row) {
    
    m_gridLayer = CCLayer::create();
    m_gridLayer->setContentSize(this->getContentSize());
    this->addChild(m_gridLayer, 1);
    
    if (col > BUILDING_SPACE_COL_MAX) {
        col = BUILDING_SPACE_COL_MAX;
    }
    if (row > BUILDING_SPACE_ROW_MAX) {
        row = BUILDING_SPACE_ROW_MAX;
    }
    
    _gridSize = CCSize(col, row);
    
    int i, j, k;
    
    for (k = 0; k<GRID_HOLDER_MAX; k++) {
        if (m_grid[k]) {
            m_grid[k]->removeFromParentAndCleanup(true);
            m_grid[k] = NULL;
        }
    }
    
    CCPoint _left_top = CCPointZero;
    CCPoint _right_bottom = CCPointZero;
    CCSize _allGridSize = CCSizeZero;
    bool _make_snap = false;

    for (j=0; j<row; j++) {
        for (i=0; i<col; i++) {
            k = (j*col)+i;
            
            m_grid[k] = CCSprite::createWithSpriteFrameName("grid_road_top.png");
            
            if (k==0) {
                setContentSize(CCSize(m_grid[0]->getContentSize().width*col, m_grid[0]->getContentSize().height*row));
            }
            m_grid[k]->setPosition(ccp(
                                       (i*m_grid[k]->getContentSize().width*0.5f)-(-j*m_grid[k]->getContentSize().width*0.5f),
                                       (i*m_grid[k]->getContentSize().height*0.5f)-(j*m_grid[k]->getContentSize().height*0.5f)
                                       ));
            if (_left_top.x > m_grid[k]->getPositionX()) {
                _left_top.x = m_grid[k]->getPositionX();
            }
            if (_right_bottom.x < m_grid[k]->getPositionX()) {
                _right_bottom.x = m_grid[k]->getPositionX();
            }
            
            if (_left_top.y > m_grid[k]->getPositionY()) {
                _left_top.y = m_grid[k]->getPositionY();
            }
            if (_right_bottom.y < m_grid[k]->getPositionY()) {
                _right_bottom.y = m_grid[k]->getPositionY();
            }
            
            if (col != row) {
                m_grid[k]->setPositionX(m_grid[k]->getPositionX()-m_grid[k]->getContentSize().width*0.25f);
            }
            
            m_gridLayer->addChild(m_grid[k]);
            
        }
    }
    
    _allGridSize = _right_bottom - _left_top;
    
    for (i = 0; i<GRID_HOLDER_MAX; i++) {
        if (m_grid[i]) {
            m_grid[i]->setPositionX(m_grid[i]->getPositionX()-(_allGridSize.width/2));
            if (m_grid[i]->getPositionX() == -m_grid[i]->getContentSize().width/2 || m_grid[i]->getPositionX() == m_grid[i]->getContentSize().width/2) {
                if (m_grid[i]->getPositionY() == -m_grid[i]->getContentSize().height/2 || m_grid[i]->getPositionY() == m_grid[i]->getContentSize().height/2) {
                    _make_snap = true;
                }
            }
        }
    }
    
    if (_make_snap) {
        
        for (i = 0; i<GRID_HOLDER_MAX; i++) {
            if (m_grid[i] && this->decoration) {
                if (this->decoration->pdVo.flip) {
                    m_grid[i]->setPositionY(m_grid[i]->getPositionY()+(m_grid[i]->getContentSize().height*0.5f));
                }else {
                    m_grid[i]->setPositionY(m_grid[i]->getPositionY()-(m_grid[i]->getContentSize().height*0.5f));
                }
            }
        }
        
    }
    
    if (col != row && this->decoration) {
        
        this->decoration->setPositionX(-m_grid[0]->getContentSize().width*0.25f);
        if (this->decoration->pdVo.flip) {
            this->decoration->setPositionY(+m_grid[0]->getContentSize().height*0.25f);
        }else {
            this->decoration->setPositionY(-m_grid[0]->getContentSize().height*0.25f);
        }
        
    }
    
    for (int i=0; i<4; i++) {
        if (!m_arrowSprites[i] ) {
            if (i == 0 || i == 1) {
                m_arrowSprites[i] = CCSprite::createWithSpriteFrameName("arrow1.png");
            }else {
                m_arrowSprites[i] = CCSprite::createWithSpriteFrameName("arrow2.png");
            }
            m_gridLayer->addChild(m_arrowSprites[i]);
        }
    }
    m_arrowSprites[0]->setPosition(ccp(-(g_gameContent->tileSize.width*col*0.25f+g_gameContent->tileSize.width*0.5f),-(g_gameContent->tileSize.height*row*0.25f+g_gameContent->tileSize.height*0.5f)));
    m_arrowSprites[1]->setPosition(ccp( (g_gameContent->tileSize.width*col*0.25f+g_gameContent->tileSize.width*0.5f),-(g_gameContent->tileSize.height*row*0.25f+g_gameContent->tileSize.height*0.5f))); m_arrowSprites[1]->setScaleX(-1);
    m_arrowSprites[2]->setPosition(ccp(-(g_gameContent->tileSize.width*col*0.25f+g_gameContent->tileSize.width*0.5f), (g_gameContent->tileSize.height*row*0.25f+g_gameContent->tileSize.height*0.5f)));
    m_arrowSprites[3]->setPosition(ccp( (g_gameContent->tileSize.width*col*0.25f+g_gameContent->tileSize.width*0.5f), (g_gameContent->tileSize.height*row*0.25f+g_gameContent->tileSize.height*0.5f))); m_arrowSprites[3]->setScaleX(-1);
    
}

void PlaceHolder::hideButton() {
    
    m_gridLayer->setVisible(false);
    
    if (building) {
//        if(g_gameTutorial->tutorialMode) {
//            g_gameTutorial->clearNodeAndKeepTouch();
//            g_gameTutorial->clearButtonEffect();
//        }
        
        building->removeFromParent();
    }
    
//    if (track) {
//        track->removeFromParent();
//    }
    
    if (decoration) {
        decoration->removeFromParent();
    }
    
    if (bgMenuSprite) {
        bgMenuSprite->stopAllActions();
        bgMenuSprite->runAction(CCFadeTo::create(PLACEHOLDER_MENU_MOVING_SPEED*0.25f, 0));
    }
    
    if (m_clippingLayer) {
        m_clippingLayer->stopAllActions();
        CCModalController::hideAllChildrensAnimated(m_clippingLayer, PLACEHOLDER_MENU_MOVING_SPEED*0.25f);
    }
    
    bool _release_no_button = true;
    
    if (menuSet) {
        CCPoint distp_ = CCPoint(menuSet->getPosition().x,-(menuSet->getContentSize().height*0.5f));
        menuSet->stopAllActions();
        menuSet->runAction(CCMoveTo::create(PLACEHOLDER_MENU_MOVING_SPEED*0.25f, distp_));
        _release_no_button = false;
    }
    
    if (menuSwitch) {
        CCPoint distp_ = CCPoint(menuSwitch->getPosition().x,-(menuSwitch->getContentSize().height*0.5f));
        menuSwitch->stopAllActions();
        menuSwitch->runAction(CCSequence::create(CCDelayTime::create(PLACEHOLDER_MENU_DELAY), CCMoveTo::create(PLACEHOLDER_MENU_MOVING_SPEED*0.25f, distp_), NULL));
        _release_no_button = false;
    }
    
    if (menuNo) {
        CCPoint distp_ = CCPoint(menuNo->getPosition().x,-(menuNo->getContentSize().height*0.5f));
        CCAction* _action = NULL;
        if (m_type == PHT_EDT_OBSTACLE || m_type == PHT_NEW_OBSTACLE) {
            _action = CCSequence::create(CCDelayTime::create(PLACEHOLDER_MENU_DELAY*2),
                                         CCMoveTo::create(PLACEHOLDER_MENU_MOVING_SPEED*0.25f, distp_),
                                         CCCallFuncN::create(this, callfuncN_selector(PlaceHolder::tellParentToReleaseThis)), NULL);
        }else {
            _action = CCSequence::create(CCDelayTime::create(PLACEHOLDER_MENU_DELAY*2),
                                         CCMoveTo::create(PLACEHOLDER_MENU_MOVING_SPEED*0.25f, distp_), NULL);
        }
        menuNo->stopAllActions();
        menuNo->runAction(_action);
        _release_no_button = false;
    }
    
    if (menuYes) {
        CCPoint distp_ = CCPoint(menuYes->getPosition().x,-(menuYes->getContentSize().height/2));
        menuYes->stopAllActions();
        menuYes->runAction(CCSequence::create(CCDelayTime::create(PLACEHOLDER_MENU_DELAY*3),
                                              CCMoveTo::create(PLACEHOLDER_MENU_MOVING_SPEED*0.25f, distp_),
                                              CCCallFuncN::create(this, callfuncN_selector(PlaceHolder::tellParentToReleaseThis)), NULL));
        _release_no_button = false;
    }
    
    if (onDone) {
        onDone(m_sender);
    }
    
    if (_release_no_button) {
        tellParentToReleaseThis(this);
    }
    
}

void PlaceHolder::tellParentToReleaseThis(CCNode* sender) {
    
    if (SceneVille* _scene = (SceneVille*)g_currentScene) {
        ((IsoMapBuilding*)_scene->isoMap)->removePlaceHolder();
    }
}

bool PlaceHolder::isOnTrack() {
    
    SceneVille* _scene = (SceneVille*)g_currentScene;

    CCPoint t_coord = IsoMap::IsoToCoord(this->getPosition(), _tileSize*2, _shift);
    int t_tileIndex = int(t_coord.y*(ISO_TILE_COL/2)+t_coord.x);
    
    if(_scene->isoMap->tilesTrack[t_tileIndex] > 0) {
        return true;
    }
    
    return false;
    
}

bool PlaceHolder::canPlace() {
    
    bool placing = false;

    switch (m_type) {
        case PHT_NEW_BUILDING:
        case PHT_NEW_OBSTACLE:
        case PHT_NEW_DECORATION:
            if (_active) {
                placing = true;
            }
            break;
        case PHT_EDT_BUILDING:
        case PHT_NEW_TRACK:
        case PHT_EDT_TRACK:
        case PHT_EDT_OBSTACLE:
        case PHT_EDT_DECORATION:
            placing = true;
            break;
    }
    
    return placing;// && enough_coin;
    
}

bool PlaceHolder::isActive() {
    
    return _active;
    
}

bool PlaceHolder::revert() {
    
    if (this->building) {
        this->building->pbVo = pbVo_recover;
        if (this->building->pbVo.map[0] >= 0) {
            
            CCLog_("Revert");
                        
            this->building->setPosition(IsoMap::getPositionByMapIndexes(this->building->pbVo.map, _tileSize, _shift));
        }
        return true;
    }
    
    if (this->track) {
        this->track->ptVo = ptVo_recover;
        this->track->updateSprite();
        return true;
    }
    
    if (this->decoration) {
        this->decoration->pdVo = pdVo_recover;
        if (this->decoration->pdVo.map[0] >= 0) {
            this->decoration->setPosition(IsoMap::getPositionByMapIndexes(this->decoration->pdVo.map, _tileSize, _shift));
        }
        return true;
    }
    
    return false;
    
}

void PlaceHolder::setHolder(CCPoint tilesize, CCPoint shift) {
    
    _tileSize = tilesize;
    _shift = shift;
    
    if (onBegin) {
        onBegin(m_sender);
    }
    
}

void PlaceHolder::snapWithGrid(CCPoint location) {
    
    CCPoint tileToPoint = CCPoint(location.x+_diffOffset.x, location.y+_diffOffset.y);
    CCPoint coord = IsoMap::IsoToCoord(tileToPoint, _tileSize, _shift);
    
    CCPoint ext = CCPointZero;
    if (int(_gridSize.width)%2 == 0 || int(_gridSize.height)%2 == 0) {
        ext.y = _tileSize.y/2;
    }
    
    CCPoint pos = IsoMap::CoordToIso(coord, _tileSize, _shift);
    this->setPosition(pos+ext);
    
    if(_oldPos.x!=this->getPositionX()&&_oldPos.y!=this->getPositionY()) {
        g_gameAudio->playEffect(AUDIO_MOVE_BUILDING);
        _oldPos = this->getPosition();
    }
    
}

void PlaceHolder::snapWithQuadGrid(CCPoint location) {
    
    CCPoint coord = IsoMap::IsoToCoord(location, _tileSize*2, _shift);
    
    CCPoint ext = CCPointZero;
    if (int(_gridSize.width*2)%2 == 0 || int(_gridSize.height*2)%2 == 0) {
        ext.y = _tileSize.y/2;
    }
    this->setPosition(IsoMap::CoordToIso(coord, _tileSize*2, _shift)+ext);
    
    if(_oldPos.x!=this->getPositionX()&&_oldPos.y!=this->getPositionY()) {
        g_gameAudio->playEffect(AUDIO_MOVE_BUILDING);
        _oldPos = this->getPosition();
    }
}

void PlaceHolder::setMeAGoodPosition() {
    
    if (this->building) {
        
        this->setPosition(IsoMap::getPositionByMapIndexes(this->building->pbVo.map, _tileSize, _shift));
        
        GDBuildingVO _bvo = g_gameContent->getBuildingVO(this->building->pbVo.type, this->building->pbVo.level);
        if (_bvo.type == "") {
            CCLog_("(PlaceHolder::setMeAGoodPosition) Error: Given decoration=%s doesn't exist!", this->decoration->pdVo.type.c_str());
            return;
        }
        
        if (_bvo.grid_col != _bvo.grid_row) {
            if (this->building->pbVo.flip) {
                this->setPositionX(this->getPosition().x + m_grid[0]->getContentSize().width*0.25f);
                this->setPositionY(this->getPosition().y - m_grid[0]->getContentSize().height*0.25f);
            }else {
                this->setPositionX(this->getPosition().x + m_grid[0]->getContentSize().width*0.25f);
                this->setPositionY(this->getPosition().y + m_grid[0]->getContentSize().height*0.25f);
            }
        }

    }else if (this->decoration) {
    
        this->setPosition(IsoMap::getPositionByMapIndexes(this->decoration->pdVo.map, _tileSize, _shift));
        
        GDDecorationVO _dvo = g_gameContent->getDecorationVO(this->decoration->pdVo.type);
        if (_dvo.type == "") {
            CCLog_("(PlaceHolder::setMeAGoodPosition) Error: Given decoration=%s doesn't exist!", this->decoration->pdVo.type.c_str());
            return;
        }
        
        if (_dvo.grid_col != _dvo.grid_row) {
            if (this->decoration->pdVo.flip) {
                this->setPositionX(this->getPosition().x + m_grid[0]->getContentSize().width*0.25f);
                this->setPositionY(this->getPosition().y - m_grid[0]->getContentSize().height*0.25f);
            }else {
                this->setPositionX(this->getPosition().x + m_grid[0]->getContentSize().width*0.25f);
                this->setPositionY(this->getPosition().y + m_grid[0]->getContentSize().height*0.25f);
            }
        }
        
        
    }
    
}

void PlaceHolder::verifyMap(void* ptr_isomap) {
    
    IsoMapBuilding *isoMap = (IsoMapBuilding*)ptr_isomap;
    if (!isoMap) {
        CCLog_("(PlaceHolder::verifyMap) Warning: isoMap is uninitialized!");
        return;
    }
    
    int i=0, j=0, k=0;

    for (i=0; i<GRID_HOLDER_MAX; i++) {
        
        if (this->building) {
            this->building->pbVo.map[i] = -1;
        }
        
        if (this->decoration) {
            this->decoration->pdVo.map[i] = -1;
        }
        
        if (m_grid[i]) {
            j++;
            
            CCPoint location = this->getPosition()+m_grid[i]->getPosition();
            CCPoint coord = isoMap->IsoToCoord(location, _tileSize, _shift);
            CCPoint coord2 = isoMap->IsoToCoord2(location, _tileSize, _shift);
            
            if (coord2.x > 0.0f && coord2.x <= ISO_TILE_COL && coord2.y > 0.0f && coord2.y <= ISO_TILE_ROW) {

                int t_tileIndex = coord.y*ISO_TILE_COL+coord.x;
                
                switch (m_type) {
                    case PHT_NEW_BUILDING:
                    case PHT_EDT_BUILDING: {
                        if (this->building) {
                            this->building->pbVo.map[i] = t_tileIndex;
                        }
                    }break;
                    case PHT_NEW_DECORATION:
                    case PHT_EDT_DECORATION: {
                        if (this->decoration) {
                            this->decoration->pdVo.map[i] = t_tileIndex;
                        }
                    }break;
                    case PHT_NEW_TRACK:
                    case PHT_EDT_TRACK:
                    case PHT_NEW_OBSTACLE:
                    case PHT_EDT_OBSTACLE:
                        if (this->track) {
                            
                        }
                        break;
                }
                
                if (isoMap->tilesMap[t_tileIndex] == NULL && isoMap->tilesLegal[t_tileIndex] <= g_playerState->player.racing.level) {
                    k++;
                }
                
            }
            
        }
        
    }
    
    if (k >= j) {
        _active = true;
    }else {
        _active = false;
    }
    
    CCPoint t_coord = IsoMap::IsoToCoord(this->getPosition(), _tileSize*2, _shift);
    int t_trackIndex = int(t_coord.y*(ISO_TILE_COL/2)+t_coord.x);
    if (track) {
        track->ptVo.index = t_trackIndex;
    }
    
    if ((m_type == PHT_NEW_OBSTACLE) ||
        (m_type == PHT_EDT_OBSTACLE)) {
        if ((isoMap->tilesTrack[t_trackIndex] == 1 && isoMap->tilesObstacle[t_trackIndex] == 0) ||
            (isoMap->tilesTrack[t_trackIndex] == 3 && isoMap->tilesObstacle[t_trackIndex] == 0)) {
            _active = true;
        }else {
            _active = false;
        }
        
        if (isoMap->getStartTrackIndex() == t_trackIndex && t_trackIndex != -1) {
            _active = false;
        }
        
    }
    
    for (i = 0; i<GRID_HOLDER_MAX; i++) {
        
        if (m_grid[i]) {
            if (_active) {
                m_grid[i]->setColor(ccc3(255, 255, 255));
            }else {
                m_grid[i]->setColor(ccc3(255, 0, 0));
            }
        }
    }
    
}

PlaceHolderLaunchInstantDialogType PlaceHolder::getLaunchInstantDialogType(Building *builder) {
    
    if (building && building->pbVo.buid == "") {
        
//        SceneVille *_scene = (SceneVille*)g_currentScene;
        if (BUILD_HABITAT_WITH_GEMS
            && building->bVo.building_type == BUILDING_TYPE_HABITAT
            && ((SceneVille*)g_currentScene)->tutMgr->getTutUnits().size() == 0)
            return PHT_LIDT_HABITAT;
        
        if (builder)
            return PHT_LIDT_BUILDERBUSY;
        
        if (g_playerState->player.balance.gold < building->bVo.cost_coins)
            return PHT_LIDT_NOTENOUGHCOINS;
        
    }
    
    if (decoration
        && decoration->pdVo.duid == ""
        && g_playerState->player.balance.gold < decoration->dVo.cost_coins)
        return PHT_LIDT_NOTENOUGHCOINS_DEC;
    
    return PHT_LIDT_NONE;
    
}

void PlaceHolder::place(CCObject *sender) {

    SceneVille *_scene = (SceneVille*)g_currentScene;
    
    if (!_active) {
        if (building) {
            if (building->pbVo.buid != "") {
                return;
            }
            g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR,
                                                     g_gameContent->Word["unavailable_space"].c_str());
            return;
        }
        if (decoration) {
            if (decoration->pdVo.duid != "") {
                return;
            }
            g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR,
                                                     g_gameContent->Word["unavailable_space"].c_str());
            return;
        }
    }
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    Building * _builder = g_playerState->getBuilder();
    
    switch (getLaunchInstantDialogType(_builder)) {
            
        case PHT_LIDT_BUILDERBUSY: {
         
            if (_scene->modalController->getParent() != NULL) return;
            CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
            InstantDialog *_dialog = InstantDialog::create(_scene->modalController, building, (void*)_builder);
            _dialog->setPosition(_screenSize*0.5f);
            _scene->addChild(_scene->modalController, kModalControllerZOrder);
            _scene->modalController->pushView(_dialog);
            
        }break;
            
        case PHT_LIDT_NOTENOUGHCOINS: {
            
            if (_scene->modalController->getParent() != NULL) return;
            CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
            InstantDialog *_dialog = NULL;
            if (building->pbVo.level == 1) {
                _dialog = InstantDialog::createWithBuildingVO(_scene->modalController, building->bVo);
            }else {
                _dialog = InstantDialog::create(_scene->modalController, building);
            }
            _dialog->setPosition(_screenSize*0.5f);
            _scene->addChild(_scene->modalController, kModalControllerZOrder);
            _scene->modalController->pushView(_dialog);
            
        }break;
            
        case PHT_LIDT_NOTENOUGHCOINS_DEC: {
            
            if (_scene->modalController->getParent() != NULL) return;
            CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
            InstantDialog *_dialog = InstantDialog::createWithType(_scene->modalController,
                                                                   IT_DECORATION_BUYRESOURCE, decoration, NULL);
            _dialog->setPosition(_screenSize*0.5f);
            _scene->addChild(_scene->modalController, kModalControllerZOrder);
            _scene->modalController->pushView(_dialog);
            
        }break;
            
        case PHT_LIDT_HABITAT: {
            
            if (_scene->modalController->getParent() != NULL) return;
            CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
            InstantDialog *_dialog = InstantDialog::createWithType(_scene->modalController,
                                                                   IT_BUYHABITAT_WITHGEMS, building, NULL);
            _dialog->setPosition(_screenSize*0.5f);
            _scene->addChild(_scene->modalController, kModalControllerZOrder);
            _scene->modalController->pushView(_dialog);
            
        }break;
            
        default:
        case PHT_LIDT_NONE: {
            
            if (placeBuilding(BuildingBuildTypeNormal)) return;
            if (placeTrack()) return;
            if (placeDecoration(DecorationBuyTypeNormal)) return;

        }break;
    }

}

void PlaceHolder::cancel(CCObject *sender)
{
    
    if (CCDirector::sharedDirector()->getActionManager()->numberOfRunningActionsInTarget(menuNo) != 0) return;
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene)
        return;
    
    IsoMapBuilding *mapBuilding = (IsoMapBuilding*)_scene->isoMap;
    
    if (!mapBuilding)
        return;
    
    if (!mapBuilding->getCancelBuildingPlacementEnabled())
        return;
    
    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    mapBuilding->removePlaceHolder();
}

void PlaceHolder::remove(CCObject* sender)
{
    if (CCDirector::sharedDirector()->getActionManager()->numberOfRunningActionsInTarget(menuNo) != 0) return;
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene) return;
    
    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    ((IsoMapBuilding*)_scene->isoMap)->removingStuffInPlaceHolder();
}

void PlaceHolder::flip(CCObject *sender) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
   if (building || decoration) {
            
        rotateObject();
        
    }else if (track) {
        
        switchTrack(false);
        
    }
    
    updateMenu(NULL, true);
    
}

bool PlaceHolder::placeBuilding(BuildingBuildType buildType) {
    
    if (!building) return false;
    
    if (m_type == PHT_NEW_BUILDING &
        m_type == PHT_NEW_DECORATION &
        m_type == PHT_NEW_TRACK &
        m_type == PHT_NEW_OBSTACLE)
    {
        g_gameAudio->playEffect(AUDIO_UNDERCONSTRUCTION);
    }
    
    if (_active) {
        this->building->setPosition(this->getPosition()+this->building->getPosition());
    }else {
        this->building->pbVo = pbVo_recover;
        if (this->building->pbVo.map[0] >= 0) {
            this->building->setPosition(IsoMap::getPositionByMapIndexes(this->building->pbVo.map, _tileSize, _shift));
        }
    }
    
    if (!canPlace()) return false;
    SceneVille *_scene = (SceneVille*)g_currentScene;
    IsoMapBuilding *_isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
    _isoMapBuilding->placingStuffFromPlaceHolder(false, (int)buildType);
    if (onDone) onDone(m_sender);
    
    return true;
    
}

bool PlaceHolder::placeTrack() {
    
    if (!track) return false;
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    
    if (_active) {
        if (this->track->trackStart) { // also tell mapPlayerState that this track is the start track
            g_playerState->player.racing.start = -1;
        }
        if (ptVo_recover.index > 0) {
            _scene->isoMap->purgeTrack(ptVo_recover);
        }
    }else {
        revert();
    }
  
    if (!canPlace()) return false;
    IsoMapBuilding *_isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
    _isoMapBuilding->placingStuffFromPlaceHolder(false, BuildingBuildTypeNormal);
    if (onDone) onDone(m_sender);
    
    return true;
    
}

bool PlaceHolder::placeDecoration(DecorationBuyType buyType) {
    
    if (!decoration) return false;

    if (_active) {
        this->decoration->setPosition(this->getPosition()+this->decoration->getPosition());
    }else {
        this->decoration->pdVo = pdVo_recover;
        if (this->decoration->pdVo.map[0] >= 0) {
            this->decoration->setPosition(IsoMap::getPositionByMapIndexes(this->decoration->pdVo.map, _tileSize, _shift));
        }
    }
    
    if (!canPlace()) return false;
    SceneVille *_scene = (SceneVille*)g_currentScene;
    IsoMapBuilding *_isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
    _isoMapBuilding->placingStuffFromPlaceHolder(false, (int)buyType);
    if (onDone) onDone(m_sender);

    return true;

}

void PlaceHolder::rotateObject() {
    
    if (building) {
        
        GDBuildingVO _bvo = g_gameContent->getBuildingVO(building->pbVo.type, building->pbVo.level);
        if (_bvo.type == "") {
            CCLog_("building=%s doesn't exist!", building->pbVo.type.c_str());
        }
        
        building->pbVo.flip = !building->pbVo.flip;
        building->updateFlip();
        
        if (building->pbVo.flip) {
            _composeGrid(_bvo.grid_row, _bvo.grid_col);
        } else {
            _composeGrid(_bvo.grid_col, _bvo.grid_row);
        }
        
    } else if (decoration) {
        
        GDDecorationVO _dvo = g_gameContent->getDecorationVO(decoration->pdVo.type);
        if (_dvo.type == "") {
            CCLog_("decoration=%s doesn't exist!", decoration->pdVo.type.c_str());
            return;
        }
        
        decoration->pdVo.flip = !decoration->pdVo.flip;
        decoration->updateFlip();
        
        if (decoration->pdVo.flip) {
            _composeGrid(_dvo.grid_row, _dvo.grid_col);
        }else {
            _composeGrid(_dvo.grid_col, _dvo.grid_row);
        }
        
    }
    
}

void PlaceHolder::switchTrack(bool force) {
    
    if (!track) {
        CCLog_("(PlaceHolder::switchTrack) Error: track is uninitiailzed!\n");
        return;
    }
    
    if (!g_gameContent) {
        CCLog_("(PlaceHolder::switchTrack) Error: gameContent is uninitialized!\n");
        return;
    }
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene) {
        CCLog_("(PlaceHolder::switchTrack) Error: SceneMapCommand is uninitialized!");
        return;
    }
    
    GDTrackVO vo = g_gameContent->getTrackVO(track->ptVo.type);
    if (vo.type == "") {
        CCLog_("(PlaceHolder::switchTrack) Error: track=%s doesn't exist!\n", track->ptVo.type.c_str());
        return;
    }
    
    if (vo.track_type == PT_OBJECT_TRACK) {
        
        track->changeTrack(this->getPosition(), _tileSize, _shift, force);
    
        if (trackStart_recover && (this->track->ptVo.no == 1 || this->track->ptVo.no == 3)) {
            this->track->trackStart = true;
            this->track->updateSprite();
        }
        
        //command->updateMenu();
        
    }else if (vo.track_type == PT_OBJECT_OBSTACLE) {

        track->changeObstacle(this->getPosition(), _tileSize, _shift);
        
    }
    
}

void PlaceHolder::setStartOrChangeDirection(CCObject *sender) {
    
    if (menuSet->getOpacity() == 255 && CCDirector::sharedDirector()->getActionManager()->numberOfRunningActionsInTarget(menuSet) == 0) {
        if (SceneVille* _scene = (SceneVille*)g_currentScene) {
            g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
            ((IsoMapBuilding*)_scene->isoMap)->setStartOrChangeDirection();
        }
    }
    
}

void PlaceHolder::moveBy(cocos2d::CCPoint coord) {
    
    switch (m_type) {
        case PHT_NEW_BUILDING:
        case PHT_EDT_BUILDING:
            this->setPosition(this->getPosition()+CCPoint((coord.x*_tileSize.x/2)+(coord.y*_tileSize.x) , (coord.x*_tileSize.y)+(coord.y*_tileSize.y/2)));
            break;
        case PHT_NEW_TRACK:
        case PHT_EDT_TRACK:
        case PHT_NEW_OBSTACLE:
        case PHT_EDT_OBSTACLE:
            this->setPosition(this->getPosition()+CCPoint((coord.x*_tileSize.x)-(coord.y*_tileSize.x), (coord.x*_tileSize.y)+(coord.y*_tileSize.y)));
            break;
            default:
            break;
    }
    
}

PlaceHolderType PlaceHolder::getType() {
    
    return m_type;
    
}

CCSize PlaceHolder::getGridSize() {
    
    return _gridSize;
    
}

PlayerBuildingVO PlaceHolder::getBuildingRecoverVO() {
    
    return pbVo_recover;
    
}

PlayerTrackVO PlaceHolder::getTrackRecoverVO() {
    
    return ptVo_recover;
    
}

PlayerDecorationVO PlaceHolder::getDecorationRecoverVO() {
    
    return pdVo_recover;
    
}

void PlaceHolder::setSender(void *sender_) {
    
    m_sender = sender_;
    
}

void PlaceHolder::onHit(void *inst_, void *sender_) {
    
    CCLog_("hit!");
    
}

void PlaceHolder::selectTrack(CCObject *sender) {
    
    ItemTrack* _choose_track = (ItemTrack*)sender;
    if (_choose_track->isLock()) {
        g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, _choose_track->getLockMessage().c_str());
        return;
    }
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    
    for (vector<Track*>::iterator it = g_playerState->Tracks.begin(); it != g_playerState->Tracks.end(); ++it) {
        Track* _track = (Track*)(*it);
        if (_track->tVo.type != "" && _track->tVo.track_type == "track") {
            _track->changeTrackType(_choose_track->vo);
        }
    }
    track->changeTrackType(_choose_track->vo);
    
    for (vector<ItemTrack*>::iterator it=m_item_tracks.begin(); it!=m_item_tracks.end(); ++it) {
        if (ItemTrack* _item_track = (ItemTrack*)(*it)) {
            if (_item_track == _choose_track) {
                _item_track->select(true);
            } else {
                _item_track->select(false);
            }
        }
    }
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    _scene->isoMap->switchTrackGlue();
    _scene->hudPanel->updateHUDTrack(_scene->isoMap->isTrackComplete(), false);
    
}
