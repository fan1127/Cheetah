//
//  SearchItemDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 6/3/2557 BE.
//
//

#include "SearchItemDialog.h"

#include "../../Extensions/CCOpenURL/CCOpenURL.h"

#include "../../Helpers/UIDisplayHelper.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/CreateSpriteHelper.h"
#include "../../UIs/NumericProgressBar.h"

USING_NS_CC;

using namespace std;
#include <spine/Json.h>
using namespace cocos2d::extension;

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/BackendService.h"
extern BackendService *g_gameBackend;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../AppMacros.h"
#include "../../Extensions/CCNikButton.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/StringHelper.h"
#include "../../DialogSettings.h"

extern TargetDevice targetDevice;
extern float sizeMultiplier;
extern float retinaMultiplier;

SearchItemDialog* SearchItemDialog::create(CCModalController* parentModalController_, GDItemVO item_vo_) {
    
    SearchItemDialog *pRet = new SearchItemDialog();
    if (pRet && pRet->init(parentModalController_, item_vo_, kDialogAchievementSize)) {
        pRet->autorelease();
        return pRet;
    }
    
    CC_SAFE_DELETE(pRet);
    return NULL;
    
}

bool SearchItemDialog::init(CCModalController* parentModalController_, GDItemVO item_vo_, CCSize size) {
    
    if (!ScrollDialog::init(parentModalController_, size)) {
        return false;
    }
    
    //    m_modalController->onHitShadow = onHitShadow;
    m_modalController->userObject = this;

    addTitle(g_gameContent->Word["search"].c_str());

    createScrollContent(SEARCHITEM_DIALOG_SETTINGS);

    search(item_vo_.type);
    
    return true;
    
}

void SearchItemDialog::search(std::string item_type_)
{
}

void SearchItemDialog::onSearchRetrieved(void *sender_, std::string resp_json_str_)
{
}
