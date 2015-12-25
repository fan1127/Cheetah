//
//  Visitor.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/15/2556 BE.
//
//

#ifndef __Cheetah__Visitor__
#define __Cheetah__Visitor__

#include "cocos2d.h"
#include <spine/spine-cocos2dx.h>

#include "../../Entities/IsoMap.h"
#include "../../Helpers/SearchCell.h"

#define kVisitorSrcDir              "common/environment/"

#define kVisitorWalkingSpeed        0.25f
#define kVisitorRunningSpeed        0.5f

#define kVisitorAnimationIdle       "idle_stand"
#define kVisitorAnimationWalk       "walk"
#define kVisitorAnimationRun        "run"
#define kVisitorAnimationBye        "idle_bye"
#define kVisitorAnimationDrink      "idle_drink"
#define kVisitorAnimationCamera     "idle_stand_camera"
#define kVisitorAnimationHeadUp     "idle_stand_head_up"
#define kVisitorAnimationHeadDown   "idle_stand_head_low"
#define kVisitorAnimationJump1      "idle_stand_jump1"
#define kVisitorAnimationJump2      "idle_stand_jump2"
#define kVisitorAnimationPopCorn    "idle_stand_popcorn"
#define kVisitorAnimationStretch    "idle_stretch_oneself"

#define kVisitorActionMin           5
#define kVisitorActionMax           25

enum VisitorState {
    VTS_IDLE,
    VTS_WALK,
    VTS_DRINK,
    VTS_BYE,
    VTS_CAMERA,
    VTS_POPCORN,
    VTS_HEAD1,
    VTS_HEAD2,
    VTS_JUMP1,
    VTS_JUMP2,
    VTS_STRETCH,
    VTS_RUN,
    VisitorStateLength
};

class TouchableItem;

class Visitor: public cocos2d::CCNode {
    
    cocos2d::extension::CCSkeletonAnimation*    m_visitorSpine;
    cocos2d::CCAction*                          m_moveAction;
    VisitorState                                m_state;
    cocos2d::CCPoint                            m_distPosition;
    int                                         m_actionCount;
    int                                         m_goHomeState;
    
    cocos2d::CCSize                             m_tileSize;
    
    void _flip(bool flip_);


public:
    
    TouchableItem*                              m_coin;
    
    Pet*                                        m_pet;
    
    void*                                       isoMap;
    bool                                        pleaseReleaseMe;
    
    static Visitor *createWithRandomSpineName();
    static Visitor *createWithSpineName(const char *spine_name_);
    bool initWithSpineName(const char *spine_name_);
    virtual ~Visitor();
//    virtual void cleanup();
    
    void pauseVisitor();
    void resumeVisitor();
    
    bool updateMoving(float delta_);
    void decisionMaking(void *isoMap_);
    
    //Fix path
    void enterRanch();
    void goHome();
    
    //Path finding
    void findPath(cocos2d::CCPoint currentPos_, cocos2d::CCPoint targetPos);
    cocos2d::CCPoint nextPathPos();
    void ClearOpenList() { m_openList.clear(); };
    void ClearVisitedList() { m_visitedList.clear(); };
    void ClearPathToGoal() { m_pathToGoal.clear(); };
    bool m_initializedStartGoal;
    bool m_foundGoal;
    void setStartAndGoal(SearchCell start_, SearchCell goal_);
    void pathOpened(int x_, int y_, float newcost_, SearchCell *parent_);
    SearchCell *getNextCell();
    void continuePath();
    SearchCell *m_startCell;
    SearchCell *m_goalCell;
    std::vector<SearchCell*> m_openList;
    std::vector<SearchCell*> m_visitedList;
    std::vector<cocos2d::CCPoint> m_pathToGoal;
    int m_countDeep;
    
    void setVisitorState(VisitorState state_);
    virtual void setDistAndCurrPos(cocos2d::CCPoint position_);
    
    // Path Finding
//    void findPath(cocos2d::CCPoint currentPos_, cocos2d::CCPoint targetPos_);

    void dropCoin();
    void dropCoinComplete();
    void removeCoinEffect();
    void removeCoin();
    void collectCoin();
    void addLabelEffect();
    void removeLabel();
    static void collectCoin(void *inst_, void *sender_);
    
    cocos2d::CCPoint getRandomPointNearBuilding(void *player_state_);
    
    void autoCollectionToPet();
    
    void startRepeatAction();
    void randomAction();
};

#endif /* defined(__Cheetah__Visitor__) */
