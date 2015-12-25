//
//  PathFinding.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/18/2556 BE.
//
//

#include "PathFinding.h"
USING_NS_CC;

PathFinding::PathFinding() {
    
    m_initialzedStartGoal = false;
    m_foundGoal = false;
    
}

PathFinding::~PathFinding() {
    
}

void PathFinding::setup(cocos2d::CCSize space, int min, int max, int *map, int mapsize, int mapcol) {
    
    m_space = space;
    m_min = min;
    m_max = max;
    m_map = map;
    m_mapsize = mapsize;
    m_mapcol = mapcol;
    
}

void PathFinding::findPath(CCPoint currentPos_, CCPoint targetPos_) {
    
    if (!m_initialzedStartGoal) {
        
        for (int i=0; i<m_openList.size(); i++) {
            delete m_openList[i];
        }
        m_openList.clear();
        
        for (int i=0; i<m_visitedList.size(); i++) {
            delete m_visitedList[i];
        }
        m_visitedList.clear();
        
        for (int i=0; m_pathToGoal.size(); i++) {
//            delete m_pathToGoal[i];
        }
        m_pathToGoal.clear();
    
        // Initialize start
        SearchCell _start;
        _start.m_xcoord = currentPos_.x;
        _start.m_ycoord = currentPos_.y;
        
        // Initialize goal
        SearchCell _goal;
        _goal.m_xcoord = targetPos_.x;
        _goal.m_ycoord = targetPos_.y;
        
        setStartAndGoal(_start, _goal);
        m_initialzedStartGoal = true;
        
    }

    if (m_initialzedStartGoal) {
        
        continuePath();
        
    }
    
}

void PathFinding::setStartAndGoal(SearchCell start_, SearchCell goal_) {
    
    m_startCell = new SearchCell(start_.m_xcoord, start_.m_ycoord, NULL);
    m_goalCell = new SearchCell(goal_.m_xcoord, goal_.m_ycoord, &goal_);
    
    m_startCell->G = 0;
    m_startCell->H = m_startCell->ManHattatDistance(m_goalCell);
    m_startCell->parent = NULL;
    
    m_openList.push_back(m_startCell);
    
}

SearchCell *PathFinding::getNextCell() {
    
    float _bestF = 999999.0f;
    int _cellIndex = -1;
    SearchCell *_nextCell = NULL;
    
    for (int i=0; i<m_openList.size(); i++) {
        if (m_openList[i]->GetF() < _bestF) {
            _bestF = m_openList[i]->GetF();
            _cellIndex = i;
        }
    }
    
    if (_cellIndex >= 0) {
        _nextCell = m_openList[_cellIndex];
        m_visitedList.push_back(_nextCell);
        m_openList.erase(m_openList.begin() + _cellIndex);
    }
    
    return _nextCell;
    
}

void PathFinding::pathOpened(int x_, int y_, float newCost_, SearchCell *parent_) {
 
    // if can't walk through, return then
//    if (CELL_BLOCKED) {
//        return;
//    }

    int _id = y_ * WORLD_SIZE + x_;
    for (int i=0; i<m_visitedList.size(); i++) {
        if (_id == m_visitedList[i]->m_id) {
            return;
        }
    }
    
    SearchCell *_newChild = new SearchCell(x_, y_, parent_);
    _newChild->G = newCost_;
    _newChild->H = parent_->ManHattatDistance(m_goalCell);
    
    for (int i=0; i<m_openList.size(); i++) {
        if (_id == m_openList[i]->m_id) {
            float _newF = _newChild->G + newCost_ + m_openList[i]->H;
            
            if (m_openList[i]->GetF() > _newF) {
                m_openList[i]->G = _newChild->G + newCost_;
                m_openList[i]->parent = _newChild;
            }else {
                delete _newChild;
                _newChild = NULL;
                return;
            }
        }
    }
    
    m_openList.push_back(_newChild);
    
}

void PathFinding::continuePath() {
    
    if (m_openList.empty()) {
        return;
    }
    
    SearchCell *_currentCell = getNextCell();
    if (_currentCell->m_id == m_goalCell->m_id) {
        
        m_goalCell->parent = _currentCell->parent;
        SearchCell *_getPath;
        for (_getPath = m_goalCell; _getPath != NULL; _getPath = _getPath->parent) {
            m_pathToGoal.push_back(ccp(_getPath->m_xcoord, _getPath->m_ycoord));
        }
        
        m_foundGoal = true;
        return;
        
    }else {
    
        //rightCell
        pathOpened(_currentCell->m_xcoord+m_space.width,
                   _currentCell->m_ycoord,
                   _currentCell->G + m_max,
                   _currentCell);
        //leftCell
        pathOpened(_currentCell->m_xcoord-m_space.width,
                   _currentCell->m_ycoord,
                   _currentCell->G + m_max,
                   _currentCell);
        //upCell
        pathOpened(_currentCell->m_xcoord,
                   _currentCell->m_ycoord+m_space.height,
                   _currentCell->G + m_max,
                   _currentCell);
        //downCell
        pathOpened(_currentCell->m_xcoord,
                   _currentCell->m_ycoord-m_space.height,
                   _currentCell->G + m_max,
                   _currentCell);
        //left-up diagonal cell
        pathOpened(_currentCell->m_xcoord-m_space.width,
                   _currentCell->m_ycoord+m_space.height,
                   _currentCell->G + m_min,
                   _currentCell);
        //right-up diagonal
        pathOpened(_currentCell->m_xcoord+m_space.width,
                   _currentCell->m_ycoord+m_space.height,
                   _currentCell->G + m_min,
                   _currentCell);
        //left-down diagonal cell
        pathOpened(_currentCell->m_xcoord-m_space.width,
                   _currentCell->m_ycoord-m_space.height,
                   _currentCell->G + m_min,
                   _currentCell);
        //right-down diagonal
        pathOpened(_currentCell->m_xcoord+m_space.width,
                   _currentCell->m_ycoord-m_space.height,
                   _currentCell->G + m_min,
                   _currentCell);
        
        for (int i=0; i<m_openList.size(); i++) {
            if (_currentCell->m_id == m_openList[i]->m_id) {
                m_openList.erase(m_openList.begin() + i);
            }
        }
    }
    
}

CCPoint PathFinding::nextPathPos(CCPoint pos_) {

    CCPoint _nextPos = m_pathToGoal[m_pathToGoal.size()-1];
    m_pathToGoal.pop_back();
    return _nextPos;
    
}



