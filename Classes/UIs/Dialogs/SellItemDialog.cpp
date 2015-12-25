//
//  SellItemDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 3/24/2557 BE.
//
//

#include "SellItemDialog.h"
USING_NS_CC;
USING_NS_CC_EXT;

#include "../../Settings.h"
#include "../../Extensions/Modal/CCModalController.h"

#include "../../AppMacros.h"
extern float sizeMultiplier;
extern float retinaMultiplier;

extern TargetDevice targetDevice;
#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIItemSell.h"

#include "../../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../Entities/PlayerState.h"
extern PlayerState *g_playerState;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../Helpers/CCSpriteHelper.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../Helpers/CCStringHelper.h"
#include "ItemStorageDialog.h"
#include "../../Helpers/StringHelper.h"
#include "../Effects/CollectItemEffect.h"
#include "../../Helpers/AnalyticsHelper.h"
#include "../../Helpers/AnalyticsClientWrapper.h"
#include "../../Helpers/FlurryCpp.h"

SellItemDialog *SellItemDialog::create(CCModalController *parentModalController,
                                       GDItemVO ivo,
                                       void* root) {
    
    SellItemDialog *p = new SellItemDialog();
    p->init(parentModalController,
            ivo,
            root,
            cocos2d::CCSize(1000.0f, 700.0f));
    p->autorelease();
    return p;
    
}

bool SellItemDialog::init(CCModalController *parentModalController,
                          GDItemVO ivo,
                          void* root,
                          CCSize size) {
    
    if (!SimpleDialog::init(parentModalController, size)) {
        return false;
    }
    
    m_entity = root;
    itemVo = ivo;
    
    return compose();
    
}

bool SellItemDialog::compose() {
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    CREATE_CHAR_BUFFER(_buffer2, 64);
    
    m_value_max = g_playerState->getNItem(itemVo.type);
    m_value = 1;//m_value_max;
    m_price = itemVo.coins_value;
    
    onValueChanged(NULL);
    
    snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["sell_animal_dialog_title"].c_str());
    snprintf(_buffer2, _buffer2_size, _buffer, itemVo.title.c_str());
    snprintf(_buffer, _buffer_size, "%s", _buffer2);
    
    createTitle(_buffer);

    CCSprite *_arrow = UIHelper::getSprite(UIHelper::UpgradeAndTradeItemsForAnimalArrow);
    _arrow->setPositionY(getContentSize().height*0.05f);
    addChild(_arrow);
    
    CCSprite *_item_sprite = CCSprite::createWithSpriteFrameName(itemVo.src.c_str());
    _item_sprite->setPosition(ccp(-getContentSize().width*0.25f,
                                  _arrow->getPositionY()+_arrow->getContentSize().height*0.5f));
    addChild(_item_sprite);
    
    //Button Subtract
    CCSprite* _minus = UIHelper::getSprite(UIHelper::MinusButton);
    CCMenuItemSprite *_menuItemSubtract = CCMenuItemScaleSprite::create(_minus, this,
                                                                        menu_selector(SellItemDialog::onValueChanged));
    _menuItemSubtract->setTag(-1);
    CCMenu* _menuSubtract = CCMenu::create(_menuItemSubtract, NULL);
    _menuSubtract->setPosition(_item_sprite->getPosition()
                               + ccp(-100.0f*sizeMultiplier, -125.0f*sizeMultiplier));
    addChild(_menuSubtract);
    
    CCSprite* _plus = UIHelper::getSprite(UIHelper::PlusButton);
    CCMenuItemSprite *_menuItemAddition = CCMenuItemScaleSprite::create(_plus, this,
                                                                        menu_selector(SellItemDialog::onValueChanged));
    _menuItemAddition->setTag(1);
    CCMenu* _menuAddition = CCMenu::create(_menuItemAddition, NULL);
    _menuAddition->setPosition(_item_sprite->getPosition()
                               + ccp(100.0f*sizeMultiplier, -125.0f*sizeMultiplier));
    addChild(_menuAddition);
    
    snprintf(_buffer, _buffer_size, "%d", m_value);
    m_label_value = CCLabelHelper::createBMFont(_buffer, kFontSizeExtraLarge, true);
    m_label_value->setPosition(_item_sprite->getPosition() + ccp(0.0f, -125.0f*sizeMultiplier));
    addChild(m_label_value);
    
    CCSprite *_coin = UIHelper::getSprite(UIHelper::HUDCoin);
    _coin->setScale(0.75f);
    addChild(_coin);
    
    snprintf(_buffer, _buffer_size, "%d", m_price*m_value);
    m_label_price = CCLabelHelper::createBMFont(_buffer, kFontSizeExtraLarge, true);
    addChild(m_label_price);
    
    UIHelper::setTwoNodesToMiddlePosition(_coin,
                                          m_label_price,
                                          15.0f*sizeMultiplier,
                                          ccp(getContentSize().width*0.25f,
                                              _arrow->getPositionY()));
    
    addButton(0, NULL, g_gameContent->Word["sell"].c_str(), NULL, 0.0f,
              UIHelper::getSpritePath(UIHelper::UpgradeInstantBuyButton));
    
    return true;
}

void SellItemDialog::addButton(int index,
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
    _button_sprite->addChild(_label);

    if (icon_framename) {
        _label->setAnchorPoint(ccp(0.0f, 0.5f));
        CCSprite *_icon = CCSprite::createWithSpriteFrameName(icon_framename);
        _icon->setAnchorPoint(ccp(0.0f, 0.5f));
        _button_sprite->addChild(_icon);
        float _width = _label->getContentSize().width + _icon->getContentSize().width;
        _label->setPosition(ccp((_button_sprite->getContentSize().width-_width)*0.5f,
                                _button_sprite->getContentSize().height*0.5f));
        _icon->setPosition(ccp(_label->getPositionX()+_label->getContentSize().width,
                               _label->getPositionY()));
    }
    
    CCMenuItemSprite *_menuItem = CCMenuItemScaleSprite::create(_button_sprite,
                                                                this,
                                                                menu_selector(SellItemDialog::onButton));
    _menuItem->setTag(index);
    m_menu[index] = CCMenu::create(_menuItem, NULL);
    m_menu[index]->setPosition(ccp(x_pos,-200.f * sizeMultiplier));
    addChild(m_menu[index]);
    
    if (label) {
        CCLabelTTF *title_label = CCLabelTTF::create(label, kDefaultFont, 32.0f);
        title_label->setColor(kGreenColour);
        title_label->setPosition(m_menu[index]->getPosition()
                                 + ccp(0.0f, _button_sprite->getContentSize().height*sizeMultiplier*1.35f));
        addChild(title_label);
    }
}

void SellItemDialog::onValueChanged(cocos2d::CCObject *sender) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
 
    if (CCMenuItemSprite *menu = (CCMenuItemSprite*)sender) {
        
        CREATE_CHAR_BUFFER(_buffer, 32);
        
        m_value += menu->getTag();
        if (m_value < 1) {
            m_value = 1;
        }
        if (m_value > m_value_max) {
            m_value = m_value_max;
        }
        
        if (m_label_value) {
            snprintf(_buffer, _buffer_size, "%d", m_value);
            m_label_value->setString(_buffer);
        }
        if (m_label_price) {
            snprintf(_buffer, _buffer_size, "%d", m_price * m_value);
            m_label_price->setString(_buffer);
        }
        
    }
    
    if ((m_price * m_value) + g_playerState->player.balance.gold > g_playerState->getTotalCoinStorageCapacity()) {
        g_sceneManager->notification->addMessage(MSGN_TYPE_WARNING,
                                                 g_gameContent->Word["warning_coin_over_capacity_msg"].c_str());
    }
    
}

void SellItemDialog::onClose(CCObject *sender) {
    
    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    
    if (this->numberOfRunningActions() > 0) return;
    
    if (getTag() == 1) {
        m_modalController->closeAll();
    }else {
        m_modalController->popView();
    }
    
}

void SellItemDialog::onButton(CCObject *sender) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    CCDictionary *params = CCDictionary::create();
    params->setObject(CCString::createWithFormat("%d",m_value), "coin");
    FlurryCpp::logEvent(EVENT_COIN_EARN_SELL_ITEM, params);
    
    CSJson::Value eventParameters;
    eventParameters["coin"] = m_value;
    AnalyticsClient::getInstance()->logEvent(EVENT_COIN_EARN_SELL_ITEM, eventParameters);
    
    g_sceneManager->showLoading();
    BackendService::getInstance()->send(APIItemSell::create(itemVo.type,
                                                            m_value,
                                                            this,
                                                            SellItemDialog::onItemSold));
    
}

void SellItemDialog::onItemSold(void *sender, std::string resp_str) {
    
    g_gameAudio->playEffect(AUDIO_COLLECT);
    
    PlayerState* _playerState = PlayerState::getInstance();
    
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseUserBalance(_root);
    _playerState->parseItemData(_root);
    Json_dispose(_root);
    
    g_sceneManager->hideLoading();
    
    SellItemDialog* _this = (SellItemDialog*)sender;
    if (!IS_INSTANCE_OK(_this, SellItemDialog)) return;
    
    ItemStorageDialog* _storageDialog = (ItemStorageDialog*)_this->m_entity;
    if (IS_INSTANCE_OK(_storageDialog, ItemStorageDialog)) {
        _storageDialog->updateContent();
    }
    
    _this->m_modalController->popView();
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!IS_INSTANCE_OK(_scene, SceneVille))
        return;
    HUDPanel *hudPanel = _scene->hudPanel;
    CCSprite *coinIconSprite = hudPanel->resourceBars[HUDPanelBarCoins]->getIconSprite();
    CCPoint endPoint = coinIconSprite->convertToWorldSpaceAR(CCPointZero);
    CollectItemEffect::addCollectItemEffect(CollectItemEffect::create(UIHelper::getSpritePath(UIHelper::HUDCoin),
                                                                      kCollectItemEffectDefaultSprite,
                                                                      _scene->getContentSize()*0.5f,
                                                                      endPoint,
                                                                      _this,
                                                                      SellItemDialog::onCollectCoinsFinished), _scene);
    
}

void SellItemDialog::onCollectCoinsFinished(void *sender) {
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!IS_INSTANCE_OK(_scene, SceneVille))
        return;
    
    _scene->hudPanel->updateAllResourceBars();
    _scene->hudPanel->checkTrade();

}

