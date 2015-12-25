//
//  MiniUpgradeDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/19/2557 BE.
//
//

#include "MiniUpgradeDialog.h"
USING_NS_CC;
using namespace std;

#include <stdio.h>
#include "../../Settings.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Helpers/UIDisplayHelper.h"

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/PlayerState.h"
extern PlayerState *g_playerState;

extern float sizeMultiplier;
extern float margin;

#include "../../Entities/Building.h"
#include "../../UIs/Dialogs/InstantDialog.h"

#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/CCStringHelper.h"
#include "../../UIs/EXPProgress.h"

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#define ITEMS_SPACING       (150.0f*sizeMultiplier)
#define ITEMS_TOP_MARGIN    (130.0f*sizeMultiplier)

MiniUpgradeDialog *MiniUpgradeDialog::createWithEntity(void *entity_)
{
    MiniUpgradeDialog *pRet = new MiniUpgradeDialog();
    if (pRet->initWithEntity(entity_) && pRet->compose())
    {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return NULL;
}

void MiniUpgradeDialog::updateContentSize(int numberOfElements)
{
    float heightRatio = 1.0f;
    if (numberOfElements == 3)
    {
        heightRatio = 1.25f;
    }
    if (numberOfElements == 2)
    {
        heightRatio = 1.1f;
    }
    if (numberOfElements == 1)
    {
        heightRatio = 0.7f;
    }

    m_bg->setContentSize(ccp(kMiniDialogSize2.width*1.15f,
            kMiniDialogSize2.height*heightRatio)*sizeMultiplier);
    m_block_sprite->setContentSize(m_bg->getContentSize());
    setContentSize(m_bg->getContentSize());
}

bool MiniUpgradeDialog::compose()
{
    if (Building *_building = (Building*)m_entity)
    {
        CREATE_CHAR_BUFFER(_buf1, 64);
        CREATE_CHAR_BUFFER(_buf2, 64);
        
        GDBuildingVO _nxt_bvo = g_gameContent->getBuildingVO(_building->pbVo.type, _building->pbVo.level+1);
        if (_nxt_bvo.type == "") {
            return false;
        }

        int etypes[ET_BUILDING_LIBRARY+1] = { 0, 0, 0, 0, 0, 3, 2, 2, 2, 2, 2, 2, 3, 2, 0, 3, 2 };

        updateContentSize(etypes[m_entity_type]);

        update(0);
        
        //to upper
        snprintf(_buf1, _buf1_size, g_gameContent->Word["upgrade_to_level"].c_str(), _nxt_bvo.level);
        char const* buffer = UIDisplayHelper::stringToUpperCase(_buf1).c_str();
        snprintf(_buf1, _buf1_size, "%s", buffer);
        
        CCLabelBMFont *_title = CCLabelHelper::createBMFont(_buf1, StyleMiniDialogTitle);
         _title->setPositionY(this->getContentSize().height*0.5f
                              -_title->getContentSize().height*0.85f);

        addChild(_title);
        
        switch (m_entity_type) {
                
            default:
            case ET_UNKNOWN: {
                
            }break;
                
            case ET_BUILDING_HQ: {
                
#define ITEMS_MARGIN_FROM_LEFT_EDGE (100.0f*sizeMultiplier)
#define ITEMS_HOW_FAR_DOWN (-165.0f*sizeMultiplier)
#define ITEMS_HOW_FAR_DOWN_SECOND_LINE (-180.0f*sizeMultiplier)
#define H_MARGIN_BETWEEN_ITEMS (10.0f*sizeMultiplier) //was _frame->getContentSize().width*0.15f
#define V_MARGIN_BETWEEN_ITEMS (10.0f*sizeMultiplier)

                m_bg->setContentSize(ccp(kMiniDialogSize2.width*1.5f,
                                         kMiniDialogSize2.height*1.28f)*sizeMultiplier);
                m_block_sprite->setContentSize(m_bg->getContentSize());
                setContentSize(m_bg->getContentSize());
                
                _title->setPositionY(this->getContentSize().height*0.5f
                                     -60.0f*sizeMultiplier);
                
                CCPoint _tmp_pos = ccp(-120.0f*sizeMultiplier, _title->getPositionY() -90.0f*sizeMultiplier);
                snprintf(_buf1, _buf1_size, "%s", CCStringHelper::createWithNumberFormat(g_playerState->player.racing.exp).c_str());
                snprintf(_buf2, _buf2_size, "+%s", CCStringHelper::createWithNumberFormat(_nxt_bvo.village_exp_gain).c_str());
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeRanchIcon), _tmp_pos, g_gameContent->Word["experience_points"].c_str(), _buf1, _buf2);
                
                CCLabelBMFont *firstLine = CCLabelHelper::createBMFont(g_gameContent->Word["title_unlock_more_buildings"].c_str(), kFontSizeSmall, kTextColorLight);
                firstLine->setAnchorPoint(ccp(0.0f, 0.5f));
                firstLine->setPosition(ccp(m_bg->getContentSize().width*-0.5f
                                           +55.0f*sizeMultiplier,
                                           _tmp_pos.y
                                           + (-80.0f*sizeMultiplier)));
                addChild(firstLine);
                
                //for the items
                _tmp_pos = ccp(m_bg->getContentSize().width*-0.5f
                               + ITEMS_MARGIN_FROM_LEFT_EDGE, //how far from left edge
                               _tmp_pos.y
                               + ITEMS_HOW_FAR_DOWN); //how far down
                
                CCPoint _tmp_pos_first_line = _tmp_pos;
                
                int itemsPerLine = 6;
                
                int i = 0;
                string _last_building_type = "";
                CCSprite* _last_frame = NULL;
                
                int countFirstLine = 0;
                
                vector<GDBuildingVO> leftOverBuildings;
                
                for (vector<GDBuildingVO>::iterator it=g_gameContent->Building.begin(); it!=g_gameContent->Building.end(); ++it) {
                    GDBuildingVO _bvo = (GDBuildingVO)(*it);
                    
                    if (_bvo.type != "" &&
                        _bvo.building_type != BUILDING_TYPE_HQ) {
                        
                        int amountForCurrentLevel = g_gameContent->getBuildingMax(_bvo.type, _building->pbVo.level);
                        int amountForNextLevel = g_gameContent->getBuildingMax(_bvo.type, _nxt_bvo.level);
                        int amountDiff = amountForNextLevel - amountForCurrentLevel;
                        
                        if (amountDiff > 0
                            && _bvo.level == 1)
                        {
                            snprintf(_buf1, _buf1_size, "_icon_%s.png", _bvo.src.c_str());
                            
                            CCSprite* _frame = CCSprite::createWithSpriteFrameName(_buf1);
                            if (!_frame) _frame = CCSprite::createWithSpriteFrameName("_icon_animal_barn_1.png");
                            _frame->setAnchorPoint(ccp(0.5f, 0.5f));
                            _frame->setPosition(_tmp_pos +
                                                ccp( (int(i%itemsPerLine)*
                                                      (_frame->getContentSize().width+
                                                      H_MARGIN_BETWEEN_ITEMS)
                                                      ),
                                                    -(int(i/itemsPerLine)*
                                                      (_frame->getContentSize().height+
                                                       V_MARGIN_BETWEEN_ITEMS))));
                            addChild(_frame);
                            
                            if (amountForCurrentLevel == 0) {
                                strncpy(_buf1, "New!", _buf1_size);
                            } else {
                                snprintf(_buf1, _buf1_size, "+%d", amountDiff);
                            }
                            
                            CCLabelBMFont* _label = CCLabelHelper::createBMFont(_buf1, kFontSizeSmall, true);
                            _label->setAnchorPoint(ccp(0.5f, 0.0f));
                            _label->setPosition(ccp(_frame->getContentSize().width*0.5f, 0.0f));
                            _frame->addChild(_label);
                            
                            i++;
                            countFirstLine++;
                        } else {
                            //add to temp vector
                            leftOverBuildings.push_back(_bvo);
                        }
                        
                    }
                }
                
                //resets position for the second line
                _tmp_pos = _tmp_pos_first_line + ccp(0.0f,
                                                     ITEMS_HOW_FAR_DOWN_SECOND_LINE);
                
                i = 0; //resets i for the second line
                
                CCLabelBMFont *secondLine = CCLabelHelper::createBMFont(g_gameContent->Word["title_increase_upgrade_limits"].c_str(), kFontSizeSmall, kTextColorLight);
                secondLine->setAnchorPoint(ccp(0.0f, 0.5f));
                secondLine->setPosition(ccp(m_bg->getContentSize().width*-0.5f
                                            +55.0f*sizeMultiplier,
                                            _tmp_pos.y
                                            + (80.0f*sizeMultiplier)));
                addChild(secondLine);
                
                if (countFirstLine <= 0) { //make secondLine appear as firstLine
                    firstLine->setVisible(false);
                    _tmp_pos = _tmp_pos_first_line;
                    secondLine->setPosition(firstLine->getPosition());
                }
                
                int countSecondLine = 0;
                
                for (vector<GDBuildingVO>::iterator it=leftOverBuildings.begin(); it!=leftOverBuildings.end(); ++it) {
                    GDBuildingVO _bvo = (GDBuildingVO)(*it);
                    if (_bvo.level > 1
                        && _bvo.unlocks_at_level == _nxt_bvo.level) {
                        
                        if (_bvo.type == _last_building_type) {
                            if (_last_frame) {
                                _last_frame->removeFromParentAndCleanup(true);
                                _last_frame = NULL;
                                i--;
                            }
                        }
                        snprintf(_buf1, _buf1_size, "_icon_%s.png", _bvo.src.c_str());
                        
                        CCSprite* _frame = CCSprite::createWithSpriteFrameName(_buf1);
                        if (!_frame) _frame = CCSprite::createWithSpriteFrameName("_icon_animal_barn_1.png");
                        _frame->setAnchorPoint(ccp(0.5f, 0.5f));
                        _frame->setPosition(_tmp_pos +
                                            ccp( (int(i%itemsPerLine)*
                                                  (_frame->getContentSize().width+
                                                   H_MARGIN_BETWEEN_ITEMS)
                                                  ),
                                                -(int(i/itemsPerLine)*
                                                  (_frame->getContentSize().height+
                                                   V_MARGIN_BETWEEN_ITEMS))));
                        addChild(_frame);
                        
                        CCSprite *_iconUpgrade = UIHelper::getSprite(UIHelper::UpgradeUpArrow);
                        _iconUpgrade->setAnchorPoint(ccp(1.05f,1.05f));
                        _iconUpgrade->setPosition(_frame->getContentSize());
                        _iconUpgrade->setScale(1.0f);
                        _frame->addChild(_iconUpgrade);
                        
                        _last_building_type = _bvo.type;
                        _last_frame = _frame;
                        i++;
                        countSecondLine++;
                    }
                }
                
                if (countSecondLine <= 0) {
                    secondLine->setVisible(false);
                }
                
                leftOverBuildings.clear();
                
            }break;
                
            case ET_BUILDING_TICKETBOOTH: {

                CCPoint _tmp_pos = ccp(-186*sizeMultiplier, _title->getPositionY() -ITEMS_TOP_MARGIN);
                snprintf(_buf1, _buf1_size, "%s", CCStringHelper::createWithNumberFormat((int)_building->bVo.param_2).c_str());
                snprintf(_buf2, _buf2_size, "+%s", CCStringHelper::createWithNumberFormat((int)round(_nxt_bvo.param_2-_building->bVo.param_2)).c_str());
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeCoinsStorage), _tmp_pos, HOLDS_MAX, _buf1, _buf2);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, -ITEMS_SPACING);
                snprintf(_buf1, _buf1_size, "%s", CCStringHelper::createWithNumberFormat((int)_building->bVo.param_1).c_str());
                snprintf(_buf2, _buf2_size, "+%s", CCStringHelper::createWithNumberFormat((int)round(_nxt_bvo.param_1-_building->bVo.param_1)).c_str());
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeTickets), _tmp_pos, g_gameContent->Word["ticket_price"].c_str(), _buf1, _buf2);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, -ITEMS_SPACING);
                snprintf(_buf1, _buf1_size, "%s", CCStringHelper::createWithNumberFormat(g_playerState->player.racing.exp).c_str());
                snprintf(_buf2, _buf2_size, "+%s", CCStringHelper::createWithNumberFormat(_nxt_bvo.village_exp_gain).c_str());
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeRanchIcon), _tmp_pos, g_gameContent->Word["experience_points"].c_str(), _buf1, _buf2);
                
            }break;
                
            case ET_BUILDING_COIN_STORAGE: {

                CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f, _title->getPositionY() -ITEMS_TOP_MARGIN);
                snprintf(_buf1, _buf1_size, "%s", CCStringHelper::createWithNumberFormat((int)_building->bVo.param_1).c_str());
                snprintf(_buf2, _buf2_size, "+%s", CCStringHelper::createWithNumberFormat((int)round(_nxt_bvo.param_1 - _building->bVo.param_1)).c_str());
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeCoinsStorage), _tmp_pos, COIN_CAPACITY_INCREASE, _buf1, _buf2);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, -ITEMS_SPACING);
                snprintf(_buf1, _buf1_size, "%s", CCStringHelper::createWithNumberFormat(g_playerState->player.racing.exp).c_str());
                snprintf(_buf2, _buf2_size, "+%s", CCStringHelper::createWithNumberFormat(_nxt_bvo.village_exp_gain).c_str());
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeRanchIcon), _tmp_pos, g_gameContent->Word["experience_points"].c_str(), _buf1, _buf2);
                

            }break;
                
            case ET_BUILDING_PRODUCER: {

                CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f, _title->getPositionY() -ITEMS_TOP_MARGIN);
                snprintf(_buf1, _buf1_size, "%s", CCStringHelper::createWithNumberFormat((int)_building->bVo.param_1).c_str());
                snprintf(_buf2, _buf2_size, "+%s", CCStringHelper::createWithNumberFormat((int)round(_nxt_bvo.param_1 - _building->bVo.param_1)).c_str());
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeFood1), _tmp_pos, HOLDS_MAX, _buf1, _buf2);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, -ITEMS_SPACING);
                snprintf(_buf1, _buf1_size, "%s", CCStringHelper::createWithNumberFormat((int)round(_building->bVo.param_2*3600)).c_str()); // multiplier by 3600 to convert to hour unit
                snprintf(_buf2, _buf2_size, "+%s", CCStringHelper::createWithNumberFormat((int)round((_nxt_bvo.param_2 - _building->bVo.param_2)*3600)).c_str());
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeFood2), _tmp_pos, g_gameContent->Word["produce_rate"].c_str(), _buf1, _buf2);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, -ITEMS_SPACING);
                snprintf(_buf1, _buf1_size, "%s", CCStringHelper::createWithNumberFormat(g_playerState->player.racing.exp).c_str());
                snprintf(_buf2, _buf2_size, "+%s", CCStringHelper::createWithNumberFormat(_nxt_bvo.village_exp_gain).c_str());
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeRanchIcon), _tmp_pos, g_gameContent->Word["experience_points"].c_str(), _buf1, _buf2);
                
            }break;
                
            case ET_BUILDING_FOOD_STORAGE: {

                CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f, _title->getPositionY() -ITEMS_TOP_MARGIN);
                snprintf(_buf1, _buf1_size, "%s", CCStringHelper::createWithNumberFormat((int)_building->bVo.param_1).c_str());
                snprintf(_buf2, _buf2_size, "+%s", CCStringHelper::createWithNumberFormat((int)round(_nxt_bvo.param_1 - _building->bVo.param_1)).c_str());
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeFood1), _tmp_pos, FOOD_CAPACITY_INCREASE, _buf1, _buf2);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, -ITEMS_SPACING);
                snprintf(_buf1, _buf1_size, "%s", CCStringHelper::createWithNumberFormat(g_playerState->player.racing.exp).c_str());
                snprintf(_buf2, _buf2_size, "+%s", CCStringHelper::createWithNumberFormat(_nxt_bvo.village_exp_gain).c_str());
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeRanchIcon), _tmp_pos, g_gameContent->Word["experience_points"].c_str(), _buf1, _buf2);
                
            }break;
                
            case ET_BUILDING_HABITAT: {

                CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f, _title->getPositionY() -ITEMS_TOP_MARGIN);
                int _egr1 = (int)((_building->bVo.param_1-1)*100);
                int _egr2 = (int)((_nxt_bvo.param_1-1)*100);
                snprintf(_buf1, _buf1_size, "%d%%", _egr1);
                snprintf(_buf2, _buf2_size, "+%d%%", _egr2 - _egr1);
                addContent(UIHelper::getSpritePath(UIHelper::TraceBuildingsOptionsSleepIcon), _tmp_pos, g_gameContent->Word["energy_gain_rate"].c_str(), _buf1, _buf2);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, -ITEMS_SPACING);
                snprintf(_buf1, _buf1_size, "%d", g_playerState->player.racing.exp);
                snprintf(_buf2, _buf2_size, "+%d", _nxt_bvo.village_exp_gain);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeRanchIcon), _tmp_pos, g_gameContent->Word["experience_points"].c_str(), _buf1, _buf2);
                
            }break;
                
            case ET_BUILDING_ANIMAL: {

                CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f, _title->getPositionY() -ITEMS_TOP_MARGIN);
                int _animal_capacity_1 = _building->bVo.param_1;
                int _animal_capacity_2 = _nxt_bvo.param_1;
                snprintf(_buf1, _buf1_size, "%d", _animal_capacity_1);
                snprintf(_buf2, _buf2_size, "+%d", _animal_capacity_2 - _animal_capacity_1);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeAnimalRoom), _tmp_pos, g_gameContent->Word["animal_capacity"].c_str(), _buf1, _buf2);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, -ITEMS_SPACING);
                snprintf(_buf1, _buf1_size, "%d", g_playerState->player.racing.exp);
                snprintf(_buf2, _buf2_size, "+%d", _nxt_bvo.village_exp_gain);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeRanchIcon), _tmp_pos, g_gameContent->Word["experience_points"].c_str(), _buf1, _buf2);

                
            }break;
                
            case ET_BUILDING_MARKET: {

                CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f, _title->getPositionY() -ITEMS_TOP_MARGIN);
                int _tradable_animal_1 = _building->bVo.param_1;
                int _tradable_animal_2 = _nxt_bvo.param_1;
                snprintf(_buf1, _buf1_size, "%d", _tradable_animal_1);
                snprintf(_buf2, _buf2_size, "+%d", _tradable_animal_2 - _tradable_animal_1);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeTradableAnimal), _tmp_pos, g_gameContent->Word["tradable_animal"].c_str(), _buf1, _buf2);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, -ITEMS_SPACING);
                snprintf(_buf1, _buf1_size, "%d", g_playerState->player.racing.exp);
                snprintf(_buf2, _buf2_size, "+%d", _nxt_bvo.village_exp_gain);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeRanchIcon), _tmp_pos, g_gameContent->Word["experience_points"].c_str(), _buf1, _buf2);
                
            }break;
                
            case ET_BUILDING_INVENTORY: {

                CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f, _title->getPositionY() -ITEMS_TOP_MARGIN);
                int _item_capacity_1 = _building->bVo.param_1;
                int _item_capacity_2 = _nxt_bvo.param_1;
                snprintf(_buf1, _buf1_size, "%d", _item_capacity_1);
                snprintf(_buf2, _buf2_size, "+%d", _item_capacity_2 - _item_capacity_1);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeItemCapacity), _tmp_pos, g_gameContent->Word["item_capacity"].c_str(), _buf1, _buf2);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, -ITEMS_SPACING);
                snprintf(_buf1, _buf1_size, "%d", g_playerState->player.racing.exp);
                snprintf(_buf2, _buf2_size, "+%d", _nxt_bvo.village_exp_gain);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeRanchIcon), _tmp_pos, g_gameContent->Word["experience_points"].c_str(), _buf1, _buf2);
                
            }break;
                
            case ET_BUILDING_ASSOCIATION: {

                CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f, _title->getPositionY() -ITEMS_TOP_MARGIN);
                int _track_points_1 = _building->bVo.param_1;
                int _total_track_points = _track_points_1 + g_gameContent->GameConfig["HQ_BASE_TRACKPOINTS"];
                int _track_points_2 = _nxt_bvo.param_1;
                snprintf(_buf1, _buf1_size, "%d", _total_track_points);
                snprintf(_buf2, _buf2_size, "+%d", _track_points_2 - _track_points_1);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeTrackpoint), _tmp_pos, g_gameContent->Word["track_points"].c_str(), _buf1, _buf2);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, -ITEMS_SPACING);
                snprintf(_buf1, _buf1_size, "%d", g_playerState->player.racing.exp);
                snprintf(_buf2, _buf2_size, "+%d", _nxt_bvo.village_exp_gain);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeRanchIcon), _tmp_pos, g_gameContent->Word["experience_points"].c_str(), _buf1, _buf2);
                
                GDTrackVO _tvo = g_gameContent->getTrackVOUnlocksAtLevel(_nxt_bvo.level);
                if (_tvo.type != "") {
                    _tmp_pos = _tmp_pos + ccp(120.0f*sizeMultiplier, -ITEMS_SPACING);
                    snprintf(_buf1, _buf1_size, "%s: ", g_gameContent->Word["unlocks"].c_str());
                    CCLabelBMFont* _label_unlocks = CCLabelHelper::createBMFont(_buf1, kFontSizeSmall, kTextColorDark);
                    _label_unlocks->setPosition(_tmp_pos);
                    addChild(_label_unlocks);
                    snprintf(_buf1, _buf1_size, "Animal_stats/_icon_%s.png", _tvo.src.c_str());
                    CCSprite* _track_sprite = CCSprite::createWithSpriteFrameName(_buf1);
                    _track_sprite->setScale(0.5f);
                    _track_sprite->setPosition(ccp(_label_unlocks->getPositionX()+_label_unlocks->getContentSize().width*0.5f+margin+_track_sprite->getContentSize().width*0.25f,
                                                   _label_unlocks->getPositionY()));
                    addChild(_track_sprite);
                }
                
            }break;
                
            case ET_BUILDING_EXPLORER: {

                CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f, _title->getPositionY() -ITEMS_TOP_MARGIN);
                int _distance_level_1 = _building->bVo.param_1;
                int _distance_level_2 = _nxt_bvo.param_1;
                snprintf(_buf1, _buf1_size, "%d", _distance_level_1);
                snprintf(_buf2, _buf2_size, "+%d", _distance_level_2 - _distance_level_1);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeDistance), _tmp_pos, g_gameContent->Word["distance_level"].c_str(), _buf1, _buf2);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, -ITEMS_SPACING);
                float _maxbonus_level_1 = _building->bVo.param_4;
                float _maxbonus_level_2 = _nxt_bvo.param_4;
                snprintf(_buf1, _buf1_size, "%.2fx", _maxbonus_level_1);
                snprintf(_buf2, _buf2_size, "+%.2fx", _maxbonus_level_2 - _maxbonus_level_1);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeMaxBonus), _tmp_pos, g_gameContent->Word["max_streak_bonus"].c_str(), _buf1, _buf2);
                
                _tmp_pos = _tmp_pos + ccp(0.0f, -ITEMS_SPACING);
                snprintf(_buf1, _buf1_size, "%d", g_playerState->player.racing.exp);
                snprintf(_buf2, _buf2_size, "+%d", _nxt_bvo.village_exp_gain);
                addContent(UIHelper::getSpritePath(UIHelper::UpgradeRanchIcon), _tmp_pos, g_gameContent->Word["experience_points"].c_str(), _buf1, _buf2);
                
            }break;
                
            case ET_BUILDING_LIBRARY: {

                if (Building* _library_building = g_playerState->getBuilding(kBuildingLibrary, 0)) {
                    GDBuildingVO _nxt_library_building_vo = g_gameContent->getBuildingVO(kBuildingLibrary, _library_building->pbVo.level);
                    if (_nxt_library_building_vo.type != "") {
                        
                        CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f, _title->getPositionY() -ITEMS_TOP_MARGIN);
                        int _ability_val_1 = g_gameContent->getNAbilitiesUnlocksAtLevel(_library_building->pbVo.level, true);// _building->bVo.param_1;
                        int _ability_val_2 = g_gameContent->getNAbilitiesUnlocksAtLevel(_library_building->pbVo.level+1, true);// _nxt_bvo.param_1;
                        int _ability_add = _ability_val_2 - _ability_val_1;
                        if (_ability_add > 0) {
                            snprintf(_buf1, _buf1_size, "%s", CCStringHelper::createWithNumberFormat(_ability_val_1).c_str());
                            snprintf(_buf2, _buf2_size, "+%s", CCStringHelper::createWithNumberFormat(_ability_add).c_str());
                            addContent(UIHelper::getSpritePath(UIHelper::UpgradeTrackpoint), _tmp_pos, g_gameContent->Word["abilities"].c_str(), _buf1, _buf2);
                            _tmp_pos = _tmp_pos + ccp(0.0f, -ITEMS_SPACING);
                        }
                        snprintf(_buf1, _buf1_size, "%d", g_playerState->player.racing.exp);
                        snprintf(_buf2, _buf2_size, "+%d", _nxt_library_building_vo.village_exp_gain);
                        addContent(UIHelper::getSpritePath(UIHelper::UpgradeRanchIcon), _tmp_pos, g_gameContent->Word["experience_points"].c_str(), _buf1, _buf2);
                        
                    }
                }
                
            }break;
                
        }
        
        bool _can_upgrade = false;
        Building* _building_hq = g_playerState->getBuilding(kBuildingHQ, 0);
        if (m_entity_type == ET_BUILDING_HQ) {
        
            if (g_playerState->player.racing.level >= _nxt_bvo.unlocks_at_level) {
                _can_upgrade = true;
            }
            
        }else {

            if (_building_hq) {
                if ((_building_hq->pbVo.level >= _nxt_bvo.unlocks_at_level)) {
                    _can_upgrade = true;
                }
            }
            
        }
        
        if (_can_upgrade) {
            
            addInstantButton(g_gameContent->cal_instant_building_cost(_nxt_bvo));
            UIDisplayHelper::convertToDisplayTime2(_nxt_bvo.time, _buf1, _buf1_size);
            addUpgradeButton(_nxt_bvo.cost_coins, _buf1);
            
        }else {
            
            addRequirementMessage(_building_hq, _nxt_bvo);
            
        }
        
        return true;
        
    }
    
    return false;
}

void MiniUpgradeDialog::addInstantButton(int value_)
{
    float _spacing = getContentSize().height*0.15f;
    CREATE_CHAR_BUFFER(_buffer, 64);
    snprintf(_buffer, _buffer_size, "%d ", value_);
    
    //Upgrade Button
    CCSprite *_blue = UIHelper::getSprite(UIHelper::UpgradeInstantBuyButton);
    
    CCLabelBMFont *_label = CCLabelHelper::createBMFont(g_gameContent->Word["upgrade_button_label"].c_str(), kFontSizeMedium-5.0f-2.0f, true);
    _label->setPosition(ccp(_blue->getContentSize().width*0.5f, _blue->getContentSize().height*0.75f));
    _blue->addChild(_label);
    CCSprite *_icon = UIHelper::getSprite(UIHelper::NSClaimGemBlue);
    _icon->setAnchorPoint(ccp(0.0f, 0.5f));
    _icon->setPosition(ccp(_blue->getContentSize().width, 0));
    _blue->addChild(_icon);
    CCLabelBMFont *_value = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium-5.0f, true);
    _value->setAnchorPoint(ccp(0.0f, 0.5f));
    _value->setPosition(ccp(_icon->getPositionX()-_icon->getContentSize().width-MINI_DIALOG_BUTTON_VALUE_OFFSET, _blue->getContentSize().height*0.30f));
    _value->setString(CCStringHelper::createWithNumberFormat(value_).c_str());
    _blue->addChild(_value);

    UIDisplayHelper::centerItems(_blue, _icon, _value, 10, 0.3f);

    CCSprite *_blue_pressed = UIHelper::getSprite(UIHelper::UpgradeInstantBuyButton);
    _blue_pressed->setScale(BUTTON_SCALE_DOWN);
    _blue_pressed->setPosition(_blue->getContentSize()*((1.0f-BUTTON_SCALE_DOWN)*0.5f));
    CCLabelBMFont *_label_pressed = CCLabelHelper::createBMFont(g_gameContent->Word["upgrade_button_label"].c_str(), kFontSizeMedium-5.0f-2.0f, true);
    _label_pressed->setPosition(ccp(_blue_pressed->getContentSize().width*0.5f, _blue_pressed->getContentSize().height*0.75f));
    _blue_pressed->addChild(_label_pressed);
    CCSprite *_icon_pressed = UIHelper::getSprite(UIHelper::NSClaimGemBlue);
    _icon_pressed->setAnchorPoint(ccp(1.0f, 0.0f));
    _icon_pressed->setPosition(ccp(_blue->getContentSize().width, 0));
    _blue_pressed->addChild(_icon_pressed);
    CCLabelBMFont *_value_pressed = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium-5.0f, true);
    _value_pressed->setAnchorPoint(ccp(1.0f, _value_pressed->getAnchorPoint().y));
    _value_pressed->setPosition(ccp(_icon_pressed->getPositionX()-_icon_pressed->getContentSize().width-MINI_DIALOG_BUTTON_VALUE_OFFSET, _blue_pressed->getContentSize().height*0.30f));
    _blue_pressed->addChild(_value_pressed);

    UIDisplayHelper::centerItems(_blue_pressed, _icon_pressed, _value_pressed, 10, 0.3f);

    CCMenuItemSprite *_menuItem = CCMenuItemSprite::create(_blue, _blue_pressed, this, menu_selector(MiniUpgradeDialog::onInstant));
    CCMenu *_button_upgrade = CCMenu::create(_menuItem, NULL);
    _button_upgrade->setPosition((getContentSize()*-0.5f)+(_blue->getContentSize()*0.5)+ccp(_spacing*0.4f, _spacing*0.4f));
    _button_upgrade->setContentSize(_blue->getContentSize());
    addChild(_button_upgrade);
    m_instantButton = _button_upgrade;
    
    CCLabelBMFont *_instant_label = CCLabelHelper::createBMFont(g_gameContent->Word["instant_button_label"].c_str(), kFontSizeSmall, kTextColorLightBrown);
    _instant_label->setAnchorPoint(ccp(0.0f, 0.5f));
    addChild(_instant_label);
    
    CCSprite *_instant_icon = UIHelper::getSprite(UIHelper::UpgradeEnergyIcon);
    _instant_icon->setAnchorPoint(ccp(0.0f, 0.5f));
    addChild(_instant_icon);
    
    float _label_width = _instant_label->getContentSize().width +_instant_icon->getContentSize().width;
    _instant_icon->setPosition(_button_upgrade->getPosition()+
                                ccp(-(_button_upgrade->getContentSize().width*0.5f)+
                                    (_button_upgrade->getContentSize().width-_label_width)*0.5f,
                                    _button_upgrade->getContentSize().height*0.7f));

    _instant_label->setPosition(ccp(_instant_icon->getPositionX()+_instant_icon->getContentSize().width + 10 * sizeMultiplier, _instant_icon->getPositionY()));
}

void MiniUpgradeDialog::addUpgradeButton(int value_, const char *top_label_) {
    
    float _spacing = getContentSize().height*0.15f;
    CREATE_CHAR_BUFFER(_buffer, 64);
//    snprintf(_buffer, _buffer_size, "%d ", value_);
    
    //Upgrade Button
    CCSprite *_button_sprite = UIHelper::getSprite(UIHelper::UpgradeAndTradeItemsForAnimalBuyButton);
    CCLabelBMFont *_label = CCLabelHelper::createBMFont(g_gameContent->Word["upgrade_button_label"].c_str(), kFontSizeMedium-5.0f-2.0f, true);
    _label->setPosition(ccp(_button_sprite->getContentSize().width*0.5f, _button_sprite->getContentSize().height*0.75f));
    _button_sprite->addChild(_label);
    CCSprite *_icon = UIHelper::getSprite(UIHelper::NSBuyCoinIcon);
    _icon->setAnchorPoint(ccp(1.0f, 0.0f));
    _icon->setPosition(ccp(_button_sprite->getContentSize().width, 0));
    _button_sprite->addChild(_icon);
    CCLabelBMFont *_value = CCLabelHelper::createBMFont("", kFontSizeMedium-5.0f, true);
    _value->setAlignment(kCCTextAlignmentRight);
    _value->setAnchorPoint(ccp(1.0f, _value->getAnchorPoint().y));
    _value->setPosition(ccp(_icon->getPositionX()-_icon->getContentSize().width-MINI_DIALOG_BUTTON_VALUE_OFFSET, _button_sprite->getContentSize().height*0.30f));
    _value->setString(CCStringHelper::createWithNumberFormat(value_).c_str());
    _button_sprite->addChild(_value);

    UIDisplayHelper::centerItems(_button_sprite, _icon, _value, 10, 0.3f);
    
    CCSprite *_button_sprite_pressed = UIHelper::getSprite(UIHelper::UpgradeAndTradeItemsForAnimalBuyButton);
    _button_sprite_pressed->setScale(BUTTON_SCALE_DOWN);
    _button_sprite_pressed->setPosition(_button_sprite->getContentSize()*((1.0f-BUTTON_SCALE_DOWN)*0.5f));
    CCLabelBMFont *_label_pressed = CCLabelHelper::createBMFont(g_gameContent->Word["upgrade_button_label"].c_str(), kFontSizeMedium-5.0f-2.0f, true);
    _label_pressed->setPosition(ccp(_button_sprite_pressed->getContentSize().width*0.5f, _button_sprite_pressed->getContentSize().height*0.75f));
    _button_sprite_pressed->addChild(_label_pressed);
    CCSprite *_icon_pressed = UIHelper::getSprite(UIHelper::NSBuyCoinIcon);
    _icon_pressed->setAnchorPoint(ccp(1.0f, 0.0f));
    _icon_pressed->setPosition(ccp(_button_sprite_pressed->getContentSize().width, 0));
    _button_sprite_pressed->addChild(_icon_pressed);
    CCLabelBMFont *_value_pressed = CCLabelHelper::createBMFont(_value->getString(), kFontSizeMedium-5.0f, true);
    _value_pressed->setAlignment(kCCTextAlignmentRight);
    _value_pressed->setAnchorPoint(ccp(1.0f, _value_pressed->getAnchorPoint().y));
    _value_pressed->setPosition(ccp(_icon_pressed->getPositionX()-_icon_pressed->getContentSize().width-MINI_DIALOG_BUTTON_VALUE_OFFSET, _button_sprite_pressed->getContentSize().height*0.30f));
    _button_sprite_pressed->addChild(_value_pressed);

    UIDisplayHelper::centerItems(_button_sprite_pressed, _icon_pressed, _value_pressed, 10, 0.3f);

    if (Building *_building = (Building*)m_entity) {
        GDBuildingVO _nxt_bvo = g_gameContent->getBuildingVO(_building->pbVo.type, _building->pbVo.level+1);
        if (g_playerState->player.balance.gold < _nxt_bvo.cost_coins) {
            _value->setColor(kTextColorOrange2);
            _value_pressed->setColor(kTextColorOrange2);
        }
    }
    
    CCMenuItemSprite *_menuItem = CCMenuItemSprite::create(_button_sprite, _button_sprite_pressed, this, menu_selector(MiniUpgradeDialog::onUpgrade));
    CCMenu *_button_upgrade = CCMenu::create(_menuItem, NULL);
    _button_upgrade->setPosition(ccp(getContentSize().width*0.5f,-getContentSize().height*0.5f)+ccp(-_button_sprite->getContentSize().width*0.5f, _button_sprite->getContentSize().height*0.5f)+ccp(-_spacing*0.4f, _spacing*0.4f));
    _button_upgrade->setContentSize(_button_sprite->getContentSize());
    addChild(_button_upgrade);
    m_upgradeButton = _button_upgrade;
    
    snprintf(_buffer, _buffer_size, "%s", top_label_);
    CCLabelBMFont *_instant_label = CCLabelHelper::createBMFont(_buffer, kFontSizeSmall, kTextColorLightBrown);
    _instant_label->setAnchorPoint(ccp(0.0f, 0.5f));
    addChild(_instant_label);
    
    CCSprite *_instant_icon = UIHelper::getSprite(UIHelper::UpgradeClockIcon);
    _instant_icon->setAnchorPoint(ccp(0.0f, 0.5f));
    addChild(_instant_icon);
    
    float _label_width = _instant_label->getContentSize().width +_instant_icon->getContentSize().width;
    _instant_icon->setPosition(_button_upgrade->getPosition()+ccp(-(_button_upgrade->getContentSize().width*0.5f)+(_button_upgrade->getContentSize().width-_label_width)*0.5f, _button_upgrade->getContentSize().height*0.7f));
    _instant_label->setPosition(ccp(_instant_icon->getPositionX()+_instant_icon->getContentSize().width + 10 * sizeMultiplier, _instant_icon->getPositionY()));
}

void MiniUpgradeDialog::addRequirementMessage(Building* hq_, GDBuildingVO nxt_bvo_ ) {
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    
    int level = 0;
    
    if (nxt_bvo_.building_type == BUILDING_TYPE_HQ) {
        snprintf(_buffer, _buffer_size, g_gameContent->Word["required_hq_level_to_upgrade"].c_str(), g_gameContent->Word["town_keyword"].c_str(), nxt_bvo_.unlocks_at_level);
        level = nxt_bvo_.unlocks_at_level;
    }else {
        if (hq_) {
            snprintf(_buffer, _buffer_size, g_gameContent->Word["required_hq_level_to_upgrade"].c_str(), hq_->bVo.title.c_str(), nxt_bvo_.unlocks_at_level);
            level = nxt_bvo_.unlocks_at_level;
        }else {
            GDBuildingVO _building_hq_vo = g_gameContent->getBuildingVO(kBuildingHQ, nxt_bvo_.unlocks_at_level);
            snprintf(_buffer, _buffer_size, g_gameContent->Word["required_hq_level_to_upgrade"].c_str(), _building_hq_vo.title.c_str(), nxt_bvo_.unlocks_at_level);
            level = nxt_bvo_.unlocks_at_level;
        }
    }
    
    
    if (nxt_bvo_.building_type == BUILDING_TYPE_HQ) {
        
        CCLabelBMFont *_detail = CCLabelHelper::createBMFont(_buffer,
                                                             kFontSizeSmall,
                                                             false,
                                                             kTextColorLightBrown,
                                                             300.0f*sizeMultiplier,
                                                             kCCTextAlignmentLeft);
        _detail->setAnchorPoint(ccp(0.0f, 0.5f));
        _detail->setPosition(ccp(getContentSize().width*-0.5f
                                 +(80.0f*sizeMultiplier),
                                 getContentSize().height*-0.5f
                                 +(134.0f*sizeMultiplier)
                                 ));
        addChild(_detail);
        
        
        EXPProgress *expProgress = EXPProgress::create();
        expProgress->setPosition(ccp(getContentSize().width*0.5f
                                     -(130.0f*sizeMultiplier),
                                     getContentSize().height*-0.5f
                                     +(134.0f*sizeMultiplier)
                                     ));
        expProgress->setClearUI(true);
        expProgress->setLevel(level);
        expProgress->setScale(0.75f);
        addChild(expProgress);
        
    }else {
        
        
        
        CCLabelBMFont *_detail = CCLabelHelper::createBMFont(_buffer,
                                                             kFontSizeSmall,
                                                             false,
                                                             kTextColorLightBrown,
                                                             300.0f*sizeMultiplier,
                                                             kCCTextAlignmentLeft);
        _detail->setAnchorPoint(ccp(0.0f, 0.5f));
        _detail->setPosition(ccp(getContentSize().width*-0.5f
                                 +(60.0f*sizeMultiplier),
                                 getContentSize().height*-0.5f
                                 +(134.0f*sizeMultiplier)
                                 ));
        addChild(_detail);
        
        CCSprite *_image = CCSprite::createWithSpriteFrameName("oldversion/_background_lable_mansion.png");
        _image->setAnchorPoint(ccp(0.5f, 0.0f));
        _image->setScale(1.0f);
        _image->setPosition(ccp(getContentSize().width*0.5f
                                -(25.0f*sizeMultiplier)
                                -_image->getContentSize().width*0.5f*_image->getScaleX(),
                                getContentSize().height*-0.5f
                                +(30.0f*sizeMultiplier)
                                ));
        addChild(_image);

    }
    
}

void MiniUpgradeDialog::onInstant(CCObject *sender_) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    
    if (!_scene->modalController->getParent()) {
        
        CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
        
        InstantDialog *_dialog = InstantDialog::create(_scene->modalController, m_entity);
        _dialog->setPosition(_screenSize*0.5f);
        _scene->addChild(_scene->modalController, kModalControllerZOrder);
        _scene->modalController->pushView(_dialog);
        close();
        
    }

}

void MiniUpgradeDialog::onUpgrade(CCObject *sender_) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    g_gameAudio->playEffect(AUDIO_UNDERCONSTRUCTION);
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    Building *_building = (Building*)m_entity;
    
    Building::ActionResult _result = _building->upgradeState();
    switch (_result) {
        default:
        case Building::MAXIMUM_LEVEL_REACHED: {
            return;
        }break;
        case Building::BUILDER_IS_BUSY:
        case Building::NOT_ENOUGH_COINS: {
            if (!_scene->modalController->getParent()) {
                CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
                _building->setTag(1); //set tag=1 to indicate that we want to buy resources rather than instant upgrade
                InstantDialog *_dialog = InstantDialog::create(_scene->modalController, _building, g_playerState->getBuilder());
                _building->setTag(0);
                _dialog->setPosition(_screenSize*0.5f);
                _scene->addChild(_scene->modalController, kModalControllerZOrder);
                _scene->modalController->pushView(_dialog);
            }
        }break;
        case Building::OK: {
            
        }break;
    }
    
    close();
    
}