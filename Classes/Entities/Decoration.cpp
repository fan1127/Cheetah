//
//  Decoration.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/25/2556 BE.
//
//

#include "Decoration.h"
USING_NS_CC;
using namespace cocos2d::extension;
using namespace std;

#include "../AppMacros.h"
extern TargetDevice targetDevice;

#include "../SceneManager.h"
extern SceneManager *g_sceneManager;

extern float scaleFactor;
//extern CCPoint screenFactor;
extern unsigned int currentTime;

#include "../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../Services/BackendService.h"
#include "../Services/APIs/APIDecorationUpdate.h"

#include "../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

Decoration *Decoration::createWithDecorationVO(GDDecorationVO dvo_) {
    
    Decoration *pRet = new Decoration();
    if (pRet->initWithDecorationVO(dvo_, PlayerDecorationVO())) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
    
}

Decoration *Decoration::createWithDecorationVO(GDDecorationVO dvo_, PlayerDecorationVO pdvo_) {
    
    Decoration *pRet = new Decoration();
    if (pRet->initWithDecorationVO(dvo_, pdvo_)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
    
}

bool Decoration::initWithDecorationVO(GDDecorationVO dvo_, PlayerDecorationVO pdvo) {
    
    if (!CCNode::init()) {
        return false;
    }
    
    dVo = dvo_;
    pdVo = pdvo;
    pdVo.type = dVo.type;
    
    spineDecoration = NULL;
    didMove = false;
    
    createDecoration(false);

    return true;
    
}

bool Decoration::createDecoration() {
    
    return createDecoration(true);
    
}

bool Decoration::createDecoration(bool bumping_) {
    
    if (!m_spineLayer) {
        m_spineLayer = CCLayer::create();
        addChild(m_spineLayer);
    }
    
    if (!spineDecoration) {
        
        spineDecoration = g_sceneManager->sharedSkeletonData->getSkeleton(kDecorationSrcPath, dVo.src.c_str());
        //spineDecoration->preferenceRootParent = preferenceRootParent;
        spineDecoration->setContentSize(CCSize(dVo.src_width, dVo.src_height));
        spineDecoration->timeScale = 1.0f;
        if (targetDevice != TD_IPAD_RETINA) {
            spineDecoration->setScale(0.5f);
        }
        spineDecoration->setAnimation("idle", true);
        m_spineLayer->addChild(spineDecoration, 1);
        
    }else {

        spineDecoration->unscheduleUpdate();
        spineDecoration->scheduleUpdate();
        
    }
    
    setPosition(IsoMap::getPositionByMapIndexes(pdVo.map,
                                                g_gameContent->tileSize,
                                                g_gameContent->tileShift));
    
    stopAllActions();
    if (bumping_) {
        runAction(CCSequence::create(CCScaleTo::create(0.1f, 0.8f),
                                     CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f), 0.5f),
                                     NULL));
    }
    
    return true;
    
}

void Decoration::updateFlip() {
    
    if (m_spineLayer) {
        m_spineLayer->setContentSize(this->getContentSize());
        float _scaleX = fabsf(m_spineLayer->getScaleX());
        if (pdVo.flip) {
            m_spineLayer->setScaleX(-_scaleX);
        }else {
            m_spineLayer->setScaleX(_scaleX);
        }
    }
    
}

string Decoration::getMoveJsonRendered() {
    
    CREATE_CHAR_BUFFER(_buffer, 512);
    string _json_str = "";
    
    _json_str.append("{");
    snprintf(_buffer, _buffer_size, "\"duid\":\"%s\",", pdVo.duid.c_str());
    _json_str.append(_buffer);
    
    CREATE_CHAR_BUFFER(_map, ((BUILDING_SPACE_GRID_NUM*5)+2));
    snprintf(_map, _map_size, "[");
    for (int i=0; i<BUILDING_SPACE_GRID_NUM; i++) {
        if (pdVo.map[i] >= 0) {
            if (i != 0) {
                snprintf(_map, _map_size, "%s,%d", _map, pdVo.map[i]);
            }else {
                snprintf(_map, _map_size, "%s%d", _map, pdVo.map[i]);
            }
        }
    }
    snprintf(_map, _map_size, "%s]", _map);
    snprintf(_buffer, _buffer_size, "\"map\":%s", _map);
    _json_str.append(_buffer);
    _json_str.append("}");
    return _json_str.c_str();
    
}

void Decoration::keep() {
    
    stopAllActions();
    if (dVo.decoration_type != kDecorationTypeUnwanted) {
        
        if (SceneVille *_scene = (SceneVille*)g_currentScene) {
            _scene->isoMap->purgeDecoration(this);
        }
        
        pdVo.status = kDecorationStatusKept;
        
        spineDecoration->stopAllActions();
        spineDecoration->runAction(CCSequence::create(CCFadeOut::create(0.25f),
                                                      CCCallFuncO::create(this, callfuncO_selector(Decoration::onKept), this),
                                                      NULL));
        
        g_gameAudio->playEffect(AUDIO_ANIMAL_PUSH);
        
    }
    
}

void Decoration::onKept(cocos2d::CCObject *object_) {
    
    removeFromParent();
    spineDecoration->setOpacity(255);
    spineDecoration->setColor(ccWHITE);
    pdVo.emptyMap();
    BackendService::getInstance()->send(APIDecorationUpdate::create(this,
                                                                    this->getParent(),
                                                                    IsoMap::onDecorationUpdated));
    
}

void Decoration::removeMe() {
    
    removeMeAnimated(true);
    
}

void Decoration::removeMeAnimated(bool animated_) {
    
    if (SceneVille *_scene = (SceneVille*)g_currentScene) {
        _scene->isoMap->purgeDecoration(this);
    }
    
    if (animated_) {
        
        decorationPoint = spineDecoration->getPosition();
        decorationParent = spineDecoration->getParent();
        decorationScale = spineDecoration->getScale();
        decorationOrder = spineDecoration->getZOrder();
        
        spineDecoration->runAction(CCSequence::create(CCScaleTo::create(0.1, decorationScale*0.57),
                                                      CCScaleTo::create(0.25, decorationScale*2.13),
                                                      CCDelayTime::create(1.0f),
                                                      CCCallFuncO::create(this, callfuncO_selector(IsoMapBuilding::releaseDecoration), this),
                                                      NULL));
        spineDecoration->runAction(CCFadeOut::create(0.25f));
        
        removeMeAnimateSmoke();
        
        removeMeAnimateLeaf();
        
    }else {
        
        spineDecoration->runAction(CCCallFuncO::create(this, callfuncO_selector(IsoMapBuilding::releaseDecoration), this));
        
    }
    
}

void Decoration::removeMeAnimateSmoke() {
    
    CCSkeletonAnimation *spine = g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "tree_smoke");
    spine->setPosition(spineDecoration->getPosition());
    spine->setScale(decorationScale);
    spine->setAnimation("idle", false);
    decorationParent->addChild(spine, decorationOrder - 1);
    spine->runAction(CCSequence::create(CCDelayTime::create(spine->states[0]->animation->duration),
                                        CCRemoveSelf::create(),
                                        NULL));

}

void Decoration::removeMeAnimateLeaf() {
    
    int count = 10 + arc4random()%6;
    
    for (int i = 0; i < count; i++) {
        CCSkeletonAnimation *spine = g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "tree_leaf_particle");
        int originX = arc4random()%2 ? 1 : -1 * arc4random()%(int)(g_gameContent->tileSize.width*0.25f);
        int originY = - g_gameContent->tileSize.height*0.25f - arc4random()%(int)(g_gameContent->tileSize.height*0.25f);
        spine->setPosition(decorationPoint + ccp(originX, originY));
        spine->setScale((decorationScale + 0.01f * (arc4random()%50)));
        spine->setScaleX(spine->getScaleX() * (arc4random()%2 ? 1 : -1));
        spine->setOpacity(0);
        decorationParent->addChild(spine , decorationOrder - 1);
        spine->timeScale = 1 + 0.01f * (arc4random()%50);
        spine->runAction(CCSequence::create(CCFadeIn::create(arc4random()%4 * 0.1f),
                                            CCCallFuncO::create(this, callfuncO_selector(Decoration::animateLeaf), spine),
                                            NULL));
    }
    
}

void Decoration::animateLeaf(CCSkeletonAnimation *spine) {
    
    spine->setAnimation("idle", false);
    
    spine->runAction(CCSequence::create(CCDelayTime::create(spine->states[0]->animation->duration),
                                        CCRemoveSelf::create(),
                                        NULL));

}


