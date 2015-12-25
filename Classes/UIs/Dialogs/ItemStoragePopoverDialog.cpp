//
//  ItemStoragePopoverDialog.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/4/2557 BE.
//
//

#include "ItemStoragePopoverDialog.h"

#include "ItemStorageItem.h"

#include <spine/spine-cocos2dx.h>

#include "../../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

#include "../../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../../AppMacros.h"
extern float sizeMultiplier;

#include "../../Extensions/CCLabelHelper.h"
#include "../../Helpers/CCNodeHelper.h"

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../Extensions/CCNikButton.h"

USING_NS_CC;
using namespace std;
using namespace cocos2d::extension;

//extern CCPoint screenFactor;

#include "SellItemDialog.h"

#include "../../Settings.h"

ItemStoragePopoverDialog *ItemStoragePopoverDialog::create(IntKeyValue *item_, GDItemVO itvo_, void* root_) {
    
    ItemStoragePopoverDialog *pRet = new ItemStoragePopoverDialog();
    pRet->init(item_, itvo_, root_);
    pRet->autorelease();
    return pRet;
    
}

bool ItemStoragePopoverDialog::init(IntKeyValue *item_, GDItemVO itvo_, void* root_) {
    
    m_root = root_;
    item = item_;
    itvo = itvo_;
    ItemStorageDialog* _dialog = (ItemStorageDialog*)root_;
    
    CCSize _popoverSize = CCSizeZero;
    
    // 1. create all elements and sum the content size
    CCLabelBMFont *_titleLabel = CCLabelHelper::createBMFont(itvo.title.c_str(), kFontSizeMedium, true);
    CCSize _contentSize = CCNodeHelper::getContentSize(_titleLabel);
    _popoverSize.height += _contentSize.height + kPopOverDialogSpacing*sizeMultiplier;
    if (_popoverSize.width < _contentSize.width) {
        _popoverSize.width = _contentSize.width;
    }
    
    CCLabelBMFont *_descriptionLabel = CCLabelHelper::createBMFont(itvo_.description2.c_str(), kFontSizeMedium-5.0f, false, kTextColorDark, kPopOverDialogNarmalWidth*sizeMultiplier, kCCTextAlignmentCenter);
    _contentSize = CCNodeHelper::getContentSize(_descriptionLabel);
    _popoverSize.height += _contentSize.height + kPopOverDialogSpacing*sizeMultiplier;
    if (_popoverSize.width < _contentSize.width) {
        _popoverSize.width = _contentSize.width;
    }
    
    CCNikButton *_sellButton = NULL;
    if (_dialog->getPlayerState() == g_playerState) {
        
        CCSprite *sellButtonSprite = UIHelper::getSprite(UIHelper::UISprites(UIHelper::NotificationNo));
        sellButtonSprite->setScale(0.75f);
        _sellButton = CCNikButton::create(sellButtonSprite, NULL);
        _sellButton->setTarget(this, menu_selector(ItemStoragePopoverDialog::sell), CCNikButtonEventTouchUpInside);
        CCLabelBMFont *_sellLabel = CCLabelHelper::createBMFont("Sell", kFontSizeMedium-5.0f, true);
        _sellLabel->setPosition(_sellButton->getContentSize()*0.5f + ccp(0.0f, 5.0f*sizeMultiplier));
        _sellButton->addChild(_sellLabel);
        _sellButton->setTouchPriority(-2000);
        
        _contentSize = _sellButton->getContentSize();
        _popoverSize.height += _contentSize.height + kPopOverDialogSpacing*sizeMultiplier;
        if (_popoverSize.width < _contentSize.width) {
            _popoverSize.width = _contentSize.width;
        }
        
    }
    
    _popoverSize.height -= kPopOverDialogSpacing*sizeMultiplier;
    _popoverSize = _popoverSize + kPopOverDialogPadding*sizeMultiplier;

    // 2. create a popup dialog with contentSize calculated from all elements
    if (!CCPopOverDialog::init(_popoverSize, CCPopoverArrowDirectionAny)) {
        return false;
    }
    
    // 3. set position of all elements regard contentSize
    _titleLabel->setPosition(ccp(getContentSize().width*0.5f,
                                 getContentSize().height*1.0f - kPopOverDialogPadding.height*sizeMultiplier*0.5f - CCNodeHelper::getContentSize(_titleLabel).height*0.5f));
    addChild(_titleLabel);
    
    _descriptionLabel->setPosition(_titleLabel->getPosition() + ccp(0.0f,
                                                                    - CCNodeHelper::getContentSize(_titleLabel).height*0.5f - kPopOverDialogSpacing*sizeMultiplier - CCNodeHelper::getContentSize(_descriptionLabel).height*0.5f));
    addChild(_descriptionLabel);
    
    if (_sellButton) {
        _sellButton->setPosition(_descriptionLabel->getPosition() + ccp(0.0f,
                                                                        - CCNodeHelper::getContentSize(_descriptionLabel).height*0.5f - kPopOverDialogSpacing*sizeMultiplier - _sellButton->getContentSize().height*0.5f));
        addChild(_sellButton);
    }
    
    
    return true;
    
}

void ItemStoragePopoverDialog::sell(cocos2d::CCObject *sender) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (_scene->modalController->getNStackView() == 1) {
        SellItemDialog *_dialog = SellItemDialog::create(_scene->modalController, itvo, m_root);
        _dialog->setPosition(_screenSize*0.5f);
        _scene->modalController->pushView(_dialog);
        dismissPopover(true);
    }

}
