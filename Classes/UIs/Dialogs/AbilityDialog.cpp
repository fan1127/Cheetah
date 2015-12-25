//
//  IAPDialog.cpp
//  Cheetah
//
//  Created by Teerapat on 4/3/57 BE.
//
//

#include "AbilityDialog.h"
#include "../../Helpers/CCStringHelper.h"
#include "../../Helpers/CCSpriteHelper.h"

USING_NS_CC;
using namespace std;


#include "../../Extensions/Modal/CCModalController.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/CreateSpriteHelper.h"

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../../AppMacros.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../Helpers/CCNodeHelper.h"
#include "../../Services/GameAudioService.h"
#include "../../Helpers/StringHelper.h"
#include "../../DialogSettings.h"
#include "../../Helpers/UIHelper.h"

extern GameAudioService* g_gameAudio;

extern TargetDevice targetDevice;
extern float sizeMultiplier;
extern float retinaMultiplier;

#define kSpacesBetweenAbilityItems (10.0f*sizeMultiplier)

AbilityDialog* AbilityDialog::create(CCModalController* parentModalController_, PlayerState* playerState_) {
    
    if (AbilityDialog *pRet = new AbilityDialog()) {
        pRet->m_modalController = parentModalController_;
        pRet->m_playerState = playerState_;
        if (pRet->init(parentModalController_, kDialogAbilitySize)) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
}

bool AbilityDialog::init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize)
{
    if (!ScrollDialog::init(parentModalController_, dialogSize))
    {
        return false;
    }

    m_modalController->onHitShadow = onHitShadow;
    m_modalController->userObject = this;
    _modalController = m_modalController;
    
    addTitle(g_gameContent->Word["title_library_dialog"].c_str());

    if (m_playerState == g_playerState) {
        if (Ability* _ability = m_playerState->getUpgradingAbility()) {
            CREATE_CHAR_BUFFER(_buffer, 64);
            snprintf(_buffer, _buffer_size, g_gameContent->Word["ability_is_being_upgraded"].c_str(), _ability->aVo.title.c_str(), _ability->paVo.level+1);
            m_label_foot = CCLabelHelper::createBMFont(_buffer, StyleDialogTextDark);
        }else {
            m_label_foot = CCLabelHelper::createBMFont(g_gameContent->Word["upgrade_ability_dialog_foot_msg"].c_str(), StyleDialogTextDark);
        }
        m_label_foot->setAnchorPoint(ccp(0.5, 0.5));
        m_label_foot->setPosition(ccp(0,
                                      -m_background->getContentSize().height*0.5
                                      +170.0f*sizeMultiplier
                                      ));
        this->addChild(m_label_foot);
    }
    
    m_noteButton = NULL;
    if (g_gameContent->Word["note_library_dialog"] != "") {
        m_noteButton = CCMenuHelper::createMenu(UIHelper::getSpritePath(UIHelper::NSHelpButton), this, menu_selector(AbilityDialog::onNote));
        m_noteButton->setPosition(ccp(getContentSize().width*0.5f - 85*sizeMultiplier - m_noteButton->getContentSize().width*0.5f,
                                      -getContentSize().height*0.5f + 75*sizeMultiplier + m_noteButton->getContentSize().height*0.5f));
        addChild(m_noteButton);
    }

    createScrollContent(ABILITY_DIALOG_SETTINGS);

    updateContent();

    return true;
}

void AbilityDialog::updateContent()
{
//    if (!m_products) return;
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    
    CCNikScrollViewButton *lastButton = NULL;
    CCNikScrollViewButton *button;
    
    
    bool _upgrade_in_progress = false;
    for (vector<string>::iterator it=g_gameContent->AbilitiesOrder.begin(); it!=g_gameContent->AbilitiesOrder.end(); ++it) {
        
        string _type = *it;
        
        if (g_gameContent->Abilites[_type].size() > 0) {
            
            GDAbilityVO _vo = g_gameContent->Abilites[_type][0];
            GDAbilityVO _nxt_vo = g_gameContent->Abilites[_type][0];
            if (Ability* _ability = m_playerState->Abilities[_type]) {
                _vo = _ability->aVo;
                int index = _ability->aVo.level;
                if (index > g_gameContent->Abilites[_ability->aVo.type].size()-1) index = g_gameContent->Abilites[_ability->aVo.type].size()-1;
                _nxt_vo = g_gameContent->Abilites[_ability->aVo.type][index];
                if (_ability->paVo.status == kAbilityIsUpgrading) {
                    _upgrade_in_progress = true;
                }
            }
        }
    }

    int i = 0;
    for (vector<string>::iterator it=g_gameContent->AbilitiesOrder.begin(); it!=g_gameContent->AbilitiesOrder.end(); ++it) {
        
        string _type = *it;
        
        if (g_gameContent->Abilites[_type].size() > 0) {
            
            m_buttonLockedReason = 0;
            
            int _reach_max_level = false;
            bool _zero_level = false;
            GDAbilityVO _vo = g_gameContent->Abilites[_type][0];

            if (Ability* _ability = m_playerState->Abilities[_type]) {
                _vo = _ability->aVo;
                
                if(_ability->paVo.level >= g_gameContent->Abilites[_type].size()) {
                    _reach_max_level = true;
                }
                if (_ability->paVo.level <= 0) {
                    _zero_level = true;
                }
            } else {
                _zero_level = true;
            }
            
            GDAbilityVO _nxt_vo = g_gameContent->Abilites[_type][0];
            bool _upgrading = false;
            if (Ability* _ability = m_playerState->Abilities[_type]) {
                _vo = _ability->aVo;
                int index = _ability->aVo.level;
                if (index > g_gameContent->Abilites[_ability->aVo.type].size()-1) index = g_gameContent->Abilites[_ability->aVo.type].size()-1;
                _nxt_vo = g_gameContent->Abilites[_ability->aVo.type][index];
                if (_ability->paVo.status == kAbilityIsUpgrading) {
                    _upgrading = true;
                }
            }
            
            GDAbilityVO _vo_to_display = _vo;
            if (_upgrading) {
                if (_vo.level > 1) {
                    _vo_to_display = g_gameContent->getAbilityVO(_vo.type, _vo.level-1);
                }
            }
            
            bool disabled = false;
            
            CCString *picName = CCString::createWithFormat("%s.png",_vo.src.c_str());
            
            Building* _library = m_playerState->getBuilding(kBuildingLibrary, 0);
            
            bool _library_under_construction = (_library->pbVo.status != BUILDING_STATUS_DELIVERED);
            
            bool _not_enough_level = (_library->pbVo.level < _nxt_vo.unlocks_at_level);
            
            if (!_upgrading) {
                if(_zero_level ||
                   _upgrade_in_progress ||
                   _not_enough_level ||
                   _library_under_construction)
                {
                    if (m_playerState == g_playerState) {
                        disabled = true;
                        picName = CCString::createWithFormat("%s_disabled.png",_vo.src.c_str());
                    }
                }
            }

            UIHelper::UISprites bg = UIHelper::AbilitiesAbilitiesButtonBg;

            if (disabled)
            {
                bg = UIHelper::AbilitiesButtonBgDisabled;
            }

            button = CCNikScrollViewButton::create(UIHelper::getSprite(bg), NULL);
            button->setPosition(ccp((button->getContentSize().width*i)
                                    +(kSpacesBetweenAbilityItems*i),
                                    m_clippingLayer->getContentSize().height
                                    -button->getContentSize().height
                                    -10.0f*sizeMultiplier
                                    ));
            button->setTag(i);
            
            CCLabelBMFont* skillNameTxt;
            CCLabelBMFont* abilityDescriptionTxt;
            CCLabelBMFont* abilityStatusTxt;
            CCLabelBMFont* effectTxt;
            CCLabelBMFont* durationTxt;
            CCSprite *_pic;
            CCSprite *_stunpic;

            UIHelper::UISprites sprite;
            
            if(_zero_level){
                
                sprite = UIHelper::AbilitiesLock;
            } else {
                
                UIHelper::UISprites abilityButton = UIHelper::AbilitiesAbilitiesButton;
                
                auto pos = picName->m_sString.find_first_of("12345");
                CCAssert(pos != string::npos, "Invalid ability icon name!");
                
                int abilities[] = {3, 4, 2, 0, 1};
                
                int num = abilities[picName->m_sString[pos] - '0' - 1];
                UIHelper::UISprites abilitySprite = (UIHelper::UISprites)(UIHelper::AbilitiesPowerup1 + num);
                
                if (disabled) {
                    abilityButton = UIHelper::AbilitiesButtonDisabled;
                    abilitySprite = (UIHelper::UISprites)(UIHelper::AbilitiesPowerup1Disabled + num);
                }
                
                auto picBb = UIHelper::getSprite(abilityButton);
                picBb->setPosition(ccp(button->getContentSize().width*0.5f,button->getContentSize().height*0.72f));
                button->addChild(picBb);
                
                sprite = UIHelper::UISprites(abilitySprite);
            }

            _pic = UIHelper::getSprite(sprite);
            _pic->setPosition(ccp(button->getContentSize().width*0.5f,button->getContentSize().height*0.72f));
            button->addChild(_pic, 1);
            
            if(_zero_level){
                snprintf(_buffer, _buffer_size, g_gameContent->Word["ability_locked"].c_str(), _vo_to_display.title.c_str());
            } else{
                snprintf(_buffer, _buffer_size, g_gameContent->Word["ability_level"].c_str(), _vo_to_display.title.c_str(), _vo_to_display.level);
            }
            skillNameTxt = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium, true);
            skillNameTxt->setPosition(ccp(button->getContentSize().width*0.5,
                                          button->getPositionY()+button->getContentSize().height*0.81));
            button->addChild(skillNameTxt);
            
            strncpy(_buffer, "", _buffer_size);

            auto yShift = -135*sizeMultiplier;
            auto yStunShift = -20*sizeMultiplier;

            if (m_playerState == g_playerState) {
                if (_library_under_construction) {
                    snprintf(_buffer, _buffer_size, g_gameContent->Word["library_is_not_complete_cannot_upgrade_ability_msg_short"].c_str(), _library->bVo.title.c_str());
                    m_buttonLockedReason = kButtonLockedReasonLibraryUnderConstruction;
                }
                
                if (_upgrade_in_progress) {
                    strncpy(_buffer, g_gameContent->Word["upgrade_in_progress"].c_str(), _buffer_size);
                    m_buttonLockedReason = kButtonLockedReasonUpgradeInProgress;
                }
                
                if (_not_enough_level) {
                    snprintf(_buffer, _buffer_size, g_gameContent->Word["need_building_lv"].c_str(), _library->bVo.title.c_str(), _nxt_vo.unlocks_at_level);
                    m_buttonLockedReason = _nxt_vo.unlocks_at_level;
                }
                
                if (_reach_max_level) { //check if at max level
                    strncpy(_buffer, g_gameContent->Word["max_level_reached"].c_str(), _buffer_size);
                    m_buttonLockedReason = kButtonLockedReasonReachMaxLevel;
                }
                
                if (_upgrading) {
                    strncpy(_buffer, g_gameContent->Word["ability_is_upgrading"].c_str(), _buffer_size);
                    m_buttonLockedReason = kButtonLockedReasonUpgrading;
                }

                abilityStatusTxt = CCLabelHelper::createBMFont(_buffer, kFontSizeSmall+5.0f, true);
                abilityStatusTxt->setAnchorPoint(ccp(0.5f,1.0f));
                abilityStatusTxt->setWidth(350*sizeMultiplier);
                abilityStatusTxt->setPosition(ccp(button->getContentSize().width*0.5, button->getContentSize().height*0.6));
                
                button->addChild(abilityStatusTxt);
                
                abilityStatusTxt->stopAllActions();
                
                if (_upgrading) {
                    abilityStatusTxt->runAction(CCRepeatForever::create(CCSequence::create(CCEaseIn::create(CCFadeOut::create(0.6f), 5.0f),
                                                                                           CCEaseOut::create(CCFadeIn::create(0.6f), 5.0f),
                                                                                           NULL)));
                }
            }

            if (_vo_to_display.type != "") {

                _stunpic = UIHelper::getSprite(UIHelper::AbilitiesStanIcon);
                _stunpic->setAnchorPoint(ccp(0.0f, 0.5f));
                _stunpic->setPosition(ccp(90.0f*sizeMultiplier,
                                          button->getContentSize().height*0.55 + yShift + yStunShift));
                button->addChild(_stunpic);
            
                effectTxt = CCLabelHelper::createBMFont(g_gameContent->Word["stun_duration"].c_str(), kFontSizeSmall, false, kTextColorDarkBrown);
                effectTxt->setPosition(ccp(224.0f*sizeMultiplier,
                                           button->getContentSize().height*0.6 + yShift + yStunShift));
                effectTxt->setAnchorPoint(ccp(0,0.5));
                button->addChild(effectTxt);
                
                snprintf(_buffer, _buffer_size, "%0.1fs", _vo_to_display.stun_duration);
                durationTxt = CCLabelHelper::createBMFont(_buffer, kFontSizeLarge, true);
                durationTxt->setPosition(ccp(224.0f*sizeMultiplier,
                                             button->getContentSize().height*0.52 + yShift + yStunShift));
                durationTxt->setAnchorPoint(ccp(0,0.5));
                button->addChild(durationTxt);
                
                abilityDescriptionTxt = CCLabelHelper::createBMFont(_vo_to_display.description.c_str(), kFontSizeSmall, false, kTextColorDarkBrown);
                abilityDescriptionTxt->setAnchorPoint(ccp(0,1));
                abilityDescriptionTxt->setWidth(320*sizeMultiplier);
                abilityDescriptionTxt->setPosition(ccp(90.0f*sizeMultiplier,
                                                       button->getPositionY()+button->getContentSize().height*0.32 + yShift));
                button->addChild(abilityDescriptionTxt);
            }
            
            m_scrollView->addChild(button, m_buttonLockedReason);
            button->setTarget(this, menu_selector(AbilityDialog::onButton), CCNikScrollViewButtonStateTouchUp);
            button->setDelegate(m_scrollView);
            
//            if (m_playerState != g_playerState) button->setEnabled(true);
            
            lastButton = button;
        }
        i++;
    }
    
    if (lastButton) {
        m_scrollView->setContentView(CCSizeMake(MAX(lastButton->getPositionX() + lastButton->getContentSize().width + 60*retinaMultiplier, m_scrollView->getContentSize().width), m_scrollView->getContentSize().height));
    }
}

void AbilityDialog::onButton(CCObject *sender_) {

    if (m_playerState != g_playerState) return;
    
    if (CCNode* _button = (CCNode*)sender_) {
        
        if(SceneVille* _scene = (SceneVille*)g_currentScene) {
            
            g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
            
            CREATE_CHAR_BUFFER(_buffer, 64);
            
            if (abilityUpgradeDialog) {
                abilityUpgradeDialog->removeFromParentAndCleanup(true);
                abilityUpgradeDialog = NULL;
            }
            
            GDAbilityVO _vo       = g_gameContent->Abilites[g_gameContent->AbilitiesOrder[_button->getTag()]][0];
            GDAbilityVO _nxt_vo   = g_gameContent->Abilites[g_gameContent->AbilitiesOrder[_button->getTag()]][0];
            bool _upgrading = false;
            if (Ability* _ability = m_playerState->Abilities[g_gameContent->AbilitiesOrder[_button->getTag()]]) {
                _vo = _ability->aVo;
                int index = _ability->aVo.level;
                if (index > g_gameContent->Abilites[_ability->aVo.type].size()-1) index = g_gameContent->Abilites[_ability->aVo.type].size()-1;
                _nxt_vo = g_gameContent->Abilites[_ability->aVo.type][index];
                
                if (_ability->paVo.status == kAbilityIsUpgrading) {
                    _upgrading = true;
                }
            }
            
            int _reason = _button->getZOrder();
            
            Building* _library = m_playerState->getBuilding(kBuildingLibrary, 0);
            
            if (_reason == kButtonLockedReasonReachMaxLevel) {
                g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, g_gameContent->Word["ability_already_at_max_level"]);
            } else if (_reason == kButtonLockedReasonUpgradeInProgress) {
                g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, g_gameContent->Word["other_ability_is_being_upgraded_msg"]);
            } else if (_reason == kButtonLockedReasonUpgrading) {
                snprintf(_buffer, _buffer_size, g_gameContent->Word["upgrading_to_level"].c_str(), _nxt_vo.level);
                g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, _buffer);
            } else if (_reason == kButtonLockedReasonLibraryUnderConstruction) {
                snprintf(_buffer, _buffer_size, g_gameContent->Word["library_is_not_complete_cannot_upgrade_ability_msg"].c_str(), _library->bVo.title.c_str());
                g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, _buffer);
            }
            
            if (_reason > 0) {
                
                GDBuildingVO _building_vo = g_gameContent->getBuildingVO(kBuildingLibrary, _button->getZOrder());
                
                if (_nxt_vo.level > 1) {
                    snprintf(_buffer, _buffer_size, g_gameContent->Word["required_hq_level_to_upgrade"].c_str(), _building_vo.title.c_str(), _reason);
                } else {
                    snprintf(_buffer, _buffer_size, g_gameContent->Word["locked_ability_description"].c_str(), _building_vo.title.c_str(), _reason);
                }
                g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, _buffer);
            }
            
            if (_reason != 0) { //if has any reason at all
                return; //prevent from creating abilityUpgradeDialog
            }
            
            if (_upgrading) {
                abilityUpgradeDialog = AbilityUpgradeDialog::createWithEntity(_vo, _vo);
            }
            else
            {
                abilityUpgradeDialog = AbilityUpgradeDialog::createWithEntity(_vo, _nxt_vo);
            }
            abilityUpgradeDialog->retain();
            abilityUpgradeDialog->setPosition((_button->getPosition()+_button->getContentSize()+CCSize(_button->getContentSize().width*0.1f, 1.0f))-m_scrollView->getContentOffset());
            _scene->mapToScreenUI->addChild(abilityUpgradeDialog);
            
        }

    }
    
}

void AbilityDialog::onHitShadow(cocos2d::CCPoint& pnt, void *sender_) {

    if(SceneVille* _scene = (SceneVille*)g_currentScene) {
        if (_scene->mapToScreenUI) {
            _scene->mapToScreenUI->closeAllUIs();
        }
    }

}

#pragma mark - CCNikScrollViewDelegate

void AbilityDialog::scrollViewWillBeginDragging(CCNikScrollView *view) {
    
     if(SceneVille* _scene = (SceneVille*)g_currentScene) {
         if (_scene->mapToScreenUI) {
             _scene->mapToScreenUI->closeAllUIs();
         }
     }
    
}

void AbilityDialog::onNote(cocos2d::CCObject *obj_) {
    
    CREATE_CHAR_BUFFER(_buffer, 512);
    snprintf(_buffer, _buffer_size, g_gameContent->Word["note_library_dialog"].c_str(),
            g_gameContent->getBuildingVO(kBuildingLibrary, 1).title.c_str(),
            g_gameContent->getBuildingVO(kBuildingLibrary, 1).title.c_str());
    
    CCLabelBMFont* _note_label = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium-5.0f, false, kTextColorDark, getContentSize().width*0.4f, kCCTextAlignmentCenter);
    CCSize _contentSize = CCNodeHelper::getContentSize(_note_label);
    CCSize _padding = kPopOverDialogPadding*sizeMultiplier;
    
    CCPopOverDialog* _hintPopOver = CCPopOverDialog::create(_contentSize + _padding, CCPopoverArrowDirectionUp);
    _note_label->setPosition(_hintPopOver->getContentSize()*0.5f);
    _hintPopOver->addChild(_note_label);
    _hintPopOver->presentPopover(m_noteButton, true);

    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
}

void AbilityDialog::onClose(cocos2d::CCObject *sender_) {
    
    ModalDialog::onClose(sender_);
    
    if(SceneVille* _scene = (SceneVille*)g_currentScene) {
        if (_scene->mapToScreenUI) {
            _scene->mapToScreenUI->closeAllUIs();
        }
    }
    
}
