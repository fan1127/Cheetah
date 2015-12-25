//
//  MiniInfoDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/20/2557 BE.
//
//

#ifndef __Cheetah__MiniInfoDialog__
#define __Cheetah__MiniInfoDialog__

#include "MiniDialog.h"

class MiniInfoDialog : public MiniDialog {
    
public:
    
    static MiniInfoDialog *createWithEntity(void *entity_);
    virtual bool compose();
        
};

#endif /* defined(__Cheetah__MiniInfoDialog__) */
