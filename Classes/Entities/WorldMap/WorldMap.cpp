//
//  WorldMap.cpp
//  Cheetah
//
//  Created by Teerapat on 11/3/57 BE.
//
//

#include "WorldMap.h"
using namespace std;

#include "../../AppMacros.h"
extern TargetDevice targetDevice;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIMapCloudOpen.h"
#include "../../Services/APIs/APIMapOpponentNew.h"

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../../Scenes/SceneWorldMap.h"
extern CCScene* g_currentScene;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;
extern PlayerState* g_opponentState;

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../UIs/BlockingLayer.h"

#include "../../Helpers/CCStringHelper.h"

#include <spine/Json.h>

#include "../../Extensions/CCNikButton.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Extensions/checkCollision/CollisionDetection.h"
#include "../../Helpers/CCStringHelper.h"

#include "../../UIs/Dialogs/LogDialog.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../Entities/Pet.h"
#include "../../UIs/Elements/AlertDialog.h"

#include "../../Helpers/CustomEventsHelper.h"
#include "../../UIs/BlockingLayer.h"
#include "../../user.h"


int indexVisit;
extern float retinaMultiplier;
extern float sizeMultiplier;

extern unsigned int currentTime;

const int maxCloud = 729;
const int layerCloud = 15;

WorldMap* WorldMap::create() {
    
    if (WorldMap *pRet = new WorldMap()) {
        if (pRet->init()) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool WorldMap::init() {
    
    if (!CCLayer::init()) {
        return false;
    }
    
    tagChange = 0;
    
    unlockVillTable = new int[MAX_UNLOCK_VILL_TABLE];
    
    if(_touch_sprites != NULL) {
        _touch_sprites->removeAllObjects();
        CC_SAFE_DELETE(_touch_sprites);
    }
    _touch_sprites = new CCArray;
    _selected_village = NULL;

    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    this->setContentSize(screenSize);
    
    _mapbatchNode = NULL;
    switch (targetDevice) {
        case TD_IPHONE:         _mapbatchNode = CCSpriteBatchNode::create("common/world/medium/worldmap-medium.png", 1);    break;
        case TD_IPHONE_RETINA:  _mapbatchNode = CCSpriteBatchNode::create("common/world/medium/worldmap-medium.png", 1);    break;
        case TD_IPAD:           _mapbatchNode = CCSpriteBatchNode::create("common/world/medium/worldmap-medium.png", 1);    break;
        case TD_IPAD_RETINA:    _mapbatchNode = CCSpriteBatchNode::create("common/world/large/worldmap-large.png", 1);     break;
    }
    _mapbatchNode->setContentSize(CCSize(7000.0f*retinaMultiplier, 7000.0f*retinaMultiplier));
    
    _cloudbatchNode = NULL;
    switch (targetDevice) {
        case TD_IPHONE:         _cloudbatchNode = CCSpriteBatchNode::create("common/world/medium/worldmap-medium.png", 1);   break;
        case TD_IPHONE_RETINA:  _cloudbatchNode = CCSpriteBatchNode::create("common/world/medium/worldmap-medium.png", 1);   break;
        case TD_IPAD:           _cloudbatchNode = CCSpriteBatchNode::create("common/world/medium/worldmap-medium.png", 1);   break;
        case TD_IPAD_RETINA:    _cloudbatchNode = CCSpriteBatchNode::create("common/world/large/worldmap-large.png", 1);   break;
    }
    _cloudbatchNode->setContentSize(_mapbatchNode->getContentSize());
    
    _avatarbatchNode = NULL;
    switch (targetDevice) {
        case TD_IPHONE:         _avatarbatchNode = CCSpriteBatchNode::create("common/world/medium/worldmap-medium.png", 1);   break;
        case TD_IPHONE_RETINA:  _avatarbatchNode = CCSpriteBatchNode::create("common/world/medium/worldmap-medium.png", 1);   break;
        case TD_IPAD:           _avatarbatchNode = CCSpriteBatchNode::create("common/world/medium/worldmap-medium.png", 1);   break;
        case TD_IPAD_RETINA:    _avatarbatchNode = CCSpriteBatchNode::create("common/world/large/worldmap-large.png", 1);   break;
    }
    _avatarbatchNode->setContentSize(_mapbatchNode->getContentSize());
    
    unlockLayer = CCLayer::create();
    newLayer = CCLayer::create();
    treasureLayer = CCLayer::create();
    
    _scrollView = CCNikScrollView::create();
    _scrollView->setZoomingEnabled(true);
    _scrollView->setDelegate(this);
    _scrollView->setContentSize(this->getContentSize());
    _scrollView->setContentView(_mapbatchNode->getContentSize());
    _scrollView->setMaximumScale(1.2f);
    
    if(debugWorld) _scrollView->setMinimumScale(_scrollView->getContentSize().width/_mapbatchNode->getContentSize().width);
    else _scrollView->setMinimumScale(_scrollView->getContentSize().width*MAP_ZOOM_LEVEL/_mapbatchNode->getContentSize().width);

    _scrollView->addChild(_mapbatchNode,-4);
    this->addChild(_scrollView, -4);
    
    // status -> "other","own","empty"
    if(!debugWorld) this->createGrass();
    this->createMapFromJson();
    
    Building *garage = g_playerState->getBuilding(kBuildingGarage, 0);
    if (garage) {
        levelGarage=garage->pbVo.level;
        if(levelGarage==1 && garage->pbVo.status == BUILDING_STATUS_CONSTRUCT) {
            levelGarage=0;
        }
    }
    else levelGarage = 0;

    this->createCloud();
    plotChestGems();
    
    _scrollView->addChild(_cloudbatchNode,-2);
    _scrollView->addChild(treasureLayer, -2);
    _scrollView->addChild(_avatarbatchNode,2);
    _scrollView->addChild(unlockLayer,2);
    _scrollView->addChild(newLayer,2);
    

//    if(!debugWorld&&!g_gameTutorial->tutorialMode) {
    if (g_playerState->Checklist[TutSecondRacePart1::key()]) {
        this->showPriceUnlockMap();
    }
    
    //----------------------------------------
    _scrollView->setMaximumScale(1.75f);
    
    if(zoomAtVille) {
        CCSprite *lastVill = (CCSprite*)_mapbatchNode->getChildByTag(indexVisit);
        
        if(lastVill!=0) {
            zoomPoint = lastVill->getPosition();
            zoomPoint.x += lastVill->getContentSize().width*0.5;
            zoomPoint.y += lastVill->getContentSize().height*0.5;
            
            _scrollView->zoomOnPosition(lastVill->getPosition(), MAP_MAXIMUM_SCALE_FOR_ZOOMING, 0.0f);
            this->runAction(CCSequence::create(CCDelayTime::create(0.5f),
                                               CCCallFuncO::create(this, callfuncO_selector(WorldMap::startZoomWin), NULL),
                                               NULL));
        }
    }
    else {
        _scrollView->zoomOnPosition(ccp(homeX, homeY), MAP_MAXIMUM_SCALE_FOR_ZOOMING, 0.0f);
        this->runAction(CCSequence::create(CCDelayTime::create(0.5f),
                                           CCCallFuncO::create(this, callfuncO_selector(WorldMap::startZoomMap), NULL),
                                           NULL));
    }
    
    scheduleUpdate();
    
    return true;
}

void WorldMap::plotChestGems() { //treasure
    
    for (std::map<std::string, ChestGems*>::iterator it=g_playerState->ChestGemsDict.begin(); it!=g_playerState->ChestGemsDict.end(); ++it) {
        if (ChestGems* _chestgems = (ChestGems*)it->second) {
            if (_chestgems->cgVo.locate == "worldmap" &&
                _chestgems->cgVo.posy <= 0.0f) {
                _chestgems->removeFromParentAndCleanup(false);
                
                CCPoint pos = _treasure_pos[(int)_chestgems->cgVo.posx];
//                CCLog_("%f,%f", pos.x, pos.y);
                _chestgems->setPosition(pos);
                _chestgems->reload();
                treasureLayer->addChild(_chestgems);
                _chestgems->showup(true);
            }
        }
    }
    
}
void WorldMap::showPriceUnlockMap()
{
    CREATE_CHAR_BUFFER(_buffer, 64);
    
    unlockLayer->removeAllChildren();
    
    for (int i=0; i<maxCloud; ++i) {
        if(!_cloudList.at(i).active && _cloudList.at(i).unlock_level >- 1) {
            if(_cloudList.at(i).link_cloud[0] == -1) {
                if (CCSprite *tempCloud = (CCSprite*)_cloudbatchNode->getChildByTag(i)) {
                
                    CCLabelBMFont* priceLabel;
                    CCSprite *_coin = CCSprite::createWithSpriteFrameName("oldversion/_icon_coin_6.png");
                    
                    if(_cloudList.at(i).unlock_level < levelGarage+1) {
                        priceLabel = CCLabelHelper::createBMFont(retinaMultiplier, " ", kFontSizeLarge, true, kTextColorWhite);
                        if(i==74) priceLabel->setPosition(ccp(tempCloud->getPositionX()-20*retinaMultiplier,tempCloud->getPositionY()+70*retinaMultiplier));
                        else priceLabel->setPosition(tempCloud->getPosition());
                        priceLabel->setAnchorPoint(ccp(0.5,0));
                        priceLabel->setString(CCStringHelper::createWithNumberFormat(_cloudList.at(i).price).c_str());
                        unlockLayer->addChild(priceLabel,0,20000+i);
                        if (g_playerState->player.balance.gold < _cloudList.at(i).price) {
                            priceLabel->setColor(kTextColorOrange2);
                        }
                    } else if(_cloudList.at(i).unlock_level == levelGarage+1) {
                        priceLabel = CCLabelHelper::createBMFont(retinaMultiplier, " ", kFontSizeMedium, true, kTextColorWhite);
                        priceLabel->setPosition(tempCloud->getPosition());
                        priceLabel->setAnchorPoint(ccp(0.5,0));
                        priceLabel->setString(CCStringHelper::createWithNumberFormat(_cloudList.at(i).price).c_str());
                        unlockLayer->addChild(priceLabel,0,20000+i);
                        snprintf(_buffer, _buffer_size, g_gameContent->Word["need_building_lv"].c_str(), g_gameContent->Word["garage_title"].c_str(), levelGarage+1);
                        priceLabel->setString(_buffer);
                        _coin->setVisible(false);
                    } else {//if(_cloudList.at(i).unlock_level > levelGarage+1) {
                        priceLabel = CCLabelHelper::createBMFont(retinaMultiplier, " ", kFontSizeSmall, true, kTextColorWhite);
                        unlockLayer->addChild(priceLabel,0,20000+i);
                        _coin->setVisible(false);
                    }
                    
                    _coin->setPosition(ccp(priceLabel->getPositionX()+priceLabel->getContentSize().width*0.5+_coin->getContentSize().width*0.25,priceLabel->getPositionY() ));
                    _coin->setAnchorPoint(ccp(0,0.5));
                    unlockLayer->addChild(_coin,0,10000+i);
                    
                }
            } else {
                
                if(!_cloudList.at(i).child_cloud) {
                    
                    if (CCSprite *tempCloud = (CCSprite*)_cloudbatchNode->getChildByTag(i)) {
                        
                        CCPoint _pos0 = tempCloud->getPosition(); //will calculate for the central position
                        CCPoint _pos1 = tempCloud->getPosition(); //will calculate for the most left-bottom position
                        CCPoint _pos2 = tempCloud->getPosition(); //will calculate for the most right-top position
                        
                        for (int j=0; j<LINKED_CLOUD_MAX; ++j) {
                            if(_cloudList.at(i).link_cloud[j] > 0) {
                                _cloudList.at(_cloudList.at(i).link_cloud[j]).child_cloud = true;
                                if (CCSprite *_cloud = (CCSprite*)_cloudbatchNode->getChildByTag(_cloudList.at(i).link_cloud[j])) {
                                    if (_pos1.x > _cloud->getPositionX()) {
                                        _pos1.x = _cloud->getPositionX();
                                    }
                                    if (_pos1.y > _cloud->getPositionY()) {
                                        _pos1.y = _cloud->getPositionY();
                                    }
                                    if (_pos2.x < _cloud->getPositionX()) {
                                        _pos2.x = _cloud->getPositionX();
                                    }
                                    if (_pos2.y < _cloud->getPositionY()) {
                                        _pos2.y = _cloud->getPositionY();
                                    }
                                }
                            }
                        }
                        //calculate for the central position
                        _pos0 = (_pos1 + _pos2) / 2;
                    
//                        //snap to the most central cloud
//                        float _shortest_dist = 0.0f;
//                        bool _first_time = true;
//                        for (int j=0; j<LINKED_CLOUD_MAX; ++j) {
//                            if(_cloudList.at(i).link_cloud[j] > 0) {
//                                if (CCSprite *_cloud = (CCSprite*)_cloudbatchNode->getChildByTag(_cloudList.at(i).link_cloud[j])) {
//                                    float _dist = _pos0.getDistance(_cloud->getPosition());
//                                    if (_shortest_dist > _dist || _first_time) {
//                                        _shortest_dist = _dist;
//                                        _first_time = false;
//                                        _pos0 = _cloud->getPosition();
//                                    }
//                                }
//                            }
//                        }
                        
                        
                        CCLabelBMFont* priceLabel;
                        CCSprite *_coin = CCSprite::createWithSpriteFrameName("oldversion/_icon_coin_6.png");
                        
                        if(_cloudList.at(i).unlock_level < levelGarage+1) {
                            priceLabel = CCLabelHelper::createBMFont(retinaMultiplier, " ", kFontSizeLarge, true, kTextColorWhite);
                            
                            priceLabel->setPosition(_pos0);
                            priceLabel->setString(CCStringHelper::createWithNumberFormat(_cloudList.at(i).price).c_str());
                            priceLabel->setAnchorPoint(ccp(0.5,0.5));
                            unlockLayer->addChild(priceLabel,0,20000+i);
                            
                            if (g_playerState->player.balance.gold < _cloudList.at(i).price) {
                                priceLabel->setColor(kTextColorOrange2);
                            }
                            
                        } else if(_cloudList.at(i).unlock_level==levelGarage+1) {
                            
                            priceLabel = CCLabelHelper::createBMFont(retinaMultiplier, " ", kFontSizeMedium, true, kTextColorWhite);
                            priceLabel->setPosition(_pos0);
                            priceLabel->setString(CCStringHelper::createWithNumberFormat(_cloudList.at(i).price).c_str());
                            priceLabel->setAnchorPoint(ccp(0.5,0.5));
                            unlockLayer->addChild(priceLabel,0,20000+i);
                            
                            snprintf(_buffer, _buffer_size, g_gameContent->Word["need_building_lv"].c_str(), g_gameContent->Word["garage_title"].c_str(), levelGarage+1);
                            
                            priceLabel->setString(_buffer);
                            _coin->setVisible(false);
                            
                        } else {
                            
                            priceLabel = CCLabelHelper::createBMFont(retinaMultiplier, "", kFontSizeSmall, true, kTextColorWhite);
                            unlockLayer->addChild(priceLabel,0,20000+i);
                            
                            _coin->setVisible(false);
                            
                        }
                        
                        _coin->setPosition(ccp(priceLabel->getPositionX()+priceLabel->getContentSize().width*0.5+_coin->getContentSize().width*0.25,priceLabel->getPositionY() ));
                        _coin->setAnchorPoint(ccp(0,0.5));
                        unlockLayer->addChild(_coin,0,10000+i);
                    }
                }
            }
        }
        
    }

}

void WorldMap::checkCloudUnlockTutorial()
{
    int counter = 0;
    for (int i=0; i<maxCloud; ++i) {
        if(_cloudList.at(i).active&&_cloudList.at(i).unlock_level==0) {
            counter++;
        }
    }
    
    if(counter==10) {
        canTouch = false;
    }
    canTouch = true;
}

void WorldMap::onGemsIconHitHUD(void *inst_, void *sender_) {
    
    if (WorldMap* _this = (WorldMap*)sender_) {
        _this->hudPanel->resourceBars[HUDPanelBarGems]->processTargetAmount();
    }
    
}

void WorldMap::onEnter()
{
    CCLayer::onEnter();
    
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, false);
    canTouch = true;
}

void WorldMap::onExit()
{
    CCLayer::onExit();
    
    CCSprite *tempCloud;
    for (int i=0; i<maxCloud; ++i) {
        if(!_cloudList.at(i).active) {
            tempCloud = (CCSprite*)_cloudbatchNode->getChildByTag(i);
            if(tempCloud) tempCloud->stopAllActions();
        }
    }

    if (_villList.size() > 0) {
        _villList.clear();
    }
    
    if (_cloudList.size() > 0) {
        _cloudList.clear();
    }
    
    if(_touch_sprites != NULL) {
        _touch_sprites->removeAllObjects();
        CC_SAFE_DELETE(_touch_sprites);
    }
    
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    
    delete [] unlockVillTable;
}


void WorldMap::createGrass()
{
    CCSprite *_grass;
    for (int i=0; i<784; ++i) { //~6200 , ~5000 // 7000,7000
        _grass = CCSprite::createWithSpriteFrameName("worldmap_grass.png");
        _grass->setPosition(ccp(retinaMultiplier*254*(i%28),retinaMultiplier*254*(i/28)));
        _mapbatchNode->addChild(_grass,-10);
    }
}

void WorldMap::removeOverTile(CCSprite *_sprite[], float homeX,float homeY,float width,float height, int numtile)
{
    float factorCheck = 0.5;
    for (int i=0; i<numtile; ++i) {
        
        if(!_cloudList.at(i).active) {
            if(_sprite[i]->getPositionX()>_mapbatchNode->getContentSize().width+_sprite[i]->getContentSize().width*factorCheck) {
                //CCLog_("over look at index:%d",i);
                
                _sprite[i]->getParent()->removeChild(_sprite[i]);
            }
            else if(_sprite[i]->getPositionX()<-_sprite[i]->getContentSize().width*factorCheck) {
                //CCLog_("over look at index:%d",i);
                
                _sprite[i]->getParent()->removeChild(_sprite[i]);
            }
            else if(_sprite[i]->getPositionY()>_mapbatchNode->getContentSize().height+_sprite[i]->getContentSize().height*factorCheck) {
                //CCLog_("over look at index:%d",i);
                
                _sprite[i]->getParent()->removeChild(_sprite[i]);
            }
            else if(_sprite[i]->getPositionY()<-_sprite[i]->getContentSize().height*factorCheck) {
                //CCLog_("over look at index:%d",i);
                
                _sprite[i]->getParent()->removeChild(_sprite[i]);
            }
        }
    }
    
//    CCLog_("num object cloud after remove over tile = %d",_cloudbatchNode->getChildrenCount());
}


void WorldMap::createTile(CCSprite *_sprite[], float centerX, float centerY, float width, float height, int numtile)
{
    
    int counter=0;
    int indexLevel=0;
    int maxtile[layerCloud]; //14 layer
    int totaltile[layerCloud];
    double angle=0;
    double degree=0;
    int distanceX = 0;
    int distanceY = 0;
    
    maxtile[0]=0;
    for (int i=1; i<layerCloud; ++i) {
        maxtile[i]=i*8;
        //CCLog_("max tile = %d",maxtile[i]);
    }
    
    for (int i=0; i<layerCloud; ++i) {
        int j=0;
        totaltile[i]=1;
        while(j<=i) {
            totaltile[i]+=maxtile[j];
            j++;
        }
//        CCLog_("total tile = %d",totaltile[i]);
    }
    
    while(counter<numtile) {
        if(counter==0) {
            _sprite[counter]->setPosition(ccp(centerX,centerY));
        }
        else {
            indexLevel=0;
            angle=0;
            while(counter>=totaltile[indexLevel]) { //find level tile with index tile
                indexLevel++;
            }
            
            angle=(counter-totaltile[indexLevel-1])*360.0/(maxtile[indexLevel]);
            //CCLog_("angle = %f",angle);
            degree = angle*M_PI/180.0;
            
            if((int)angle%90==0) _sprite[counter]->setPosition(ccp(centerX+cos(degree)*width*indexLevel,centerY+sin(degree)*height*indexLevel));
            else {
                distanceX = width*indexLevel;
                distanceY = height*indexLevel;
                
                if(angle<90) {
                    _sprite[counter]->setPosition(ccp(centerX+distanceX-distanceX*angle/90.0,centerY+distanceY*angle/90));
                }
                else if(angle<180) {
                    _sprite[counter]->setPosition(ccp(centerX-distanceX*(angle-90)/90.0,centerY+distanceY-distanceY*(angle-90)/90));
                }
                else if(angle<270) {
                    _sprite[counter]->setPosition(ccp(centerX-distanceX+distanceX*(angle-180)/90.0,centerY-distanceY*(angle-180)/90));
                }
                else {
                    _sprite[counter]->setPosition(ccp(centerX+distanceX*(angle-270)/90.0,centerY-distanceY+distanceY*(angle-270)/90));
                }
            }
        }
        this->setZOrder(_sprite[counter], 100000-_sprite[counter]->getPositionY());
        
        counter++;
    }
    
}


void WorldMap::setZOrder(CCNode* node,int value)
{
    node->getParent()->reorderChild(node,value);
}

void WorldMap::createCloud()
{
    
    int width = 565*retinaMultiplier;
    int height = 465*retinaMultiplier;
    int total_tile = maxCloud;
    
    CCSprite *_cloud[total_tile];
    for (int i=0; i<total_tile; ++i) {
        
        if(debugWorld) {
            _cloud[i] = CCSprite::createWithSpriteFrameName("worldcloud3.png"); //yellow cloud
            _cloud[i]->setScale(1.1);
        }
        else {
            int randomCloud = rand()%30;
            if(randomCloud>19) {
                _cloud[i] = CCSprite::createWithSpriteFrameName("worldcloud3.png");
            }
            else if(randomCloud>9) {
                _cloud[i] = CCSprite::createWithSpriteFrameName("worldcloud4.png");
            }
            else _cloud[i] = CCSprite::createWithSpriteFrameName("worldcloud5.png");
        }
        _cloud[i]->setScale(CLOUD_ADDITIONAL_SCALING); //worldcloud size is always 50%
        _cloud[i]->setAnchorPoint(ccp(0.5,0.56));
        _cloudbatchNode->addChild(_cloud[i], 0, i);
    }
    
    this->createTile(_cloud,homeX, homeY, width, height, total_tile);
    
    if(debugWorld) {
        for (int i=0; i<total_tile; ++i) {
            _cloud[i]->setOpacity(100);
            
            CREATE_CHAR_BUFFER(text, 4);
            snprintf(text, text_size, "%d", i);
            CCLabelBMFont* ttf1 = CCLabelHelper::createBMFont(retinaMultiplier, text, 80.0f, kTextColorDark);
            ttf1->setPosition(_cloud[i]->getPosition());
            _scrollView->addChild(ttf1,-3);
            this->setZOrder(ttf1, 100000-_cloud[i]->getPositionY());
        }
    }
    
    //optimize with clear over scene
    else {
        this->setCloudAnimation(_cloud, total_tile);
    }
    
    this->checkUnlockCloud();
    if(!debugWorld) this->removeOverTile(_cloud, homeX, homeY, width, height, total_tile);
}



void WorldMap::checkUnlockCloud()
{
    g_gameContent->getUnlockVillTable(unlockVillTable);
    
    //this load unlock cloud
    createCloudFromJson();
    
    //open cloud start game
//    if(g_gameTutorial->tutorialMode) {
    if (!g_playerState->Checklist[TutSecondRacePart1::key()]) {
        for (int i=0; i<11; ++i) {
            _cloudList.at(i).active = true;
            _cloudbatchNode->removeChildByTag(i);
        }
        _cloudList.at(24).active = true;
        _cloudbatchNode->removeChildByTag(24);
    } else {
        for (int i=0; i<maxCloud; ++i) {
//            CCLog_("Cloud:%d unlock at level:%d", i, _cloudList.at(i).unlock_level);
            if(_cloudList.at(i).unlock_level<0) {
                _cloudList.at(i).active = true;
                CCLog_("remove cloud by tag: %d", i);
                _cloudbatchNode->removeChildByTag(i);
            }
        }
    }

    //check open cloud with village from server
    for (std::map<int, PlayerWorldMapVillageVO>::iterator it=g_playerState->WorldMapVillages.begin(); it!=g_playerState->WorldMapVillages.end(); ++it) {
        PlayerWorldMapVillageVO vo = (PlayerWorldMapVillageVO)it->second;
        
//        CCLog_("Size World Map = %lu",g_playerState->WorldMapVillages.size());
        
        if (vo.village_id != 0) {
            //default unlocked villages at the beginning of the game
            if (vo.village_id == 1) { }
            else if (vo.village_id == 8) { }
            else if (vo.village_id == 15) { }
            //the rest of the villages
//            else if (vo.village_id < 49) { //2,9,26
            else if (vo.village_id < 155) { //2,9,26
//                CCLog_("Open new cloud = %d",unlockVillTable[vo.village_id]);
                openMapWithIndex(unlockVillTable[vo.village_id]);
            }
//            CCLog_("id=%d, uuid=%s, name=%s", vo.village_id, vo.uuid.c_str(), vo.name.c_str());
        }
    }
    
//    CCLog_("num object cloud = %d",_cloudbatchNode->getChildrenCount());
    
    
//    //===================================

    if(!debugWorld) {
        //show vill or hide vill
        checkVillActivate();
        
    }
    else  { //debug mode show all
        
        CCLog_("============== Debug Mode ==============");
        
        CCObject* _obj = NULL;
        CCARRAY_FOREACH(_touch_sprites, _obj)
        {
            CCSprite *vill = (CCSprite*)_obj;
            
            CCLabelBMFont* ttf1 = CCLabelHelper::createBMFont(retinaMultiplier, "name", kFontSizeExtraLarge, true, kTextColorWhite);
            ttf1->setPosition(ccp(vill->getPositionX()+44*retinaMultiplier+vill->getContentSize().width*0.5f,vill->getPositionY()+26*retinaMultiplier));
            ttf1->setAnchorPoint(ccp(0.5,1));
            ttf1->setString(CCString::createWithFormat("id %03d",vill->getTag()-100000)->getCString());
            _scrollView->addChild(ttf1,100);

        }
    }
    
}

void WorldMap::checkVillActivate()
{
//    int idCount = 0;
    
    PlayerState *_playerState = PlayerState::getPlayer();
    std::vector<int> index_of_attackable_vills;
    
    for (int i=0; i<_playerState->WorldMapVillages.size(); i++) {
        if (_playerState->WorldMapVillages[i].status == "other") {
            index_of_attackable_vills.push_back(i);
        }
    }

    int index_select = -1;

    if (!index_of_attackable_vills.empty())
    {
        index_select = index_of_attackable_vills[rand()%index_of_attackable_vills.size()];
    }

    int vill_count = 0;
    
    CREATE_CHAR_BUFFER(buf_, 32);
    CCObject* _obj = NULL;
    CCARRAY_FOREACH(_touch_sprites, _obj)
    {
        CCSprite *vill = (CCSprite*)_obj;
        if(vill->getTag()!=9999+100000) { //ไม่ต้องใส่ชื่อใหม่ให้ home
            if(!_cloudList.at(unlockVillTable[vill->getTag()-100000]).active) {
                vill->setVisible(false);
//                CCLog_("clear at %d",vill->getTag());
            }
            else if(!_villList.at(vill->getTag()-100000).active) {
                
                _villList.at(vill->getTag()-100000).active = true; //show by cloud
                int village_vo_index = vill->getTag()-100000;
                PlayerWorldMapVillageVO vo = g_playerState->WorldMapVillages[village_vo_index];
                
                //prevent server send empty slot player
                if (vo.status.compare("empty") != 0) {
                    vill->setVisible(true);
                    
                    //save map status to sharedUserDefault
                    CREATE_CHAR_BUFFER(_buffer, 16);
                    snprintf(_buffer, _buffer_size, "village%d", vo.village_id);
                    
                    if(vo.status.compare("other")==0) {
                        
                        //popup new village
                        if(CCUserDefault::sharedUserDefault()->getIntegerForKey(_buffer) == 1 &&
                           g_playerState->Checklist[TutSecondRacePart1::key()]) { //เคยเป็นบ้านของเรา
                            
                            float _slideUpY = 0.0f;
                            
                            CREATE_CHAR_BUFFER(_buf, 128);
                            snprintf(_buf, _buf_size, "map_horsepower_rank_%d.png", 1);
                            CCSpriteFrame *_rankSpriteFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(UIHelper::getSpritePath(UIHelper::NSClassesRibbonEnemy));
                            if (_rankSpriteFrame) {
                                _slideUpY = _rankSpriteFrame->getRect().size.height*0.5f;
                            }
                            
                            CCSprite *newPopOver = CCSprite::createWithSpriteFrameName("oldversion/_popover_circle_2.png");
                            newPopOver->setPosition(ccp(vill->getPositionX() + vill->getContentSize().width*0.5,
                                                        vill->getPositionY() + vill->getContentSize().height + _slideUpY));
                            newPopOver->setAnchorPoint(ccp(0.5,0));
                            newLayer->addChild(newPopOver,100,vill->getTag());
                            
                            CCSprite *dropItemPopOverArrow = CCSprite::createWithSpriteFrameName("oldversion/_popover_circle_2_arrow.png");
                            dropItemPopOverArrow->setPosition(CCPoint(newPopOver->getContentSize().width*0.5f, 0.0f));
                            newPopOver->addChild(dropItemPopOverArrow);
                            
                            CCLabelBMFont* ttf1 = CCLabelHelper::createBMFont(retinaMultiplier, "New!", kFontSizeExtraLarge, false, kTextColorGreen);
                            ttf1->setPosition(newPopOver->getContentSize()*0.5);
                            newPopOver->addChild(ttf1);
                            
                            newPopOver->stopAllActions();
                            CCSequence *animate_once = CCSequence::create(CCScaleTo::create(0.5f, 1.1f),
                                                                          CCScaleTo::create(0.5f, 1.0f),
                                                                          NULL);
                            
                            newPopOver->runAction(CCRepeatForever::create(animate_once));
                        }
                        
                        CCUserDefault::sharedUserDefault()->setIntegerForKey(_buffer, 2);
                        snprintf(_buffer, _buffer_size, "timestamp%d", vo.village_id);
                        CCUserDefault::sharedUserDefault()->setIntegerForKey(_buffer, 2147483647);
                        
                        if (index_select != -1 && index_select == village_vo_index &&
                            CustomEventsHelper::Instance().shouldAnimateIdle(kIdleAnimationTapVill))
                        {
                            vill->runAction(CustomEventsHelper::Instance().getIdleAnimation());
                            
                            float scaleDifference = 0.2f;
                            float duration = 0.15f;
                            
                            CCPoint originalVillPos = vill->getPosition();
                            CCSize originalVillSize = vill->getContentSize();
                            
                            CCAction *scaleUp1 =
                            CCSpawn::create(CCScaleTo::create(duration, 1.0f + scaleDifference),
                                            CCMoveTo::create(duration,
                                                             originalVillPos -
                                                             ccp(originalVillSize.width*scaleDifference*0.25f,
                                                                 originalVillSize.height*scaleDifference*0.25f)),
                                            NULL);
                            
                            CCAction *scaleDown1 =
                            CCSpawn::create(CCScaleTo::create(duration*2, 1.0f),
                                            CCMoveTo::create(duration*2,
                                                             originalVillPos +
                                                             ccp(originalVillSize.width*scaleDifference*0.25f,
                                                                 originalVillSize.height*scaleDifference*0.25f)),
                                            NULL);
                            
                            CCAction *scaleUp2 =
                            CCSpawn::create(CCScaleTo::create(duration*2, 1.0f + scaleDifference),
                                            CCMoveTo::create(duration*2,
                                                             originalVillPos -
                                                             ccp(originalVillSize.width*scaleDifference*0.25f,
                                                                 originalVillSize.height*scaleDifference*0.25f)),
                                            NULL);
                            
                            CCAction *scaleDown2 =
                            CCSpawn::create(CCScaleTo::create(duration, 1.0f),
                                            CCMoveTo::create(duration,
                                                             originalVillPos +
                                                             ccp(originalVillSize.width*scaleDifference*0.25f,
                                                                 originalVillSize.height*scaleDifference*0.25f)),
                                            NULL);
                            
                            CCAction *specialIdleAnimation =
                            CCRepeatForever::create(CCSequence::create(CCDelayTime::create(3.0f),
                                                                       scaleUp1,
                                                                       scaleDown1,
                                                                       scaleUp2,
                                                                       scaleDown2,
                                                                       NULL));
                            vill->stopAllActions();
                            vill->runAction(specialIdleAnimation);
                        }
                    }
                    else if(vo.status.compare("own")==0) {
                        CCUserDefault::sharedUserDefault()->setIntegerForKey(_buffer, 1);
                        snprintf(_buffer, _buffer_size, "timestamp%d", vo.village_id);
                        CCUserDefault::sharedUserDefault()->setIntegerForKey(_buffer, vo.timestamp + vo.wait);
                    }
//                    else {
//                        snprintf(_buffer, _buffer_size, "timestamp%d", vo.village_id);
//                        CCUserDefault::sharedUserDefault()->setIntegerForKey(_buffer, 2147483647);
//                    }
                    
                    
                    if(vill->getTag()==indexVisit) {
                        zoomAtVille = true;
                    }
                    
                    CCSpriteFrame *sp;
                    if(vo.status.compare("other")==0) {
                        float xVill = vill->getPositionX()+vill->getContentSize().width*0.5f;
                        sp = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("enemy.png");
                        
                        //test
//                        vo.bot = 0;
                        
                        if(vo.bot == 0) { //ไม่ใช่บอท
                            
                            /*
                            //debug mode random FbId not real user
                            long long opFbId = 1014023342;
                            if(idCount==0) opFbId = 1097695967;
                            else if(idCount==1) opFbId = 557055802;
                            else if(idCount==2) opFbId = 1111111111;
                            //--------------------------
                            
                            idCount++;
                            CREATE_CHAR_BUFFER(_buffer, 64);
                            
                            snprintf(_buffer, _buffer_size, "http://graph.facebook.com/%llu/picture?type=square", opFbId);
                            std::string opUrl = _buffer;
                            
                            snprintf(_buffer, _buffer_size, "%llu.jpg", opFbId);
                            std::string opFname = _buffer;
                            */
                            
//                            std::string opFname = vo.photo;
//                            std::string opUrl = vo.photo;
                           
                            CCSprite *frameAvatar = CCSprite::createWithSpriteFrameName("map_avatar_default.png");
                            frameAvatar->setPosition(ccp(xVill-6*retinaMultiplier,
                                                         vill->getPositionY()
                                                         + vill->getContentSize().height
                                                         + 65*retinaMultiplier));
                            frameAvatar->setScale(0.5f);
                            frameAvatar->setAnchorPoint(ccp(0.5,0.5));
                            _avatarbatchNode->addChild(frameAvatar,1);
                            
                            //vill->userObject = frameAvatar;
                            
                        }
                        
                        
                        CCSprite *rankStripe = CCSprite::createWithSpriteFrameName("map_ribbon_enemy.png");
                        rankStripe->setPosition(ccp(xVill,vill->getPositionY()+200*retinaMultiplier));
                        _avatarbatchNode->addChild(rankStripe,1);
                        
                        CCLabelBMFont* ttf1 = CCLabelHelper::createBMFont(retinaMultiplier, "name", kFontSizeLarge, true, kTextColorWhite);
                        ttf1->setPosition(ccp(xVill,vill->getPositionY()+28*retinaMultiplier));
                        ttf1->setAnchorPoint(ccp(0.5,1));
                        if (vo.village_id != 0) ttf1->setString(vo.name.c_str());
                        _scrollView->addChild(ttf1, 1, vill->getTag()-100000);
                        
                        CCLabelBMFont* powerLabel = CCLabelHelper::createBMFont(retinaMultiplier, "", kFontSizeMedium, false, kTextColorWhite);
                        powerLabel->setPosition(ccp(xVill,vill->getPositionY()+232*retinaMultiplier));
                        powerLabel->setAnchorPoint(ccp(0.5,1));
                        _scrollView->addChild(powerLabel,3,vill->getTag()-50000);
                        
                        //mark village that can change opponent
                        if(currentTime>vo.timestamp+vo.wait) {
                            CCSprite *changeMark = CCSprite::createWithSpriteFrameName("map_icon_change_opponent.png");
                            changeMark->setPosition(ccp(xVill+ vill->getContentSize().width * 0.3, vill->getPositionY() + 50*retinaMultiplier));
                            
                            //avatar mark 50000-99999
                            _avatarbatchNode->addChild(changeMark,1,vill->getTag()-50000);
                        }
                        
                        
                        bool matchTypePet = false;
                        for(std::vector<GDPetVO>::iterator it=g_gameContent->Pet.begin(); it!=g_gameContent->Pet.end(); ++it) {
                            GDPetVO _vo = (GDPetVO)(*it);
                            
                            matchTypePet = false;
                            if (_vo.type == vo.host_type) {
                                
                                //CCLog_("== %s OK Match ==",_vo.type.c_str());
                                snprintf(buf_, buf__size, "map_class_%d.png", _vo.rarity_stars);
                                CCSprite *rankPic = CCSprite::createWithSpriteFrameName(buf_);
                                rankPic->setPosition(ccp(vill->getPositionX(),rankStripe->getPositionY()));
                                _avatarbatchNode->addChild(rankPic,1,vill->getTag()-100000);
                                matchTypePet = true;
                                break;
                            }
                        }
                        if(!matchTypePet) {
                            //CCLog_("Animal Type : %s not match!",vo.host_type.c_str());
                            snprintf(buf_, buf__size, "map_class_%d.png", 1);
                            CCSprite *rankPic = CCSprite::createWithSpriteFrameName(buf_);
                            rankPic->setPosition(ccp(vill->getPositionX(),rankStripe->getPositionY()));
                            _avatarbatchNode->addChild(rankPic,1,vill->getTag()-100000);
                        }
                        
                        snprintf(buf_, buf__size, "%d", vo.horse_power);
                        powerLabel->setString(buf_);
                        
                    }
                    else {
                        sp = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("territory.png");
                    }
                    
                    vill->setDisplayFrame(sp);
//                    CCLog_("Name new village = %s",vo.name.c_str());
                }
                else {
//                    CCLog_("receive data player with empty");
                    vill->setVisible(false);
                }
                
            }
            
            if(_villList.at(vill->getTag()-100000).active) {
                vill_count++;
            }
        }
    }
    
//    CCLog_("Vill Count = %d",vill_count);
    
    CCUserDefault::sharedUserDefault()->setIntegerForKey("vill_count", vill_count);
}

void WorldMap::setCloudAnimation(CCSprite *_sprite[], int numtile) {
    
    for (int i=0; i<numtile; ++i) {
        float delay = float(rand()%100/10.0);
        _sprite[i]->stopAllActions();
        CCSequence *cloudAnimate = CCSequence::create(CCDelayTime::create(delay),
                                                     CCEaseSineInOut::create(CCScaleTo::create(10.0f, 1.2f*CLOUD_ADDITIONAL_SCALING)),
                                                     CCEaseSineInOut::create(CCScaleTo::create(10.0f, 1.0f*CLOUD_ADDITIONAL_SCALING)),
                                                     NULL);
        
        _sprite[i]->runAction(CCRepeatForever::create(cloudAnimate));
    }
    
}

void WorldMap::createCloudFromJson()
{
    if (_cloudList.size() > 0) {
        _cloudList.clear();
    }
    
    //create and restore default maximum cloud
    for (int i=0; i<maxCloud; i++) {
        GDWorldCloudVO _cloud = GDWorldCloudVO();
        _cloudList.push_back(_cloud);
    }
    
    
//    for (int i=0; i<maxCloud; i++) {
//        CCLog_("Cloud:%d unlock at level:%d", i, _cloudList.at(i).unlock_level);
//    }
    
    int counter = 0;
    for (std::vector<GDCloudGroupVO>::iterator it=g_gameContent->Cloud.begin(); it!=g_gameContent->Cloud.end(); ++it) {
        GDCloudGroupVO _cloudVO = (GDCloudGroupVO)(*it);
        
        for (int j=0; j<LINKED_CLOUD_MAX; j++) {
            if(_cloudVO.link_cloud[j]!=-1) {
                int tempID = _cloudVO.link_cloud[j];
                
                int count = 0;
                for (int k=0; k<LINKED_CLOUD_MAX; k++) {
                    if(tempID!=_cloudVO.link_cloud[k]&&_cloudVO.link_cloud[k]!=-1) {
                        _cloudList.at(tempID).link_cloud[count] = _cloudVO.link_cloud[k];
                        count++;
                    }
                }
                
                for (int k=0; k<UNLOCK_VILL_MAX; k++) {
                    if(_cloudVO.unlock_vill[k]!=-1) {
                        _cloudList.at(tempID).unlock_vill[k] = _cloudVO.unlock_vill[k];
                    }
                }

                _cloudList.at(tempID).group        = _cloudVO.group;
                _cloudList.at(tempID).unlock_level = _cloudVO.unlock_level;
                _cloudList.at(tempID).price        = _cloudVO.price;
            }
        }
        
        counter++;
    }

}

void WorldMap::createMapFromJson() {
    
    unsigned long _fileSize = 0;
    unsigned char *_fileContents = NULL;
    
    std::string _fullPath = CCFileUtils::sharedFileUtils()->fullPathFromRelativeFile("map.json", "common/world/");
    _fileContents = CCFileUtils::sharedFileUtils()->getFileData(_fullPath.c_str(), "r", &_fileSize);

    //JsonAllocator allocator;
    Json *_root = Json_create((char*)_fileContents);
    if (!_root) {
        CCLog_("Invalid JSON format in map.json!");
        //return false;
    }
    //else CCLog_("Load JSON OK");
    
    int tag = 0;
    if (_villList.size() > 0) {
        _villList.clear();
    }
    
    int counterTreasure = 0;
    
    CCSprite *_obj = NULL;
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse World Map
    if (Json *_worldmaps = Json_getItem(_root, "worldmap")) {
        int _nworldmaps = Json_getSize(_worldmaps);
        
        GDWorldMapVillVO village = GDWorldMapVillVO();
        _treasure_pos.clear();
        for (int i=0; i<_nworldmaps; i++) {
            
            if (Json* _area = Json_getItemAt(_worldmaps, i)) {
            
                std::string filename = Json_getString(_area, "name", "unknown");
                filename += ".png";
                int _x = Json_getInt(_area, "x", 0);
                int _y = Json_getInt(_area, "y", 0);
                int z_obj = 0;
                
                switch (targetDevice) {
                    case TD_IPHONE:
                    case TD_IPHONE_RETINA:
                    case TD_IPAD:           _y-=3450;   break;
                    case TD_IPAD_RETINA:    _y+=50;    break;
                }
                
                
                if(filename == "treasure.png") {
                    
                    //TODO: Improve this shit later
                    filename =  "selecttion_world.png";
                    _obj = CCSprite::createWithSpriteFrameName(filename.c_str());
                    _obj->setPosition(ccp(_x*retinaMultiplier,
                                          (_mapbatchNode->getContentSize().height-_y)*retinaMultiplier));
                    _obj->setAnchorPoint(ccp(0,0));
                    
                    _treasure_pos.push_back(ccp(_obj->getPositionX()+_obj->getContentSize().width*0.5 ,
                                                _obj->getPositionY()+_obj->getContentSize().height*0.5));
                    
                } else {
                    
                    _obj = CCSprite::createWithSpriteFrameName(filename.c_str());
                    _obj->setPosition(ccp(_x*retinaMultiplier,
                                          (_mapbatchNode->getContentSize().height-_y)*retinaMultiplier));
                    _obj->setAnchorPoint(ccp(0,0));
                    
                    z_obj = 100000-(_obj->getPosition().y+(_obj->getContentSize().height*0.5f));
                    _obj->setTag(tag);
                    _mapbatchNode->addChild(_obj, z_obj);
                    
                }
                
                if(filename == "home.png") {
                    
                    village.type = "home";
                    village.name = "Home";
                    village.active = false;
                    village.tag = 9999+100000;
                    
                    _villList.push_back(village);
                    
                    homeX = _obj->getPosition().x+_obj->getContentSize().width*0.5-10.0f;
                    homeY = _obj->getPosition().y+_obj->getContentSize().height*0.5+25.0f;
                    _touch_sprites->addObject(_obj);
                    
                    //home set tag
                    _obj->setTag(9999+100000);
                    
                    CCLabelBMFont* ttf1 = CCLabelHelper::createBMFont(retinaMultiplier, g_gameContent->Word["map_you"].c_str(), kFontSizeExtraLarge, true, kTextColorWhite);
                    ttf1->setPosition(ccp(_obj->getPositionX()+
                                          _obj->getContentSize().width*0.5f,
                                          _obj->getPositionY()+60*retinaMultiplier));
                    
                    ttf1->setAnchorPoint(ccp(0.5,1));
                    _scrollView->addChild(ttf1,-3);
                    
                    
                }
                else if(filename=="enemy.png" || filename=="test_vill0.png") {
                    
                    tag++;
                    
                    village.type = "enemy"; //home, enemy, territory
                    village.name = CCString::createWithFormat("id %d",tag)->getCString();
                    village.active = false;
                    village.tag = tag+100000;
                    
                    _villList.push_back(village);
                    _touch_sprites->addObject(_obj);
                    
                    //village set tag 100001 - 100050
                    _obj->setTag(tag+100000);
                    
                }
                else if(filename=="territory.png") {
                    
                    tag++;
                    
                    village.type = "territory"; //home, enemy, territory
                    village.name = CCString::createWithFormat("id %d",tag)->getCString();
                    village.active = false;
                    village.tag = tag+100000;
                    
                    _villList.push_back(village);
                    _touch_sprites->addObject(_obj);
                    
                    _obj->setTag(tag+100000);
                    
                }
                else if(filename=="town.png") {
                    //_touch_sprites->addObject(_obj); //cannot touch
                    
                    _obj->setTag(99);
                }
                else if(filename=="world_island0.png") {
                    _obj->setPosition(ccp(_obj->getPositionX(),_obj->getPositionY()-38*retinaMultiplier));
                }
                else if(filename=="selecttion_world.png") {
                    _obj->setOpacity(0);
                    //tag treasure 300000,300001,300002, ...
                    
                    counterTreasure++;
                    
                    _obj->setTag(tag+300000);
                }
                
            }
            
        }
    }
    
    //    CCLog_("_villList size = %ld",_villList.size());
    //    CCLog_("Num touch sprite = %d",_touch_sprites->count());
    
    Json_dispose(_root);
    
    delete _fileContents;
    _fileContents = NULL;
    
}


void WorldMap::startZoomWin()
{
    CCSprite *lastVill = (CCSprite*)_mapbatchNode->getChildByTag(indexVisit);
    if(lastVill!=0) {
        indexVisit=0;
        
//        _scrollView->setMaximumScale(1.2);
        _scrollView->zoomOnPosition(zoomPoint, MAP_MINIMUM_SCALE_FOR_ZOOMING, MAP_ZOOM_DURATION);
    }
}

void WorldMap::startZoomMap()
{
    //===============
//    _scrollView->setMaximumScale(1.2);
    
    _scrollView->zoomOnPosition(ccp(homeX, homeY), MAP_MINIMUM_SCALE_FOR_ZOOMING, MAP_ZOOM_DURATION);
}

void WorldMap::clearPopup(bool animate)
{
    CCLog_("WorldMap::clearPopup");

    if(showPopup) {
        int tag = m_dialog->getTag();
        if(tag < maxCloud && tag > 0) {
            CCSprite *ownCloud = (CCSprite*)_cloudbatchNode->getChildByTag(tag);
            if(ownCloud) {
                ownCloud->setColor(ccc3(255,255,255));
                for (int j=0; j<LINKED_CLOUD_MAX; ++j) {
                    if(_cloudList.at(tag).link_cloud[j] < maxCloud && _cloudList.at(tag).link_cloud[j]>0) {
                        CCSprite *linkCloud = (CCSprite*)_cloudbatchNode->getChildByTag(_cloudList.at(tag).link_cloud[j]);
                        linkCloud->setColor(ccc3(255,255,255));
                    }
                }
            }

        }
        m_dialog->dismissPopover(animate);
        showPopup = false;
        
    }
    
    prepareShowPopup = false;
    _selected_village = NULL;
    visitButton = NULL;
}

#pragma mark - CCNikScrollViewDelegate

void WorldMap::scrollViewDidScroll(CCNikScrollView *view)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if (prepareShowPopup)
    {
        float distance2 = startTouch.getDistanceSq(view->contentNode->getPosition());
        CCLog_("WorldMap::scrollViewDidScroll %f", sqrtf(distance2));

        if (distance2 > ANDROID_TOUCH_MOVE_THRESHOLD*ANDROID_TOUCH_MOVE_THRESHOLD)
        {
            clearPrepareTouch();
        }
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

    clearPrepareTouch();

    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

    if(fabsf(ccpDistance(dialogPos, _scrollView->getContentOffset()))>screenSize.height*0.2) {
//        if(!(g_gameTutorial->tutorialMode&&!g_playerState->Checklist["guide4"])) {
        if (!g_playerState->Checklist[TutSecondRacePart1::key()]) {
            clearPopup(true);
        }
    }
#endif
}

void WorldMap::scrollViewDidZoom(CCNikScrollView *view)
{
//    CCLog_("scrollViewDidZoom");
    clearPrepareTouch();
//    if(!(g_gameTutorial->tutorialMode&&!g_playerState->Checklist["guide4"])) {
    if (!g_playerState->Checklist[TutSecondRacePart1::key()]) {
        clearPopup(true);
    }
}



bool WorldMap::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent) {
    
    if(canTouch) {
        CCPoint location = _scrollView->contentNode->convertTouchToNodeSpaceAR(pTouch);
        CCPoint screenPoint = pTouch->getLocationInView();
        CCPoint locationScreen = CCDirector::sharedDirector()->convertToGL(screenPoint);

        startTouch = _scrollView->contentNode->getPosition();

        CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
        if(locationScreen.x>screenSize.width*0.64&&locationScreen.y>screenSize.height*0.9) {
//            CCLog_("Touch coin , diamond panel");
        }
        else if(locationScreen.x<screenSize.width*0.25&&locationScreen.y>screenSize.height*0.85) {
//            CCLog_("Touch name , level , fan panel");
        }
        else {
            if(!prepareShowPopup) {
                
                for(int i = 0; i < unlockLayer->getChildrenCount(); i+=2)
                {
                    CCObject* child = unlockLayer->getChildren()->objectAtIndex(i);
                    CCLabelBMFont *target = (CCLabelBMFont*)child;
                    
                    //price label tag = 20,000+i
                    int priceTag = target->getTag()-20000;
                    
                    if(target && priceTag < maxCloud) {
                    
                        if(_cloudList.at(priceTag).unlock_level <= levelGarage && !_cloudList.at(priceTag).active) {
                            
                            CCRect rect = target->boundingBox();
                            rect.origin = rect.origin - rect.size*0.7;
                            rect.size = rect.size * 2.8;
                            
                            if (rect.containsPoint(location)) {
                                clearPopup(true);
                                prepareShowPopup = true;
                                
                                target->stopAllActions();
                                target->setScale(ACTIVE_CLOUD_PRICE_SCALE);
                                
                                CCSprite *coin = (CCSprite*)unlockLayer->getChildByTag(priceTag+10000);
                                coin->stopAllActions();
                                coin->setScale(ACTIVE_CLOUD_PRICE_SCALE);
                                
                                return true;
                            }
                        }
                    }
                }
                
                
                CCObject* _obj = NULL;
                CCARRAY_FOREACH(_touch_sprites, _obj)
                {
                    CCSprite *target = (CCSprite*)_obj;
                    if(target!=0) {
                        if(target->isVisible()) {
                            CCRect rect = target->boundingBox();
                            if(target->getTag()!=9999+100000) { //home ไม่ต้องขยายขนาดรับ touch
                                rect.origin = rect.origin - rect.size*0.2;
                                rect.size = rect.size * 1.4;
                            }
                            
                            if (rect.containsPoint(location)) {
                                if(target->getAnchorPoint().x==0) {
                                    clearChildrenPopover(target);
                                    clearPopup(true);
                                    
                                    prepareShowPopup = true;
                                    
                                    target->setScale(0.9);
                                    target->setAnchorPoint(ccp(0.5,0));
                                    target->setPosition(ccp(target->getPositionX()+target->getContentSize().width*0.5,target->getPositionY()));
                                    
//                                    if(!g_playerState->Checklist["guide4"]) {
//                                        if(target->getTag()==100001) {
//                                            tutorialVill->stopAllActions();
//                                            tutorialVill->runAction(CCSequence::create(
//                                                                                       CCEaseSineInOut::create(CCScaleTo::create(0.1f, 1.2f)),
//                                                                                       CCEaseSineInOut::create(CCScaleTo::create(0.2f, 1.0f)),
//                                                                                       NULL));
//                                        }
//                                    }
                                }
                                else {
                                    
                                    target->stopAllActions();
                                    target->setScale(1.0f);
                                    target->setAnchorPoint(ccp(0,0));
                                    target->setPosition(ccp(target->getPositionX()-target->getContentSize().width*0.5,target->getPositionY()));
                                    
                                    prepareShowPopup = false;
                                }
                                return true;
                            }
                        }
                    }
                }
            }
        }
        
    }
    
    return true;
}

void WorldMap::clearChildrenPopover(cocos2d::CCSprite *sender)
{

    CCSprite *target = (CCSprite*)newLayer->getChildByTag(sender->getTag());
    if(target) {
        target->stopAllActions();
        target->removeFromParent();
        target = NULL;
    }

}

void WorldMap::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) {

    if(canTouch) {
        CCPoint location = _scrollView->contentNode->convertTouchToNodeSpace(pTouch);
        CCPoint screenPoint = pTouch->getLocationInView();
        CCPoint locationScreen = CCDirector::sharedDirector()->convertToGL(screenPoint);
        CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
        if(locationScreen.x>screenSize.width*0.64&&locationScreen.y>screenSize.height*0.9) {
//            CCLog_("Touch coin , diamond panel");
        }
        else {
            bool clearTouchBegan = false;
            
            for(int i = 0; i < unlockLayer->getChildrenCount(); i+=2)
            {
                CCObject* child = unlockLayer->getChildren()->objectAtIndex(i);
                CCLabelBMFont *target = (CCLabelBMFont*)child;
                
                //price label tag = 20000+i
                int priceTag = target->getTag()-20000;

                if(target && priceTag < maxCloud) {
                    if(_cloudList.at(priceTag).unlock_level<=levelGarage&&!_cloudList.at(priceTag).active) {

                        CCRect rect = target->boundingBox();
                        rect.origin = rect.origin - rect.size*0.7;
                        rect.size = rect.size * 2.8;
                        if (rect.containsPoint(location)) {

                            g_gameAudio->playEffect(AUDIO_UI_CLICK);
                            
                            target->stopAllActions();
                            target->runAction(CCEaseSineInOut::create(CCScaleTo::create(0.15f, DEFAULT_CLOUD_PRICE_SCALE)));
                            CCSprite *coin = (CCSprite*)unlockLayer->getChildByTag(priceTag+10000);
                            coin->stopAllActions();
                            coin->runAction(CCEaseSineInOut::create(CCScaleTo::create(0.15f, DEFAULT_CLOUD_PRICE_SCALE)));
                            
                            showDialogPopup(target);
                            
                            clearTouchBegan = true;
                            break;
                        }
                    }
                }
            }
            
            CCObject* _obj = NULL;
            CCARRAY_FOREACH(_touch_sprites, _obj)
            {
                CCSprite *target = (CCSprite*)_obj;
                if(target!=0) {
                    CCRect rect = target->boundingBox();
                    if(target->getTag()!=9999+100000) { //home ไม่ต้องขยายขนาดรับ touch
                        rect.origin = rect.origin - rect.size*0.2;
                        rect.size = rect.size * 1.4;
                    }

                    if (rect.containsPoint(location)) {
                        if(target->getAnchorPoint().x==0.5) {
                            g_gameAudio->playEffect(AUDIO_UI_CLICK);
                            
                            target->stopAllActions();
                            target->runAction(CCEaseSineInOut::create(CCScaleTo::create(0.15f, 1.2f)));
                            clearTouchBegan = true;

                            PlayerWorldMapVillageVO vo = g_playerState->WorldMapVillages[target->getTag()-100000];
                            if(vo.status.compare("other")==0||target->getTag()==9999+100000) {
                                
                                g_playerState->LastRacedVillageIndex = target->getTag() - 100000;
                                showDialogPopup(target);
                            }
                            else {
                                m_dialog = CCPopOverDialog::create(CCSizeMake(500*retinaMultiplier/_scrollView->contentNode->getScale(), 400*retinaMultiplier/_scrollView->contentNode->getScale()), CCPopoverArrowDirectionUp);
                                m_dialog->setAutoDismiss(true);
                                
                                CCLabelBMFont *_titleLabel = CCLabelHelper::createBMFont(retinaMultiplier, g_gameContent->Word["map_ville_fans_title"].c_str(), (kFontSizeMedium), false, kTextColorDark);
                                _titleLabel->setPosition(ccp(m_dialog->getContentSize().width*0.17f,m_dialog->getContentSize().height*0.85f));
                                _titleLabel->setAnchorPoint(ccp(0,1));
                                m_dialog->addChild(_titleLabel);
                                
                                CCLabelBMFont *_fanLabel = CCLabelHelper::createBMFont(retinaMultiplier, g_gameContent->Word["map_ville_fans_description"].c_str(), (kFontSizeMedium), false, kTextColorGreen);
                                _fanLabel->setPosition(ccp(m_dialog->getContentSize().width*0.17f,m_dialog->getContentSize().height*0.62f));
                                _fanLabel->setWidth(m_dialog->getContentSize().width*0.66);
                                _fanLabel->setAnchorPoint(ccp(0,1));
                                m_dialog->addChild(_fanLabel);
                                
                                showPopup=false;
                                m_dialog->setTag(9999999);
                                
                                m_dialog->presentPopover(_scrollView->contentNode, target, true);
                                
                                _titleLabel->setScale(1.0f/_scrollView->contentNode->getScale());
                                _fanLabel->setScale(1.0f/_scrollView->contentNode->getScale());

                            }
                            
                            break;
                        }
                    }
                }
            }
            

            if(!clearTouchBegan) {
                float threshold = 0.0f;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                threshold = 20/retinaMultiplier;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
                threshold = 20/retinaMultiplier;
#endif

                if (fabsf(ccpDistance(dialogPos, _scrollView->getContentOffset()))<threshold)
                {
                    clearPopup(true);
                }
                else if(showPopup)
                {
                    dialogPos = _scrollView->getContentOffset();
                }
            }

            clearPrepareTouch();
        }
    }
}

void WorldMap::clearPrepareTouch()
{
//    CCLog_("Clear prepare touch");
    if(prepareShowPopup) {
        for(int i = 0; i < unlockLayer->getChildrenCount(); i++)
        {
            CCObject* child = unlockLayer->getChildren()->objectAtIndex(i);
            CCLabelBMFont *target = (CCLabelBMFont*)child;
            if(target!= 0) {
                if(target->getScale()<1.0) {
                    target->stopAllActions();
                    target->setScale(1.0f);
                    
                    CCSprite *coin = (CCSprite*)unlockLayer->getChildByTag(target->getTag()+10000);
                    if(coin!= 0) {
                        coin->stopAllActions();
                        coin->setScale(1.0f);
                    }
                    
                    break;
                }
            }
        }
        
        //clear bug village anchorpoint
        CCObject* _obj = NULL;
        CCARRAY_FOREACH(_touch_sprites, _obj)
        {
            CCSprite *target = (CCSprite*)_obj;
            if(target->getAnchorPoint().x==0.5) {
                
                target->stopAllActions();
                target->setAnchorPoint(ccp(0,0));
                target->setScale(1.0f);
                target->setPosition(ccp(target->getPositionX()-target->getContentSize().width*0.5,target->getPositionY()));
                
                break;
            }
        }
    }
    
    prepareShowPopup = false;
}

void WorldMap::showDialogPopup(CCObject* pObj)
{
    CCSprite *target = (CCSprite*)pObj;
    target->stopAllActions();
    target->setScale(1.0f);
    
//    CCLog_("Show dialog Popup with tag = %d",target->getTag());
    
    if(prepareShowPopup) {
        if(target->getTag()==9999+100000) {
//            target->setScale(1.0f);
            target->setAnchorPoint(ccp(0,0));
            target->setPosition(ccp(target->getPositionX()-target->getContentSize().width*0.5,target->getPositionY()));

            clearPopup(true);
            
            if(numLog>0) {
//                showClaimDialog(target);
            }
            else
            {
                //===================================================================
                
                CCMenu *_homeButton = CCMenuHelper::createMenu(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::PossibleRewardsVisitButton)), this, menu_selector(WorldMap::touchHomeButton));
                CCNode *_homeButtonItem = CCMenuHelper::getMenuItemScaleSprite(_homeButton);
                
                CCSize dialogSize = CCSizeMake((_homeButton->getContentSize().width + 90.0f * sizeMultiplier),
                                               (_homeButton->getContentSize().height + 60.0f * sizeMultiplier));
                
                m_dialog = CCPopOverDialog::create(dialogSize/_scrollView->contentNode->getScale()
                                                   , CCPopoverArrowDirectionUp);
                m_dialog->setAutoDismiss(false);
                
                m_dialog->presentPopover(_scrollView->contentNode, target, true);
                m_dialog->setTag(target->getTag());
                
                showPopup = true;
                dialogPos = _scrollView->getContentOffset();
                
                _homeButton->setAnchorPoint(ccp(0.0f, 0.0f));
                _homeButton->setPosition((dialogSize/_scrollView->contentNode->getScale())*0.5f);
                m_dialog->addChild(_homeButton);
                
                
                CCLabelBMFont *_homeLabel = CCLabelHelper::createBMFont(g_gameContent->Word["map_ville_button_go_home"].c_str(), (kFontSizeLarge-3.0f), true);
                _homeLabel->setPosition(ccp(_homeButton->getContentSize().width*0.5f, _homeButton->getContentSize().height*0.55));
                _homeButtonItem->addChild(_homeLabel);
                _homeButton->setScale(MAP_BUTTON_SCALE);
            }
            
            
        }
        else if(target->getTag()>99999) { // touch village
//            target->setScale(1.0f);
            target->setAnchorPoint(ccp(0,0));
            target->setPosition(ccp(target->getPositionX()-target->getContentSize().width*0.5,target->getPositionY()));

            clearPopup(true);
            
            //---------------------------------------
            
            _selected_village = target;
            
            PlayerWorldMapVillageVO vo = g_playerState->WorldMapVillages[target->getTag()-100000];
            if(vo.timestamp+vo.wait>currentTime || !g_playerState->Checklist[TutSecondRacePart1::key()]) {
                
                visitButton = CCMenuHelper::createMenu(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::PossibleRewardsVisitButton)), this, menu_selector(WorldMap::touchVisitButton));
                CCNode *visitButtonItem = CCMenuHelper::getMenuItemScaleSprite(visitButton);
                
                CCSize dialogSize = CCSizeMake((visitButton->getContentSize().width + 90.0f * sizeMultiplier),
                                               (visitButton->getContentSize().height + 60.0f * sizeMultiplier));
                
                m_dialog = CCPopOverDialog::create(dialogSize/_scrollView->contentNode->getScale()
                                                   , CCPopoverArrowDirectionUp);
                m_dialog->setAutoDismiss(false);
                m_dialog->presentPopover(_scrollView->contentNode, target, true);
                m_dialog->setTag(target->getTag());
                
                showPopup = true;
                dialogPos = _scrollView->getContentOffset();
                
                visitButton->setAnchorPoint(ccp(0.0f, 0.0f));
                visitButton->setPosition((dialogSize/_scrollView->contentNode->getScale())*0.5f);
                
                m_dialog->addChild(visitButton);
                visitButtonItem->setTag(target->getTag());
                
                
                CCLabelBMFont *_visitLabel = CCLabelHelper::createBMFont(g_gameContent->Word["map_ville_button_visit"].c_str(), kFontSizeLarge-3.0f, true);
                _visitLabel->setPosition(ccp(visitButton->getContentSize().width*0.5f, visitButton->getContentSize().height*0.55));
                visitButtonItem->addChild(_visitLabel);
                visitButton->setScale(MAP_BUTTON_SCALE);
            }
            else
            {
                //can search for new opponent
                CCMenu *_changeButton = CCMenuHelper::createMenu(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::PossibleRewardsChangeButton)), this, menu_selector(WorldMap::touchChangeButton));
                CCNode *_changeButtonItem = CCMenuHelper::getMenuItemScaleSprite(_changeButton);
                
                visitButton = CCMenuHelper::createMenu(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::PossibleRewardsVisitButton)), this, menu_selector(WorldMap::touchVisitButton));
                
                CCNode *visitButtonItem = CCMenuHelper::getMenuItemScaleSprite(visitButton);
                
                CCSize dialogSize = CCSizeMake((_changeButton->getContentSize().width + 90.0f * sizeMultiplier),
                                               (_changeButton->getContentSize().height + 10.0f * sizeMultiplier)
                                               + (visitButton->getContentSize().height + 40.0f * sizeMultiplier));
                m_dialog = CCPopOverDialog::create(dialogSize/_scrollView->contentNode->getScale()
                                                   , CCPopoverArrowDirectionUp);
                m_dialog->setAutoDismiss(false);
                m_dialog->presentPopover(_scrollView->contentNode, target, true);
                
                showPopup = true;
                dialogPos = _scrollView->getContentOffset();
                
                _changeButton->setAnchorPoint(ccp(0.0f, 0.0f));
                _changeButton->setPosition(CCSizeMake(dialogSize.width*0.5f,
                                                      dialogSize.height*0.72f)
                                           /_scrollView->contentNode->getScale());
                m_dialog->addChild(_changeButton);
                _changeButtonItem->setTag(target->getTag());
                _changeButton->setScale(MAP_BUTTON_SCALE);
                
                CCLabelBMFont *_changeLabel = CCLabelHelper::createBMFont(g_gameContent->Word["map_ville_button_new_opponent"].c_str()
                                                                          , kFontSizeMedium-3.0f, true, kTextColorWhite,
                                                                          _changeButton->getContentSize().width * 0.7f,
                                                                          kCCTextAlignmentCenter);
                _changeButtonItem->addChild(_changeLabel);
                
                CCSprite *changeMark = UIHelper::getSprite(UIHelper::UISprites(UIHelper::PossibleRewardsChangeIcon));
                _changeButtonItem->addChild(changeMark);
                
                UIHelper::setTwoNodesToMiddlePosition(changeMark, _changeLabel, 20.0f*sizeMultiplier, ccp(_changeButton->getContentSize().width*0.5f, _changeButton->getContentSize().height*0.55f));
                
                visitButton->setAnchorPoint(ccp(0.0f, 0.0f));
                visitButton->setPosition(CCSizeMake(dialogSize.width*0.5f,
                                                    dialogSize.height*0.27f)
                                         /_scrollView->contentNode->getScale());
                m_dialog->addChild(visitButton);
                visitButtonItem->setTag(target->getTag());
                visitButton->setScale(MAP_BUTTON_SCALE);
                
                CCLabelBMFont *_visitLabel = CCLabelHelper::createBMFont(g_gameContent->Word["map_ville_button_visit"].c_str(), kFontSizeLarge-3.0f, true);
                _visitLabel->setPosition(ccp(visitButton->getContentSize().width*0.5f, visitButton->getContentSize().height*0.55));
                visitButtonItem->addChild(_visitLabel);
            }
        } else { //touch unlock cloud
            
            int cloudTag = target->getTag()-20000;
            
            CCLog_("Cloud Unlock Tag = %d",cloudTag);
            
            CCMenu *_exploreButton = CCMenuHelper::createMenu(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::PossibleRewardsVisitButton)), this, menu_selector(WorldMap::touchExploreButton));
            CCNode *_exploreButtonItem = CCMenuHelper::getMenuItemScaleSprite(_exploreButton);

            clearPopup(true);
            
            CCSize dialogSize = CCSizeMake((_exploreButton->getContentSize().width + 90.0f * sizeMultiplier),
                                           (_exploreButton->getContentSize().height + 60.0f * sizeMultiplier));
            
            m_dialog = CCPopOverDialog::create(dialogSize/_scrollView->contentNode->getScale()
                                               , CCPopoverArrowDirectionAny);
            m_dialog->setAutoDismiss(false);
            
            m_dialog->presentPopover(_scrollView->contentNode, target, true);
            m_dialog->setTag(cloudTag);
            
            showPopup = true;
            dialogPos = _scrollView->getContentOffset();
            
            //=======highlight cloud=========
            CCSprite *ownCloud = (CCSprite*)_cloudbatchNode->getChildByTag(cloudTag);
            ownCloud->setColor(ccc3(176,224,230));
            
            for (int j=0; j<LINKED_CLOUD_MAX; ++j) {
                if(_cloudList.at(cloudTag).link_cloud[j]>0) {
                    CCLog_("%d", _cloudList.at(cloudTag).link_cloud[j]);
                    CCSprite *linkCloud = (CCSprite*)_cloudbatchNode->getChildByTag(_cloudList.at(cloudTag).link_cloud[j]);
                    linkCloud->setColor(ccc3(176,224,230));
                }
            }
            
            _exploreButton->setAnchorPoint(ccp(0.0f, 0.0f));
            _exploreButton->setPosition((dialogSize/_scrollView->contentNode->getScale())*0.5f);
            m_dialog->addChild(_exploreButton);
            _exploreButtonItem->setTag(cloudTag);
            
            CCLabelBMFont *_exploreLabel = CCLabelHelper::createBMFont(g_gameContent->Word["map_ville_button_explore"].c_str(), kFontSizeLarge-3.0f, true);
            _exploreLabel->setPosition(ccp(_exploreButton->getContentSize().width*0.5f, _exploreButton->getContentSize().height*0.55f));
            _exploreButtonItem->addChild(_exploreLabel);
            _exploreLabel->setTag(cloudTag+1);
            
            _exploreButton->setScale(MAP_BUTTON_SCALE);
        }
    }
    
    clearPrepareTouch();
}

void WorldMap::touchExploreButton(cocos2d::CCObject *sender) {
    
    if(!canTouch) return;
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    CCNode *target = (CCNode*)sender; //button = cloud = price coins tag
    indexUnlockCloud = target->getTag();
    CCLabelBMFont* unlockCoin = (CCLabelBMFont*)unlockLayer->getChildByTag(target->getTag()+20000);
    if(unlockCoin!=0) _posCloud = unlockCoin->getPosition();
    
    GDWorldCloudVO cloud = _cloudList.at(target->getTag());
    if (g_playerState->player.balance.gold < cloud.price) {
        SceneWorldMap* _command = (SceneWorldMap*)g_currentScene;
        if (!_command->modalController->getParent()) {
            CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
            InstantDialog *_dialog = InstantDialog::createWithType(_command->modalController, IT_CLOUD_BUYRESOURCE, &cloud, NULL);
            _dialog->setPosition(_screenSize*0.5f);
            _command->addChild(_command->modalController, 100);
            _command->modalController->pushView(_dialog);
        }
        return;
    }

    setAnimateWaiting();
    BackendService::getInstance()->send(APIMapCloudOpen::create(_cloudList.at(target->getTag()).group,
                                                                false,
                                                                this,
                                                                WorldMap::onCloudOpenned));
    
}

void WorldMap::setAnimateWaiting() {
    
    canTouch = false;
    clearPopup(false);
    
    _wait = CCSprite::createWithSpriteFrameName("compass_arrows.png");
    _wait->setPosition(_posCloud);
    _wait->setFlipX(true);
    _scrollView->addChild(_wait,4);
    _wait->setTag(indexUnlockCloud);
    
    _bus = CCSprite::createWithSpriteFrameName("compass.png");
    _bus->setPosition(_posCloud);
    _bus->setScale(0.75);
    _scrollView->addChild(_bus,4);
    
    _wait->stopAllActions();
    _wait->runAction(CCRepeatForever::create(CCRotateBy::create(5.0f, 360)));
    
    CCLabelBMFont* unlockCoin = (CCLabelBMFont*)unlockLayer->getChildByTag(indexUnlockCloud+20000);
    unlockCoin->setString(CCString::createWithFormat("")->getCString());
    
    CCSprite* coin = (CCSprite*)unlockLayer->getChildByTag(indexUnlockCloud+10000);
    coin->setVisible(false);
    
}

void WorldMap::removeNewBalloon()
{
    CCSprite *newBalloon = (CCSprite*)_scrollView->contentNode->getChildByTag(200000);
    if(newBalloon) {
        newBalloon->stopAllActions();
        newBalloon->removeFromParent();
    }
}


void WorldMap::touchHomeButton(cocos2d::CCObject *sender)
{
    if(canTouch) {

        g_sceneManager->showBlockingLayer();

        g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
        
        canTouch = false;
        clearPopup(false);
        
        _scrollView->setMaximumScale(1.75f);
        _scrollView->zoomOnPosition(ccp(homeX, homeY), MAP_MAXIMUM_SCALE_FOR_ZOOMING, MAP_ZOOM_DURATION);
        this->runAction(CCSequence::create(
                                           CCDelayTime::create(MAP_ZOOM_DURATION+0.1f),
                                           CCCallFuncO::create(this, callfuncO_selector(WorldMap::gotoVILL), NULL),
                                           NULL));
    }
}

void WorldMap::touchChangeButton(cocos2d::CCObject *sender) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    clearPopup(true);
    CCSprite *target = (CCSprite*)sender;
    tagChange = target->getTag()-100000;
    
    if (typeid(*g_currentScene) == typeid(SceneWorldMap)) {
        if (SceneWorldMap* _sceneWorldMap = (SceneWorldMap*)g_currentScene) {
            if (_sceneWorldMap->modalController->getNStackView() == 0) {
                if (AlertDialog* _dialog = AlertDialog::create(_sceneWorldMap->modalController,
                                                               g_gameContent->Word["map_ville_findnewoppo_confirmdialog_title"].c_str(),
                                                               g_gameContent->Word["map_ville_findnewoppo_confirmdialog_message"].c_str(),
                                                               g_gameContent->Word["okay"].c_str(),
                                                               g_gameContent->Word["cancel"].c_str(),
                                                               true,
                                                               this, WorldMap::onNewOpponent)) {
                    _dialog->setPosition(_sceneWorldMap->getContentSize()*0.5f);
                    addChild(_sceneWorldMap->modalController, 100);
                    _sceneWorldMap->modalController->pushView(_dialog);
                }
            }
        }
    }
    
}

void WorldMap::touchVisitButton(cocos2d::CCObject *sender)
{
    g_sceneManager->showBlockingLayer();

    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    canTouch = false;
    clearPopup(false);
    
    CCNode *target = (CCNode*)sender;
    CCObject* _obj = NULL;
    CCARRAY_FOREACH(_touch_sprites, _obj)
    {
        CCSprite *vill = (CCSprite*)_obj;
        
        CCLog_("COMPARE TAG: %d == %d", vill->getTag(), target->getTag());
        if (vill->getTag()==target->getTag()) {
            indexVisit=vill->getTag();
            
            CCLog_("index vill = %d",indexVisit);
            
            _scrollView->setMaximumScale(1.75f);
            _scrollView->zoomOnPosition(ccp(vill->getPositionX()+vill->getContentSize().width*0.5,vill->getPositionY()+vill->getContentSize().height*0.5), MAP_MAXIMUM_SCALE_FOR_ZOOMING, MAP_ZOOM_DURATION);

            PlayerWorldMapVillageVO vo = g_playerState->WorldMapVillages[vill->getTag()-100000];
            if (vo.village_id != 0) {
//                CCLog_("id=%d, uuid=%s, name=%s", vo.village_id, vo.uuid.c_str(), vo.name.c_str());
                snprintf(select_uuid, 256, "%s", vo.uuid.c_str());
            }
            break;
        }
    }
    
    this->runAction(CCSequence::create(
                                       CCDelayTime::create(MAP_ZOOM_DURATION+0.1f),
                                       CCCallFuncO::create(this, callfuncO_selector(WorldMap::gotoMATCH), NULL),
                                       NULL));
}

void WorldMap::gotoVILL() {

    if (SceneWorldMap* _currentScene = (SceneWorldMap*)g_currentScene) {
        _currentScene->fadeOutButWaitForCurrentTime(SCENE_VILLE);
    }
    
}

void WorldMap::gotoMATCH() {
    
#ifdef DEBUG_SELECT_UUID
    snprintf(select_uuid, 256, DEBUG_SELECT_UUID);
#endif
    
    CustomEventsHelper::Instance().checkAndIncreaseTimesUsedFor(kIdleAnimationTapVill);
    
    CCLog_("visit uuid: %s", select_uuid);
    if (SceneWorldMap* _currentScene = (SceneWorldMap*)g_currentScene) {
        _currentScene->fadeOutButWaitForOpponent(SCENE_MATCH, select_uuid);
    }
    
}

void WorldMap::updateCloudPriceColor() {
    
    for (int i=0; i<maxCloud; ++i) {
        if(!_cloudList.at(i).active&&_cloudList.at(i).unlock_level>-1) {
            
//            if (CCLabelBMFont* priceLabel = (CCLabelBMFont*)unlockLayer->getChildByTag(i)) {
            if (CCLabelBMFont* priceLabel = (CCLabelBMFont*)unlockLayer->getChildByTag(i+20000)) {
                if (g_playerState->player.balance.gold < _cloudList.at(i).price) {
                    priceLabel->setColor(kTextColorOrange2);
                } else {
                    priceLabel->setColor(kTextColorWhite);
                }
            }
        }
    }
}

void WorldMap::openMapWithAnimation(int idCloud) {
    
    g_gameAudio->playEffect(AUDIO_MAP_OPEN);
    id_cloudOpen = idCloud;
    
    CCSprite *tempCloud = (CCSprite*)_cloudbatchNode->getChildByTag(idCloud);
    if (tempCloud) {
        tempCloud->stopAllActions();
        tempCloud->runAction(CCSequence::create(CCMoveBy::create(0.2f, ccp(0,15*retinaMultiplier)),
                                                CCDelayTime::create(0.2f),
                                                CCCallFuncO::create(this, callfuncO_selector(WorldMap::animateOpenCloud), NULL),
                                                NULL));
    }
    
    for (int j=0; j<LINKED_CLOUD_MAX; ++j) {
        if(_cloudList.at(idCloud).link_cloud[j]>0) {
            tempCloud = (CCSprite*)_cloudbatchNode->getChildByTag(_cloudList.at(idCloud).link_cloud[j]);
            if (tempCloud) {
                tempCloud->stopAllActions();
                tempCloud->runAction(CCSequence::create(CCMoveBy::create(0.2f, ccp(0,15*retinaMultiplier)),
                                                        NULL));
            }
            
        }
    }
}

void WorldMap::openMapWithIndex(int idCloud) {
    //=======remove cloud=========
    _cloudList.at(idCloud).active = true;
    _cloudbatchNode->removeChildByTag(idCloud);
    
    for (int j=0; j<LINKED_CLOUD_MAX; ++j) {
        if(_cloudList.at(idCloud).link_cloud[j]>0) {
            _cloudList.at(_cloudList.at(idCloud).link_cloud[j]).active = true;
            _cloudbatchNode->removeChildByTag(_cloudList.at(idCloud).link_cloud[j]);
        }
    }
}

void WorldMap::animateOpenCloud() {

    CCSprite *tempCloud = (CCSprite*)_cloudbatchNode->getChildByTag(id_cloudOpen);
    tempCloud->stopAllActions();
    tempCloud->runAction(CCSpawn::create(CCEaseExponentialOut::create(CCScaleTo::create(0.5f, 2.25f*CLOUD_ADDITIONAL_SCALING)),
                                         CCFadeOut::create(0.5f),
                                         NULL));
    
    for (int j=0; j<LINKED_CLOUD_MAX; ++j) {
        if(_cloudList.at(id_cloudOpen).link_cloud[j]>0) {
            tempCloud = (CCSprite*)_cloudbatchNode->getChildByTag(_cloudList.at(id_cloudOpen).link_cloud[j]);
            tempCloud->stopAllActions();
            tempCloud->runAction(CCSpawn::create(CCEaseExponentialOut::create(CCScaleTo::create(0.5f, 2.25f*CLOUD_ADDITIONAL_SCALING)),
                                                 CCFadeOut::create(0.5f),
                                                 NULL));
            
        }
    }
    
    checkVillActivate();
    
    scheduleOnce(schedule_selector(WorldMap::removeOpenCloud), 1.0f);

}

void WorldMap::removeOpenCloud() {
    
    //=======remove cloud=========
    _cloudbatchNode->removeChildByTag(id_cloudOpen);
    for (int j=0; j<LINKED_CLOUD_MAX; ++j) {
        if(_cloudList.at(id_cloudOpen).link_cloud[j]>0) {
            _cloudbatchNode->removeChildByTag(_cloudList.at(id_cloudOpen).link_cloud[j]);
        }
    }
    
    Building *garageCheck = g_playerState->getBuilding(kBuildingGarage, 0);
    
    if(garageCheck) canTouch = true;
    else if(!g_playerState->Checklist["tutorialGarage"]) { //ยังไม่เคยสอน garage
        checkCloudUnlockTutorial();
    }
    else canTouch=true;
    
}


void WorldMap::updateNewVillage(std::string resp_json_str_) {

    //JsonAllocator allocator;
    Json *_root = Json_create(resp_json_str_.c_str());
    
    if(_root) {
        Json *_error = Json_getItem(_root, "error");
        Json *_warning = Json_getItem(_root, "warning");
        if(_error || _warning) {
            canTouch=true;
            CCLog_("======== New Opponent error =======");
        } else {
            
            if (Json *_villages = Json_getItem(_root, "villages")) {
                if (Json *_village = Json_getItem(_villages, CCString::createWithFormat("%d",tagChange)->getCString())) { //it isn't an array, so we have to access like dictionary
                    
                    if (CCSprite *updateVill = (CCSprite*)_mapbatchNode->getChildByTag(tagChange+100000)) {
                    
                        updateVill->setAnchorPoint(ccp(0.5,0));
                        updateVill->setPosition(ccp(updateVill->getPositionX()+updateVill->getContentSize().width/2,updateVill->getPositionY()));
                        updateVill->stopAllActions();
                        
                        g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, g_gameContent->Word["map_ville_found_new_opponent"].c_str());
                        
                        PlayerWorldMapVillageVO vo = g_playerState->WorldMapVillages[tagChange];
                        if (vo.status.compare("empty") == 0) {
                            
                            //if (CCSprite* avatarFrame = static_cast<CCSprite*>(updateVill->userObject)) {
                                //avatarFrame->removeFromParent();
                            //}
                            
                            updateVill->removeFromParent();
                            if (CCLabelBMFont* ttf1 = (CCLabelBMFont*)_scrollView->contentNode->getChildByTag(tagChange)) {
                                ttf1->removeFromParent();
                            }
                            
                            if (CCSprite *rankPic = (CCSprite*)_avatarbatchNode->getChildByTag(tagChange))
                            {
                                rankPic->removeFromParent();
                            }
                            
                            if (CCLabelBMFont *updatePowerLabel = (CCLabelBMFont*)_scrollView->contentNode->getChildByTag(tagChange+50000)) {
                                updatePowerLabel->removeFromParent();
                            }
                            
                            if (CCSprite *markChange = (CCSprite*)_avatarbatchNode->getChildByTag(tagChange+50000)) {
                                markChange->removeFromParent();
                            }
                            
                            canTouch = true;
                            
                        }else {
                            
                            std::string name = Json_getString(_village, "name", "");
                            int power = Json_getInt(_village, "horse_power", 0);
                            
                            CCLabelBMFont* ttf1 = (CCLabelBMFont*)_scrollView->contentNode->getChildByTag(tagChange);
                            if(ttf1) {
                                ttf1->setString(CCString::createWithFormat("%s", name.c_str())->getCString());
                            }
                            
                            CREATE_CHAR_BUFFER(buf_, 32);
                            
                            
                            
//                            for(std::vector<GDPetVO>::iterator it=g_gameContent->Pet.begin(); it!=g_gameContent->Pet.end(); ++it) {
//                                GDPetVO _vo = (GDPetVO)(*it);
                            GDPetVO _vo = g_gameContent->getPetVO(vo.host_type);
                            
                                if (_vo.type == vo.host_type) {
                                    //CCLog_("== %s OK Match ==",_vo.type.c_str());
                                    snprintf(buf_, buf__size, "map_class_%d.png", _vo.rarity_stars);
                                    
                                    CCSprite *rankPic = (CCSprite*)_avatarbatchNode->getChildByTag(tagChange);
                                    if(rankPic) {
                                        CCSpriteFrame *sp = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(buf_);
                                        rankPic->setDisplayFrame(sp);
                                    }
//                                    break;
                                }
//                            }
                            
                            CCLabelBMFont *updatePowerLabel = (CCLabelBMFont*)_scrollView->contentNode->getChildByTag(tagChange+50000);
                            if(updatePowerLabel) {
                                CREATE_CHAR_BUFFER(buf_, 8);
                                snprintf(buf_, buf__size, "%d", power);
                                updatePowerLabel->setString(buf_);
                            }
                            
                            CCSprite *markChange = (CCSprite*)_avatarbatchNode->getChildByTag(tagChange+50000);
                            if(markChange) {
                                markChange->setVisible(false);
                            }

                            CCLog_("Village run action 3");
                            updateVill->runAction(CCSequence::create(CCEaseSineInOut::create(CCScaleTo::create(0.1f, 1.2f)),
                                                                     CCEaseSineInOut::create(CCScaleTo::create(0.2f, 1.0f)),
                                                                     CCCallFuncO::create(this, callfuncO_selector(WorldMap::stopChangeVill), NULL),
                                                                     NULL));
                        }

                    }
                }
            }
        }
        
        Json_dispose(_root);
    }
    

}

void WorldMap::updateVillages() {
    
    for (std::map<int, PlayerWorldMapVillageVO>::iterator it = g_playerState->WorldMapVillages.begin(); it != g_playerState->WorldMapVillages.end(); ++it) {
        
        if (CCSprite *updateVill = (CCSprite*)_mapbatchNode->getChildByTag((*it).first+100000)) {
        
            updateVill->setAnchorPoint(ccp(0.5,0));
            updateVill->setPosition(ccp(updateVill->getPositionX()+updateVill->getContentSize().width/2,updateVill->getPositionY()));
            updateVill->stopAllActions();
            
            PlayerWorldMapVillageVO vo = (*it).second;
            
            if (vo.status.compare("empty") == 0) {
                
                //if (CCSprite* avatarFrame = static_cast<CCSprite*>(updateVill->userObject)) {
                    //avatarFrame->removeFromParent();
                //}
                
                updateVill->removeFromParent();
                if (CCLabelBMFont* ttf1 = (CCLabelBMFont*)_scrollView->contentNode->getChildByTag(tagChange)) {
                    ttf1->removeFromParent();
                }
                
                //                            CREATE_CHAR_BUFFER(buf_, 32);
                //                            for(std::vector<GDPetVO>::iterator it=g_gameContent->Pet.begin(); it!=g_gameContent->Pet.end(); ++it) {
                //                                GDPetVO _vo = (GDPetVO)(*it);
                //
                //                                if (_vo.type == vo.host_type) {
                //                                    //CCLog_("== %s OK Match ==",_vo.type.c_str());
                //                                    snprintf(buf_, buf__size, "map_horsePower_rank_%d.png", _vo.rarity_stars);
                
                if (CCSprite *rankPic = (CCSprite*)_avatarbatchNode->getChildByTag(tagChange)) {
                    //                                        if (CCSpriteFrame *sp = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(buf_)) {
                    //                                            rankPic->setDisplayFrame(sp);
                    //                                        }
                    rankPic->removeFromParent();
                }
                //                                    break;
                //                                }
                //                            }
                
                if (CCLabelBMFont *updatePowerLabel = (CCLabelBMFont*)_scrollView->contentNode->getChildByTag(tagChange+50000)) {
                    updatePowerLabel->removeFromParent();
                }
                
                if (CCSprite *markChange = (CCSprite*)_avatarbatchNode->getChildByTag(tagChange+50000)) {
                    markChange->removeFromParent();
                }
                
                canTouch = true;
                
            }else {
                
                std::string name = vo.name; //Json_getString(_village, "name", "");
                int power = vo.horse_power;// Json_getInt(_village, "horse_power", 0);
                
                CCLabelBMFont* ttf1 = (CCLabelBMFont*)_scrollView->contentNode->getChildByTag(tagChange);
                if(ttf1) {
                    ttf1->setString(name.c_str());
                }
                
                CREATE_CHAR_BUFFER(buf_, 32);
                
                
                
                //                            for(std::vector<GDPetVO>::iterator it=g_gameContent->Pet.begin(); it!=g_gameContent->Pet.end(); ++it) {
                //                                GDPetVO _vo = (GDPetVO)(*it);
                GDPetVO _vo = g_gameContent->getPetVO(vo.host_type);
                
                if (_vo.type == vo.host_type) {
                    //CCLog_("== %s OK Match ==",_vo.type.c_str());
                    snprintf(buf_, buf__size, "map_class_%d.png", _vo.rarity_stars);
                    
                    CCSprite *rankPic = (CCSprite*)_avatarbatchNode->getChildByTag(tagChange);
                    if(rankPic) {
                        CCSpriteFrame *sp = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(buf_);
                        rankPic->setDisplayFrame(sp);
                    }
                    //                                    break;
                }
                //                            }
                
                CCLabelBMFont *updatePowerLabel = (CCLabelBMFont*)_scrollView->contentNode->getChildByTag(tagChange+50000);
                if(updatePowerLabel) {
                    char buf_[8] = {0};
                    snprintf(buf_, buf__size, "%d", power);
                    updatePowerLabel->setString(buf_);
                }
                
                CCSprite *markChange = (CCSprite*)_avatarbatchNode->getChildByTag(tagChange+50000);
                if(markChange) {
                    markChange->setVisible(false);
                }

                updateVill->runAction(CCSequence::create(CCEaseSineInOut::create(CCScaleTo::create(0.1f, 1.2f)),
                                                         CCEaseSineInOut::create(CCScaleTo::create(0.2f, 1.0f)),
                                                         CCCallFuncO::create(this, callfuncO_selector(WorldMap::stopChangeVill), NULL),
                                                         NULL));
            }
            
         }
        
    }

}

void WorldMap::stopChangeVill() {
    
    if (CCSprite *updateVill = (CCSprite*)_mapbatchNode->getChildByTag(tagChange+100000)) {
        updateVill->stopAllActions();
        updateVill->setAnchorPoint(ccp(0,0));
        updateVill->setScale(1.0f);
        updateVill->setPosition(ccp(updateVill->getPositionX()-updateVill->getContentSize().width/2,updateVill->getPositionY()));
    }
    canTouch = true;
    
}

//void WorldMap::nextLog()
//{
//    if(numLog>0) {
//        CCSprite *lastVill = (CCSprite*)_mapbatchNode->getChildByTag(9999+100000);
//        showClaimDialog(lastVill);
//    }
//}

bool WorldMap::collectGems(ChestGems* chestgems_) {
    
    // collect coins
    if (collectableGems != NULL && chestgems_ != NULL) {
        
        int _collect_amount = chestgems_->cgVo.gems;
        collectableGems->add(chestgems_->getParent()->convertToWorldSpace(chestgems_->getPosition()), getParent()->getScale(), _collect_amount);
        
        hudPanel->resourceBars[HUDPanelBarGems]->addTargetAmount(g_playerState->player.balance.gem, AUDIO_FUSE_EVOLVE_2);
        
        CREATE_CHAR_BUFFER(_buffer, 64);
        snprintf(_buffer, _buffer_size, "%d", _collect_amount);
        
        float totalAnimationTime = kCollectAnimTotalAnimationTime;
        float fadeTime = kCollectAnimFadeTime;
        
        CCLabelBMFont *_collectedAmountLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeVeryLarge, true);
        _collectedAmountLabel->setColor(kTextColorGem);
        _collectedAmountLabel->setPosition(ccp(0.0f, chestgems_->getContentSize().height*0.7f*retinaMultiplier));
        chestgems_->addChild(_collectedAmountLabel, 10);
        _collectedAmountLabel->
        runAction(CCSequence::create(CCSpawn::create(CCSequence::create(CCDelayTime::create(totalAnimationTime-fadeTime),
                                                                        CCFadeOut::create(fadeTime),
                                                                        NULL),
                                                     CCMoveBy::create(totalAnimationTime, ccp(0.0f, kCollectAnimDistanceMoved*retinaMultiplier))
                                                     ,
                                                     NULL)
                                     ,
                                     CCRemoveSelf::create(),
                                     NULL));
        
    }
    
    return false;
    
}

#pragma mark - API Callback

void WorldMap::onCloudOpenned(void *sender_, std::string resp_str) {
    
    PlayerState* _playerState = PlayerState::getInstance();
    
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseUserBalance(_root);
    _playerState->parseWorldMapVillages(_root);
    Json_dispose(_root);
    
    SceneWorldMap* _sceneWorldMap = (SceneWorldMap*)g_currentScene;
    if (typeid(*_sceneWorldMap) != typeid(SceneWorldMap)) return;
    
    _sceneWorldMap->worldMap->openMapWithAnimation(_sceneWorldMap->worldMap->indexUnlockCloud);
    _sceneWorldMap->worldMap->_cloudList.at(_sceneWorldMap->worldMap->id_cloudOpen).active = true;
    for (int j=0; j<LINKED_CLOUD_MAX; ++j) {
        if(_sceneWorldMap->worldMap->_cloudList.at(_sceneWorldMap->worldMap->id_cloudOpen).link_cloud[j]<=0) continue;
        _sceneWorldMap->worldMap->_cloudList.at(_sceneWorldMap->worldMap->_cloudList.at(_sceneWorldMap->worldMap->id_cloudOpen).link_cloud[j]).active = true;
    }
    
    _sceneWorldMap->worldMap->_wait->stopAllActions();
    _sceneWorldMap->worldMap->_wait->removeFromParent();
    _sceneWorldMap->worldMap->_bus->removeFromParent();
    
    _sceneWorldMap->worldMap->canTouch = true;
    _sceneWorldMap->worldMap->updateCloudPriceColor();
    _sceneWorldMap->addMoreTutorial();
    
    _sceneWorldMap->hudPanel->updateAllResourceBars();
    
}

void WorldMap::onNewOpponent(void *sender_, bool confirm_) {
    
    if (typeid(*g_currentScene) != typeid(SceneWorldMap)) return;
    
    if (WorldMap* _this = (WorldMap*)sender_) {
        if (typeid(*_this) != typeid(WorldMap)) return;
        if (confirm_) {
            _this->canTouch = false;
            BackendService::getInstance()->send(APIMapOpponentNew::create(_this->tagChange,
                                                                          _this,
                                                                          WorldMap::onNewOpponnentReceived));
        }
    }
    
}

void WorldMap::onNewOpponnentReceived(void *sender, std::string resp_str) {
    
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    g_playerState->parseWorldMapVillages(_root);
    Json_dispose(_root);
    
    if (typeid(*g_currentScene) != typeid(SceneWorldMap)) return;
    
    if (WorldMap* _this = (WorldMap*)sender) {
        _this->updateNewVillage(resp_str);
    }
    
}

bool WorldMap::isAllLegalCloudsOpenned() {
    
    int count_unlocked = 0;
    int count_active = 0;
    
    for (std::vector<GDWorldCloudVO>::iterator it = _cloudList.begin(); it != _cloudList.end(); ++it) {
        GDWorldCloudVO vo = *it;
        if (vo.unlock_level == 0) {
            count_unlocked ++;
            for (int i=0; i<LINKED_CLOUD_MAX; i++) {
                if (vo.link_cloud[i] >= 0) {
                    count_unlocked++;
                }
            }
        }
        if (vo.active) {
            count_active ++;
        }
    }
    CCLog_("unlocked = %d", count_unlocked);
    CCLog_("active = %d", count_active);
    
    return count_unlocked == count_active;
    
}

