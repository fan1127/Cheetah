//
//  APIRaceHistoryList.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIRaceHistoryList__
#define __Cheetah__APIRaceHistoryList__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIRaceHistoryList : public APIRootClass {
    
public:
    
    static APIRaceHistoryList *create(void *sender,
                                      void (*callback)(void*,std::string));
    
    virtual std::string name() { return "race_history_list"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIRaceHistoryList__) */
