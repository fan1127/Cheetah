//
//  AnimalBarnDialog.cpp
//  Cheetah
//
//  Created by Kaweerut on 2/27/57 BE.
//
//

#include "AnimalBarnDialog.h"
USING_NS_CC;
USING_NS_CC_EXT;

using namespace std;

#include "../../Settings.h"
#include "../../Extensions/Modal/CCModalController.h"

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/BackendService.h"
extern BackendService *g_gameBackend;

#include "../../AppMacros.h"
extern TargetDevice targetDevice;
//extern CCPoint screenFactor;
extern float sizeMultiplier;
extern float retinaMultiplier;
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/CCNodeHelper.h"
#include "MiniDialog.h"

#include "../../Extensions/CCClippingLayer.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../../Extensions/CCNikScrollViewButton.h"
#include "../../Extensions/CCNikButton.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/UIDisplayHelper.h"

//#include "../../Entities/TutorialManager.h"
//extern TutorialManager *g_gameTutorial;

#include "../../Services/GameAudioService.h"
#include "../../Helpers/StringHelper.h"
#include "../../DialogSettings.h"

extern GameAudioService* g_gameAudio;

//#include "../../UIs/Dialogs/AnimalDialog.h"

AnimalBarnDialog* AnimalBarnDialog::create(CCModalController* parentModalController_, PlayerState* playerState_) {
    
    if (AnimalBarnDialog *pRet = new AnimalBarnDialog()) {
        pRet->m_modalController = parentModalController_;
        pRet->m_playerState = playerState_;
        if (pRet->init(parentModalController_, kDialogAnimalBarnSize)) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
}

bool AnimalBarnDialog::init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize)
{
    if (!ScrollDialog::init(parentModalController_, dialogSize))
    {
        return false;
    }
    
    Building *animalDorm = m_playerState->getBuilding(kBuildingAnimal, 0);
    if (!animalDorm) {
        return false;
    }
    
    m_selectButtonEnabled = true;

#ifdef kDialogFooterTextEnabled
    if (m_playerState == g_playerState) {
        CREATE_CHAR_BUFFER(_buffer, 128);
        CREATE_CHAR_BUFFER(_buffer2, 128);
        
        snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["animal_barn_footer_msg"].c_str());
        snprintf(_buffer2, _buffer2_size, _buffer, animalDorm->bVo.title.c_str());
        snprintf(_buffer, _buffer_size, "%s", _buffer2);
        m_label_footer = CCLabelHelper::createBMFont(_buffer, StyleDialogTextDark);
        m_label_footer->setAnchorPoint(ccp(0.0f, 0.5f));
        m_label_footer->setPosition(ccp(-getContentSize().width*0.5f+(kDialogPadding*sizeMultiplier),
                                        -getContentSize().height*0.42f));
        this->addChild(m_label_footer);
    }
#endif

    addHelpButton("note_animal_barn_dialog", menu_selector(AnimalBarnDialog::onNote));

    m_label_footer_right = CCLabelHelper::createBMFont("", StyleDialogTextDark);
    m_label_footer_right->setAnchorPoint(ccp(1.0f, 0.5f));
    m_label_footer_right->setPosition(ccp(m_help->getPositionX()-(50*sizeMultiplier), m_help->getPositionY() + 50*sizeMultiplier));
    this->addChild(m_label_footer_right);

    addTitle("");

    createScrollContent(ANIMALBARN_DIALOG_SETTINGS);

    this->updateContent();

    m_openAnimalDialogDelay = 0.0f;

    scheduleUpdate();

    return true;
}

void AnimalBarnDialog::updateContent()
{
    int count = 0;
    int i = 1;
    int j = 0;
    float marginWidth = m_scrollView->getContentSize().width*0.05f;
    float marginHeight = m_scrollView->getContentSize().height*0.1f;
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    CREATE_CHAR_BUFFER(_buffer2, 64);

    _lastItem = NULL;
    
    Building *animalDorm = m_playerState->getBuilding(kBuildingAnimal, 0);
    if (!animalDorm) {
        return;
    }
    
    int nslot = animalDorm->bVo.param_1;
    
    for (vector<Pet*>::iterator it=m_playerState->Pets.begin(); it!=m_playerState->Pets.end(); ++it) {
        if (Pet *_pet = (Pet*)(*it)) {
            if (strcmp(_pet->ppVo.located_in.c_str(), "") == 0) {
                if (count < nslot) {
                    AnimalBarnItem *_item = AnimalBarnItem::create(_pet);
                    _item->setTarget(this, menu_selector(AnimalBarnDialog::selectAnimalItem), CCNikScrollViewButtonStateTouchUp);
                    _item->setDelegate(m_scrollView);
                    _item->setPosition(ccp(marginWidth + (_item->getContentSize().width + marginWidth)*j,
                                           ((m_scrollView->getContentSize().height -
                                             (_item->getContentSize().height*2 + marginHeight))*0.5f
                                            + (_item->getContentSize().height + marginHeight)*i)
                                           -(0.0f*sizeMultiplier)));
                    
                    m_scrollView->addChild(_item);
                    _lastItem = _item;
                    
                    count++;
                    i--;
                    if (i < 0) {
                        j++;
                        i = 1 ;
                    }
                }
            }
        }
    }
    if(count==0){
        CCLabelBMFont *nonAnimal = CCLabelHelper::createBMFont(retinaMultiplier, g_gameContent->Word["non_animal"].c_str(), kFontSizeLarge, true);
        nonAnimal->setAnchorPoint(ccp(0.5f, 0.5f));
        nonAnimal->setPosition(m_frontBackground->getContentSize()*0.5f);
        
        m_scrollView->addChild(nonAnimal);
    }

    setTitle(animalDorm->bVo.title.c_str());

    snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["animal_barn_footer_msg_2"].c_str());
    snprintf(_buffer2, _buffer2_size, _buffer, count, nslot);
    snprintf(_buffer, _buffer_size, "%s", _buffer2);
    m_label_footer_right->setString(_buffer);

    if (!_lastItem)
    {
        return;
    }

    m_scrollView->setContentView(CCSizeMake(MAX(_lastItem->getPositionX() + _lastItem->getContentSize().width, m_scrollView->getContentSize().width), m_scrollView->getContentSize().height));
//    m_scrollView->zoomOnPosition((CCSizeMake(MAX(_lastItem->getPositionX() + _lastItem->getContentSize().width, m_scrollView->getContentSize().width), m_scrollView->getContentSize().height)), 1.0f, 1.0f);
}

void AnimalBarnDialog::update(float delta_)
{
    if (m_openAnimalDialogDelay > 0.0f)
        m_openAnimalDialogDelay -= delta_;
}

void AnimalBarnDialog::selectAnimalItem(AnimalBarnItem *item) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    if (!m_selectButtonEnabled) return;
    
    if (m_openAnimalDialogDelay > 0.0f) return;
    m_openAnimalDialogDelay = 0.2f;
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();

    if (m_playerState == g_playerState) {
        _animalDetailDialog = AnimalDialog::create(this->m_modalController, item->pet, AnimalDialogType_2);
    }else {
        _animalDetailDialog = AnimalDialog::create(this->m_modalController, item->pet, AnimalDialogType_2, IMV_VIEWONLY);
    }
    
    //play effect habitat
    if(item->pet->pVo.sound_src.compare("") == 0) {
        g_gameAudio->playEffect(AUDIO_PIG_GRUNT);
    }
    else {
        CREATE_CHAR_BUFFER(_buffer, 64);
        snprintf(_buffer, _buffer_size, "sfx/animalx/%s%s", item->pet->pVo.sound_src.c_str(), AUDIO_EXT);
        g_gameAudio->playEffect(_buffer);
    }

    _animalDetailDialog->setPosition(_screenSize*0.5f);
    this->m_modalController->pushView(_animalDetailDialog);
}

void AnimalBarnDialog::onClose(CCObject *sender_)
{
    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    if (!m_closeButtonEnabled) return;
    
    if (this->numberOfRunningActions() > 0) return;
    
    if (getTag() == 1) { //close all
        this->m_modalController->closeAll();
    }
    else
    {
        this->m_modalController->popView();
    }
}

void AnimalBarnDialog::onNote(cocos2d::CCObject *obj_) {
    
    CREATE_CHAR_BUFFER(_buffer, 512);
    snprintf(_buffer, _buffer_size, g_gameContent->Word["note_animal_barn_dialog"].c_str(),
            g_gameContent->getBuildingVO(kBuildingAnimal, 1).title.c_str(),
            g_gameContent->getBuildingVO(kBuildingHabitat, 1).title.c_str());
    CCLabelBMFont* _note_label = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium-5.0f, false, kTextColorDark, getContentSize().width*0.4f, kCCTextAlignmentCenter);
    
    CCSize _contentSize = CCNodeHelper::getContentSize(_note_label);
    CCSize _padding = kPopOverDialogPadding*sizeMultiplier;
    
    CCPopOverDialog* _hintPopOver = CCPopOverDialog::create(_contentSize + _padding, CCPopoverArrowDirectionUp);
    _note_label->setPosition(_hintPopOver->getContentSize()*0.5f);
    _hintPopOver->addChild(_note_label);
    _hintPopOver->presentPopover(m_help, true);
 
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
}
