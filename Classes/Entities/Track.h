//
//  Track.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/6/56 BE.
//
//

#ifndef __Cheetah__Track__
#define __Cheetah__Track__

#include "cocos2d.h"
#include "../VOs/PlayerTrackVO.h"

// __
// \ \
//  \_\ //
#define TTYPE_STRAIGHT_UP_LEFT 3

//    __
//   / /
//  /_/ //
#define TTYPE_STRAIGHT_UP_RIGHT 1

#define TTYPE_OUTER_CORNER_TOP_LEFT 5
#define TTYPE_OUTER_CORNER_TOP_RIGHT 2
#define TTYPE_OUTER_CORNER_BOTTOM_LEFT 6
#define TTYPE_OUTER_CORNER_BOTTOM_RIGHT 4

class Visitor;
class Pet;
class BananaTrap;

class Track: public cocos2d::CCNode {
    
    cocos2d::CCSprite*      m_trackSprite;
    cocos2d::CCSprite*      m_startSprite;
    cocos2d::CCSprite*      m_directionSprite;
    cocos2d::CCSprite*      m_markSprite;
    float                   m_directionRunnber;
    
    bool m_hasTrap;
    bool m_trapReserved;
    
    bool m_trapBlacklistedForAI;
    int m_trapBlacklistedRangeForAI;
    
    BananaTrap *m_bananaTrap;
    
public:
#ifdef DEBUG_TRACK
    cocos2d::CCLabelBMFont *debugNo;
#endif
    void                    *preferenceRootParent;
    cocos2d::CCArray        *m_visitors;
    
    GDTrackVO           tVo;
    PlayerTrackVO       ptVo;
    
    bool                trackStart;
    bool                trackDirection;
    
    Track*              nextTrack;
    Track*              prevTrack;
    
    int                 trackIndex;
    
    Pet*                m_petOwnerTrap;
    
    static Track* createWithTrackVO(GDTrackVO vo, PlayerTrackVO ptvo);
    
    bool initWithTrackVO(GDTrackVO tvo, PlayerTrackVO ptvo);
    virtual ~Track();
//    virtual void cleanup();
    void changeTrackType(GDTrackVO tvo_);
    void updateSprite();
    void updatePosition();
    void updateFlip();
    
    bool setAsTrackStart(bool start);
    bool changeTrack(cocos2d::CCPoint position, cocos2d::CCSize tileSize, cocos2d::CCPoint shift, bool force);
    bool changeObstacle(cocos2d::CCPoint position, cocos2d::CCSize tileSize, cocos2d::CCPoint shift);
//    void showMark(bool show_);
    
    void addVisitor(Visitor *visitor);
    cocos2d::CCArray *getVisitors();
    void visitorDropCoin();
    
    void setTrapBlacklistedForAIForRange(bool blacklist, int range);
    int trapBlacklistedRangeForAI() { return m_trapBlacklistedRangeForAI; };
    bool trapBlacklistedForAI() { return m_trapBlacklistedForAI; };
    bool trapAvailable() { return !(m_trapReserved || m_hasTrap || m_trapBlacklistedForAI); };
    bool trapReserved() { return m_trapReserved; };
    void reserveTrap(bool reserve);
    void addTrap(BananaTrap *bananaTrap);
    void removeBananaTrap();
    void removeTrap();
    void removeTrapWithoutRemovingTrapBlackList();
    bool hasTrap() { return m_hasTrap; };

    cocos2d::CCRect getTrackBoundingBox() const;
};

#endif /* defined(__Cheetah__Track__) */
