//
//  SearchCell.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/18/2556 BE.
//
//

#ifndef Cheetah_SearchCell_h
#define Cheetah_SearchCell_h

#define WORLD_SIZE  120

struct SearchCell {
    
    int m_xcoord, m_ycoord;
    int m_id;
    SearchCell *parent;
    float G;
    float H;
    
    SearchCell() {
        m_xcoord = 0;
        m_ycoord = 0;
        m_id = 0;
        parent = NULL;
        G = 0;
        H = 0;
        
    };
    
    SearchCell(int x, int y, SearchCell *parent_) {
        m_xcoord = x;
        m_ycoord = y;
        m_id = m_ycoord * WORLD_SIZE + m_xcoord;
        parent = parent_;
        G = 0;
        H = 0;
        
    }
    
    float GetF() {
        return G + H;
    }
    
    float ManHattatDistance(SearchCell *goal_node_) {
        
        float x = (float)(fabs((float)this->m_xcoord - (float)goal_node_->m_xcoord));
        float y = (float)(fabs((float)this->m_ycoord - (float)goal_node_->m_ycoord));
        
        return x + y;
        
    }
    
};

#endif
