//
//  CutTreeDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/7/2557 BE.
//
//

#include "CutTreeDialog.h"
USING_NS_CC;
USING_NS_CC_EXT;

#include "../../Settings.h"
#include "../../Extensions/Modal/CCModalController.h"

#include "../../AppMacros.h"
extern TargetDevice targetDevice;
extern float sizeMultiplier;
extern float retinaMultiplier;
#include "../../Services/GameAudioService.h"
extern GameAudioService *g_gameAudio;

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APITreeRemove.h"

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Settings.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/CCStringHelper.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/StringHelper.h"

CutTreeDialog *CutTreeDialog::create(CCModalController *parentModalController, Decoration *tree) {
    
    CutTreeDialog *pRet = new CutTreeDialog();
    pRet->init(parentModalController, tree, kDialogSmallSize);
    pRet->autorelease();
    return pRet;
    
}

bool CutTreeDialog::init(CCModalController *parentModalController, Decoration *tree, CCSize size) {
    
    if (!SimpleDialog::init(parentModalController, size)) {
        return false;
    }
    
    unwanted_tree = tree;

    std::stringstream ss;
    ss << g_gameContent->Word["remove_tree"] << " " << unwanted_tree->dVo.title << "?";

    createTitle(ss.str().c_str());

    CREATE_CHAR_BUFFER(_buffer, 16);
    snprintf(_buffer, _buffer_size, "%s",  CCStringHelper::createWithNumberFormat(unwanted_tree->dVo.cost_coins).c_str() );
    createButton(_buffer, SimpleDialog::Center, SimpleDialog::Coin);

    autoResize(300*sizeMultiplier,
               200*sizeMultiplier);

    return true;
    
}

void CutTreeDialog::onButton(CCObject *sender_) {

    if (g_playerState->player.balance.gold < unwanted_tree->dVo.cost_coins) {
        SceneVille *_scene = (SceneVille*)g_currentScene;
        if (_scene->modalController->getParent()) {
            CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
            if (InstantDialog *_dialog = InstantDialog::create(_scene->modalController, unwanted_tree , NULL)) {
                _dialog->setPosition(_screenSize*0.5f);
                _scene->modalController->pushView(_dialog);
            }
        }
        return;
    }
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    BackendService::getInstance()->send(APITreeRemove::create(unwanted_tree, false, this, CutTreeDialog::onTreeRemoved));
    m_modalController->closeAll();
    
}

void CutTreeDialog::onTreeRemoved(void *sender, std::string resp_str) {

    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*g_currentScene) != typeid(SceneVille)) return;

    PlayerState *_player = PlayerState::getInstance();
    //JsonAllocator allocator;
    Json*root = Json_create(resp_str.c_str());
    _player->parseChestGemsData(root);
    _player->removeDecorationData(root);
    _player->parseUserBalance(root);
    Json_dispose(root);

    _scene->hudPanel->updateHUD();
    _scene->hudPanel->updateAllResourceBars();
    _scene->hudPanel->updateBadgeAchievement();
    
    IsoMapBuilding *_isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
    if (!_isoMapBuilding || typeid(*_isoMapBuilding) != typeid(IsoMapBuilding)) return;
    _isoMapBuilding->plotChestGems(false);
    
}
