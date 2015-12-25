//
//  Visitor.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/15/2556 BE.
//
//

#include "Visitor.h"
USING_NS_CC;
using namespace std;
using namespace cocos2d::extension;

#include "../../Entities/PlayerState.h"
#include "../../Entities/IsoMap.h"

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../SceneManager.h"
extern SceneManager *g_sceneManager;

//extern CCPoint screenFactor;
#include "../../AppMacros.h"
extern TargetDevice targetDevice;
extern float sizeMultiplier;

#include "../../Entities/TouchableItem.h"

Visitor *Visitor::createWithRandomSpineName() {
    CREATE_CHAR_BUFFER(_buffer, 64);
    snprintf(_buffer, _buffer_size, "people%d", arc4random()%4);
    return Visitor::createWithSpineName(_buffer);
}

Visitor *Visitor::createWithSpineName(const char *spine_name_) {
    
    Visitor *pRet = new Visitor();
    pRet->initWithSpineName(spine_name_);
    pRet->autorelease();
    return pRet;
    
}

bool Visitor::initWithSpineName(const char *spine_name_) {
    
    if (!CCNode::init()) {
        return false;
    }
    
    m_initializedStartGoal = false;
    m_foundGoal = false;
    
    m_coin = NULL;
    isoMap = NULL;
    
    m_visitorSpine = g_sceneManager->sharedSkeletonData->getSkeleton(kVisitorSrcDir, spine_name_);//CCSkeletonAnimation::createWithFile(_buf_json, _buf_atlas);
    m_visitorSpine->timeScale = 1.0f;
    m_visitorSpine->setScale(sizeMultiplier);
    this->addChild(m_visitorSpine);
    
    m_moveAction = NULL;
    
    //count how many visitor's action, and when reach the limit then send this guy home
    m_actionCount = kVisitorActionMin+arc4random()%(kVisitorActionMax-kVisitorActionMin);
    m_goHomeState = 0;
    pleaseReleaseMe = false;
    
    m_startCell = NULL;
    m_goalCell = NULL;
    
    setVisitorState(VTS_IDLE);

    return true;
    
}

Visitor::~Visitor() {
//void Visitor::cleanup() {

    if (m_goalCell) {
        delete m_goalCell;
        m_goalCell = NULL;
    }
    
    for (vector<SearchCell*>::iterator it=m_openList.begin(); it!=m_openList.end(); ++it) {
        if (SearchCell* _cell = (SearchCell*)(*it)) {
            if (_cell == m_startCell) {
                m_startCell = NULL;
            }
            delete _cell;
            _cell = NULL;
        }
    }
    m_openList.clear();
    
    for (vector<SearchCell*>::iterator it=m_visitedList.begin(); it!=m_visitedList.end(); ++it) {
        if (SearchCell* _cell = (SearchCell*)(*it)) {
            if (_cell == m_startCell) {
                m_startCell = NULL;
            }
            delete _cell;
            _cell = NULL;
        }
    }
    m_visitedList.clear();
    
//    CCNode::cleanup();
    
}

void Visitor::enterRanch() {
    
    IsoMap *_isoMap = (IsoMap*)isoMap;
    PlayerState *_mapPlayerState = (PlayerState*)_isoMap->mapPlayerState;
    m_pathToGoal.clear();
    if (Building *_gate = _mapPlayerState->getBuilding(kBuildingGate, 0)) {
        m_pathToGoal.push_back(_gate->getPosition()+g_gameContent->tileSize);
    }else {
//       m_pathToGoal.push_back(ccp(3180.0f,2400.0f));
        m_pathToGoal.insert(m_pathToGoal.begin(), IsoMap::getPositionByMapIndex(7242, g_gameContent->tileSize, g_gameContent->tileShift));

    }
    setVisitorState(VTS_WALK);
     
}

void Visitor::goHome() {
    
    CCLog_("GO HOME!");
    IsoMap *_isoMap = (IsoMap*)isoMap;
    PlayerState *_mapPlayerState = (PlayerState*)_isoMap->mapPlayerState;
    CCPoint _currSnapPos = _isoMap->getSnapGridPosition(this->getPosition(), g_gameContent->tileSize, g_gameContent->tileShift, CCSize(1, 1));
    CCPoint _toPos = IsoMap::getPositionByMapIndex(7242, g_gameContent->tileSize, g_gameContent->tileShift);//ccp(3180.0f,2400.0f);
    if (Building *_gate = _mapPlayerState->getBuilding(kBuildingGate, 0)) {
//        m_pathToGoal.push_back(_gate->getPosition()+g_gameContent->tileSize);
        _toPos = _gate->getPosition()+g_gameContent->tileSize;
    }
    m_initializedStartGoal = false;
    findPath(_currSnapPos, _toPos);
    int i =0;
    while (!m_foundGoal && i<1000) {;//ISO_TILE_NUM) {
        continuePath();
        i++;
    }
    if (m_pathToGoal.size()>0) m_pathToGoal.erase(m_pathToGoal.begin());
//    m_pathToGoal.insert(m_pathToGoal.begin(), ccp(3012.0f,2264.0f));
//    m_pathToGoal.insert(m_pathToGoal.begin(), ccp(3498.0f,1932.0f));
    m_pathToGoal.insert(m_pathToGoal.begin(), IsoMap::getPositionByMapIndex(6994, g_gameContent->tileSize, g_gameContent->tileShift));
    m_pathToGoal.insert(m_pathToGoal.begin(), IsoMap::getPositionByMapIndex(2915, g_gameContent->tileSize, g_gameContent->tileShift));
    
    setVisitorState(VTS_WALK);
    
}

void Visitor::findPath(CCPoint currentPos_, CCPoint targetPos_) {
    
    if (!m_initializedStartGoal) {
        for (int i=0; i<m_openList.size(); i++) {
            if (m_openList[i] == m_startCell) {
                m_startCell = NULL;
            }
            if (m_openList[i] == m_goalCell) {
                m_goalCell = NULL;
            }
            delete m_openList[i];
        }
        m_openList.clear();
        
        for (int i=0; i<m_visitedList.size(); i++) {
            if (m_visitedList[i] == m_startCell) {
                m_startCell = NULL;
            }
            if (m_visitedList[i] == m_goalCell) {
                m_goalCell = NULL;
            }
            delete m_visitedList[i];
        }
        m_visitedList.clear();
        m_pathToGoal.clear();
        
        //Initialize start
        SearchCell start;
        start.m_xcoord = currentPos_.x;
        start.m_ycoord = currentPos_.y;

        SearchCell goal;
        goal.m_xcoord = targetPos_.x;
        goal.m_ycoord = targetPos_.y;
        
        setStartAndGoal(start, goal);
        m_initializedStartGoal = true;
        m_foundGoal = false;
        m_countDeep = 0;
    }
    
    if (m_initializedStartGoal) {
        continuePath();
    }
    
}

void Visitor::setStartAndGoal(SearchCell start_, SearchCell goal_) {
    
    if (!m_startCell) {
        m_startCell = new SearchCell(start_.m_xcoord, start_.m_ycoord, NULL);
    }else {
        m_startCell->m_xcoord = start_.m_xcoord;
        m_startCell->m_ycoord = start_.m_ycoord;
        m_startCell->parent = NULL;
    }
    m_startCell->m_id = IsoMap::getTileIndexByPosition(ccp(start_.m_xcoord, start_.m_ycoord), g_gameContent->tileSize, g_gameContent->tileShift);

    if (!m_goalCell) {
        m_goalCell = new SearchCell(goal_.m_xcoord, goal_.m_ycoord, &goal_);
    }else {
        m_goalCell->m_xcoord = start_.m_xcoord;
        m_goalCell->m_ycoord = start_.m_ycoord;
        m_goalCell->parent = &goal_;
    }
    m_goalCell->m_id = IsoMap::getTileIndexByPosition(ccp(goal_.m_xcoord, goal_.m_ycoord), g_gameContent->tileSize, g_gameContent->tileShift);
    
    m_startCell->G = 0;
    m_startCell->H = m_startCell->ManHattatDistance(m_goalCell);
    m_startCell->parent = NULL;
    
    m_openList.push_back(m_startCell);
    
}

SearchCell *Visitor::getNextCell() {
    
    float bestF = 9999999.0f;
    int cellIndex = -1;
    SearchCell *nextCell = NULL;
    
    for (int i=0; i<m_openList.size(); i++) {
        if (m_openList[i]->GetF() < bestF) {
            bestF = m_openList[i]->GetF();
            cellIndex = i;
        }
    }
    
    if (cellIndex >= 0) {
        nextCell = m_openList[cellIndex];
        m_visitedList.push_back(nextCell);
        m_openList.erase(m_openList.begin() + cellIndex);
    }
    
    return nextCell;
    
}

void Visitor::pathOpened(int x_, int y_, float newcost_, SearchCell *parent_) {
 
    int id = IsoMap::getTileIndexByPosition(ccp(x_, y_), g_gameContent->tileSize, g_gameContent->tileShift);
    
    int ids[] = {
        IsoMap::getTileIndexByPosition(ccp(x_+g_gameContent->tileSize.width*0.1f, y_+g_gameContent->tileSize.height*0.1f), g_gameContent->tileSize, g_gameContent->tileShift),
        IsoMap::getTileIndexByPosition(ccp(x_+g_gameContent->tileSize.width*0.1f, y_-g_gameContent->tileSize.height*0.1f), g_gameContent->tileSize, g_gameContent->tileShift),
        IsoMap::getTileIndexByPosition(ccp(x_-g_gameContent->tileSize.width*0.1f, y_-g_gameContent->tileSize.height*0.1f), g_gameContent->tileSize, g_gameContent->tileShift),
        IsoMap::getTileIndexByPosition(ccp(x_-g_gameContent->tileSize.width*0.1f, y_+g_gameContent->tileSize.height*0.1f), g_gameContent->tileSize, g_gameContent->tileShift)
    };
    
    IsoMap *_isoMap = (IsoMap*)isoMap;
    Building *buildings[] = {
        (Building*)_isoMap->tilesMap[ids[0]],
        (Building*)_isoMap->tilesMap[ids[1]],
        (Building*)_isoMap->tilesMap[ids[2]],
        (Building*)_isoMap->tilesMap[ids[3]],
    };
    
    int i, j = 0;
    for (i=0; i<4; i++) {
        if (Building *building = buildings[i]) {
            if ((building != _isoMap->reservedDummyForTrack) &&
                (building == buildings[0])) {
                j++;
            }
        }
    }
    if (j >= 4) {
        return;
    }

    for (int i=0; i<m_visitedList.size(); i++) {
        if (id == m_visitedList[i]->m_id) {
            return;
        }
    }
    
    SearchCell *newChild = new SearchCell(x_, y_, parent_);
    newChild->m_id = IsoMap::getTileIndexByPosition(ccp(x_, y_), g_gameContent->tileSize, g_gameContent->tileShift);
    newChild->G = newcost_;
    newChild->H = parent_->ManHattatDistance(m_goalCell);
    
    for (int i=0; i<m_openList.size(); i++) {
        if (id == m_openList[i]->m_id) {
            float newF = newChild->G + newcost_ + m_openList[i]->H;
            if (m_openList[i]->GetF() > newF) {
                m_openList[i]->G = newChild->G + newcost_;
                m_openList[i]->parent = newChild;
            }else {
                delete newChild;
                newChild = NULL;
                return;
            }
        }
    }
    
    m_openList.push_back(newChild);
    
}

void Visitor::continuePath() {
    
    if (m_openList.empty()) {
        m_initializedStartGoal = false;
        return;
    }
    
    SearchCell *currentCell = getNextCell();
    if (currentCell->m_id == m_goalCell->m_id) {
        
        m_goalCell->parent = currentCell;
        SearchCell *getPath;
        for (getPath = m_goalCell; getPath != NULL; getPath=getPath->parent) {
            m_pathToGoal.push_back(ccp(getPath->m_xcoord, getPath->m_ycoord));
        }
        m_initializedStartGoal = false;
        m_foundGoal = true;
        this->setVisitorState(m_state);
        return;
        
    }else {
        //rightCell
        pathOpened(currentCell->m_xcoord+g_gameContent->tileSize.width*0.5f,
                   currentCell->m_ycoord,
                   currentCell->G + 7,
                   currentCell);
        //leftCell
        pathOpened(currentCell->m_xcoord-g_gameContent->tileSize.width*0.5f,
                   currentCell->m_ycoord,
                   currentCell->G + 7,
                   currentCell);
        //upCell
        pathOpened(currentCell->m_xcoord,
                   currentCell->m_ycoord+g_gameContent->tileSize.height*0.5f,
                   currentCell->G + 7,
                   currentCell);
        //downCell
        pathOpened(currentCell->m_xcoord,
                   currentCell->m_ycoord-g_gameContent->tileSize.height*0.5f,
                   currentCell->G + 7,
                   currentCell);
        //left-up diagonal cell
        pathOpened(currentCell->m_xcoord-g_gameContent->tileSize.width*0.5f,
                   currentCell->m_ycoord+g_gameContent->tileSize.height*0.5f,
                   currentCell->G + 10,
                   currentCell);
        //right-up diagonal
        pathOpened(currentCell->m_xcoord+g_gameContent->tileSize.width*0.5f,
                   currentCell->m_ycoord+g_gameContent->tileSize.height*0.5f,
                   currentCell->G + 10,
                   currentCell);
        //left-down diagonal cell
        pathOpened(currentCell->m_xcoord-g_gameContent->tileSize.width*0.5f,
                   currentCell->m_ycoord-g_gameContent->tileSize.height*0.5f,
                   currentCell->G + 10,
                   currentCell);
        //right-down diagonal
        pathOpened(currentCell->m_xcoord+g_gameContent->tileSize.width*0.5f,
                   currentCell->m_ycoord-g_gameContent->tileSize.height*0.5f,
                   currentCell->G + 10,
                   currentCell);
        
        for (int i=0; i<m_openList.size(); i++) {
            if (currentCell->m_id == m_openList[i]->m_id) {
                m_openList.erase(m_openList.begin()+i);
            }
        }
        
    }
    
    if (m_countDeep > ISO_TILE_NUM) {
        m_initializedStartGoal = false;
    }
    m_countDeep++;
    
}

CCPoint Visitor::nextPathPos() {

    CCPoint nextPos = m_pathToGoal[m_pathToGoal.size()-1];
    m_pathToGoal.pop_back();
    return nextPos;

}

void Visitor::pauseVisitor()
{
    m_visitorSpine->timeScale = 0.0f;
    pauseSchedulerAndActions();
}

void Visitor::resumeVisitor()
{
    m_visitorSpine->timeScale = 1.0f;
    resumeSchedulerAndActions();
}

bool Visitor::updateMoving(float delta_) {

    if (m_state == VTS_WALK || m_state == VTS_RUN) {

        float _speed = kVisitorWalkingSpeed*(1+(targetDevice == TD_IPAD_RETINA));
        if (m_state == VTS_RUN) {
            _speed = kVisitorRunningSpeed*(1+(targetDevice == TD_IPAD_RETINA));
        }
        
        CCPoint _currPosition = this->getPosition();
        float x = m_distPosition.x - _currPosition.x;
        float y = m_distPosition.y - _currPosition.y;
        float d = sqrtf(x*x + y*y);
        
        if (d > _speed) {
            
            float a = atan2f(y, x);
            _currPosition.x += _speed * cosf(a);
            _currPosition.y += _speed * sinf(a);
            this->setPosition(_currPosition);
            
        }else {
        
            if (m_pathToGoal.size() > 0) {
                m_distPosition = m_pathToGoal[m_pathToGoal.size()-1];
                m_pathToGoal.pop_back();
                _flip(m_distPosition.x > this->getPositionX());
            }else {
                if (m_goHomeState == 1) {
                    m_goHomeState = 2;
                }
                return false;
            }
            
        }
        
    }else {
        
        if (m_visitorSpine->states[0]) {
            if (m_visitorSpine->states[0]->time > m_visitorSpine->states[0]->animation->duration) {
                return false;
            }
        }

    }
    
    return true;
    
}

void Visitor::decisionMaking(void *isoMap_) {
   
    isoMap = isoMap_;
    IsoMap *_isoMap = (IsoMap*)isoMap;
    if (!_isoMap) {
        return;
    }
    
    PlayerState *_mapPlayerState = (PlayerState*)_isoMap->getMapPlayerState();
    if (!_mapPlayerState) {
        return;
    }
    
    if (m_actionCount <= 0 && m_goHomeState == 0) {
        m_goHomeState = 1;
        goHome();
        return;
    }
    
    if (m_goHomeState == 1) {
        return;
    }
    
    if (m_goHomeState == 2) {
        m_goHomeState = 3;
        pleaseReleaseMe = true;
    }
    
    m_tileSize = g_gameContent->tileSize;
    
    int r = arc4random()%100;
    int n = VisitorStateLength;
    m_state = (VisitorState)r;
    if (r >= n) {
        m_state = VTS_WALK;
    }
    
    if (m_state == VTS_WALK ||
        m_state == VTS_RUN) {
        /*
        Building *building = _mapPlayerState->Buildings[rand()%_mapPlayerState->Buildings.size()];
        if ((building->pbVo.type == kBuildingTicket) ||
            (building->pbVo.type == kBuildingGate)) {
                return;
        }*/
        CCPoint _currSnapPos = _isoMap->getSnapGridPosition(this->getPosition(), g_gameContent->tileSize, g_gameContent->tileShift, CCSize(1, 1));
        CCPoint _toSnapPos = getRandomPointNearBuilding(_mapPlayerState);
        /*
        int rn = arc4random()%9;
        switch (rn) {
            default:
            case 0:
                _toSnapPos = building->getPosition()+ccp(-g_gameContent->tileSize.width*building->bVo.grid_col*0.50f,
                                                         -g_gameContent->tileSize.height*building->bVo.grid_row*0.0f);
                break;
            case 1:
                _toSnapPos = building->getPosition()+ccp(-g_gameContent->tileSize.width*building->bVo.grid_col*0.375f,
                                                         -g_gameContent->tileSize.height*building->bVo.grid_row*0.125f);
            case 2:
                _toSnapPos = building->getPosition()+ccp(-g_gameContent->tileSize.width*building->bVo.grid_col*0.25f,
                                                         -g_gameContent->tileSize.height*building->bVo.grid_row*0.25f);
                break;
            case 3:
                _toSnapPos = building->getPosition()+ccp(-g_gameContent->tileSize.width*building->bVo.grid_col*0.125f,
                                                         -g_gameContent->tileSize.height*building->bVo.grid_row*0.375f);
                break;
            case 4:
                _toSnapPos = building->getPosition()+ccp(-g_gameContent->tileSize.width*building->bVo.grid_col*0.00f,
                                                         -g_gameContent->tileSize.height*(building->bVo.grid_row*0.5f));
                break;
            case 5:
                _toSnapPos = building->getPosition()+ccp( g_gameContent->tileSize.width*building->bVo.grid_col*0.125f,
                                                         -g_gameContent->tileSize.height*building->bVo.grid_row*0.375f);
                break;
            case 6:
                _toSnapPos = building->getPosition()+ccp( g_gameContent->tileSize.width*building->bVo.grid_col*0.25f,
                                                         -g_gameContent->tileSize.height*building->bVo.grid_row*0.25f);
                break;
            case 7:
                _toSnapPos = building->getPosition()+ccp( g_gameContent->tileSize.width*building->bVo.grid_col*0.375f,
                                                         -g_gameContent->tileSize.height*building->bVo.grid_row*0.125f);
                break;
            case 8:
                _toSnapPos = building->getPosition()+ccp( g_gameContent->tileSize.width*building->bVo.grid_col*0.50f,
                                                         -g_gameContent->tileSize.height*building->bVo.grid_row*0.0f);
                break;
        }
        */

        findPath(_currSnapPos, _toSnapPos);
        
    }else {
        m_actionCount --;
    }
    
    this->setVisitorState(m_state);
    
}

void Visitor::setVisitorState(VisitorState state_) {
    
    switch (state_) {
        default:
        case VTS_IDLE:
            m_visitorSpine->setAnimation(kVisitorAnimationIdle, true);
            break;
        case VTS_WALK:
            m_visitorSpine->setAnimation(kVisitorAnimationWalk, true);
            break;
        case VTS_DRINK:
            m_visitorSpine->setAnimation(kVisitorAnimationDrink, false);
            break;
        case VTS_BYE:
            m_visitorSpine->setAnimation(kVisitorAnimationBye, false);
            break;
        case VTS_CAMERA:
            m_visitorSpine->setAnimation(kVisitorAnimationCamera, false);
            break;
        case VTS_POPCORN:
            m_visitorSpine->setAnimation(kVisitorAnimationPopCorn, false);
            break;
        case VTS_HEAD1:
            m_visitorSpine->setAnimation(kVisitorAnimationHeadUp, false);
            break;
        case VTS_HEAD2:
            m_visitorSpine->setAnimation(kVisitorAnimationHeadDown, false);
            break;
        case VTS_JUMP1:
            m_visitorSpine->setAnimation(kVisitorAnimationJump1, false);
            break;
        case VTS_JUMP2:
            m_visitorSpine->setAnimation(kVisitorAnimationJump2, false);
            break;
        case VTS_STRETCH:
            m_visitorSpine->setAnimation(kVisitorAnimationStretch, false);
            break;
        case VTS_RUN:
            m_visitorSpine->setAnimation(kVisitorAnimationRun, true);
            break;
    }
    
    m_state = state_;
    
}

void Visitor::setDistAndCurrPos(cocos2d::CCPoint position_) {
    
    this->setPosition(position_);
    m_distPosition = position_;
    
}

void Visitor::_flip(bool flip_) {
    
    float _scale_x = fabsf(this->getScaleX());
    
    if (flip_) {
        this->setScaleX(-_scale_x);
    }else {
        this->setScaleX( _scale_x);
    }
    
}

void Visitor::dropCoin()
{
    if (m_coin) return;
    
    Visitor *_visitor = this;
    _visitor->setVisitorState(VTS_JUMP1);
    
    TouchableItem *coin = TouchableItem::createWithSpriteFrameName("cheese.png", this);
    coin->onTabbed = Visitor::collectCoin;
    coin->setSender(this);
    if (_visitor->getScaleX() == 1) {
        coin->setPosition(_visitor->getPosition() + ccp(- coin->getContentSize().width*0.5f, _visitor->getContentSize().height - coin->getContentSize().height*0.5*0.65f));
    } else {
        coin->setPosition(_visitor->getPosition() + ccp(- coin->getContentSize().width*0.5*0.75f, _visitor->getContentSize().height - coin->getContentSize().height*0.5*0.65f));
    }
    
    IsoMap *isoMap = (IsoMap *)this->isoMap;
    isoMap->addChild(coin, int(1000000));
    
    CCSpriteFrame *frame1 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("_coin_anim_1.png");
    CCSpriteFrame *frame2 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("_coin_anim_2.png");
    CCSpriteFrame *frame3 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("_coin_anim_3.png");
    CCSpriteFrame *frame4 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("_coin_anim_4.png");
    CCSpriteFrame *frame5 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("_coin_anim_3.png");
    CCSpriteFrame *frame6 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("_coin_anim_2.png");
    
    CCAnimation * anim = CCAnimation::create();
    anim->addSpriteFrame(frame1);
    anim->addSpriteFrame(frame2);
    anim->addSpriteFrame(frame3);
    anim->addSpriteFrame(frame4);
    anim->setDelayPerUnit(0.1f);
    anim->setLoops(true);
    CCAnimate *theAnim = CCAnimate::create(anim);
    
    CCAnimation * anim2 = CCAnimation::create();
    anim2->addSpriteFrame(frame5);
    anim2->addSpriteFrame(frame6);
    anim2->setDelayPerUnit(0.1f);
    anim2->setLoops(true);
    CCAnimate *theAnim2 = CCAnimate::create(anim2);
    coin->m_itemSprite->runAction(CCRepeatForever::create(CCSequence::create(theAnim, CCFlipX::create(true), theAnim2, CCFlipX::create(false), NULL)));
    
    float g = 9.8f*6;
    float u = 100;
    float allTime = 0;
    
    float ux = arc4random()%50 + 50;
    coin->setScale(0);
    
    CCArray *actions = CCArray::create();
    actions->addObject(CCDelayTime::create(0.3f));
    
    float c1 = (arc4random()%6)*0.1f + 0.5f;
    float c2 = (arc4random()%11)*0.1f + 0.5f;
    
    while (u > 20) {
        float s = (u*u)/(2*g);
        float t = sqrtf(2*s/g)/5;
        float sx = ux*t;
        sx = _visitor->getScaleX() != 1 ? sx : -sx;
        CCMoveBy *move1 = CCMoveBy::create(t, ccp(sx, s));
        
        CCMoveBy *move2 = CCMoveBy::create(t*c2, ccp(sx*c1, -s*c2));
        actions->addObject(move1);
        actions->addObject(move2);
        allTime += (t + t*0.9f);
        u = u/2.f;
    }
    coin->runAction(CCScaleTo::create(allTime, 0.5f));
    coin->runAction(CCSequence::create(actions));
    
    m_coin = coin;

    this->runAction(CCSequence::create(CCDelayTime::create(allTime),
                                       CCCallFunc::create(this, callfunc_selector(Visitor::dropCoinComplete)),
                                       NULL));
}

void Visitor::dropCoinComplete()
{
    if (m_pet->isActiveGlowing()) {
        this->autoCollectionToPet();
    } else {
        this->runAction(CCSequence::create(CCDelayTime::create(1),
                                           CCCallFunc::create(this, callfunc_selector(Visitor::removeCoinEffect)),
                                        NULL));
    }
}

void Visitor::removeCoinEffect()
{
    if (!m_coin) return;
    
    CCArray *fadeInOuts = CCArray::create();
    for (int i = 0; i < 10; i++) {
        fadeInOuts->addObject(CCFadeIn::create(0.2f - i*0.02f));
        fadeInOuts->addObject(CCFadeOut::create(0.2f - i*0.02f));
    }
    fadeInOuts->addObject(CCCallFunc::create(this, callfunc_selector(Visitor::removeCoin)));
    m_coin->m_itemSprite->runAction(CCSequence::create(fadeInOuts));
}

void Visitor::removeCoin()
{
    if (!m_coin) return;
    m_coin->m_itemSprite->stopAllActions();
    m_coin->removeFromParentAndCleanup(true);
    m_coin = NULL;
}

void Visitor::collectCoin()
{
    if (!m_coin) return;
    addLabelEffect();
    removeCoin();
}

void Visitor::addLabelEffect()
{
    
}

void Visitor::removeLabel()
{
    
}

void Visitor::collectCoin(void *inst_, void *sender_)
{
    Visitor *visitor = (Visitor*)sender_;
    visitor->collectCoin();
}

CCPoint Visitor::getRandomPointNearBuilding(void *player_state_) {
    
    CCPoint point = CCPointZero;
    
    if (PlayerState *_mapPlayerState = (PlayerState*)player_state_) {
        Building *building = _mapPlayerState->Buildings[rand()%_mapPlayerState->Buildings.size()];
        int i = 0;
        while (((building->pbVo.type == kBuildingTicket) ||
                   (building->pbVo.type == kBuildingGate)) && i < 100) {
            building = _mapPlayerState->Buildings[rand()%_mapPlayerState->Buildings.size()];
            i++;
        }
        
        
        int rn = arc4random()%9;
        switch (rn) {
            default:
            case 0:
                point = building->getPosition()+ccp(-g_gameContent->tileSize.width*building->bVo.grid_col*0.50f,
                                                         -g_gameContent->tileSize.height*building->bVo.grid_row*0.0f);
                break;
            case 1:
                point = building->getPosition()+ccp(-g_gameContent->tileSize.width*building->bVo.grid_col*0.375f,
                                                         -g_gameContent->tileSize.height*building->bVo.grid_row*0.125f);
            case 2:
                point = building->getPosition()+ccp(-g_gameContent->tileSize.width*building->bVo.grid_col*0.25f,
                                                         -g_gameContent->tileSize.height*building->bVo.grid_row*0.25f);
                break;
            case 3:
                point = building->getPosition()+ccp(-g_gameContent->tileSize.width*building->bVo.grid_col*0.125f,
                                                         -g_gameContent->tileSize.height*building->bVo.grid_row*0.375f);
                break;
            case 4:
                point = building->getPosition()+ccp(-g_gameContent->tileSize.width*building->bVo.grid_col*0.00f,
                                                         -g_gameContent->tileSize.height*(building->bVo.grid_row*0.5f));
                break;
            case 5:
                point = building->getPosition()+ccp( g_gameContent->tileSize.width*building->bVo.grid_col*0.125f,
                                                         -g_gameContent->tileSize.height*building->bVo.grid_row*0.375f);
                break;
            case 6:
                point = building->getPosition()+ccp( g_gameContent->tileSize.width*building->bVo.grid_col*0.25f,
                                                         -g_gameContent->tileSize.height*building->bVo.grid_row*0.25f);
                break;
            case 7:
                point = building->getPosition()+ccp( g_gameContent->tileSize.width*building->bVo.grid_col*0.375f,
                                                         -g_gameContent->tileSize.height*building->bVo.grid_row*0.125f);
                break;
            case 8:
                point = building->getPosition()+ccp( g_gameContent->tileSize.width*building->bVo.grid_col*0.50f,
                                                         -g_gameContent->tileSize.height*building->bVo.grid_row*0.0f);
                break;
        }
    }
    
    return point;
    
}

void Visitor::autoCollectionToPet()
{
    if (!m_coin) return;
//    CCPoint finalPoint = m_pet->getPositionAfterSecond(0.1f);
//    m_coin->runAction(CCMoveTo::create(0.1f, finalPoint));
//    m_coin->m_itemSprite->runAction(CCSequence::create(CCFadeOut::create(0.1f),
//                                                       CCCallFunc::create(this, callfunc_selector(Visitor::collectCoin)),
//                                                       NULL));
}

void Visitor::startRepeatAction()
{
    randomAction();
    this->runAction(CCSequence::create(CCDelayTime::create(3 + arc4random()%5),
                                       CCCallFunc::create(this, callfunc_selector(Visitor::startRepeatAction)),
                                       NULL));
}

void Visitor::randomAction()
{
    switch (arc4random()%6) {
        case 0:
            setVisitorState(VTS_CAMERA);
            break;
        case 1:
            setVisitorState(VTS_POPCORN);
            break;
        case 2:
            setVisitorState(VTS_CAMERA);
            break;
        case 3:
            setVisitorState(VTS_POPCORN);
            break;
        case 4:
            setVisitorState(VTS_JUMP1);
            break;
        case 5:
            setVisitorState(VTS_JUMP2);
            break;
        default:
            break;
    }
}
