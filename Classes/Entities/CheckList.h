//
//  CheckList.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/30/2556 BE.
//
//

#ifndef __Cheetah__CheckList__
#define __Cheetah__CheckList__

#include "cocos2d.h"

enum CheckListList{
    PCL_FIRST_PET_EARNED,
    PCL_TUTORIAL1_LEARNED,
    PCL_TUTORIAL2_LEARNED,
    
};

#define PCL_CHECKLIST_NUM   sizeof(CheckListList)

class CheckList {
    
    bool m_checklist[PCL_CHECKLIST_NUM];
    
public:
    
    CheckList();
    
    bool parseCheckListData(const char *json_);
    void setCheckList(int checklist_no_, bool value_);
    bool getCheckList(int checklist_no_);
    
    void updateCheckListInBackend();
    
};

#endif /* defined(__Cheetah__CheckList__) */
