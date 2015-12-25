//
//  PathFinding.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/18/2556 BE.
//
//

#ifndef __Cheetah__PathFinding__
#define __Cheetah__PathFinding__

#include "cocos2d.h"
#include <math.h>

#define WORLD_SIZE 64

struct SearchCell {
    
public:
    
    int m_xcoord, m_ycoord;
    int m_id;
    SearchCell *parent;
    float G;
    float H;
    
    SearchCell() : parent(0) {};
    SearchCell(int x, int y, SearchCell *parent_=0) : m_xcoord(0), m_ycoord(0),
    parent(parent_), m_id((y * 64 )+ x), G(0), H(0) {};
  
    float GetF() { return G + H; }
    float ManHattatDistance(SearchCell*nodeEnd) {
        float x = (float)(fabs(this->m_xcoord - nodeEnd->m_xcoord));
        float y = (float)(fabs(this->m_ycoord - nodeEnd->m_ycoord));
        return x + y;
    }

};

class PathFinding {
    
    bool m_initialzedStartGoal;
    bool m_foundGoal;
    
    cocos2d::CCSize m_space;
    int m_min;
    int m_max;
    int *m_map;
    int m_mapsize;
    int m_mapcol;
    
    SearchCell *m_startCell;
    SearchCell *m_goalCell;
    
    std::vector<SearchCell*>        m_openList;
    std::vector<SearchCell*>        m_visitedList;
    std::vector<cocos2d::CCPoint>  m_pathToGoal;
    
    void setStartAndGoal(SearchCell start, SearchCell goal);
    void pathOpened(int x, int y, float newCost, SearchCell *parent);
    SearchCell *getNextCell();
    void continuePath();
    
public:
    
    PathFinding();
    ~PathFinding();
    
    void setup(cocos2d::CCSize space, int min, int max, int *map, int mapsize, int mapcol);
    void findPath(cocos2d::CCPoint currentPos, cocos2d::CCPoint targetPos);
    cocos2d::CCPoint nextPathPos(cocos2d::CCPoint pos_);
    void clearOpenList() { m_openList.clear(); }
    void clearVisitList() { m_visitedList.clear(); }
    void clearPathToGoal() { m_pathToGoal.clear(); }

};

#endif /* defined(__Cheetah__PathFinding__) */
