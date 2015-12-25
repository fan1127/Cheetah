//
//  ShopPanel.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/3/56 BE.
//
//

#include "ShopPanel.h"
USING_NS_CC;
using namespace std;

#include "../AppMacros.h"

#include "../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

extern float sizeMultiplier;

#include "../Extensions/CCMenuItemScaleSprite.h"
#include "../Extensions/CCPhatTouchableSprite.h"
extern float margin;

bool ShopPanel::init()
{
    if (!CCLayer::init())
    {
        return false;
    }

    this->setTouchEnabled(true);
    this->setTouchMode(kCCTouchesAllAtOnce);

    m_sender = NULL;

    onCloseForTut = NULL;
    onBuildingAddedForTut = NULL;
    senderForTut = NULL;
    m_itemTypeCanBeGrabbed = "";
    m_dangerButtonsEnabled = true;
    m_zoomWhenAddBuildings = false;
    m_manuallyCloseShopPanelEnabled = true;

    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();

    m_panelLayer = CCLayer::create();
    this->addChild(m_panelLayer, 1);

    m_size610 = CCSizeMake(610.0f * sizeMultiplier, 610.0f * sizeMultiplier);
    m_panelBg = UIHelper::getScalableSprite(UIHelper::ScreenForSlice);

    m_panelBg->setContentSize(cocos2d::CCSize(1380.0f, 2000.0f) * sizeMultiplier);
    m_panelBg->setAnchorPoint(ccp(0.5f, 0.5f));
    m_panelBg->setPosition(ccp(0, _screenSize.height - m_panelBg->getContentSize().height*0.5f - 100*sizeMultiplier));
    m_panelLayer->addChild(m_panelBg , 1);

    m_tabScale = 0.6f;
    m_tabNormalIconScale = 0.8f;

    // Menu
    CCSprite *townButtonNormal = UIHelper::getScalableSpriteAsNormal(UIHelper::TabInactive);
    townButtonNormal->setScaleX(m_tabScale);
    CCSprite *townIconNormal = UIHelper::getSprite(UIHelper::BuildBuildingsIcon);
    townIconNormal->setPosition(townButtonNormal->getContentSize()*0.5f);
    townButtonNormal->addChild(townIconNormal);
    townIconNormal->setScaleX(1.0f / m_tabScale * m_tabNormalIconScale);
    townIconNormal->setScaleY(townIconNormal->getScaleY() * m_tabNormalIconScale);

    auto townButtonSelected = UIHelper::getScalableSpriteAsNormal(UIHelper::TabActive);
    townButtonSelected->setScaleX(m_tabScale);
    CCSprite *townIconSelected = UIHelper::getSprite(UIHelper::BuildBuildingsIcon);
    townIconSelected->setPosition(ccp(townButtonSelected->getContentSize().width*0.5f, townButtonSelected->getContentSize().height*0.5f));
    townButtonSelected->addChild(townIconSelected);
    townIconSelected->setScaleX(1.0f / m_tabScale);
//    townButtonSelected->setContentSize(ccp(townButtonSelected->getContentSize().width*m_tabScale, townButtonSelected->getContentSize().height));

    m_townMenuItem = CCMenuItemScaleSprite::create(townButtonNormal, this, menu_selector(ShopPanel::townShop));
    m_townMenuItem->setDisabledImage(townButtonSelected);
    m_townMenuItem->setEnabled(false);

    m_townMenu= CCMenu::create(m_townMenuItem, NULL);
    m_townMenu->setContentSize(CCSizeMake(m_townMenuItem->getContentSize().width*0.5f * m_townMenuItem->getScaleX(), m_townMenuItem->getContentSize().height*m_townMenuItem->getScaleY()));
    m_townMenu->setPosition(ccp(m_size610.width*0.45f, _screenSize.height - m_townMenu->getContentSize().height*1.25f));
    m_panelLayer->addChild(m_townMenu, 5);

    CCSprite *pineButtonNormal = UIHelper::getScalableSpriteAsNormal(UIHelper::TabInactive);
    pineButtonNormal->setScaleX(m_tabScale);
    CCSprite *pineIconNormal = UIHelper::getSprite(UIHelper::BuildCrTreeIcon);
    pineIconNormal->setPosition(pineButtonNormal->getContentSize()*0.5f);
    pineButtonNormal->addChild(pineIconNormal);
    pineIconNormal->setScaleX(1.0f / m_tabScale * m_tabNormalIconScale);
    pineIconNormal->setScaleY(pineIconNormal->getScaleY() * m_tabNormalIconScale);

    CCSprite *pineButtonSelected = UIHelper::getScalableSpriteAsNormal(UIHelper::TabActive);
    pineButtonSelected->setScaleX(m_tabScale);
    CCSprite *pineIconSelected = UIHelper::getSprite(UIHelper::BuildCrTreeIcon);
    pineIconSelected->setPosition(ccp(townButtonSelected->getContentSize().width*0.5f,townButtonSelected->getContentSize().height*0.5f));
    pineButtonSelected->addChild(pineIconSelected);
    pineIconSelected->setScaleX(1.0f / m_tabScale);

    //count new decoration
    m_badgeDecoreSprite = UIHelper::getSprite(UIHelper::HUDNewMarkerSmall);
    m_badgeDecoreSprite->setPosition(ccp(pineButtonNormal->getContentSize().width*0.8f, pineButtonNormal->getContentSize().height*0.8f));
    pineButtonNormal->addChild(m_badgeDecoreSprite);
    m_badgeDecoreLabel = CCLabelHelper::createBMFont("", kFontSizeMedium, false);
    m_badgeDecoreLabel->setPosition(m_badgeDecoreSprite->getContentSize()*0.5f);
    m_badgeDecoreSprite->addChild(m_badgeDecoreLabel);
    m_badgeDecoreSprite->setScaleX(1.0f / m_tabScale);

    m_pineMenuItem = CCMenuItemScaleSprite::create(pineButtonNormal, this, menu_selector(ShopPanel::pineShop));
    m_pineMenuItem->setDisabledImage(pineButtonSelected);

    m_pineMenu = CCMenu::create(m_pineMenuItem, NULL);
    m_pineMenu->setContentSize(CCSizeMake(m_pineMenuItem->getContentSize().width, m_townMenuItem->getContentSize().height));
    m_pineMenu->setPosition(ccp(m_size610.width*0.85f, _screenSize.height - m_pineMenu->getContentSize().height*0.85f));
    m_panelLayer->addChild(m_pineMenu, 0);

    m_clippingLayer = CCClippingLayer::create();
    m_clippingLayer->setAnchorPoint(ccp(0, 0));
    m_clippingLayer->preferenceRootParent = m_panelLayer;
    m_clippingLayer->setContentSize(CCSizeMake(m_size610.width*2.0f, m_townMenu->getPositionY() - m_townMenu->getContentSize().height*1.5f));
    m_panelLayer->addChild(m_clippingLayer, 4);

    m_scrollView = CCNikScrollView::create();
    m_scrollView->setBounce(true);
    m_scrollView->setTouchMode(kCCTouchesOneByOne);
    m_scrollView->setDelegate(this);
    m_scrollView->setContentSize(CCSizeMake(m_size610.width, m_townMenu->getPositionY() - m_townMenu->getContentSize().height - 20 * sizeMultiplier));
    m_clippingLayer->addChild(m_scrollView);

    CCNikScrollViewButton *lastButton;
    for (int i = 0; i < 10; i++)
    {
        CCNikScrollViewButton *button = CCNikScrollViewButton::create(UIHelper::getSprite(UIHelper::BuildBuildingsIcon), UIHelper::getSprite(UIHelper::BuildBuildingsIcon));
        button->setDelegate(m_scrollView);
        button->setPosition(ccp(m_scrollView->getContentSize().width*0.5f, button->getContentSize().height*0.5f + button->getContentSize().height*1.2f*i));
        m_scrollView->addChild(button);
        lastButton = button;
    }

    m_scrollView->setContentView(CCSizeMake(m_scrollView->getContentSize().width, lastButton->getPositionY() + lastButton->getContentSize().height*0.6f));
    // Default
    m_panelClosePos = CCPoint(-m_size610.width*1.2f, 0.0f);
    m_panelOpenPos = CCPoint(0.0f, 0.0f);
    m_panelLayer->setPosition(m_panelClosePos);

    m_itemBuildings.clear();
    for (vector<GDBuildingVO>::iterator it = g_gameContent->Building.begin(); it != g_gameContent->Building.end(); ++it) {
        GDBuildingVO _vo = *it;
        if (_vo.type == "" || !_vo.in_shop) {
            continue;
        }
        ShopItemBuilding *_item = ShopItemBuilding::createWithBuildingVO(_vo,
                                                                         CCSizeMake(m_scrollView->getContentSize().width,
                                                                                    350.0f*sizeMultiplier));
        _item->retain();
        _item->setDelegate(m_scrollView);
        _item->setTarget(this, menu_selector(ShopPanel::pressButton), CCNikScrollViewButtonStateTouchDown);
        _item->setTarget(this, menu_selector(ShopPanel::addBuildingToMapAndZoom), CCNikScrollViewButtonStateTouchUp);
        _item->setDragToOutOfBoundTarget(this, menu_selector(ShopPanel::dragToOutOfBoundScrollView));

        warning_msg = ShopItemBuilding::isAnyReasonCannotBuild(_vo);

        if (warning_msg == "") {
            m_itemBuildingsUnlock.push_back(_item);
        }else if(warning_msg == g_gameContent->Word["maximum_amount_built"]){
            m_itemBuildingsLockMaximum.push_back(_item);
        }else{
            m_itemBuildingsLockMansion.push_back(_item);
        }

    }

    for (vector<ShopItemBuilding*>::iterator it = m_itemBuildingsUnlock.begin(); it != m_itemBuildingsUnlock.end(); ++it) {
        ShopItemBuilding *_item =(ShopItemBuilding*)(*it);
         m_itemBuildings.push_back(_item);
    }
    for (vector<ShopItemBuilding*>::iterator it = m_itemBuildingsLockMansion.begin(); it != m_itemBuildingsLockMansion.end(); ++it) {
        ShopItemBuilding *_item =(ShopItemBuilding*)(*it);
        m_itemBuildings.push_back(_item);
    }
    for (vector<ShopItemBuilding*>::iterator it = m_itemBuildingsLockMaximum.begin(); it != m_itemBuildingsLockMaximum.end(); ++it) {
        ShopItemBuilding *_item =(ShopItemBuilding*)(*it);
        m_itemBuildings.push_back(_item);
    }

    m_itemDecorations.clear();
    for (vector<GDDecorationVO>::iterator it = g_gameContent->Decoration.begin(); it != g_gameContent->Decoration.end(); ++it) {
        GDDecorationVO _vo = *it;

        if (_vo.type == "" || _vo.in_shop == 0)
        {
            continue;
        }

        ShopItemDecoration *_item = ShopItemDecoration::createWithDecorationVO(_vo, CCSizeMake(m_scrollView->getContentSize().width, 350.0f*sizeMultiplier));
        _item->retain();
        _item->setDelegate(m_scrollView);
        _item->setTarget(this, menu_selector(ShopPanel::pressButton), CCNikScrollViewButtonStateTouchDown);
        _item->setTarget(this, menu_selector(ShopPanel::addDecorationToMap), CCNikScrollViewButtonStateTouchUp);
        _item->setDragToOutOfBoundTarget(this, menu_selector(ShopPanel::dragToOutOfBoundScrollView));
        m_itemDecorations.push_back(_item);
    }

    updatePanelContent(SIT_BUILDING);
    closePanel();

    setDecorationNew();
    scheduleOnce(schedule_selector(ShopPanel::scheduleItemCount), 1.0f);

    return true;
}

void ShopPanel::refreshDecoration()
{
    for (vector<ShopItemDecoration*>::iterator it = m_itemDecorations.begin(); it != m_itemDecorations.end(); ++it) {
        if (ShopItemDecoration *_item = (ShopItemDecoration*)(*it)) {
            _item->removeFromParentAndCleanup(true);
            _item->release();
            _item = NULL;
        }
    }
    m_itemDecorations.clear();

    for (vector<GDDecorationVO>::iterator it = g_gameContent->Decoration.begin(); it != g_gameContent->Decoration.end(); ++it)
    {
        GDDecorationVO _vo = *it;
        if (_vo.type == "" || _vo.in_shop == 0 || _vo.in_shop == 2)
        {
            continue;
        }

        ShopItemDecoration *_item = ShopItemDecoration::createWithDecorationVO(_vo, CCSizeMake(m_scrollView->getContentSize().width, 350.0f*sizeMultiplier));
        _item->retain();
        _item->setDelegate(m_scrollView);
        _item->setTarget(this, menu_selector(ShopPanel::pressButton), CCNikScrollViewButtonStateTouchDown);
        _item->setTarget(this, menu_selector(ShopPanel::addDecorationToMap), CCNikScrollViewButtonStateTouchUp);
        _item->setDragToOutOfBoundTarget(this, menu_selector(ShopPanel::dragToOutOfBoundScrollView));
        m_itemDecorations.push_back(_item);
    }
}

void ShopPanel::setItemCount()
{
    int n = 0;

    for (vector<GDBuildingVO>::iterator it = g_gameContent->Building.begin(); it != g_gameContent->Building.end(); ++it) {
        GDBuildingVO _vo = *it;
        if (_vo.type == "" || !_vo.in_shop) {
            continue;
        }
        warning_msg = ShopItemBuilding::isAnyReasonCannotBuild(_vo);
        if (warning_msg == "") {
            n++;
        }
    }

    CCLog_("building can build : %d", n);
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (_scene) {
        _scene->hudPanel->menuButtons[HUDPanelMenuShop]->setBadge(n);
    }

}

void ShopPanel::scheduleItemCount(float delta_) {

    setItemCount();

}

void ShopPanel::setDecorationNew() {

    int n = 0;

    string status = CCUserDefault::sharedUserDefault()->getStringForKey(KEY_STATUS_DECORATION_NEW);
    if(status.empty() || status == "true") {
        Building* _hq_mansion = g_playerState->getBuilding(kBuildingHQ, 0);
        if (_hq_mansion) {
            n = g_gameContent->getNDecorationVOJustUnlocksAtLevel(_hq_mansion->pbVo.level);
        }
    }

    CCLog_("Decoration can buy : %d", n);
    if (n > 0) {
        m_badgeDecoreSprite->setVisible(true);
        m_badgeDecoreLabel->setString(CCString::createWithFormat("%d",n)->getCString());
    }else{
        m_badgeDecoreSprite->setVisible(false);
    }
}


void ShopPanel::update(float delta) {

//    if (!g_sceneManager) {
//        CCLog_("ShopPanel::update - sceneManager is unitialized!");
//        return;
//    }
//
//    SceneVille* _scene = (SceneVille*)g_currentScene;
//    if (!_scene) {
//        CCLog_("(ShopPanel::update) Error: SceneMapCommand is uninitialized!\n");
//        return;
//    }
//
//    lockCamera = false;

//    if (g_sceneManager->phatTouches) {
//
//        int n = g_sceneManager->phatTouches->getTouchesActive();
//        if (n > 0) {
//
//            CCRect panelRect = CCRect(m_panelBg->getPosition().x,
//                                      m_panelBg->getPosition().y,
//                                      m_size610.width,
//                                      m_size610.height);
//
//            CCPoint location = g_sceneManager->phatTouches->getTouchPosition(0);
//
//            switch (g_sceneManager->phatTouches->getTouchState(0)) {
//
//                case TOUCH_MOVED: {
//
//                    if (panelRect.intersectsRect(CCRect(location.x, location.y, 1, 1))) {
//
//                        if (m_menuToggleShopOpen) {
//
//                            lockCamera = true;
//
//                            if (itemPressed) {
//
//                                float _diffSwipeX = location.x - m_touchBeganPos.x;
//                                float _diffSwipeY = location.y - m_touchBeganPos.y;
//
//                                float _swipeDist = m_size610.width*0.15;
//
//                                if (_diffSwipeX>_swipeDist && (fabs(_diffSwipeX) > fabs(_diffSwipeY))) {
//
//                                    switch (m_type) {
//                                        default:
//                                            break;
//                                        case SIT_BUILDING: {
//                                            addBuildingToMap((CCObject*)graspItem, false);
//                                            graspItem = NULL;
//                                        }break;
//                                        case SIT_DECORATION: {
//                                            addDecorationToMap((CCObject*)graspItem);
//                                            graspItem = NULL;
//                                        }break;
//                                    }
//
//                                } // End check swipe
//
//                            }
//
//                        }
//                    }
//
//                }break;
//
//                case TOUCH_BEGAN: {
//
//
//                }break;
//
//                case TOUCH_ENDED:
//                case TOUCH_CANCELLED:
//                default: {
//                    m_touchBeganPos = location;
//                    itemPressed = false;
//
//                }break;
//            }
//
//
//        }
//
//    }

}

//bool ShopPanel::ccTouchBegan(CCTouch* touch, CCEvent* event) {
//    
//    CCPoint location = m_panelBg->getParent()->convertTouchToNodeSpace(touch);
//    
//    auto bb1 = m_townMenu->boundingBox();
//    auto bb2 = m_pineMenu->boundingBox();
//    
//    //    CCRect rect = m_panelBg->boundingBox() + CCRectMake(0,0,0, bb2.size.height);
//    CCRect rect = CCRectMake(m_panelBg->boundingBox().origin.x,
//                             m_panelBg->boundingBox().origin.y,
//                             m_panelBg->boundingBox().size.width,
//                             m_panelBg->boundingBox().size.height
//                             + bb2.size.height);
//    
//    if (rect.containsPoint(location) || bb1.containsPoint(location) || bb2.containsPoint(location))
//    {
//        return true;
//    }
//    
//    CCLog_("m_manuallyCloseShopPanelEnabled:%d",m_manuallyCloseShopPanelEnabled);
//    
//    if (m_manuallyCloseShopPanelEnabled)
//        closePanel();
//    
//    return false;
//}

void ShopPanel::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent) {
    
    if (pTouches->count() <= 0) return;
    
//    CCPoint location = ((CCTouch*)(*pTouches->begin()))->getLocation();
//    location = this->convertToNodeSpace(location);
    CCPoint location = m_panelBg->getParent()->convertTouchToNodeSpace((CCTouch*)(*pTouches->begin()));

    
//    CCRect panelRect = CCRect(m_panelBg->getPosition().x,
//                              m_panelBg->getPosition().y,
//                              m_size610.width,
//                              m_size610.height);
    
    auto bb1 = m_townMenu->boundingBox();
    auto bb2 = m_pineMenu->boundingBox();

    //    CCRect rect = m_panelBg->boundingBox() + CCRectMake(0,0,0, bb2.size.height);
    CCRect rect = CCRectMake(m_panelBg->boundingBox().origin.x,
                             m_panelBg->boundingBox().origin.y,
                             m_panelBg->boundingBox().size.width,
                             m_panelBg->boundingBox().size.height
                             + bb2.size.height);

    if (rect.containsPoint(location) || bb1.containsPoint(location) || bb2.containsPoint(location)) {
        return;
    }

//    CCLog_("m_manuallyCloseShopPanelEnabled:%d",m_manuallyCloseShopPanelEnabled);

    if (m_manuallyCloseShopPanelEnabled)
        closePanel();
    
}

void ShopPanel::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent) {
    
    if (pTouches->count() <= 0) return;
    
//    CCPoint location = ((CCTouch*)(*pTouches->begin()))->getLocation();
//    location = this->convertToNodeSpace(location);
    CCPoint location = m_panelBg->getParent()->convertTouchToNodeSpace((CCTouch*)(*pTouches->begin()));
    
    CCRect panelRect = CCRect(m_panelBg->getPosition().x,
                              m_panelBg->getPosition().y,
                              m_size610.width,
                              m_size610.height);
    
    if (panelRect.intersectsRect(CCRect(location.x, location.y, 1, 1))) {
        
        if (m_menuToggleShopOpen) {
            
            lockCamera = true;
            
            if (itemPressed) {
                
                float _diffSwipeX = location.x - m_touchBeganPos.x;
                float _diffSwipeY = location.y - m_touchBeganPos.y;
                
                float _swipeDist = m_size610.width*0.15;
                
                if (_diffSwipeX>_swipeDist && (fabs(_diffSwipeX) > fabs(_diffSwipeY))) {
                    
                    switch (m_type) {
                        default:
                            break;
                        case SIT_BUILDING: {
                            addBuildingToMap((CCObject*)graspItem, false);
                            graspItem = NULL;
                        }break;
                        case SIT_DECORATION: {
                            addDecorationToMap((CCObject*)graspItem);
                            graspItem = NULL;
                        }break;
                    }
                    
                } // End check swipe
                
            }
            
        }
    }
    
}

void ShopPanel::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent) {
    
    _touchEnded(pTouches, pEvent);
    
}

void ShopPanel::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent) {
    
    _touchEnded(pTouches, pEvent);
    
}

void ShopPanel::_touchEnded(CCSet *pTouches, CCEvent *pEvent) {
    
    if (pTouches->count() <= 0) return;
    
//    CCPoint location = ((CCTouch*)(*pTouches->begin()))->getLocation();
//    location = this->convertToNodeSpace(location);
    CCPoint location = m_panelBg->getParent()->convertTouchToNodeSpace((CCTouch*)(*pTouches->begin()));
    
    m_touchBeganPos = location;
    itemPressed = false;
    
}

ShopPanel::~ShopPanel() {

    for (vector<ShopItemBuilding*>::iterator it = m_itemBuildings.begin(); it != m_itemBuildings.end(); ++it) {
        if (ShopItemBuilding *_item = (ShopItemBuilding*)(*it)) {
            _item->removeFromParentAndCleanup(true);
            _item->release();
            _item = NULL;
        }
    }
    m_itemBuildings.clear();

    for (vector<ShopItemDecoration*>::iterator it = m_itemDecorations.begin(); it != m_itemDecorations.end(); ++it) {
        if (ShopItemDecoration *_item = (ShopItemDecoration*)(*it)) {
            _item->removeFromParentAndCleanup(true);
            _item->release();
            _item = NULL;
        }
    }
    m_itemDecorations.clear();

}

void ShopPanel::removeCCTouchDelegateFromScrollView() {
    
    if (!m_scrollView) return;
    m_scrollView->setCCTouchDelegate(NULL);
    
}

void ShopPanel::openPanel() {

    if (!m_menuToggleShopOpen) {
        m_menuToggleShopOpen = true;
        m_panelLayer->setVisible(true);
        updatePanelContent(m_type);
        if (CCDirector::sharedDirector()->getActionManager()->numberOfRunningActionsInTarget(m_panelLayer) >= 1) {
            CCDirector::sharedDirector()->getActionManager()->removeAction(m_panelLayerSlideAction);
        }
        m_panelLayerSlideAction = CCEaseElasticOut::create(CCMoveTo::create(0.7f, m_panelOpenPos), 0.45f);
        m_panelLayer->runAction(m_panelLayerSlideAction);
        m_scrollView->setScrollEnabled(true);
    }
}

void ShopPanel::closePanel() {

    closePanel(true);
}

void ShopPanel::closePanel(bool callback_) {

    if (m_menuToggleShopOpen) {

        m_menuToggleShopOpen = false;

        if (CCDirector::sharedDirector()->getActionManager()->numberOfRunningActionsInTarget(m_panelLayer) >= 1) {
            CCDirector::sharedDirector()->getActionManager()->removeAction(m_panelLayerSlideAction);
        }
        m_panelLayerSlideAction = CCSequence::create(CCEaseElasticOut::create(CCMoveTo::create(1.0f, m_panelClosePos), 0.6f),
                                                     CCCallFunc::create(this, callfunc_selector(ShopPanel::doneClosePanel)), NULL);
        m_panelLayer->runAction(m_panelLayerSlideAction);

        if (callback_ && onClose) {
            onClose(m_sender);
        }

        if (onCloseForTut) {
            onCloseForTut(senderForTut);
        }

    }

}

void ShopPanel::doneClosePanel() {

    m_panelLayer->setVisible(false);
    unscheduleUpdate();

}

bool ShopPanel::isPanelOpen() {

    return m_menuToggleShopOpen;

}

void ShopPanel::updatePanelContent(ShopItemType type) {

    CCSize _itemSize = CCSizeZero;
    float _margin = 0.0f;
    CCRect _bound = CCRectZero;
    _bound.size.height = m_scrollView->getContentSize().height;
    int i = 0;

    m_scrollView->removeAllChildren();
    m_type = type;

    switch (m_type) {

        default:
        case SIT_BUILDING: {
            int count = (int)m_itemBuildings.size();
            for (vector<ShopItemBuilding*>::iterator it = m_itemBuildings.begin(); it != m_itemBuildings.end(); ++it) {
                if (ShopItemBuilding *_item = (ShopItemBuilding*)(*it)) {


                    if (i==0) {
                        _margin = 0;
                        _itemSize.width = _item->getContentSize().width;
                        _itemSize.height = _item->getContentSize().height;

                        m_itemBuildingsUnlock.clear();
                        m_itemBuildingsLockMaximum.clear();
                        m_itemBuildingsLockMansion.clear();
                    }
                    _item->setPosition(CCPoint(0, (_itemSize.height + _margin)*(count - i - 1)));
                    _item->updateState();

                    warning_msg = ShopItemBuilding::isAnyReasonCannotBuild(_item->vo);
                    if (warning_msg == "") {
                        m_itemBuildingsUnlock.push_back(_item);
                    }else if(warning_msg == g_gameContent->Word["maximum_amount_built"]){
                        m_itemBuildingsLockMaximum.push_back(_item);
                    }else{
                        m_itemBuildingsLockMansion.push_back(_item);
                    }

                    i++;
                }

            }
            i=0;
            for (vector<ShopItemBuilding*>::iterator it = m_itemBuildingsUnlock.begin(); it != m_itemBuildingsUnlock.end(); ++it) {
                ShopItemBuilding *_item =(ShopItemBuilding*)(*it);
                _item->setPosition(CCPoint(0, (_itemSize.height + _margin)*(count - i - 1))); i++;
                m_scrollView->addChild(_item);
            }
            for (vector<ShopItemBuilding*>::iterator it = m_itemBuildingsLockMansion.begin(); it != m_itemBuildingsLockMansion.end(); ++it) {
                ShopItemBuilding *_item =(ShopItemBuilding*)(*it);
                _item->setPosition(CCPoint(0, (_itemSize.height + _margin)*(count - i - 1))); i++;
                m_scrollView->addChild(_item);
            }
            for (vector<ShopItemBuilding*>::iterator it = m_itemBuildingsLockMaximum.begin(); it != m_itemBuildingsLockMaximum.end(); ++it) {
                ShopItemBuilding *_item =(ShopItemBuilding*)(*it);
                _item->setPosition(CCPoint(0, (_itemSize.height + _margin)*(count - i - 1))); i++;
                m_scrollView->addChild(_item);
            }


            m_scrollView->setContentView(CCSizeMake(m_scrollView->getContentSize().width, (_itemSize.height + _margin)*count + _margin));

        }break;

        case SIT_DECORATION: {
            int count = 0;
            for (auto &item : m_itemDecorations)
            {
                if (!(item->vo.in_shop == 2 && g_playerState->getNKeptDecoration(item->vo.type) == 0))
                {
                    count ++;
                }
            }

            for (vector<ShopItemDecoration*>::iterator it = m_itemDecorations.begin(); it != m_itemDecorations.end(); ++it) {
                if (ShopItemDecoration *_item = (ShopItemDecoration*)(*it)) {

                    // in case of special (christmas) decoration we allow only kept one
                    if (_item->vo.in_shop == 2 && g_playerState->getNKeptDecoration(_item->vo.type) == 0)
                    {
                        continue;
                    }

                    if (i==0) {
                        _margin = 0;
                        _itemSize.width = _item->getContentSize().width;
                        _itemSize.height = _item->getContentSize().height;
                    }
                    _item->setPosition(CCPoint(0, (_itemSize.height + _margin)*(count - i - 1)));
                    _item->updateState();
                    m_scrollView->addChild(_item);
                    i++;
                }
            }

            m_scrollView->setContentView(CCSizeMake(m_scrollView->getContentSize().width, (_itemSize.height + _margin)*count + _margin));

        }break;

    }

    m_scrollView->setContentOffset(ccp(0, m_scrollView->getContentView().height - m_scrollView->getContentSize().height));
    m_scrollView->resetScrollEnabled(true);
}

//void ShopPanel::hideShopBtn() {
//
//    if (m_didShowShopBtn) {
//        m_didShowShopBtn = false;
////        btnShopMenu->stopAllActions();
////        btnShopMenu->runAction(CCEaseElasticOut::create(CCMoveTo::create(PLACEHOLDER_MENU_MOVING_SPEED, m_menuToggleShopHidePos), PLACEHOLDER_MENU_EASING_SPEED));
//    }
//
//}
//
//void ShopPanel::showShopBtn(float delayTime_) {
//
//    if (!m_didShowShopBtn) {
//        m_didShowShopBtn = true;
//        setItemCount();
////        CCLog_("shop pos = %f,%f ---> %f,%f (%f)", btnShopMenu->getPositionX(), btnShopMenu->getPositionY(), m_menuToggleShopClosePos.x, m_menuToggleShopClosePos.y, delayTime_);
////        btnShopMenu->stopAllActions();
////        btnShopMenu->runAction(CCSequence::create(CCDelayTime::create(delayTime_), CCEaseElasticOut::create(CCMoveTo::create(PLACEHOLDER_MENU_MOVING_SPEED, m_menuToggleShopClosePos), PLACEHOLDER_MENU_EASING_SPEED), NULL));
//    }
//
//}

void ShopPanel::switchShopToBuilding(CCObject *sender) {

    m_menuBuildingShop->setZOrder(3);
    m_menuTrackShop->setZOrder(1);
    m_menuDecoShop->setZOrder(0);

    m_buttonBuildingShop->runAction(CCSequence::create(CCScaleTo::create(0.05f, 1.3f),
                                                       CCScaleTo::create(0.05f, 1.0f),
                                                       CCScaleTo::create(0.1f, 1.15f),
                                                       CCScaleTo::create(0.1f, 1.0f),
                                                       NULL));
    updatePanelContent(SIT_BUILDING);
    
}

void ShopPanel::switchShopToTrack(CCObject *sender) {

    m_menuBuildingShop->setZOrder(0);
    m_menuTrackShop->setZOrder(3);
    m_menuDecoShop->setZOrder(1);

    m_buttonTrackShop->runAction(CCSequence::create(CCScaleTo::create(0.05f, 1.3f),
                                                    CCScaleTo::create(0.05f, 1.0f),
                                                    CCScaleTo::create(0.1f, 1.15f),
                                                    CCScaleTo::create(0.1f, 1.0f),
                                                    NULL));
    updatePanelContent(SIT_TRACK);

}

void ShopPanel::switchShopToDecoration(CCObject *sender) {

    m_menuBuildingShop->setZOrder(0);
    m_menuTrackShop->setZOrder(1);
    m_menuDecoShop->setZOrder(3);

    m_buttonDecoShop->runAction(CCSequence::create(CCScaleTo::create(0.05f, 1.3f),
                                                   CCScaleTo::create(0.05f, 1.0f),
                                                   CCScaleTo::create(0.1f, 1.15f),
                                                   CCScaleTo::create(0.1f, 1.0f),
                                                   NULL));

    updatePanelContent(SIT_DECORATION);

}

void ShopPanel::addDecorationToMap(void *inst, void *sender) {

    if (ShopItemDecoration* item = (ShopItemDecoration*)inst) {

        item->setScale(1.0f);
        if (ShopPanel *_this = (ShopPanel*)sender) {

            if (_this->itemPressed) {
                if (SceneVille* _scene = (SceneVille*)g_currentScene) {
                    ((IsoMapBuilding*)_scene->isoMap)->addDecorationOnPlaceHolder(item->vo);
                }
            }

        }

    }

}


#pragma mark Callback functions

void ShopPanel::setSender(void *sender_) {

    m_sender = sender_;

}

void ShopPanel::townShop(cocos2d::CCObject *sender) {

    if (!m_dangerButtonsEnabled) return;

    m_townMenuItem->setEnabled(false);
    m_townMenu->setZOrder(5);
    m_pineMenuItem->setEnabled(true);
    m_pineMenu->setZOrder(0);

    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    m_pineMenu->setPosition(ccp(m_size610.width*0.85f, _screenSize.height - m_pineMenu->getContentSize().height*0.85f));
    m_townMenu->setPosition(ccp(m_size610.width*0.45f, _screenSize.height - m_townMenu->getContentSize().height*1.25f));

    updatePanelContent(SIT_BUILDING);
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
}

void ShopPanel::pineShop(cocos2d::CCObject *sender) {

    if (!m_dangerButtonsEnabled) return;

    m_townMenuItem->setEnabled(true);
    m_townMenu->setZOrder(0);
    m_pineMenuItem->setEnabled(false);
    m_pineMenu->setZOrder(5);

    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    m_pineMenu->setPosition(ccp(m_size610.width*0.85f, _screenSize.height - m_pineMenu->getContentSize().height*1.25f));
    m_townMenu->setPosition(ccp(m_size610.width*0.45f, _screenSize.height - m_townMenu->getContentSize().height*0.85f));

    updatePanelContent(SIT_DECORATION);
    CCUserDefault::sharedUserDefault()->setStringForKey(KEY_STATUS_DECORATION_NEW,"false");
    m_badgeDecoreSprite->setVisible(false);
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
}

void ShopPanel::dragToOutOfBoundScrollView(cocos2d::CCObject *sender)
{
    if (!graspItem) return;
    
//    g_sceneManager->phatTouches->enable();
    
    if (m_menuToggleShopOpen) {
        lockCamera = true;
    }
    
    if (!itemPressed) {
        itemPressed = true;
    }
    
    if (m_itemTypeCanBeGrabbed != "") {
        switch (m_type) {
            default:
                break;
            case SIT_BUILDING: {
                if (m_itemTypeCanBeGrabbed != ((ShopItemBuilding*)graspItem)->vo.type) return;
            }break;
            case SIT_TRACK: {
            }break;
            case SIT_DECORATION: {
                if (m_itemTypeCanBeGrabbed != ((ShopItemDecoration*)graspItem)->vo.type) return;
            }break;
        }
    }

    if (onBuildingAddedForTut) {
        onBuildingAddedForTut(senderForTut, graspItem);
    }
    
    switch (m_type) {
        default:
            break;
        case SIT_BUILDING: {
            addBuildingToMap((CCObject*)graspItem, false);
            graspItem = NULL;
        }break;
        case SIT_DECORATION: {
            addDecorationToMap((CCObject*)graspItem);
            graspItem = NULL;
        }break;
    }
    

    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
//    ShopItemBuilding *item = (ShopItemBuilding *)sender;
//    _isoMapBuilding->lockCamera(_isoMapBuilding);
    if (_isoMapBuilding->placeHolder) {
        m_scrollView->setCCTouchDelegate(_isoMapBuilding->placeHolder);
    }
    
//    closePanel();
    
}

void ShopPanel::pressButton(cocos2d::CCObject *sender) {
    
    ShopItemBuilding *item = (ShopItemBuilding *)sender;
    graspItem = item;
    
//    CCLog_("Press Button : %s",item->vo.building_type.c_str());    
}


void ShopPanel::addBuildingToMapAndZoom(cocos2d::CCObject *sender) {
    
    addBuildingToMap(sender, m_zoomWhenAddBuildings);
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*_scene) != typeid(SceneVille)) return;
    IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
    if (!_isoMapBuilding ||
        typeid(*_isoMapBuilding) != typeid(IsoMapBuilding) ||
        !_isoMapBuilding->placeHolder) return;
    
    CCPoint autoPos = _isoMapBuilding->getPositionOfAvailableSpaceFor(((ShopItemBuilding*)graspItem)->vo);
    if (autoPos.x < 0.0f || autoPos.y < 0.0f) return;
    _isoMapBuilding->placeHolder->setPosition(autoPos);
    
    //Pan an Zoom to the building by
    _isoMapBuilding->placeHolder->done();
    
}

void ShopPanel::addBuildingToMap(CCObject *sender, bool zoom) {
    
    if(sender == NULL) {
        return;
    }
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
    ShopItemBuilding *item = (ShopItemBuilding *)sender;
    graspItem = item;
    
    if (m_itemTypeCanBeGrabbed != "") {
        if (m_itemTypeCanBeGrabbed != item->vo.type) {
            return;
        }
    }
    
    if (onBuildingAddedForTut) {
        onBuildingAddedForTut(senderForTut, graspItem);
    }
    
#ifdef EDIT_MODE
    _isoMapBuilding->addBuildingWithPlaceHolder(item->vo, false);
#else
    if (item->warning_msg != "") {
        g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, item->warning_msg);
        closePanel();
    }else {
        _isoMapBuilding->addBuildingWithPlaceHolder(item->vo);
        
        if (zoom) {
            SceneVille *_scene = (SceneVille*)g_currentScene;
            if (_scene->scrollView) {
                _scene->scrollView->zoomOnPosition(_isoMapBuilding->placeHolder->getPosition()
                                                     -_scene->isoMap->getContentSize()/4.f
                                                     +ccp(0,item->vo.src_height*sizeMultiplier*0.4f)
                                                     ,1.0f
                                                     ,1.0f);
            }
        }
    }
#endif
    
}

void ShopPanel::addDecorationToMap(cocos2d::CCObject *sender) {
    
    if(!sender){
        return;
    }

    SceneVille* _scene = (SceneVille*)g_currentScene;
    IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
    ShopItemDecoration* item = (ShopItemDecoration*)sender;
    graspItem = item;
    
    if (m_itemTypeCanBeGrabbed != "") {
        if (m_itemTypeCanBeGrabbed != item->vo.type) {
            return;
        }
    }
    
#ifdef EDIT_MODE
    _isoMapBuilding->addDecorationOnPlaceHolder(item->vo, false);
#else
    if (item->warning_msg != "") {
        g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, item->warning_msg);
    }else {
        if (Decoration* _existing_decoration = g_playerState->getKeptDecoration(item->vo.type)) {
            _isoMapBuilding->addDecorationOnPlaceHolder(_existing_decoration);
        }else {
            _isoMapBuilding->addDecorationOnPlaceHolder(item->vo);
        }
    }
#endif
    
}

void ShopPanel::scrollViewTouchBegan(CCNikScrollView *view) {
    
//    g_sceneManager->phatTouches->disable();
    
}

void ShopPanel::scrollViewWillBeginDragging(CCNikScrollView *view) {
    
//    CCLog_("begin drag");
    
}

void ShopPanel::scrollViewDidEndDragging(CCNikScrollView *view, bool decelerate) {
    
//    CCLog_("stop drag");
    
}
