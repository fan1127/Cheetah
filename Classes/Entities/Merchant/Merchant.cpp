//
//  Merchant.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 6/20/2557 BE.
//
//

#include "Merchant.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIMerchantItemBuy.h"

#include "../../Helpers/AnalyticsClientWrapper.h"
#include "../../Helpers/FlurryCpp.h"
#include "../../Helpers/StringHelper.h"

#include "../../AppMacros.h"
extern TargetDevice targetDevice;
extern float sizeMultiplier;
extern float retinaMultiplier;
extern unsigned int currentTime;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

bool Merchant::isIntroduced() {
    
    return g_playerState->player.racing.level >= g_gameContent->GameConfig["TRADER_GUY_APPEAR_AT_HQ_LEVEL"];
    
}

int Merchant::getTimeSinceLastAppeared() {
    
    return currentTime - g_playerState->player.info.merchant_timestamp;
    
}

int Merchant::getTimeMerchantWillAppear() {
    
    unsigned int merchant_cache_timestamp =
    (unsigned int)atol(CCUserDefault::sharedUserDefault()->getStringForKey(kMerchantItemCacheTimeKey,
                                                                           "0").c_str());
    
    if (merchant_cache_timestamp == 0) {
        return 10;
    }
    
    return (merchant_cache_timestamp +
            g_gameContent->GameConfig["TRADER_GUY_APPEAR_INTERVAL"] -
            currentTime);
    
}

bool Merchant::check() {
    
    if (Merchant::getTimeSinceLastAppeared() > g_gameContent->GameConfig["TRADER_GUY_APPEAR_INTERVAL"] &&
        Merchant::isIntroduced()) {
        return true;
    }
    return false;
    
}

Merchant* Merchant::createWithIsoMap(IsoMap *isoMap_, CTutManager *tutMgr_) {
    
    if (Merchant::check()) {
        if (Merchant* pRet = new Merchant()) {
            if (pRet->initWithIsoMap(isoMap_, tutMgr_)) {
                pRet->autorelease();
                return pRet;
            }
            CC_SAFE_DELETE(pRet);
        }
    }
    return NULL;

}

bool Merchant::initWithIsoMap(IsoMap *isoMap_, CTutManager *tutMgr_) {
    
    if (!CCNode::init()) {
        return false;
    }
    
    m_itemChoose = NULL;
    
    isoMap = isoMap_;
    tutMgr = tutMgr_;
    
    spine = g_sceneManager->sharedSkeletonData->getSkeleton(kCharacterSrcDir, kMerchantCharacterFilename);
    spine->setAnimation(kMerchantAniIdle, true);
    spine->setPosition(IsoMap::getPositionByMapIndex(kMerchantStandAdTileIndex, g_gameContent->tileSize, g_gameContent->tileShift));
    spine->setScale(2.0f * retinaMultiplier);
    isoMap->addChild(spine, isoMap->getContentSize().height - spine->getPositionY());
    
    bubble = CCPhatTouchableSprite::createWithSpriteFrameName(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsBubbleEmotion)), HUD_MENU_TOUCH_PRIORITY+1);
    bubble->onHit = Merchant::onBubbleTouched;
    bubble->rootNode = isoMap->getParent();
    bubble->setSender(this);
    addObjectInBubble();
    bubble->setPosition(spine->getPosition() + ccp(bubble->getContentSize().width*0.75f, bubble->getContentSize().height));
    isoMap->addChild(bubble, isoMap->getContentSize().height);
    
    bubble->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(1.0f, ccp(0.0f, 10.0f*sizeMultiplier)),
                                                                 CCMoveBy::create(1.0f, ccp(0.0f,-10.0f*sizeMultiplier)),
                                                                 NULL)));
    return true;
    
}
void Merchant::addObjectInBubble()
{
    if (g_playerState->player.info.merchant_items == "") {
        CCLabelBMFont* _label = CCLabelHelper::createBMFont("New!", StyleMiniDialogTitle);
        _label->setPosition(bubble->getContentSize()*0.5f);
        _label->setTag(1);
        bubble->addChild(_label);
        
    }else {
        CCSprite* _icon = UIHelper::getSprite(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsMerchantBuyIcon));
        _icon->setPosition(bubble->getContentSize()*0.5f);
        _icon->setTag(1);
        bubble->addChild(_icon);
    }
}
void Merchant::removeObjectInBubble()
{
    bubble->removeChildByTag(1);
}
bool Merchant::isTouched(int tileIndex_) {
    
    int tiles[] = kMerchantTouchTileIndexes;
    int n = sizeof(tiles)/sizeof(int);
    for (int i = 0; i < n; i++) {
        if (tiles[i] == tileIndex_) {
            return true;
        }
    }
    return false;
    
}
void Merchant::merchantLeave() {
    
    if (bubble) {
        bubble->removeFromParent();
        bubble = NULL;
    }
    
    if (spine) {
        spine->setScaleX(-1);
        CCPoint _start = spine->getPosition();
        CCPoint _goal = IsoMap::getPositionByMapIndex(kMerchantGohomeTileIndex, g_gameContent->tileSize, g_gameContent->tileShift);
        float _distance = _start.getDistance(_goal);
        spine->setAnimation(kMerchantAniWalk, true);
        spine->runAction(CCSequence::create(CCMoveTo::create((_distance/kMerchantWalkSpeed)*(1+int(targetDevice==TD_IPAD_RETINA)), _goal), CCRemoveSelf::create(), NULL));
    }
    
    m_goHome = true;
    
}

void Merchant::onBubbleTouched(void *inst_, void *sender_) {
    
    Merchant* _this = (Merchant*)sender_;
    if (_this->m_itemChoose) return;
    
    _this->m_itemChoose = MerchantItemChoose::create();
    _this->m_itemChoose->sender = _this;
    _this->m_itemChoose->onMerchantItemChosen = Merchant::onMerchantItemChosen;
    _this->tutMgr->addTutToFront(_this->m_itemChoose);
    
    g_gameAudio->playEffect(AUDIO_MERCHANT_OPEN);
    
}

void Merchant::onMerchantItemChosen(void *sender_, std::string result_) {
    
    Merchant* _this = (Merchant*)sender_;
    _this->m_itemChoose = NULL;
    
    _this->removeObjectInBubble();
    _this->addObjectInBubble();
    
    //this line should stand after m_itemChoose = NULL;
    if (result_ == "") return;
    
    if (typeid(*g_currentScene) != typeid(SceneVille)) return;
    SceneVille* _scene = (SceneVille*)g_currentScene;

    Building *building = g_playerState->getBuilding(kBuildingItem, 0);
    if (!building) return;
    
    int select_item_at_index = StringHelper::toInt(&result_[0]);
    result_.erase(result_.begin()); //elase index
    result_.erase(result_.begin()); //elase :
    
    GDItemVO _item_vo = g_gameContent->getItemVO(result_);
    if (_item_vo.type == "") return;
    
    bool itemCostGems = (select_item_at_index > 0);
    
    CSJson::Value eventParameters;
    eventParameters["type"] = _item_vo.type;
    
    if (itemCostGems) {
        g_playerState->player.balance.gem -= _item_vo.gem_value;
        
        eventParameters["gem"] = _item_vo.gem_value;
        AnalyticsClient::getInstance()->logEvent(EVENT_GEM_MERCHANT_BUY_ITEM, eventParameters);
    }else {
        g_playerState->player.balance.gold -= _item_vo.buy_value;
        
        eventParameters["coin"] = _item_vo.buy_value;
        AnalyticsClient::getInstance()->logEvent(EVENT_COIN_MERCHANT_BUY_ITEM, eventParameters);
    }
    
    _scene->hudPanel->updateAllResourceBars();
    _scene->scrollView->zoomOnPosition(building->getPosition() - _scene->isoMap->getContentSize()/4.f +
                                       ccp(0, building->bVo.src_height*sizeMultiplier*0.5f), 1.3f, 1.0f);
    
    vector<GDItemVO> _gotItems;
    _gotItems.push_back(_item_vo);
    building->addItemToBarn(_gotItems);
    _scene->hudPanel->checkTrade();
 
    if (PlayerState::getInstance()->preBuyItemFromMerchant(_item_vo.type)) {
        BackendService::getInstance()->send(APIMerchantItemBuy::create(_item_vo.type,
                                                                       _this,
                                                                       Merchant::onMerchantItemBought));
        if(!PlayerState::getInstance()->merchantHasItemForSell())
        {
            _this->merchantLeave();
        }
    }
}

void Merchant::onMerchantItemBought(void *sender, std::string resp_str) {
    
    PlayerState* _playerState = PlayerState::getInstance();
    
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseItemData(_root);
    _playerState->parseUserState(_root);
    _playerState->parseUserBalance(_root);
    Json_dispose(_root);
    
    CScene *_scene = (CScene*)g_currentScene;
    _scene->hudPanel->updateAllResourceBars();
    
}
