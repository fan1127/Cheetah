//
//  CCSpriteHelper.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/31/2556 BE.
//
//

#include "CCSpriteHelper.h"
USING_NS_CC;

CCSprite* maskedSpriteWithSprite(CCSprite* sprite_, CCSprite* mask_) {
    // store the original positions of both sprites
    CCPoint textureSpriteOrigPosition(sprite_->getPosition().x, sprite_->getPosition().y);
    CCPoint maskSpriteOrigPosition(mask_->getPosition().x, mask_->getPosition().y);
    
    // convert the texture sprite position into mask sprite coordinate system
    sprite_->setPosition(ccp(sprite_->getContentSize().width/2 - mask_->getPosition().x + mask_->getContentSize().width/2, sprite_->getContentSize().height/2 - mask_->getPosition().y + mask_->getContentSize().height/2));
    
    // position the mask sprite so that the bottom left corner lies on the (o,o) coordinates
    mask_->setPosition(ccp(mask_->getContentSize().width/2, mask_->getContentSize().height/2));
    
    //CCRenderTexture* rt = CCRenderTexture::renderTextureWithWidthAndHeight((int)pMaskSprite->getContentSize().width, (int)pMaskSprite->getContentSize().height);
    CCRenderTexture* rt = CCRenderTexture::create((int)mask_->getContentSize().width, (int)mask_->getContentSize().height);
    
    ccBlendFunc bfMask = ccBlendFunc();
    bfMask.src = GL_ONE;
    bfMask.dst = GL_ZERO;
    mask_->setBlendFunc(bfMask);
    
    // turn off anti-aliasing around the mask sprite
    mask_->getTexture()->setAliasTexParameters();
    
    ccBlendFunc bfTexture = ccBlendFunc();
    bfTexture.src = GL_DST_ALPHA;
    bfTexture.dst = GL_ZERO;
    sprite_->setBlendFunc(bfTexture);
    
    rt->begin();
    mask_->visit();
    sprite_->visit();
    rt->end();
    
    // generate the resulting sprite
    //CCSprite* pOutcome = CCSprite::spriteWithTexture(rt->getSprite()->getTexture());
    CCSprite* pOutcome = CCSprite::createWithTexture(rt->getSprite()->getTexture());
    pOutcome->setFlipY(true);
    
    // restore the original sprite positions
    sprite_->setPosition(textureSpriteOrigPosition);
    mask_->setPosition(maskSpriteOrigPosition);
    pOutcome->setPosition(maskSpriteOrigPosition);
    
    return pOutcome;
    
}

CCSprite *blankSpriteWithSize(CCSize size_) {
    
    CCSprite *sprite = CCSprite::create();
    
    GLubyte *buffer = (GLubyte*)malloc(sizeof(GLubyte)*4);
    
    for (int i=0;i<4;i++) {buffer[i]=255;}
   // CCTexture2D *tex = [[CCTexture2D alloc] initWithData:buffer pixelFormat:kCCTexture2DPixelFormat_RGB5A1 pixelsWide:1 pixelsHigh:1 contentSize:size];
    CCTexture2D *tex = new CCTexture2D();
    tex->initWithData(buffer, kCCTexture2DPixelFormat_RGB5A1, 1, 1, size_);
//    [sprite setTexture:tex];
    tex->autorelease();
    sprite->setTexture(tex);
//    [sprite setTextureRect:CGRectMake(0, 0, size_.width, size_.height)];
    sprite->setTextureRect(CCRectMake(0, 0, size_.width, size_.height));
    free(buffer);
    
    return sprite;
    
}