//
//  SellAnimalDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 3/5/2557 BE.
//
//

#include "SellAnimalDialog.h"
USING_NS_CC;
USING_NS_CC_EXT;

#include "../../Settings.h"
#include "../../Extensions/Modal/CCModalController.h"

#include "../../AppMacros.h"
extern float sizeMultiplier;
extern TargetDevice targetDevice;
#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIPetSell.h"

#include "../../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../Entities/PlayerState.h"
extern PlayerState *g_playerState;
#include "../../Helpers/UIDisplayHelper.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/CCStringHelper.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/StringHelper.h"
#include "../Effects/CollectItemEffect.h"

#include "../../Helpers/AnalyticsHelper.h"
#include "../../Helpers/AnalyticsClientWrapper.h"
#include "../../Helpers/FlurryCpp.h"

SellAnimalDialog *SellAnimalDialog::create(CCModalController *parentModalController,
                                           Pet *pet) {
    
    SellAnimalDialog *p = new SellAnimalDialog();
    if (p->init(parentModalController,
                   pet,
                   cocos2d::CCSize(1000.0f, 700.0f))) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return NULL;
    
}

bool SellAnimalDialog::init(CCModalController *parentModalController,
                            Pet *pet,
                            CCSize size) {
    
    if (!SimpleDialog::init(parentModalController, size)) {
        return false;
    }
    
    this->pet = pet;
    
    return compose();
    
}

bool SellAnimalDialog::compose() {
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    CREATE_CHAR_BUFFER(_buffer2, 64);
    snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["sell_animal_dialog_title"].c_str());
    snprintf(_buffer2, _buffer2_size, _buffer, pet->pVo.title.c_str());
    snprintf(_buffer, _buffer_size, "%s", _buffer2);

    //to upper
    createTitle(_buffer);

    CCSkeletonAnimation* _pet_spine = CCSkeletonAnimation::createWithData(pet->petSpine->skeleton->data);
    _pet_spine->setScale(sizeMultiplier);
    _pet_spine->setAnimation("habitat_hungry", true);
    _pet_spine->setPosition(ccp(-getContentSize().width*0.25f, -getContentSize().height*0.05f));
    addChild(_pet_spine);
    
    CCSprite *_arrow = UIHelper::getSprite(UIHelper::UpgradeAndTradeItemsForAnimalArrow);
    _arrow->setPositionY(getContentSize().height*0.05f);
    addChild(_arrow);

    CCSprite *_gem = UIHelper::getSprite(UIHelper::HUDGem);
    _gem->setScale(0.75f);
    this->addChild(_gem);
    
    m_price = g_gameContent->get_gemcal_for_pet_sell(pet->pVo.sell_value, pet->ppVo.level);
    snprintf(_buffer, _buffer_size, " %d", m_price);
    
    CCLabelBMFont *_price = CCLabelHelper::createBMFont(_buffer, kFontSizeExtraLarge, true);
    this->addChild(_price);
    
    UIHelper::setTwoNodesToMiddlePosition(_gem, _price,
                                          15.0f*sizeMultiplier,
                                          ccp(getContentSize().width*0.25f,
                                              _arrow->getPositionY()));
    
    addButton(0, NULL, g_gameContent->Word["sell"].c_str(), NULL, 0.0f,
              UIHelper::getSpritePath(UIHelper::UpgradeInstantBuyButton));
    
    return true;
}

void SellAnimalDialog::addButton(int index,
                                 const char* label,
                                 const char *value,
                                 const char *icon_framename,
                                 float x_pos,
                                 const char *button_framename) {
    
    //Upgrade Button
    CCSprite *_button_sprite = CCSprite::createWithSpriteFrameName(button_framename);
    CCLabelBMFont *_label = CCLabelHelper::createBMFont(value, kFontSizeLarge, true);
    _label->setPosition(_button_sprite->getContentSize()*0.5f);
    _label->setAnchorPoint(ccp(0.5f,0.45f));
    _button_sprite->setPosition(CCSize(this->getPositionX(), this->getPositionY()+_button_sprite->getContentSize().height/3));
    _button_sprite->addChild(_label);
    
    if (icon_framename) {
        _label->setAnchorPoint(ccp(0.0f, 0.5f));
        CCSprite *_icon = CCSprite::createWithSpriteFrameName(icon_framename);
        _icon->setAnchorPoint(ccp(0.0f, 0.5f));
        _button_sprite->addChild(_icon);
        float _width = _label->getContentSize().width + _icon->getContentSize().width;
        _label->setPosition(ccp((_button_sprite->getContentSize().width-_width)*0.5f, _button_sprite->getContentSize().height*0.5f));
        _icon->setPosition(ccp(_label->getPositionX()+_label->getContentSize().width, _label->getPositionY()));
    }
    
    CCMenuItemSprite *_menuItem = CCMenuItemScaleSprite::create(_button_sprite, this, menu_selector(SellAnimalDialog::onButton));
    _menuItem->setTag(index);
    m_menu[index] = CCMenu::create(_menuItem, NULL);
    m_menu[index]->setPosition(ccp(x_pos, -240.f * sizeMultiplier));
    addChild(m_menu[index]);
    
    if (label) {
        CCLabelBMFont *title_label = CCLabelHelper::createBMFont(label, kFontSizeSmall, true);
        title_label->setPosition(m_menu[index]->getPosition()+ccp(0.0f, _button_sprite->getContentSize().height*sizeMultiplier*1.35f));
        addChild(title_label);
    }
    
}

void SellAnimalDialog::onClose(CCObject *sender) {
    
    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    
    if (this->numberOfRunningActions() > 0) return;
    
    if (getTag() == 1) {
        m_modalController->closeAll();
    }else {
        m_modalController->popView();
    }
    
}

void SellAnimalDialog::onButton(CCObject *sender) {
    
    CCLog_("Sell for %d!", m_price);
    if (g_playerState->getNPet() <= 1) {
        g_sceneManager->notification->addMessage(MSGN_TYPE_WARNING,
                                                 "Selling denied, you must have at leat one pet!");
        return;
    }
    
    CCDictionary *params = CCDictionary::create();
    params->setObject(CCString::createWithFormat("%d",m_price), "gem");
    FlurryCpp::logEvent(EVENT_GEM_EARN_SELL_PET, params);
    
    CSJson::Value eventParameters;
    eventParameters["gem"] = m_price;
    AnalyticsClient::getInstance()->logEvent(EVENT_GEM_EARN_SELL_PET, eventParameters);
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    g_sceneManager->showLoading();
    BackendService::getInstance()->send(APIPetSell::create(this->pet, this, SellAnimalDialog::onPetSold));
    
    
}

void SellAnimalDialog::onPetSold(void *sender, std::string resp_json) {
    
    g_gameAudio->playEffect(AUDIO_COLLECT);
    
    PlayerState* _playerState = PlayerState::getInstance();
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_json.c_str());
    _playerState->parseUserBalance(_root);
    _playerState->removePetData(_root);
    Json_dispose(_root);
    
    g_sceneManager->hideLoading();
    
    SellAnimalDialog* _this = (SellAnimalDialog*)sender;
    if (!IS_INSTANCE_OK(_this, SellAnimalDialog))
        return;
    _this->m_modalController->closeAll();

    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!IS_INSTANCE_OK(_scene, SceneVille))
        return;
    HUDPanel *hudPanel = _scene->hudPanel;
    CCSprite *gemIconSprite = hudPanel->resourceBars[HUDPanelBarGems]->getIconSprite();
    CCPoint endPoint = gemIconSprite->convertToWorldSpaceAR(CCPointZero);
    CollectItemEffect::addCollectItemEffect(CollectItemEffect::create(UIHelper::getSpritePath(UIHelper::HUDGem),
                                                                      kCollectItemEffectDefaultSprite,
                                                                      _scene->getContentSize()*0.5f,
                                                                      endPoint,
                                                                      _this,
                                                                      SellAnimalDialog::onGemsEffectFinished), _scene);
    
}

void SellAnimalDialog::onGemsEffectFinished(void *sender) {
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!IS_INSTANCE_OK(_scene, SceneVille))
        return;
    
    _scene->hudPanel->updateAllResourceBars();
    
}
