//
//  Track.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/6/56 BE.
//
//

#include "Track.h"
USING_NS_CC;

#include "../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

#include "../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../Entities/BananaTrap.h"

#include "../Entities/AI.h"

#include "Environments/Visitor.h"

#ifdef DEBUG_TRACK
#include "../Extensions/CCLabelHelper.h"
#endif

Track* Track::createWithTrackVO(GDTrackVO vo, PlayerTrackVO ptvo) {
    
    Track* pRet = new Track();
    if (pRet->initWithTrackVO(vo, ptvo)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
    
}

bool Track::initWithTrackVO(GDTrackVO tvo, PlayerTrackVO ptvo) {
    
    if (!CCNode::init()) {
        return false;
    }
    
    tVo = tvo;
    
    ptVo = ptvo;
    ptVo.type = tvo.type;
    
    m_trackSprite = NULL;
    m_startSprite = NULL;
    m_directionSprite = NULL;
    m_markSprite = NULL;
    
    m_bananaTrap = NULL;
    
    m_visitors = NULL;
    
    nextTrack = NULL;
    prevTrack = NULL;
    
    trackStart = false;
    trackDirection = false;
    
    updateSprite();
    updatePosition();
    
    m_trapReserved = false;
    
    m_trapBlacklistedForAI = false;
    m_trapBlacklistedRangeForAI = 0;
    
#ifdef DEBUG_TRACK
    CREATE_CHAR_BUFFER(_buffer, 64);
    snprintf(_buffer, _buffer_size, "%d", ptVo.no);
    debugNo = CCLabelHelper::createBMFont(_buffer, StyleDialogTitle);
    this->addChild(debugNo, 1000000);
#endif
    
    return true;
    
}

Track::~Track() {
//void Track::cleanup() {

    if (m_directionSprite) {
        m_directionSprite->removeFromParentAndCleanup(true);
        m_directionSprite = NULL;
    }
    
    if (m_startSprite) {
        m_startSprite->removeFromParentAndCleanup(true);
        m_startSprite = NULL;
    }
    
    if (m_bananaTrap) {
        m_bananaTrap->release();
        m_bananaTrap = NULL;
    }
    
    if (m_trackSprite) {
        m_trackSprite->removeFromParentAndCleanup(true);
        m_trackSprite = NULL;
    }
    
    if (m_visitors) {
        CCObject* _obj;
        CCARRAY_FOREACH(m_visitors, _obj) {
            if (Visitor* _visitor = (Visitor*)_obj) {
                _visitor->removeFromParentAndCleanup(true);
                _visitor = NULL;
            }
        }
        
        m_visitors->removeAllObjects();
        m_visitors->release();
        m_visitors = NULL;
    }
    
//    CCNode::cleanup();
    
}

void Track::changeTrackType(GDTrackVO tvo_) {
    
    tVo = tvo_;
    
//    if (tvo_.type != ptVo.type) return;
    
    ptVo.type = tVo.type;
    
    updateSprite();
    
}

void Track::updateSprite() {

    if (m_directionSprite != NULL) {
        m_directionSprite->removeFromParentAndCleanup(true);
//        m_directionSprite->release();
        m_directionSprite = NULL;
    }
    
    if (m_startSprite != NULL) {
        m_startSprite->removeFromParentAndCleanup(true);
//        m_startSprite->release();
        m_startSprite = NULL;
    }
    
    if (m_trackSprite != NULL) {
        m_trackSprite->removeFromParentAndCleanup(true);
//        m_trackSprite->release();
        m_trackSprite = NULL;
    }
    
    CREATE_CHAR_BUFFER(buffer_, 128);
    if (tVo.track_type == PT_OBJECT_TRACK) {
        snprintf(buffer_, buffer__size, "%s_%d.png", tVo.src.c_str(), ptVo.no);
    }else if (tVo.track_type == PT_OBJECT_OBSTACLE){
        snprintf(buffer_, buffer__size, "%s.png", tVo.src.c_str());
    }
    
    m_trackSprite = CCSprite::createWithSpriteFrameName(buffer_);
    //m_trackSprite->preferenceRootParent = preferenceRootParent;
    this->addChild(m_trackSprite);
    
    if (ptVo.no != 1 && ptVo.no != 3) {
        trackStart = false;
    }
    
    if (trackStart) {
        
        m_startSprite = CCSprite::createWithSpriteFrameName("track_start.png");
        //m_startSprite->preferenceRootParent = preferenceRootParent;
        addChild(m_startSprite);
        
        m_directionSprite = CCSprite::createWithSpriteFrameName("track_arrow.png");
        //m_directionSprite->preferenceRootParent = preferenceRootParent;
        m_directionSprite->runAction(CCRepeatForever::create(CCSequence::create(CCFadeTo::create(1.0f, 127), CCFadeTo::create(1.0f, 255), NULL)));
        addChild(m_directionSprite);
        
        if (ptVo.no == 1) {
            m_startSprite->setScaleX( 1);
            if (trackDirection) {
                m_directionSprite->setScaleX(-1);
                m_directionSprite->setScaleY(-1);
            }else {
                m_directionSprite->setScaleX( 1);
                m_directionSprite->setScaleY( 1);
            }
        }else if (ptVo.no == 3) {
            m_startSprite->setScaleX(-1);
            if (trackDirection) {
                m_directionSprite->setScaleX(-1);
                m_directionSprite->setScaleY( 1);
            }else {
                m_directionSprite->setScaleX( 1);
                m_directionSprite->setScaleY(-1);
            }
        }
        

        
    }
    
//    snprintf(buffer_, buffer__size, "%d", ptVo.index);
//    addChild(CCLabelTTF::create(buffer_, "dimbo.ttf", 32.0f), 1);
    
    this->setContentSize(m_trackSprite->getContentSize());
    
}

void Track::updatePosition() {

    if (ptVo.index < 0) {
        setPosition(CCPointZero);
    }else {
        setPosition(IsoMap::getPositionByTrackIndex(ptVo.index, g_gameContent->tileSize, g_gameContent->tileShift));
    }
    
}

void Track::updateFlip() {

    if (tVo.track_type == PT_OBJECT_OBSTACLE) {
        if (ptVo.no == 0) {
            m_trackSprite->setScaleX( fabsf(m_trackSprite->getScaleX()));
        }else if (ptVo.no == 1) {
            m_trackSprite->setScaleX(-fabsf(m_trackSprite->getScaleX()));
        }
    }
    
}

bool Track::changeTrack(CCPoint position, CCSize tileSize, CCPoint shift, bool force) {
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    CCPoint coord = IsoMap::IsoToCoord(position, tileSize*2, shift);
     
    int path_rounded[] = {0, 0, 0, 0};
    bool path_can_use[] = {false, false, false, false, false, false};
    
    int i = coord.x + 1;
    int j = coord.y;
    int k = int(j*(ISO_TILE_COL/2)+(i));
    path_rounded[0] = _scene->isoMap->tilesTrack[k];
    
    i = coord.x;
    j = coord.y - 1;
    k = int(j*(ISO_TILE_COL/2)+(i));
    path_rounded[1] = _scene->isoMap->tilesTrack[k];
    
    i = coord.x - 1;
    j = coord.y;
    k = int(j*(ISO_TILE_COL/2)+(i));
    path_rounded[2] = _scene->isoMap->tilesTrack[k];
    
    i = coord.x;
    j = coord.y + 1;
    k = int(j*(ISO_TILE_COL/2)+(i));
    path_rounded[3] = _scene->isoMap->tilesTrack[k];
    
    for (int i=0; i<4; i++) {
        switch (i) {
            case 0:
                switch (path_rounded[i]) {
                    case 1:
                        path_can_use[0] = true;
                        path_can_use[4] = true;
                        path_can_use[5] = true;
                        break;
                    case 2:
                        path_can_use[0] = true;
                        path_can_use[4] = true;
                        path_can_use[5] = true;
                        break;
                    case 3:
                        //nothing
                        break;
                    case 4:
                        path_can_use[0] = true;
                        path_can_use[4] = true;
                        path_can_use[5] = true;
                        break;
                    case 5:
                        //nothing
                        break;
                    case 6:
                        //nothing
                        break;
                }
                break;
            case 1:
                switch (path_rounded[i]) {
                    case 1:
                        //nothing
                        break;
                    case 2:
                        //nothing
                        break;
                    case 3:
                        path_can_use[1] = true;
                        path_can_use[2] = true;
                        path_can_use[4] = true;
                        break;
                    case 4:
                        path_can_use[1] = true;
                        path_can_use[2] = true;
                        path_can_use[4] = true;
                        break;
                    case 5:
                        //nothing
                        break;
                    case 6:
                        path_can_use[1] = true;
                        path_can_use[2] = true;
                        path_can_use[4] = true;
                        break;
                }
                break;
            case 2:
                switch (path_rounded[i]) {
                    case 1:
                        path_can_use[0] = true;
                        path_can_use[1] = true;
                        path_can_use[3] = true;
                        break;
                    case 2:
                        //nothing
                        break;
                    case 3:
                        //nothing
                        break;
                    case 4:
                        //nothing
                        break;
                    case 5:
                        path_can_use[0] = true;
                        path_can_use[1] = true;
                        path_can_use[3] = true;
                        break;
                    case 6:
                        path_can_use[0] = true;
                        path_can_use[1] = true;
                        path_can_use[3] = true;
                        break;
                }
                break;
            case 3:
                switch (path_rounded[i]) {
                    case 1:
                        //nothing
                        break;
                    case 2:
                        path_can_use[2] = true;
                        path_can_use[3] = true;
                        path_can_use[5] = true;
                        break;
                    case 3:
                        path_can_use[2] = true;
                        path_can_use[3] = true;
                        path_can_use[5] = true;
                        break;
                    case 4:
                        //nothing
                        break;
                    case 5:
                        path_can_use[2] = true;
                        path_can_use[3] = true;
                        path_can_use[5] = true;
                        break;
                    case 6:
                        //nothing
                        break;
                }
                break;
        }
    }
    
    k=0, j=0;
    for (i=0; i<6; i++) {
        if (path_can_use[i]) {
            k++;
        }
        if(i<4) {
            if (path_rounded[i]) {
                j++;
            }
        }
    }
    
    switch (j) {
            
        case 0: {
            ptVo.no++;
            if (ptVo.no > 6) {
                ptVo.no = 1;
            }
            if (ptVo.no < 1) {
                ptVo.no = 1;
            }
        }break;
            
        case 1: {
            
            if (force) {
                if (path_can_use[0]) {
                    ptVo.no = 1;
                }else {
                    ptVo.no = 3;
                }
            }else {
                
                if (k==0) {
                    ptVo.no++;
                    if (ptVo.no > 6) {
                        ptVo.no = 1;
                    }
                    if (ptVo.no < 1) {
                        ptVo.no = 1;
                    }
                }else {
                    do {
                        ptVo.no++;
                        if (ptVo.no > 6) {
                            ptVo.no = 1;
                        }
                        if (ptVo.no < 1) {
                            ptVo.no = 1;
                        }
                    }while (!path_can_use[ptVo.no-1]);
                }
                
            }
            
            
        }break;
            
        default: {
            
            if (force) {
                
                if ((path_rounded[1] == 3 || path_rounded[1] == 4 || path_rounded[1] == 6) &&
                    (path_rounded[2] == 1 || path_rounded[2] == 5 || path_rounded[2] == 6)) {
                    ptVo.no = 2;
                }else if ((path_rounded[2] == 1 || path_rounded[2] == 5 || path_rounded[2] == 6) &&
                          (path_rounded[3] == 2 || path_rounded[3] == 3 || path_rounded[3] == 5)) {
                    ptVo.no = 4;
                }else if ((path_rounded[0] == 1 || path_rounded[0] == 2 || path_rounded[0] == 4) &&
                          (path_rounded[1] == 3 || path_rounded[1] == 4 || path_rounded[1] == 6)) {
                    ptVo.no = 5;
                }else if ((path_rounded[0] == 1 || path_rounded[0] == 2 || path_rounded[0] == 4) &&
                          (path_rounded[3] == 2 || path_rounded[3] == 3 || path_rounded[3] == 5)) {
                    ptVo.no = 6;
                }else {
                    
                    if (path_can_use[0]) {
                        ptVo.no = 1;
                    }else {
                        ptVo.no = 3;
                    }
                }
                
            }else {
                
                if (k==0) {
                    ptVo.no++;
                    if (ptVo.no > 6) {
                        ptVo.no = 1;
                    }
                    if (ptVo.no < 1) {
                        ptVo.no = 1;
                    }
                }else {
                    do {
                        ptVo.no++;
                        if (ptVo.no > 6) {
                            ptVo.no = 1;
                        }
                        if (ptVo.no < 1) {
                            ptVo.no = 1;
                        }
                    }while (!path_can_use[ptVo.no-1]);
                }
                
            }
            
            
        }break;
            
    }
    
    updateSprite();
    return true;
    
}

bool Track::changeObstacle(CCPoint position, CCSize tileSize, CCPoint shift) {
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    
    CCPoint t_coord = IsoMap::IsoToCoord(position, tileSize*2, shift);
    int t_trackIndex = int(t_coord.y*(ISO_TILE_COL/2)+(t_coord.x));

    if (_scene->isoMap->tilesTrack[t_trackIndex] == 1) {
        ptVo.no = 0;
    }else if (_scene->isoMap->tilesTrack[t_trackIndex] == 3) {
        ptVo.no = 1;
    }

    updateFlip();
    return true;
    
}

//void Track::showMark(bool show_) {
//    
//    if (show_) {
//        
//        if (!m_markSprite && getParent()) {
//            m_markSprite = CCSprite::createWithSpriteFrameName("grid_road_plus.png");
//            m_markSprite->setScale(2.0f);
//            m_markSprite->setColor(ccRED);
//            addChild(m_markSprite, getZOrder()+1);
//            
//            m_markSprite->stopAllActions();
//            m_markSprite->runAction(CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.5f, 64), CCFadeTo::create(0.5f, 128), NULL)));
//        }
//        
////         m_trackSprite->stopAllActions();
////        m_trackSprite->runAction(CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.5f, 128), CCFadeTo::create(0.5f, 255), NULL)));
////        m_trackSprite->runAction(CCRepeatForever::create(CCSequence::create(CCTintTo::create(0.5f, 230, 230, 230), CCTintTo::create(0.5f, 255, 255, 255), NULL)));
//        
//    }else {
//        
//        if (m_markSprite) {
//            m_markSprite->removeFromParent();
//            m_markSprite = NULL;
//        }
////        m_trackSprite->stopAllActions();
////        m_trackSprite->runAction(CCFadeTo::create(0.5f, 255));
////        m_trackSprite->runAction(CCTintTo::create(0.5f, 255, 255, 255));
//        
//    }
//    
//}

void Track::addVisitor(Visitor *visitor)
{
    if (!m_visitors) {
        m_visitors = CCArray::create();
        m_visitors->retain();
    }
    
    m_visitors->addObject(visitor);
}

CCArray* Track::getVisitors()
{
    return m_visitors;
}

void Track::visitorDropCoin()
{
    float persentage = 50;
    if (m_visitors) {
        for (int i = 0; i < m_visitors->count(); i++) {
            Visitor *visitor = (Visitor *)m_visitors->objectAtIndex(i);
            if (arc4random()%100 < persentage) {
                visitor->dropCoin();
            }
        }
    }
}

void Track::reserveTrap(bool reserve)
{
    if (m_trapReserved == reserve) return;
    
    m_trapReserved = reserve;
    
    m_trapBlacklistedRangeForAI =
    (int)(((double)arc4random() / ARC4RANDOM_MAX) * TRAP_RESERVE_RANDOM_RANGE);
    
    this->setTrapBlacklistedForAIForRange(true, m_trapBlacklistedRangeForAI);
}

void Track::setTrapBlacklistedForAIForRange(bool blacklist, int range)
{
    Track *track = this;
    
    int i = 0;
    while (track) {
        track = track->nextTrack;
        track->m_trapBlacklistedForAI = blacklist;
//        if (blacklist) track->setScale(0.5f); else track->setScale(1.0f); //DEBUG
        i++;
        if (i >= range) break;
    }
}

void Track::addTrap(BananaTrap *bananaTrap)
{
    removeTrapWithoutRemovingTrapBlackList();
    
    //just to make sure the trap is still reserved after trap is removed
    reserveTrap(true);
    
    m_bananaTrap = bananaTrap;
    m_bananaTrap->retain();
    
    m_hasTrap = true;
}

void Track::removeBananaTrap(){
    if (m_bananaTrap) {
        m_bananaTrap->removeTrap();
    }
}

void Track::removeTrapWithoutRemovingTrapBlackList()
{
    if (m_bananaTrap) {
        m_bananaTrap->release();
        m_bananaTrap = NULL; //nil; //Changed by Tricko
    }
    m_hasTrap = false;
    reserveTrap(false);
}

void Track::removeTrap()
{
    removeTrapWithoutRemovingTrapBlackList();
    
    this->setTrapBlacklistedForAIForRange(false, m_trapBlacklistedRangeForAI);
    m_trapBlacklistedRangeForAI = 0;
}

cocos2d::CCRect Track::getTrackBoundingBox() const
{
    return m_trackSprite->boundingBox();
}
