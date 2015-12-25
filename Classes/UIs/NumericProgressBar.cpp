//
//  NumericProgressBar.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad & Teerapat on 4/3/2557 BE.
//
//

#include "NumericProgressBar.h"

#include "../Extensions/CCLabelHelper.h"
#include "../Helpers/CCStringHelper.h"
#include "../Helpers/UIHelper.h"
#include "../Settings.h"

extern float margin;
extern float retinaMultiplier;
extern float sizeMultiplier;

NumericProgressBar* NumericProgressBar::create()
{
    NumericProgressBar *obj = new NumericProgressBar();
    if (!obj->init())
    {
        return NULL;
    }

    obj->autorelease();
    
    return obj;
}

bool NumericProgressBar::init()
{
    numProgress = 0;
    numMaximum = 1;
    m_progressBarEnabled = true;
    disableText = false;

    _bar = UIHelper::getScalableSprite(UIHelper::ProgressBarBg);
    this->addChild(_bar);
    
    m_progressBar = UIHelper::getScalableSprite(UIHelper::ProgressBarColor);
    m_progressBar->setAnchorPoint(ccp(0,0.5));
    m_progressBar->setContentSize(ccp(_bar->getContentSize().width*0.75,_bar->getContentSize().height*0.9));
    m_progressBar->setPosition(ccp(-_bar->getContentSize().width*0.49f,0));
    this->addChild(m_progressBar);
    
    progress_label = CCLabelHelper::createBMFont(retinaMultiplier, "0", kFontSizeMedium-5.0f, false, kTextColorDark);
    progress_label->setAnchorPoint(ccp(1, 0.5f));
    progress_label->setPosition(ccp(-_bar->getContentSize().width*0.02f, this->getContentSize().height*0.5f));
    this->addChild(progress_label);
    
    _label = CCLabelHelper::createBMFont(retinaMultiplier, "/", kFontSizeMedium-5.0f, false, kTextColorDark);
    _label->setAnchorPoint(ccp(0.5, 0.5f));
    _label->setPosition(ccp(_bar->getContentSize().width*0.01f, progress_label->getPositionY()));
    this->addChild(_label);
    
    maximum_label = CCLabelHelper::createBMFont(retinaMultiplier, "1", kFontSizeMedium-5.0f, false, kTextColorDark);
    maximum_label->setAnchorPoint(ccp(0, 0.5f));
    maximum_label->setPosition(ccp(_bar->getContentSize().width*0.04f, progress_label->getPositionY()));
    this->addChild(maximum_label);
    
    return true;
}

void NumericProgressBar::setProgressNumeric(unsigned int progress, unsigned int maximum)
{
    numProgress = progress;
    numMaximum = maximum;

    if (numProgress > numMaximum)
    {
        numProgress = numMaximum;
    }

    updateProgressBar();
}

void NumericProgressBar::setTextPosition(TextPosition position)
{
    switch (position)
    {
        case Centered:
            break;

        case LeftBottom:
            progress_label->setPosition(ccp(-_bar->getContentSize().width*0.45f, -_bar->getPositionY() - _bar->getContentSize().height));
            _label->setPosition(        ccp(-_bar->getContentSize().width*0.43f,  progress_label->getPositionY()));
            maximum_label->setPosition( ccp(-_bar->getContentSize().width*0.41f,  progress_label->getPositionY()));
            break;
    }
}

void NumericProgressBar::setWaitProgress()
{
    progress_label->setString("");
    _label->setString("waiting...");
    maximum_label->setString("");
    m_progressBar->setVisible(false);
}

void NumericProgressBar::hideText()
{
    disableText = true;
    updateProgressBar();
}

void NumericProgressBar::setContentSize(const CCSize& contentSize)
{
    _bar->setContentSize(contentSize);

    m_progressBar->setContentSize(ccp(_bar->getContentSize().width*0.75f, _bar->getContentSize().height*0.8f));
    m_progressBar->setPosition(ccp(-_bar->getContentSize().width*0.49f, _bar->getPositionY()));

    progress_label->setPosition(ccp(-_bar->getContentSize().width*0.02f, this->getContentSize().height*0.5f));

    _label->setPosition(ccp(_bar->getContentSize().width*0.01f, progress_label->getPositionY()));

    maximum_label->setPosition(ccp(_bar->getContentSize().width*0.04f, progress_label->getPositionY()));
}

void NumericProgressBar::updateProgressBar()
{
//    CREATE_CHAR_BUFFER(_buffer, 16);
//    snprintf(_buffer, _buffer_size, "%d", numProgress);
//    progress_label->setString(_buffer);
//    
//    snprintf(_buffer, _buffer_size, "%d", numMaximum);
//    maximum_label->setString(_buffer);
    
    _label->setString("/");
    
    progress_label->setString(CCStringHelper::createWithNumberFormat(numProgress).c_str());
    maximum_label->setString(CCStringHelper::createWithNumberFormat(numMaximum).c_str());
        
    float widthBar = (float)numProgress/(float)numMaximum;
    
    //scale Factor = 0.98
    widthBar*=0.98f;
    m_progressBar->setVisible(true);

    //17 = min width bar
    if(_bar->getContentSize().width * widthBar<30*retinaMultiplier)
    {
        widthBar = 30.0/_bar->getContentSize().width;
        m_progressBar->setVisible(false);
    }

    CCLog_("Progress bar width: %f, maxWidth: %f", _bar->getContentSize().width * widthBar, (17+17)*sizeMultiplier);

    m_progressBar->setContentSize(ccp(_bar->getContentSize().width * widthBar, _bar->getContentSize().height*0.8));

    if (disableText)
    {
        progress_label->setString("");
        _label->setString("");
        maximum_label->setString("");
    }
}

void NumericProgressBar::setProgressBarEnable(bool enable)
{
    m_progressBarEnabled = enable;
    m_progressBar->setVisible(m_progressBarEnabled);
}

void NumericProgressBar::setLabelOffsetX(float offsetX)
{
    progress_label->setPosition(ccp(this->getContentSize().width*0.95f - offsetX, this->getContentSize().height*0.5f));
    _label->setPosition(ccp(this->getContentSize().width*0.95f - offsetX, this->getContentSize().height*0.5f));
    maximum_label->setPosition(ccp(this->getContentSize().width*0.95f - offsetX, this->getContentSize().height*0.5f));
}
