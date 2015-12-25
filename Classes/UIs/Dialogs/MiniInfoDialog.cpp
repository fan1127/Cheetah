//
//  MiniInfoDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/20/2557 BE.
//
//

#include "MiniInfoDialog.h"
USING_NS_CC;

#include "../../Settings.h"

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/Building.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

extern float sizeMultiplier;
//extern CCPoint screenFactor;

MiniInfoDialog *MiniInfoDialog::createWithEntity(void *entity_) {
    
    MiniInfoDialog *pRet = new MiniInfoDialog();
    if (pRet->initWithEntity(entity_) && pRet->compose()) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
    
}

bool MiniInfoDialog::compose() {
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    
    if (Building *_building = (Building*)m_entity) {
        
        m_bg->setContentSize(kMiniDialogSize1*sizeMultiplier);
        
        m_block_sprite->setContentSize(m_bg->getContentSize());
        setContentSize(m_bg->getContentSize());
        update(0);
        
        snprintf(_buffer, _buffer_size, "%s %d", g_gameContent->Word["level"].c_str(), _building->pbVo.level);
        char const* buffer = UIDisplayHelper::stringToUpperCase(_buffer).c_str();
        snprintf(_buffer, _buffer_size, "%s", buffer);
        CCLabelBMFont *_title = CCLabelHelper::createBMFont(_buffer, StyleMiniDialogTitle);
         _title->setPositionY(this->getContentSize().height*0.5f-_title->getContentSize().height*0.85f);
        addChild(_title);
        
#define CONTENT_X_POSITION (-186.0f*sizeMultiplier)
#define CONTENT_Y_POSITION (_title->getPositionY() - 120*sizeMultiplier)
#define CONTENT_LINE_2_Y_POSITION (-130*sizeMultiplier)
#define DESC_Y_POSITION (m_bg->getContentSize().height*-0.5f + 135.0f*sizeMultiplier)
        
        switch (m_entity_type) {
            
            default:
            case ET_UNKNOWN: {
                
            }break;
                
            case ET_BUILDING_HQ: {
                
                CCPoint _tmp_pos = ccp(CONTENT_X_POSITION,
                                       CONTENT_Y_POSITION);
                snprintf(_buffer, _buffer_size, "%d", (int)g_gameContent->GameConfig["HQ_BASE_COIN_STORAGE"]);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeCoinsStorage), _tmp_pos, COIN_CAPACITY_INCREASE, _buffer, NULL);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, CONTENT_LINE_2_Y_POSITION);
                snprintf(_buffer, _buffer_size, "%d", (int)g_gameContent->GameConfig["HQ_BASE_FOOD_STORAGE"]);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeFood1), _tmp_pos, FOOD_CAPACITY_INCREASE, _buffer, NULL);
                
            }break;
                
            case ET_BUILDING_UNCLE_HOUSE: {
                
                _title->setString(_building->bVo.title.c_str());
                
                CCSprite* _uncle_portrait = CCSprite::createWithSpriteFrameName("oldversion/_uncle_portrait.png");
                _uncle_portrait->setPosition(ccp(0.0f, getContentSize().height*0.05f));
                addChild(_uncle_portrait);
                
            }break;
                
            case ET_BUILDING_TICKETBOOTH: {
                
                CCPoint _tmp_pos = ccp(CONTENT_X_POSITION,
                                       CONTENT_Y_POSITION);
                snprintf(_buffer, _buffer_size, "%d", (int)_building->bVo.param_2);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeCoinsStorage), _tmp_pos, HOLDS_MAX, _buffer, NULL);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, CONTENT_LINE_2_Y_POSITION);
                snprintf(_buffer, _buffer_size, "%d", (int)_building->bVo.param_1);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeTickets), _tmp_pos, g_gameContent->Word["ticket_price"].c_str(), _buffer, NULL);
                
            }break;
                
            case ET_BUILDING_COIN_STORAGE: {
                
                CCPoint _tmp_pos = ccp(CONTENT_X_POSITION,
                                       CONTENT_Y_POSITION);
                snprintf(_buffer, _buffer_size, "%d", (int)_building->bVo.param_1);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeCoinsStorage), _tmp_pos, COIN_CAPACITY_INCREASE, _buffer, NULL);
                
            }break;
            
            case ET_BUILDING_PRODUCER: {
                
                CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f,
                                       CONTENT_Y_POSITION);
                snprintf(_buffer, _buffer_size, "%d", (int)_building->bVo.param_1);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeFood1), _tmp_pos, HOLDS_MAX, _buffer, NULL);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, CONTENT_LINE_2_Y_POSITION);
                snprintf(_buffer, _buffer_size, "%d", (int)round(_building->bVo.param_2*3600));
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeFood2), _tmp_pos, g_gameContent->Word["produce_rate"].c_str(), _buffer, NULL);

            }break;
                
            case ET_BUILDING_FOOD_STORAGE: {
                
                CCPoint _tmp_pos = ccp(CONTENT_X_POSITION,
                                       CONTENT_Y_POSITION);
                snprintf(_buffer, _buffer_size, "%d", (int)_building->bVo.param_1);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeFood1), _tmp_pos, FOOD_CAPACITY_INCREASE, _buffer, NULL);
                
            }break;
                
            case ET_BUILDING_HABITAT: {
                
                CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f,
                                       CONTENT_Y_POSITION);
                int _egr1 = (int)((_building->bVo.param_1-1)*100);
                snprintf(_buffer, _buffer_size, "%d%%", _egr1);
                addContent(UIHelper::getSpritePath(UIHelper::TraceBuildingsOptionsSleepIcon), _tmp_pos, g_gameContent->Word["energy_gain_rate"].c_str(), _buffer, NULL);
                
            }break;
                
            case ET_BUILDING_ANIMAL: {
                
                CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f,
                                       CONTENT_Y_POSITION);
                int _animal_capacity = _building->bVo.param_1;
                snprintf(_buffer, _buffer_size, "%d", _animal_capacity);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeAnimalRoom), _tmp_pos, g_gameContent->Word["animal_capacity"].c_str(), _buffer, NULL);
                
            }break;
                
            case ET_BUILDING_MARKET: {
                
                CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f,
                                       CONTENT_Y_POSITION);
                int _tradetable_animal = _building->bVo.param_1;
                snprintf(_buffer, _buffer_size, "%d", _tradetable_animal);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeTradableAnimal), _tmp_pos, g_gameContent->Word["tradable_animal"].c_str(), _buffer, NULL);
                
            }break;
                
            case ET_BUILDING_INVENTORY: {
                
                CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f,
                                       CONTENT_Y_POSITION);
                int _item_capacity = _building->bVo.param_1;
                snprintf(_buffer, _buffer_size, "%d", _item_capacity);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeItemCapacity), _tmp_pos, g_gameContent->Word["item_capacity"].c_str(), _buffer, NULL);
                
            }break;
                
            case ET_BUILDING_ASSOCIATION: {
                
                CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f,
                                       CONTENT_Y_POSITION);
                int _track_points = _building->bVo.param_1 + g_gameContent->GameConfig["HQ_BASE_TRACKPOINTS"];
                snprintf(_buffer, _buffer_size, "%d", _track_points);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeTrackpoint), _tmp_pos, g_gameContent->Word["track_points"].c_str(), _buffer, NULL);
                
            }break;
                
            case ET_BUILDING_EXPLORER: {
                
                CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f,
                                       CONTENT_Y_POSITION);
                int _distance_level = _building->bVo.param_1;
                snprintf(_buffer, _buffer_size, "%d", _distance_level);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeDistance), _tmp_pos, g_gameContent->Word["distance_level"].c_str(), _buffer, NULL);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, CONTENT_LINE_2_Y_POSITION);
                float _maxbonus_level_1 = _building->bVo.param_4;
                snprintf(_buffer, _buffer_size, "%.2fx", _maxbonus_level_1);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeMaxBonus), _tmp_pos, g_gameContent->Word["max_streak_bonus"].c_str(), _buffer, NULL);
                
            }break;
                
            case ET_BUILDING_LIBRARY: {
                
                Building* _library_building = g_playerState->getBuilding(kBuildingLibrary, 0);
                if (_library_building) {
                    CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f,
                                           CONTENT_Y_POSITION);
                    int _abilities_vale = g_gameContent->getNAbilitiesUnlocksAtLevel(_library_building->pbVo.level, true);
                    snprintf(_buffer, _buffer_size, "%d", _abilities_vale);
                    addContent(UIHelper::getSpritePath(UIHelper::RaceElementsPowerIcon), _tmp_pos, g_gameContent->Word["abilities"].c_str(), _buffer, NULL);
                }
                
            }break;
                
        }
        
        CCLabelBMFont *_detail =
        CCLabelHelper::createBMFont(_building->bVo.description.c_str(),
                                    kFontSizeSmall,
                                    false,
                                    kTextColorLightBrown,
                                    getContentSize().width*0.8f,
                                    kCCTextAlignmentLeft);
        
        _detail->setPosition(ccp(0,
                                 DESC_Y_POSITION));
        addChild(_detail);
        
        return true;
        
    }
    
    return false;
    
}