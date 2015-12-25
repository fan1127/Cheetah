//
//  ItemStorageDialog.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/4/2557 BE.
//
//

#include "ItemStorageDialog.h"

USING_NS_CC;
USING_NS_CC_EXT;

using namespace std;

#include "../../Settings.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Helpers/CreateSpriteHelper.h"
#include "../../Helpers/UIDisplayHelper.h"

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Services/BackendService.h"
extern BackendService *g_gameBackend;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

extern PlayerState* g_playerState;

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

#include "../../UIs/Dialogs/AnimalTradeDialog.h"
#include "../../UIs/Dialogs/ItemStorageItem.h"
#include "../../UIs/Dialogs/ItemStoragePopoverDialog.h"
#include "../../Helpers/StringHelper.h"
#include "../../DialogSettings.h"

ItemStorageDialog* ItemStorageDialog::create(CCModalController* parentModalController_, PlayerState* playerState_) {
    
    ItemStorageDialog *pRet = new ItemStorageDialog();
    if (pRet) {
        pRet->m_modalController = parentModalController_;
        pRet->m_playerState = playerState_;
        if (pRet->init(parentModalController_, kDialogItemBarnSize)) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool ItemStorageDialog::init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize)
{
    if (!ScrollDialog::init(parentModalController_, dialogSize))
    {
        return false;
    }

    Building *_itemStorage = m_playerState->getBuilding(kBuildingItem, 0);
    if (!_itemStorage) {
        return false;
    }
    
    m_modalController->onHitShadow = onHitShadow;
    m_modalController->userObject = this;
    m_popOver = NULL;

    addTitle("");

#ifdef kDialogFooterTextEnabled
    if (m_playerState == g_playerState) {
        char _buffer[256] = {0};
        char _buffer2[256] = {0};
        
        snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["item_storage_footer_msg"].c_str());
        snprintf(_buffer2, _buffer2_size, _buffer, _itemStorage->bVo.title.c_str());
        snprintf(_buffer, _buffer_size, "%s", _buffer2);
        m_label_footer = CCLabelHelper::createBMFont(_buffer, 45.0f, false, kTextColorLightBrown);//("", StyleDialogTextLight);
        m_label_footer->setAnchorPoint(ccp(0.0f, 0.5f));
        m_label_footer->setPosition(ccp(-getContentSize().width*0.5f+(kDialogPadding*sizeMultiplier),
                                        -getContentSize().height*0.42f));
        this->addChild(m_label_footer);
    }
#endif
    
    m_label_footer_right = CCLabelHelper::createBMFont("", 45.0f, false, kTextColorLightBrown);
    m_label_footer_right->setAnchorPoint(ccp(1.0f, 0.5f));
    m_label_footer_right->setPosition(ccp(getContentSize().width*0.4f-(kDialogPadding*sizeMultiplier), -getContentSize().height*0.31f));
    this->addChild(m_label_footer_right);

    createScrollContent(ITEMSTORAGE_DIALOG_SETTINGS);

    this->updateContent();

    return true;
}

void ItemStorageDialog::updateContent()
{
    int i = 1;
    int j = 0;
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    CCSize sizeItem = CCSizeMake(_screenSize.width*0.08f, _screenSize.height*0.15f);

    float marginWidth = (m_scrollView->getContentSize().width - sizeItem.width*5)/5.0f - 23.0f*sizeMultiplier;
    float marginHeight = (m_scrollView->getContentSize().height - sizeItem.height*2)/3.0f;
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    CREATE_CHAR_BUFFER(_buffer2, 64);
    
    
    
    ItemStorageItem *_lastItem = NULL;
    Building *_itemStorage = m_playerState->getBuilding(kBuildingItem, 0);
    if (!_itemStorage) {
        return;
    }
    
    m_playerState->updateItemData();
    
    int nslot = _itemStorage->bVo.param_1;
    int count = 0;
    m_scrollView->removeAllChildren();
    
    
    int _count = 0;
    for (vector<IntKeyValue*>::iterator it =m_playerState->Items.begin(); it != m_playerState->Items.end(); ++it) {
        if (IntKeyValue *_itemValue = (IntKeyValue*)(*it)) {
            GDItemVO _itvo = g_gameContent->getItemVO(_itemValue->key);
            
            CCLog_("[%s] = %d",_itemValue->key.c_str(), _itemValue->value);
            _count += _itemValue->value;
            if (_itvo.type != "" && _itemValue->value > 0) {
                
                ItemStorageItem *_item = ItemStorageItem::create(_itemValue, _itvo, sizeItem);
                _item->setTarget(this, menu_selector(ItemStorageDialog::selectedItem), CCNikScrollViewButtonStateTouchUp);
                _item->setDelegate(m_scrollView);
                _item->setPosition(ccp(marginWidth + (_item->getContentSize().width + marginWidth)*j,
                                       (m_scrollView->getContentSize().height - (_item->getContentSize().height*2 + marginHeight))*0.5f + (_item->getContentSize().height + marginHeight)*i));
                m_scrollView->addChild(_item);
                _lastItem = _item;

                m_zoomPos = _item->getPosition();

                count += _itemValue->value;
                CCLog_("count=%d", count);
                i--;
                if (i < 0) {
                    j++;
                    i = 1 ;
                }
                
            }
        }
    }
    
    CCLog_("total = %d", _count);
    
    if(count==0){
        CCLabelBMFont *nonAnimal = CCLabelHelper::createBMFont(retinaMultiplier, g_gameContent->Word["non_item"].c_str(), kFontSizeLarge, kTextColorWhiteGray);
        nonAnimal->setAnchorPoint(ccp(1.0f, 0.5f));
        nonAnimal->setPosition(ccp(this->getContentSize().width*0.5f,this->getContentSize().height*0.4f));
        
        m_scrollView->addChild(nonAnimal);
    }

    setTitle(_itemStorage->bVo.title.c_str());


    snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["item_storage_footer_msg_2"].c_str());
    snprintf(_buffer2, _buffer2_size, _buffer, count, nslot);
    snprintf(_buffer, _buffer_size, "%s", _buffer2);
    m_label_footer_right->setString(_buffer);
    
//    if (m_noteButton) {
//        m_noteButton->setPosition(m_label_footer_right->getPosition() + ccp(-m_label_footer_right->getContentSize().width*0.5f-kDialogPadding-m_noteButton->getContentSize().width*0.5f, 0.0f));
//    }
    
    if (!_lastItem) {
        return;
    }
    m_scrollView->setContentView(CCSizeMake(MAX(_lastItem->getPositionX() + _lastItem->getContentSize().width, m_scrollView->getContentSize().width), m_scrollView->getContentSize().height));
    m_scrollView->resetScrollEnabled(true);
    
}

void ItemStorageDialog::selectedItem(ItemStorageItem *item) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    m_popOver = ItemStoragePopoverDialog::create(item->item, item->itvo, this);
    m_popOver->presentPopover(item, true);
    
}

void ItemStorageDialog::onHitShadow(cocos2d::CCPoint& pnt, void *sender_) {
//    ItemStorageDialog *_this = (ItemStorageDialog *)((CCModalController *)sender_)->userObject;
//    if (_this->m_popOver) {
//        _this->m_popOver->dismissPopover(true);
//        _this->m_popOver = NULL;
//    }
}

void ItemStorageDialog::onItemListUpdated(void *sender, std::string resp_json_str) {
    
    ItemStorageDialog* _this = (ItemStorageDialog*)sender;
    if (_this) {
        _this->updateContent();
    }
    
    SceneVille* _sceneVille = (SceneVille*)g_currentScene;
    if (_sceneVille && typeid(*g_currentScene) == typeid(SceneVille)) {
        _sceneVille->hudPanel->updateAllResourceBars();
        _sceneVille->hudPanel->checkTrade();
    }
    
}

#pragma mark - CCNikScrollViewDelegate

void ItemStorageDialog::scrollViewWillBeginDragging(CCNikScrollView *view) {
//    if (m_popOver) {
//        m_popOver->dismissPopover(true);
//        m_popOver = NULL;
//    }
}

#pragma mark - button callback

void ItemStorageDialog::onNote(cocos2d::CCObject *obj_) {
    
    CREATE_CHAR_BUFFER(_buffer, 512);
    snprintf(_buffer, _buffer_size, g_gameContent->Word["note_item_storage_dialog"].c_str(), g_gameContent->getBuildingVO(kBuildingFusion, 1).title.c_str());
    CCLabelBMFont* _note_label = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium-5.0f, false, kTextColorDark, getContentSize().width*0.4f, kCCTextAlignmentCenter);
    
    CCSize _contentSize = CCNodeHelper::getContentSize(_note_label);
    CCSize _padding = kPopOverDialogPadding*sizeMultiplier;
    
    CCPopOverDialog* _hintPopOver = CCPopOverDialog::create(_contentSize + _padding, CCPopoverArrowDirectionUp);
    _note_label->setPosition(_hintPopOver->getContentSize()*0.5f);
    _hintPopOver->addChild(_note_label);
    _hintPopOver->presentPopover(m_noteButton, true);
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
}