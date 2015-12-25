//
//  NotEnoughSpaceDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 3/3/2557 BE.
//
//

#include "NotEnoughSpaceDialog.h"
USING_NS_CC;
using namespace std;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/PlayerState.h"
extern PlayerState *g_playerState;

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

extern float sizeMultiplier;

#include "../../Settings.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "MiniDialog.h"
#include "InstantDialog.h"
#include "../../AppMacros.h"
extern TargetDevice targetDevice;
#include "../ShopItemBuilding.h"
#include "../../Helpers/CCStringHelper.h"
#include "../../Helpers/StringHelper.h"
#include "DialogSettings.h"

NotEnoughSpaceDialog* NotEnoughSpaceDialog::create(CCModalController *parentModalController_, void *entity_) {
    
    NotEnoughSpaceDialog *pRet = new NotEnoughSpaceDialog();
    pRet->init(parentModalController_, entity_, kDialogNotEnoughSpaceSize);
    pRet->autorelease();
    return pRet;
}

bool NotEnoughSpaceDialog::init(CCModalController *parentModalController_, void *entity_, CCSize size)
{
    if (!SimpleDialog::init(parentModalController_, size))
    {
        return false;
    }
    
    m_entity = entity_;
    
    return compose();
}

bool NotEnoughSpaceDialog::compose() {

    CREATE_CHAR_BUFFER(_buffer, 64);
    
    createTitle(g_gameContent->Word["not_enough_space_title"].c_str());

    //detail
    //set message
    CCLabelBMFont *_detail = CCLabelHelper::createBMFont(g_gameContent->Word["not_enough_space_text"].c_str(), StyleDialogTextDark);
    _detail->setPositionY(m_title->getPositionY()-(m_title->getContentSize().height + 20.f * sizeMultiplier));
    addChild(_detail);
    
    int menu_type = 0; //display both habitat & barn
    
    Building *_building = (Building*)m_entity;
    if (_building) {
        if (typeid(*_building) == typeid(Building)) {
            if (_building->bVo.building_type == BUILDING_TYPE_HABITAT) {
                menu_type = 2; //display barn only
            }else if (_building->bVo.building_type == BUILDING_TYPE_ANIMALDORM) {
                menu_type = 1; //display habitat only
            }
        }
    }
    
    if (Pet *_pet = (Pet*)m_entity) {
        if (typeid(*_pet) == typeid(Pet)) {
            menu_type = 1;
        }
    }
    
    CCSprite *_habitat = NULL;
    if (menu_type == 0 || menu_type == 1) {
        
        CCSprite *_habitat = CCSprite::createWithSpriteFrameName("oldversion/_image_buy_additional_pen.png");
        _habitat->setPosition(ccp(0.0f, getContentSize().height*0.05f));
        addChild(_habitat);
        
         GDBuildingVO _bvo = g_gameContent->getBuildingVO(kBuildingHabitat, 1);
        if (BUILD_HABITAT_WITH_GEMS) {
            _bvo = g_gameContent->getBuildingVO(kBuildingHabitat, PlayerState::getInstance()->getNBuilding(kBuildingHabitat)+1);
        }
           
        if (_bvo.type != "") {
            snprintf(_buffer, _buffer_size, "%d", BUILD_HABITAT_WITH_GEMS?int(_bvo.param_2):_bvo.cost_coins);
            snprintf(_buffer, _buffer_size, "%s", CCStringHelper::createWithNumberFormat(_buffer).c_str());
            addButton(0, g_gameContent->Word["build_additional_pen"].c_str(),
                      _buffer,
                      UIHelper::getSpritePath(BUILD_HABITAT_WITH_GEMS?UIHelper::NSClaimGemBlue:UIHelper::NSBuyCoinIcon),
                      _habitat->getPositionX());
        }
        
    }
    
    if (menu_type == 0 || menu_type == 2) {
        
        if (Building* animalBarn = g_playerState->getBuilding(kBuildingAnimal, 0)) {
            GDBuildingVO _nxt_bvo = g_gameContent->getBuildingVO(animalBarn->pbVo.type, animalBarn->bVo.level+1);
            if (_nxt_bvo.type == "") {
//                if (_habitat) _habitat->setPositionX(0.0f);
//                if (m_menu[0]) m_menu[0]->setPositionX(0.0f);
                CCLog_("No upgrade!");
            }else {
                CCSprite *_animaldorm = CCSprite::createWithSpriteFrameName("oldversion/_image_add_slot.png");
                _animaldorm->setPosition(ccp(0.0f, getContentSize().height*0.05f));
                addChild(_animaldorm);
                snprintf(_buffer, _buffer_size, "%d  ", _nxt_bvo.cost_coins);
                addButton(1, g_gameContent->Word["upgrade_animal_barn"].c_str(),
                          CCStringHelper::createWithNumberFormat(_buffer).c_str(),
                          UIHelper::getSpritePath(UIHelper::NSBuyCoinIcon),
                          _animaldorm->getPositionX());
                if (_habitat) {
                    _habitat->setPositionX(-getContentSize().width*0.20f);
                    if (m_menu[0]) {
                        m_menu[0]->setPositionX(_habitat->getPositionX());
                    }
                    _animaldorm->setPositionX(getContentSize().width*0.20f);
                    if (m_menu[1]) {
                        m_menu[1]->setPositionX(_animaldorm->getPositionX());
                    }
                }
            }
        }else {
            GDBuildingVO _bvo = g_gameContent->getBuildingVO(kBuildingAnimal, 1);
            if (_bvo.type == "") {
//                if (_habitat) _habitat->setPositionX(0.0f);
//                if (m_menu[0]) m_menu[0]->setPositionX(0.0f);
            }else {
                CCSprite *_animaldorm = CCSprite::createWithSpriteFrameName("oldversion/_image_add_slot.png");
                _animaldorm->setPosition(ccp(0.0f, getContentSize().height*0.05f));
                addChild(_animaldorm);
                snprintf(_buffer, _buffer_size, "%d  ", _bvo.cost_coins);
                addButton(2, g_gameContent->Word["build_animal_barn"].c_str(),
                          CCStringHelper::createWithNumberFormat(_buffer).c_str(),
                          UIHelper::getSpritePath(UIHelper::NSBuyCoinIcon),
                          _animaldorm->getPositionX());
                if (_habitat) {
                    _habitat->setPositionX(-getContentSize().width*0.20f);
                    if (m_menu[0]) {
                        m_menu[0]->setPositionX(_habitat->getPositionX());
                    }
                    _animaldorm->setPositionX(getContentSize().width*0.20f);
                    if (m_menu[2]) {
                        m_menu[2]->setPositionX(_animaldorm->getPositionX());
                    }
                }
            }
        }
    }

    return true;
}

void NotEnoughSpaceDialog::addButton(int index_, const char* label_, const char *value_, const char *icon_filename_, float x_pos_)
{
    CCNikButton *_button_sprite = CCNikButton::create(UIHelper::getSprite(UIHelper::UpgradeAndTradeItemsForAnimalBuyButton), NULL);
    _button_sprite->setTarget(this, menu_selector(NotEnoughSpaceDialog::onButton), CCNikButtonEventTouchUpInside);
    CCSprite *_coinSprite = CCSprite::createWithSpriteFrameName(icon_filename_);
    _coinSprite->setPosition(ccp(_button_sprite->getContentSize().width*1.09f , _button_sprite->getContentSize().height*0.5f));
    _coinSprite->setAnchorPoint(ccp(1, 0.5f));
    _button_sprite->addChild(_coinSprite);
    CCLabelBMFont *_label = CCLabelHelper::createBMFont(value_, StyleButtonCoinAndGems);
    _label->setAnchorPoint(ccp(1, 0.5f));
    _label->setPosition(_button_sprite->getContentSize()*0.5f - ccp(_button_sprite->getContentSize().width-(_coinSprite->getPositionX()), 0.0f));
    _button_sprite->addChild(_label);
    _button_sprite->setPosition(ccp(0.0f, -250.f * sizeMultiplier));

    UIHelper::setTwoNodesToMiddlePosition(_coinSprite, _label, 10*sizeMultiplier, ccp(_button_sprite->getContentSize().width*0.5f, _button_sprite->getContentSize().height*0.5f));

    _button_sprite->setTag(index_);
    
    addChild(_button_sprite );

    CCLabelBMFont *_labelAboveButton = CCLabelHelper::createBMFont(label_, StyleDialogTextDark);
    _labelAboveButton->setPosition(_button_sprite->getPosition() + ccp(0, _button_sprite->getContentSize().height*0.6f + _labelAboveButton->getContentSize().height*0.5f));
    addChild(_labelAboveButton );
}

void NotEnoughSpaceDialog::onButton(CCObject *sender_)
{
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    CCMenuItemSprite *menuItem = (CCMenuItemSprite*)sender_;
    SceneVille *_scene = (SceneVille*)g_currentScene;
    IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
    
    switch (menuItem->getTag()) {
        case 0: {
            GDBuildingVO _bvo = g_gameContent->getBuildingVO(kBuildingHabitat, PlayerState::getInstance()->getNBuilding(kBuildingHabitat)+1);
            if (_bvo.type == "") {
                return;
            }
            _isoMapBuilding->addBuildingWithPlaceHolder(_bvo);
            this->m_modalController->closeAll();
            
            setToAvailablePositionAndZoom();

        }break;
        case 1: {
            CCLog_("Button 1");
        }break;
        case 2: { // Build Animal Barn
            GDBuildingVO _bvo = g_gameContent->getBuildingVO(kBuildingAnimal, 1);
            if (_bvo.type == "") {
                return;
            }
            
            string _msg = ShopItemBuilding::isAnyReasonCannotBuild(_bvo);
            if (_msg != "") {
                g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, _msg);
                return;
            }
            
            if (g_playerState->player.balance.gold >= _bvo.cost_coins) {
                _isoMapBuilding->addBuildingWithPlaceHolder(_bvo);
                this->m_modalController->closeAll();
            }else {
                CCLog_("Not enough coin!");
                if (_scene->modalController->getNStackView() == 2) {
                    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
                    InstantDialog *_dialog = InstantDialog::createWithBuildingVO(_scene->modalController, _bvo);
                    _dialog->setPosition(_screenSize*0.5f);
                    _scene->modalController->pushView(_dialog);
                }
            }
        }break;
        default:
            break;
    }
}

void NotEnoughSpaceDialog::setToAvailablePositionAndZoom() {
    
    GDBuildingVO _bvo = g_gameContent->getBuildingVO(kBuildingHabitat, PlayerState::getInstance()->getNBuilding(kBuildingHabitat)+1);
    if (_bvo.type == "") {
        return;
    }
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
    
    CCPoint autoPos = _isoMapBuilding->getPositionOfAvailableSpaceFor(_bvo);
    if (autoPos.x < 0.0f || autoPos.y < 0.0f) return;
    _isoMapBuilding->placeHolder->setPosition(autoPos);
    if (_scene->scrollView) {
        _scene->scrollView->zoomOnPosition(_isoMapBuilding->placeHolder->getPosition()
                                           -_scene->isoMap->getContentSize()/4.f
                                           +ccp(0, _bvo.src_height*sizeMultiplier*0.4f)
                                           ,1.0f
                                           ,1.0f);
    }
    
}
