//
//  MiniDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/18/2557 BE.
//
//

#include "MiniDialog.h"
USING_NS_CC;
USING_NS_CC_EXT;

#include "../../Helpers/CCStringHelper.h"

#include "../../Scenes/CScene.h"
extern CCScene *g_currentScene;

#include "../../Entities/Building.h"

#include "../../AppMacros.h"
extern TargetDevice targetDevice;
extern float sizeMultiplier;

#include "../../Helpers/CCSpriteHelper.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCPhatButton.h"

bool MiniDialog::m_centerDialog = false;

bool MiniDialog::initWithEntity(void *entity_)
{
    if (!CCNode::init())
    {
        return false;
    }
    
    m_spread = 0;
    
    m_entity = entity_;
    m_entity_type = getEntityType(m_entity);

    m_bg = UIHelper::getScalableSprite(UIHelper::PaperBg);
    addChild(m_bg);

    m_bg->setContentSize(CCSize(320, 320));

    CCSprite *_tmp = blankSpriteWithSize(m_bg->getContentSize());
    m_block_sprite = new CCPhatTouchableSprite();
    m_block_sprite->initWithTexture(_tmp->getTexture());
    m_block_sprite->autorelease();
    m_block_sprite->setColor(ccBLUE);
    m_block_sprite->setVisible(false);
    addChild(m_block_sprite);

    m_ray = UIHelper::getSprite(UIHelper::HUDPaperArrow);
    addChild(m_ray);

    m_rayLeft = UIHelper::getSprite(UIHelper::HUDPaperArrowLeft);
    addChild(m_rayLeft);

    setContentSize(m_bg->getContentSize());
    
    scheduleUpdate();
    update(0);
    open();
    
    return true;
}

void MiniDialog::update(float delta_)
{
    CScene *_scene = (CScene*)g_currentScene;
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    if (Building *_building = (Building*)m_entity){
        if (typeid(*_building) == typeid(Building)) {
            CCPoint _pos = _scene->isoMap->convertToWorldSpace(_building->getPosition());
            float _shift = (_building->bVo.src_width*(0.5f-(0.25f*(targetDevice!=TD_IPAD_RETINA)))*_scene->scrollView->getScale());
            if (m_spread == 0) {
                if (_pos.x >= _screenSize.width*0.5f) {
                    m_spread = 1;
                }else {
                    m_spread = 2;
                }
            }
            if (m_spread == 1) {
                _pos.x -= _shift;
                setAnchorPoint(ccp(0.5f, 0.0f));
                m_ray->setPositionX(getContentSize().width*0.5f+m_ray->getContentSize().width*0.23f);
                m_ray->setVisible(true);
                m_rayLeft->setVisible(false);
            }else if (m_spread == 2) {
                _pos.x += _shift;
                setAnchorPoint(ccp(-0.5f, 0.0f));
                m_rayLeft->setPositionX(-getContentSize().width*0.5f-m_ray->getContentSize().width*0.17f);
                m_ray->setVisible(false);
                m_rayLeft->setVisible(true);
            }
            setPosition(_pos);
            if (m_close) m_close->setPosition((getContentSize()*0.5f)-m_close->getContentSize()*1.4f);

            keepDialogInView();
            if(m_centerDialog)
                centerDialog();
            return;
        }
    }
    
    //default
    setAnchorPoint(ccp(-0.5f, 0.0f));
    m_rayLeft->setPositionX(-getContentSize().width*0.5f-m_ray->getContentSize().width*0.17f);
    m_ray->setVisible(false);
    m_rayLeft->setVisible(true);
    if (m_close) m_close->setPosition((getContentSize()*0.5f)-m_close->getContentSize()*1.4f);

    keepDialogInView();
    if(m_centerDialog)
        centerDialog();
}

void MiniDialog::keepDialogInView()
{
    CCPoint point = getPosition();
    CCSize size = getContentSize();
    CCPoint anchor = getAnchorPoint();

    point.x -= anchor.x * size.width;
    point.y -= anchor.y * size.height;

    float left = point.x - (size.width * 0.5f);
    float right = point.x + (size.width * 0.5f);
    float top = point.y + (size.height * 0.5f);
    float bottom = point.y - (size.height * 0.5f);

    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    CCPoint adjustment = getPosition();

    if(left < 0.f)
        adjustment.x += fabsf(left);

    if(right > screenSize.width)
        adjustment.x -= right - screenSize.width;

    if(top > screenSize.height)
        adjustment.y -= top - screenSize.height;

    if(bottom < 0.f)
        adjustment.y += fabsf(bottom);

    setPosition(adjustment);
}

void MiniDialog::centerDialog()
{
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    setAnchorPoint(ccp(0.f, 0.f));
    setPosition(_screenSize.width / 2.f, _screenSize.height / 2.f);
    m_ray->setVisible(false);
    m_rayLeft->setVisible(false);
}

void MiniDialog::open() {
    
    if (numberOfRunningActions() == 0) {
        
        compose();
        
        setScale(0.5f);
        stopAllActions();
        runAction(CCEaseElasticOut::create(CCScaleTo::create(kMiniDialogTransitionDuration*5, 1.0f), kMiniDialogTransitionDuration*5));
        
        CCModalController::showAllChildrensAnimated(this, kMiniDialogTransitionDuration);
        
    }
    
}

void MiniDialog::close() {
    
    setScale(1.0f);
    stopAllActions();
    runAction(CCSequence::create(CCScaleTo::create(kMiniDialogTransitionDuration, 0.5f),
                                 CCRemoveSelf::create(),
//                                     CCCallFunc::create(this, callfunc_selector(MiniDialog::_reopen)),
                                 NULL));
    
    CCModalController::hideAllChildrensAnimated(this, kMiniDialogTransitionDuration);

}

bool MiniDialog::compose()
{
    //let inherited class do the work
    return true;
}

void MiniDialog::addContent(const char *icon_framename_, CCPoint icon_pos_, const char *label_, const char *value_, const char *plus_number_)
{
    addContent(icon_framename_, icon_pos_, 1.0f, label_, value_, plus_number_);
}

void MiniDialog::addContent(const char *icon_framename_, CCPoint icon_pos_, float icon_scale_, const char *label_, const char *value_, const char *plus_number_)
{
    CCSprite *_icon = CCSprite::createWithSpriteFrameName(icon_framename_);
    _icon->setPosition(icon_pos_);
    _icon->setScale(icon_scale_);
    addChild(_icon);
    
    float positionFromCenter = 29.0f;
    
    CCLabelBMFont *_label = CCLabelHelper::createBMFont(label_, kFontSizeSmall, true);
    _label->setAnchorPoint(ccp(0.0f, 0.5f));
    _label->setPosition(ccp(_icon->getPositionX()+(60*sizeMultiplier),
                            _icon->getPositionY()+(positionFromCenter*sizeMultiplier)));
    
//    _label->setColor(kFontLabelColour);
    addChild(_label);
    
    CCLabelBMFont *_value = CCLabelHelper::createBMFont("", kFontSizeSmall, kTextColorLight);
    _value->setString(value_);
    _value->setAnchorPoint(ccp(0.0f, 0.5f));
    _value->setPosition(ccp(_label->getPositionX(),
                            _icon->getPositionY()-(positionFromCenter*sizeMultiplier)));
    _value->setColor(kFontValueColour);
    addChild(_value);
    
    if (plus_number_ && strcmp(plus_number_, "+0")!=0 && strcmp(plus_number_, "+0.0")!=0)
    {
        auto xPos = _icon->getPositionX()+ 130.0f*sizeMultiplier;
        auto xPosBg = _icon->getPositionX()+ 135.0f*sizeMultiplier;
        if (xPos < _value->getPositionX() + _value->getContentSize().width + 15*sizeMultiplier)
        {
            xPos = _value->getPositionX() + _value->getContentSize().width + 15*sizeMultiplier;
            xPosBg = _value->getPositionX() + _value->getContentSize().width + 20*sizeMultiplier;
        }

        CCSprite *_plus_bg = UIHelper::getSprite(UIHelper::UpgradePlusBg);
        _plus_bg->setAnchorPoint(ccp(0.0f, 0.5f));
        _plus_bg->setPosition(ccp(xPos, _icon->getPositionY()-(positionFromCenter*sizeMultiplier)));
        _plus_bg->setScale(1.25f);
        addChild(_plus_bg);
        
        CCLabelBMFont *_plus_value = CCLabelHelper::createBMFont(plus_number_, kFontSizeSmall, kTextColorWhite);
        //change to number format with comma
        _plus_value->setString(plus_number_);
        
        _plus_value->setAnchorPoint(ccp(0.0f, 0.5f));

        _plus_value->setPosition(ccp(xPosBg, _icon->getPositionY()-(positionFromCenter*sizeMultiplier)));
        addChild(_plus_value);
    }
}
