//
//  CheckList.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/30/2556 BE.
//
//

#include "CheckList.h"
USING_NS_CC;
using namespace std;

extern CCScene* g_sceneManager;

#include <spine/Json.h>
using namespace cocos2d::extension;

#include "../Services/BackendService.h"
extern BackendService* g_gameBackend;

#include "../Extensions/CCAlert/CCAlert.h"

#include "../Settings.h"

extern unsigned int currentTime;

CheckList::CheckList() {
    
    for (int i=0; i<PCL_CHECKLIST_NUM; i++) {
        m_checklist[i] = false;
    }
    
}

bool CheckList::parseCheckListData(const char *json_) {
    
    if (json_ == NULL || strcmp(json_, "")==0) {
        CCLog_("(CheckList::parsePlayerCheckListData) Warning: Json is NULL!");
        return false;
    }

    //JsonAllocator allocator;
    Json *_root = Json_create(json_);
    if (!_root) {
        return false;
    }
    
    Json *_error = Json_getItem(_root, "error");
    if (_error) {
        string _err = Json_getString(_root, "error", NULL);
        CCAlert::Show("Info", _err.c_str(), "OK", NULL, (CCObject*)BackendService::getInstance(), alert_selector(BackendService::kick_back_to_front));
        return false;
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Balance Info
    if (Json *_checklists = Json_getItem(_root, "checklist")) {
    
        int _nchecklist = Json_getSize(_checklists);
        for (int i=0; i<_nchecklist && i < PCL_CHECKLIST_NUM; i++) {
            
            Json *_checklist = Json_getItemAt(_checklists, i);
            m_checklist[i] = (int)_checklist->valueint;
            
        }

    }
    
    Json_dispose(_root);

    return true;
    
}

void CheckList::setCheckList(int checklist_no_, bool value_) {
    
    if (checklist_no_ < 0 || checklist_no_ >= PCL_CHECKLIST_NUM) {
        return;
    }
    
    m_checklist[checklist_no_] = value_;
    
}

bool CheckList::getCheckList(int checklist_no_) {
    
    if (checklist_no_ < 0 || checklist_no_ >= PCL_CHECKLIST_NUM) {
        return false;
    }
    
    return m_checklist[checklist_no_];
    
}

void CheckList::updateCheckListInBackend() {
    
    if (!g_gameBackend) {
        CCLog_("(CheckList::updateCheckListInBackend) Error: gameBackend is uninitialized!");
        return;
    }
    
//    g_gameBackend->checklist_update();
    
}
