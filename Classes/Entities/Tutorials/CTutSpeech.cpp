//
//  CTutSpeech.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "CTutSpeech.h"
USING_NS_CC;
using namespace std;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

extern CCScene* g_currentScene;

#include "../../AppMacros.h"
extern TargetDevice targetDevice;
extern float sizeMultiplier;
extern float retinaMultiplier;

#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Helpers/CCStringHelper.h"

#include "CTutUnit.h"
#include "../../UIs/Dialogs/NameDialog.h"
#include "../../Scenes/SceneVille.h"

void CTutSpeech::perform(void* tutUnit) {
    
    CTutUnit* _tutUnit = (CTutUnit*)tutUnit;
    if (!_tutUnit) return;
    
    active = true;
    this->tutUnit = tutUnit;
    
    _tutUnit->addSpotlightBG(this, CTutSpeech::_onHitTheBackground);
    
    if (message != "") {
        
        CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
        
        if (_tutUnit->messageDialogSprite == NULL) {
            _tutUnit->messageDialogSprite = CCScale9Sprite::createWithSpriteFrameName("oldversion/_ground_9sprite_dialog.png");
            _tutUnit->addChild(_tutUnit->messageDialogSprite);
        }
        
        _tutUnit->messageDialogSprite->setContentSize(CCSize(800.0f*sizeMultiplier,
                                                             360.0f*sizeMultiplier));
        _tutUnit->messageDialogSprite->setVisible(true);
        
        const auto characterLeftPos = ccp(_screenSize.width*0.2f, _screenSize.height*0.0f);
        const auto characterRightPos = ccp(_screenSize.width*0.8f, _screenSize.height*0.0f);
        
        const float speechY = 270.0f*sizeMultiplier;
        const auto speechLeftPos = ccp(characterLeftPos.x + 300.0f*sizeMultiplier, speechY);
        const auto speechRightPos = ccp(characterRightPos.x - 200.0f*sizeMultiplier, speechY);
        
        CCSprite* _arrow;
        if (_tutUnit->messageDialogSprite->getChildByTag(TAG_ARROW) == NULL) {
            _arrow = CCSprite::createWithSpriteFrameName("oldversion/_mini_ground_arrow_white.png");
            _arrow->setTag(TAG_ARROW);
            _tutUnit->messageDialogSprite->addChild(_arrow);
        } else {
            _arrow = (CCSprite*)_tutUnit->messageDialogSprite->getChildByTag(TAG_ARROW);
        }
        
        if (side == CT_CHR_SIDE_LEFT) {
            _tutUnit->messageDialogSprite->setAnchorPoint(ccp(0.0f, 0.5f));
            _tutUnit->messageDialogSprite->setPosition(speechLeftPos);
            if (chr == CT_CHR_FANS) {
                _tutUnit->messageDialogSprite->setPositionX(_tutUnit->messageDialogSprite->getPositionX()+_screenSize.width*0.1f);
            }
            _arrow->setPosition(ccp(-_arrow->getContentSize().width*0.25f,
                                    _tutUnit->messageDialogSprite->getContentSize().height*0.7f));
            _arrow->setScaleX(1);
        }else { //TCS_RIGHT
            _tutUnit->messageDialogSprite->setAnchorPoint(ccp(1.0f, 0.5f));
            _tutUnit->messageDialogSprite->setPosition(speechRightPos);
            _arrow->setPosition(ccp(_tutUnit->messageDialogSprite->getContentSize().width+_arrow->getContentSize().width*0.25f,
                                    _tutUnit->messageDialogSprite->getContentSize().height*0.7f));
            _arrow->setScaleX(-1);
        }
        
        
        CCLabelBMFont* _text;
        if (_tutUnit->messageDialogSprite->getChildByTag(TAG_TEXT) == NULL) {
            _text = CCLabelHelper::createBMFont("", kFontSizeLarge, kTextColorLightBrown);
            _text->setTag(TAG_TEXT);
            _tutUnit->messageDialogSprite->addChild(_text);
        } else {
            _text = (CCLabelBMFont*)_tutUnit->messageDialogSprite->getChildByTag(TAG_TEXT);
        }
        
        _text->setString(message.c_str());
        
        _text->setPosition(_tutUnit->messageDialogSprite->getContentSize()*0.5f);
        _text->setWidth(_tutUnit->messageDialogSprite->getContentSize().width*0.8f);
        
        //===================== START characterSpine =====================
        
        _tutUnit->characterLast = chr;
        
        if (_tutUnit->characterSpineUncle) _tutUnit->characterSpineUncle->setVisible(false);
        if (_tutUnit->characterSpineSarah) _tutUnit->characterSpineSarah->setVisible(false);
        if (_tutUnit->characterSpineMerchant) _tutUnit->characterSpineMerchant->setVisible(false);
        if (_tutUnit->characterSpineFans) _tutUnit->characterSpineFans->setVisible(false);
        
        CCSkeletonAnimation *characterSpine = NULL;
        
        switch (chr) {
            case CT_CHR_NONE:
            default: break;
            case CT_CHR_UNCLE:
            {
                if (_tutUnit->characterSpineUncle == NULL) {
                    _tutUnit->characterSpineUncle = g_sceneManager->sharedSkeletonData->getSkeleton(kTutorialCharacterSrcDir, "samuel_tutor");
                    _tutUnit->addChild(_tutUnit->characterSpineUncle);
                }
                characterSpine = _tutUnit->characterSpineUncle;
            }
                break;
            case CT_CHR_SARAH:
            {
                if (_tutUnit->characterSpineSarah == NULL) {
                    _tutUnit->characterSpineSarah = g_sceneManager->sharedSkeletonData->getSkeleton(kTutorialCharacterSrcDir, "sarah");
                    _tutUnit->addChild(_tutUnit->characterSpineSarah);
                }
                characterSpine = _tutUnit->characterSpineSarah;
            }
                break;
            case CT_CHR_MERCHANT:
            {
                if (_tutUnit->characterSpineMerchant == NULL) {
                    _tutUnit->characterSpineMerchant = g_sceneManager->sharedSkeletonData->getSkeleton(kCharacterSrcDir, "item_merchant");
                    _tutUnit->addChild(_tutUnit->characterSpineMerchant);
                }
                characterSpine = _tutUnit->characterSpineMerchant;
            }
                break;
            case CT_CHR_FANS:
            {
                if (_tutUnit->characterSpineFans == NULL) {
                    _tutUnit->characterSpineFans = g_sceneManager->sharedSkeletonData->getSkeleton(kTutorialCharacterSrcDir, "fans");
                    _tutUnit->addChild(_tutUnit->characterSpineFans);
                }
                characterSpine = _tutUnit->characterSpineFans;
            }
                break;
        }
        
        if (characterSpine) {
            characterSpine->setVisible(true);
            
            if (side == CT_CHR_SIDE_LEFT) {
                characterSpine->setPosition(characterLeftPos);
                if (chr == CT_CHR_MERCHANT) {
                    characterSpine->setScaleX(scale*-sizeMultiplier);
                    characterSpine->setScaleY(scale*sizeMultiplier);
                    
                }else {
                    characterSpine->setScaleX(scale*sizeMultiplier);
                    characterSpine->setScaleY(scale*sizeMultiplier);
                }
            }else { //TCS_RIGHT
                characterSpine->setPosition(characterRightPos);
                characterSpine->setScaleX(scale*sizeMultiplier);
                characterSpine->setScaleY(scale*sizeMultiplier);
            }
            
            _tutUnit->characterLastAni = "";
            
            _tutUnit->characterLastAni = animation;
            characterSpine->clearAnimation();
            characterSpine->setAnimation(animation.c_str(), loop);
            
            if (animation2 != "") {
                characterSpine->addAnimation(animation2.c_str(), loop2);
            }
        }
        
        characterSpine = NULL;
        
        //===================== END characterSpine =====================
        
        if (_tutUnit->chooseDialogSprite == NULL) {
            _tutUnit->chooseDialogSprite = UIHelper::getScalableSprite(UIHelper::PaperBg);
            _tutUnit->addChild(_tutUnit->chooseDialogSprite);
            _tutUnit->setTouchPriority(kCCMenuHandlerPriority-2);
            _tutUnit->chooseDialogSprite->setVisible(false);
        }
        
        switch (option) {
            case CT_SPC_OPTION_NONE:
            default:
                break;
            case CT_SPC_OPTION_CHOOSE_ITEM: {
                
                _tutUnit->chooseDialogSprite->setVisible(true);
                
                CCSprite *_btnTemp = UIHelper::getSprite(UIHelper::UISprites(UIHelper::MerchantObjectButton));
                _btnTemp->setScale(getChooseItemsScale());
                
                CCSize btnSize = _btnTemp->getContentSize() * _btnTemp->getScale();
                _btnTemp->removeFromParentAndCleanup(true);
                
                _tutUnit->chooseDialogSprite->setContentSize(CCSizeMake(kTutNChoiceKeys * btnSize.width
                                                                        + (kTutNChoiceKeys-1) * kMarginBetweenMerchantItems
                                                                        + 2 * 80.0f*sizeMultiplier*getChooseItemsScale(),
                                                                        btnSize.height
                                                                        + 100.0f*sizeMultiplier*getChooseItemsScale()));
                
                
                
                _tutUnit->chooseDialogSprite->setPosition(_screenSize.width*0.5f
                                                          + 150.0f*sizeMultiplier,
                                                          _screenSize.height
                                                          - _tutUnit->chooseDialogSprite->getContentSize().height*0.5f
                                                          - 50.0f*sizeMultiplier*getChooseItemsScale());
                
                CCArray *_menus = CCArray::create();
                
                for (int i=0; i<kTutNChoiceKeys; i++) {
                    
                    if (keys[i] == "") continue;
                    bool isSold = false;
                    int indexMinus = (int)keys[i].find("-");
                    if(indexMinus == 0)
                    {
                        isSold = true;
                        keys[i] = keys[i].substr(1);
                    }
                    
                    GDItemVO _item_vo = g_gameContent->getItemVO(keys[i]);
                    if (_item_vo.type == "") continue;
                    
                    CCPoint itemPosition = ccp(btnSize.width * 0.5f,
                                               btnSize.height * 0.5f
                                               + 30.0f*sizeMultiplier);
                    CCSprite *_btnBase ;
                    if(!isSold)
                    {
                        _btnBase = UIHelper::getSprite(UIHelper::UISprites(UIHelper::MerchantObjectButton));
                    }
                    else
                    {
                        _btnBase = UIHelper::getSprite(UIHelper::UISprites(UIHelper::MerchantObjectSoldoutButton));
                    }
                    _btnBase->setScale(getChooseItemsScale());
                    _btnBase->setPosition(ccp(0.0f, 0.0f));
                    CCMenuItemSprite *_menuItem ;
                    if(!isSold)
                    {
                        _menuItem = CCMenuItemScaleSprite::create(_btnBase, this, menu_selector(CTutSpeech::_onItemChosen));
                    }
                    else
                    {
                        _menuItem = CCMenuItemScaleSprite::create(_btnBase, this, NULL);
                    }
                    
                    
                    _menuItem->setTag(i);
                    if(!isSold)
                    {
                        CCSprite* _itemLighting = UIHelper::getSprite(UIHelper::UISprites(UIHelper::MerchantObjectLight));
                        _itemLighting->setPosition(ccp(btnSize.width * 0.5f, btnSize.height * 0.5f));
                        _itemLighting->setScale(getChooseItemsScale());
                        _menuItem->addChild(_itemLighting);
                    }
                    bool isCoins = (i == 0);
                    CCSprite *_btnInside ;
                    if(!isSold)
                    {
                        _btnInside = UIHelper::getSprite(UIHelper::UISprites(isCoins ? UIHelper::MerchantCoinsButton : UIHelper::MerchantGemsButton));
                    }
                    else
                    {
                        _btnInside = UIHelper::getSprite(UIHelper::UISprites(UIHelper::MerchantSoldoutButton));
                    }
                    _btnInside->setPosition(ccp(btnSize.width * 0.5f,
                                                85.0f*sizeMultiplier));
                    _menuItem->addChild(_btnInside);
                    if(!isSold)
                    {
                        CREATE_CHAR_BUFFER(_buffer, 32);
                        snprintf(_buffer, _buffer_size, "%s", CCStringHelper::createWithNumberFormat(isCoins ? _item_vo.buy_value : _item_vo.gem_value).c_str());
                        CCSprite *_currency_icon = UIHelper::getSprite(UIHelper::UISprites(isCoins ? UIHelper::NSBuyCoinIcon : UIHelper::NSClaimGemBlue));
                        _currency_icon->setScale(0.75f);
                        _menuItem->addChild(_currency_icon);
                        
                        CCLabelBMFont* _price = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium-5.0f, true);
                        _price->setAlignment(kCCTextAlignmentRight);
                        bool isNotEnoughCoins = (g_playerState->player.balance.gold <  _item_vo.buy_value);
                        if(isCoins && isNotEnoughCoins)
                        {
                            _price->setColor(kTextColorOrange2);
                        }
                        _menuItem->addChild(_price);
                        
                        UIHelper::setTwoNodesToMiddlePosition(_currency_icon,
                                                              _price,
                                                              10.0f*sizeMultiplier,
                                                              _btnInside->getPosition()
                                                              + ccp(0.0f, 5.0f*sizeMultiplier));
                    }
                    else
                    {
                        CCLabelBMFont* _soldText = CCLabelHelper::createBMFont("SOLD", kFontSizeMedium-5.0f, true);
                        _soldText->setAlignment(kCCTextAlignmentRight);
                        _soldText->setPosition(_btnInside->getPosition());
                        _menuItem->addChild(_soldText);
                        
                    }
                    CCLabelBMFont* _label = CCLabelHelper::createBMFont(_item_vo.title.c_str(), kFontSizeMedium, true);
                    _label->setPosition(btnSize.width * 0.5f,
                                        btnSize.height - 50.0f*sizeMultiplier);
                    _menuItem->addChild(_label);
                    
                    CCSprite* _itemSprite = CCSprite::createWithSpriteFrameName(_item_vo.src.c_str());
                    _itemSprite->setPosition(itemPosition);
                    _menuItem->addChild(_itemSprite);
                    
                    _menus->addObject(_menuItem);
                    
                    _menuItem->setContentSize(btnSize);
                    _menuItem->setPosition(ccp(_tutUnit->chooseDialogSprite->getContentSize().width * 0.5f
                                               + (btnSize.width*(i-1))
                                               + (kMarginBetweenMerchantItems*(i-1)),
                                               _tutUnit->chooseDialogSprite->getContentSize().height*0.5f));
                    
                    
                }
                
                CCMenu* _menu = CCMenu::createWithArray(_menus);
                _menu->setTouchPriority(kCCMenuHandlerPriority-2);
                _menu->setPosition(CCPointZero);
                _tutUnit->chooseDialogSprite->addChild(_menu);
                
            }break;
            case CT_SPC_OPTION_FILL_NAME: {
                
                NameDialog* inputName = NameDialog::create(this, CTutSpeech::_onNameFilled);
                _tutUnit->addChild(inputName);
                
            }break;
            case CT_SPC_OPTION_NONE_WITH_PICTURE: {
                
                _tutUnit->messageDialogSprite->setContentSize(CCSize(1100.0f*sizeMultiplier,
                                                                     360.0f*sizeMultiplier));
                
                _text->setPosition(_tutUnit->messageDialogSprite->getContentSize().width *0.3f,_tutUnit->messageDialogSprite->getContentSize().height*0.5f);
                _text->setWidth(_tutUnit->messageDialogSprite->getContentSize().width*0.5f);
                
                _tutUnit->m_spineBuildingPreview = g_sceneManager->sharedSkeletonData->getSkeleton(kBuildingSrcPath, vo.src.c_str());
                _tutUnit->m_spineBuildingPreview->setScale(retinaMultiplier);
                _tutUnit->m_spineBuildingPreview->setAnchorPoint(ccp(1.0f, 1.0f));
                _tutUnit->m_spineBuildingPreview->setPosition(_tutUnit->messageDialogSprite->getContentSize().width *0.75f,
                                                              _tutUnit->messageDialogSprite->getContentSize().height*0.5);
                _tutUnit->messageDialogSprite->addChild(_tutUnit->m_spineBuildingPreview);
                
            }break;
                
                
        }
        //----ANIMATION---
        
        _tutUnit->messageDialogSprite->setScaleX(SPEECH_ANIMATION_STARTING_SCALE);
        
        
        if (option == CT_SPC_OPTION_FILL_NAME) {
            
            g_gameAudio->playEffect(AUDIO_UI_CLICK, false);
            
            _tutUnit->messageDialogSprite->setRotation(0);
            _tutUnit->messageDialogSprite->setScale(1.0f);
            _tutUnit->messageDialogSprite->
            runAction(CCSequence::create(CCShow::create(),
                                         CCCallFunc::create(this, callfunc_selector(CTutSpeech::onAnimationFinished)),
                                         NULL));
            
        } else {
            
            if (side == CT_CHR_SIDE_LEFT) {
                _tutUnit->messageDialogSprite->setRotation(-SPEECH_ANIMATION_STARTING_ROTATION_ANGLE);
            } else {
                _tutUnit->messageDialogSprite->setRotation(SPEECH_ANIMATION_STARTING_ROTATION_ANGLE);
            }
            
            g_gameAudio->playEffect(AUDIO_UI_CLICK, false);
            
            _tutUnit->messageDialogSprite->runAction(CCDelayTime::create(SPEECH_ANIMATION_DELAY_UNTIL_TAPPABLE));
            _tutUnit->messageDialogSprite->
            runAction(CCSequence::create(CCShow::create(),
                                         CCSpawn::create(CCEaseElasticOut::create(CCScaleTo::create(SPEECH_ANIMATION_TIME_SCALING, 1.0f), SPEECH_ANIMATION_fPeriod),
                                                         CCEaseElasticOut::create(CCRotateTo::create(SPEECH_ANIMATION_TIME_ROTATION, 0), SPEECH_ANIMATION_fPeriod),
                                                         NULL),
                                         CCCallFunc::create(this, callfunc_selector(CTutSpeech::onAnimationFinished)),
                                         NULL));
        }
    }
    
}

float CTutSpeech::getChooseItemsScale()
{
    float scale = 1.0f;
    
    switch (targetDevice) {
        case TD_IPHONE:
        case TD_IPHONE_RETINA:
            scale = 0.75f;
            break;
            
        default:
            break;
    }
    
    return scale;
}

void CTutSpeech::update(float delta_)
{
    if (typeid(*g_currentScene) != typeid(SceneVille))
        return;
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if(_scene->hudPanel->isLevelUpEffectShowed())
        _scene->hudPanel->hideLevelUpEffect();
}

void CTutSpeech::onAnimationFinished(CCObject *sender)
{
    
}

void CTutSpeech::_onPetChosen(cocos2d::CCObject *object_) {
    
    if (CCMenuItem* _menuItem = (CCMenuItem*)object_) {
        if (callback) {
            callback(sender, keys[_menuItem->getTag()]);
        }
    }
    
}

void CTutSpeech::_onItemChosen(cocos2d::CCObject *sender) {
    
    CCMenuItem* _menuItem = (CCMenuItem*)sender;
    
    if (g_playerState->getNItems() >= g_playerState->getTotalItemStorageCapacity()) {
        g_sceneManager->notification->addMessage(MSGN_TYPE_WARNING, g_gameContent->Word["storage_is_full"].c_str());
        return;
    }
    
    string _item = keys[_menuItem->getTag()];
    GDItemVO _item_vo = g_gameContent->getItemVO(_item);
    if (_item_vo.type == "") return;
    
    int selected_item_index = _menuItem->getTag();
    CCAssert(selected_item_index < 10, "Selected Item Index should less than 10!");
    
    if (selected_item_index == 0) { //first item - requires coins
        if (g_playerState->player.balance.gold < _item_vo.buy_value) {
            g_sceneManager->notification->addMessage(MSGN_TYPE_WARNING, g_gameContent->Word["trade_warning_not_enough_coins"].c_str());
            return;
        }
    }else { // while the rest requires gems
        if (g_playerState->player.balance.gem < _item_vo.gem_value) {
            g_sceneManager->notification->addMessage(MSGN_TYPE_WARNING, g_gameContent->Word["trade_warning_not_enough_gems"].c_str());
            return;
        }
    }
    
    if (callback) {
        
        if (CTutUnit* _tutUnit = (CTutUnit*)tutUnit) {
            _tutUnit->removeChooseDialogSprite();
        }
        
        string _result = "";
        CREATE_CHAR_BUFFER(_buffer, 4)
        snprintf(_buffer, _buffer_size, "%d:", selected_item_index);
        _result.append(_buffer);
        _result.append(keys[selected_item_index]);
        
        callback(this->sender, _result);
        
    }
    
}

void CTutSpeech::_onHitTheBackground(void *inst_, void *sender_) {
    if (CTutSpeech* _this = (CTutSpeech*)sender_) {
        
        if (CTutUnit* _tutUnit = (CTutUnit*)_this->tutUnit) {
            if (_tutUnit->messageDialogSprite) {
                if (_tutUnit->messageDialogSprite->numberOfRunningActions() > 1) { //limited by another delay animation, so it should be 1 here instead of normally 0
                    return;
                }
            }
        }
        
        if (_this->option == CT_SPC_OPTION_NONE || _this->option == CT_SPC_OPTION_NONE_WITH_PICTURE) {
            if (_this->callback) {
                _this->callback(_this->sender, "");
            }
        }
        
    }
    
}

void CTutSpeech::_onNameFilled(void *sender_, std::string result_) {
    
    if (CTutSpeech* _this = (CTutSpeech*)sender_) {
        if (_this->callback) {
            _this->callback(_this->sender, result_);
        }
    }
    
}
