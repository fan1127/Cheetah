//
//  TutorialDialog.cpp
//  Cheetah
//
//  Created by Teerapat on 4/3/57 BE.
//
//

#include "TutorialDialog.h"
#include "../../Helpers/CCStringHelper.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Helpers/CreateSpriteHelper.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../UIs/NumericProgressBar.h"
USING_NS_CC;

using namespace std;


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
#include "../../Settings.h"
#include "../../Extensions/CCNikButton.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/StringHelper.h"
#include "../../DialogSettings.h"

extern TargetDevice targetDevice;
//extern CCPoint screenFactor;
extern float sizeMultiplier;
extern float retinaMultiplier;

TutorialDialog* TutorialDialog::create(CCModalController* parentModalController_) {
    TutorialDialog *pRet = new TutorialDialog();
    pRet->init(parentModalController_, cocos2d::CCSize(1474.0f, 1000.0f));
    pRet->autorelease();
    return pRet;
}

bool TutorialDialog::init(CCModalController* parentModalController_, CCSize size)
{
    if (!ModalDialog::init(parentModalController_, size))
    {
        return false;
    }

    m_modalController->userObject = this;

    createContent(TUTORIAL_DIALOG_SETTINGS);

    addTitle(g_gameContent->Word["complete_the_track"].c_str());

    updateContent();
    
    return true;
}

void TutorialDialog::onExit()
{
    CCModalView::onExit();
}

void TutorialDialog::updateContent()
{
    
    cocos2d::extension::CCSkeletonAnimation *tutorial_spine = g_sceneManager->sharedSkeletonData->getSkeleton("common/character_tutorial/", "connect_track_tutor");
    tutorial_spine->setAnchorPoint(ccp(0.5f, 0.5f));
    tutorial_spine->setAnimation("play", true);
    
    tutorial_spine->setScale(1.0*retinaMultiplier);
    tutorial_spine->setPosition(ccp(0,20 * sizeMultiplier));
    tutorial_spine->setScale(1.2f*sizeMultiplier);
    addChild(tutorial_spine);
}
