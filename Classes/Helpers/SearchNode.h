//
//  SearchNode.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/18/2556 BE.
//
//

#ifndef Cheetah_SearchNode_h
#define Cheetah_SearchNode_h

struct SearchNode {
    
    float x;
    float y;
    
    int F;
    int G;
    int H;
    
    SearchNode *parent;
    
    SearchNode() {
        x = 0.0f;
        y = 0.0f;
        F = 0;
        G = 0;
        H = 0;
        parent = NULL;
    }
    
//    SearchNode(float x_, float y_, float f_, float g_, float h_, SearchNode *parent_, int ncol_) {
//        x = x_;
//        y = y_;
//        F = f_;
//        G = g_;
//        H = h_;
//        parent = parent_;
//    }
    
};

#endif
