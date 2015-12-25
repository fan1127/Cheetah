//
//  CCLoadSpriteURL..h
//  Cheetah
//
//  Created by Teerapat on 16/5/2557 BE.
//
//

#include "CCLoadSpriteURL.h"
#include "CCHttpRequest.h"

#include "../Settings.h"

USING_NS_CC;
using namespace cocos2d::extension;

CCLoadSpriteURL *CCLoadSpriteURL::create(char *defaultSprite, CCSize defaultSize, char *name, char *pUrl)
{
    CCLoadSpriteURL *pRet = new CCLoadSpriteURL();
    pRet->init(defaultSprite, defaultSize, name, pUrl);
    pRet->autorelease();
    return pRet;
}

bool CCLoadSpriteURL::init(char *defaultSprite, CCSize defaultSize, char *name, char *pUrl)
{
    
    if (!CCSprite::initWithSpriteFrameName(defaultSprite)) {
        return false;
    }
    
    std::string strUrl = pUrl;
    request = NULL;
    
//    check write path for catch
//    std::string dir = CCFileUtils::sharedFileUtils()->getWritablePath();
//    dir.append(name); //The name of the image
    
//    CCLog_("Default Size width = %f", defaultSize.width);
//    CCLog_("Default Size heigjt = %f", defaultSize.height);
    
//    CCLog_("file url path = %s",dir.c_str());
    if(strUrl=="") {
        this->setScaleX(defaultSize.width/ (this->getContentSize().width * this->getScaleX()));
        this->setScaleY(defaultSize.height/ (this->getContentSize().height * this->getScaleY()));
    }
//    else if( CCFileUtils::sharedFileUtils()->isFileExist(dir)) {
//        CCLog_("image exist!!");
//        this->initWithFile(dir.c_str());
//        this->setScaleX(defaultSize.width/this->getContentSize().width);
//        this->setScaleY(defaultSize.height/this->getContentSize().height);
//    }
    else {
        this->setScaleX(defaultSize.width/ (this->getContentSize().width * this->getScaleX()));
        this->setScaleY(defaultSize.height/ (this->getContentSize().height * this->getScaleY()));
        downLoadImage(name, pUrl);
    }
    
    return true;
}

void CCLoadSpriteURL::downLoadImage(char *name, char *pUrl)
{
    std::string strImage = name;
    
    request = new cocos2d::extension::CCHttpRequest();
    request->setUrl(pUrl);
    
    request->setRequestType(cocos2d::extension::CCHttpRequest::kHttpGet);
    request->setResponseCallback(this, httpresponse_selector(CCLoadSpriteURL::onImageDownLoaded));
    request->setTag(strImage.c_str());
    CCHttpClient::getInstance()->send(request);
    request->release();
    
}

void CCLoadSpriteURL::onImageDownLoaded(cocos2d::extension::CCHttpClient* pSender, cocos2d::extension::CCHttpResponse* pResponse)
{
    if(this != NULL) {
        CCHttpResponse* response = pResponse;
        
        if (!response)
        {
            CCLog_("No Response");
            return;
        }
        
//        int statusCode = response->getResponseCode();
//        char statusString[64] = {};
//        sprintf(statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
//        CCLog_("response string : %s", statusString);
        
        if (!response->isSucceed())
        {
            CCLog_("response failed");
            CCLog_("error buffer: %s", response->getErrorBuffer());
            return;
        }
        
        std::vector<char>*buffer = response->getResponseData();
        
        CCImage *img = new CCImage();
        img->initWithImageData(&(buffer->front()), buffer->size());
        
        cocos2d::CCTexture2D* texture = new cocos2d::CCTexture2D();
        texture->initWithImage(img);
        
        //backup original size before set new image from url
        CCSize originalSize = this->getContentSize();
        float originalScaleX = this->getScaleX();
        float originalScaleY = this->getScaleY();
        
        CCRect rect = CCRectZero;
        rect.size = texture->getContentSize();
        
        //init new image with URL image data
        this->CCSprite::initWithTexture(texture, rect);
        texture->release();
        
        //set new size with old original size
        this->setScaleX( originalSize.width * originalScaleX / this->getContentSize().width);
        this->setScaleY( originalSize.height * originalScaleY / this->getContentSize().height);
        
        img->release();
    }
    else CCLog_("This = NULL!!!!");
    
//    // Save image file to device.
//    std::string writablePath = CCFileUtils::sharedFileUtils()->getWritablePath();
//    writablePath.append(response->getHttpRequest()->getTag());
//    img->saveToFile(writablePath.c_str());
//    img->release();
//    
//    if(this) {
//        //backup original size before set new image from url
//        CCSize originalSize = this->getContentSize() * this->getScale();
//        
//        //init new image
//        this->initWithFile(writablePath.c_str());
//        
//        //set new size with old original size
//        this->setScaleX(originalSize.width / this->getContentSize().width);
//        this->setScaleY(originalSize.height / this->getContentSize().height);
//    }
    
}

