    //
//  GachaPanel.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/24/2557 BE.
//
//

#ifndef __Cheetah__GachaPanel__
#define __Cheetah__GachaPanel__

#include "cocos2d.h"

#include "../UIs/GachaWheel.h"
#include "../Extensions/CCNikButton.h"
#include "../UIs/Elements/ResourceProgressButton.h"

class GachaPanel;

class CC_DLL GachaPanelDelegate
{
public:
    virtual void gachaPanelDidClosing(GachaPanel *gachaPanel) {};
};

class GachaPanel: public cocos2d::CCLayerColor {
    
    GachaPanelDelegate *m_delegate;
    
    CCNode *contentNode;
    
    CCNode *titleNode;
    
    cocos2d::CCArray *m_glowIns;
    cocos2d::CCArray *m_glowOuts;
    
    cocos2d::CCArray *m_starLightIns;
    cocos2d::CCArray *m_starLightOuts;
    
    int                     oldGems;
    
    std::string             m_lastPet_puid;
    
    bool                    m_freeSpinBtnEnabled;
    bool                    m_normalSpinBtnEnabled;
    bool                    m_premiumSpinBtnEnabled;
    bool                    m_backBtnEnabled;

    
    void _checkItemCapacity();

    CCSkeletonAnimation* m_gachaBoxFree;
    CCSkeletonAnimation* m_gachaBoxNormal;
    CCSkeletonAnimation* m_gachaBoxPremium;
    CCSkeletonAnimation* m_gachaBoxOpen;
    
    CCLabelBMFont* m_lockedLabel;

    CCLabelBMFont* helpTextLabel;
    CCLabelBMFont* helpTextSmallLabel;

    CCScale9Sprite* m_promotionRibbon;
    CCLabelBMFont* m_promotionText;

    struct Promotion
    {
        int percent;
        int originalPrice;
        bool oneTimeOffer;
        int getNewPrice() const { return int(originalPrice*percent/100.0f); }
    };

    bool m_promotion;
    bool m_promotionUsed;

    Promotion getPromotion(int originalPrice) const;
    void checkPromotion();
    bool isPromotion() const { return m_promotion; }
    void applyPromotion();
    bool showPromotionAlert() const { return m_promotion && !m_promotionUsed; }

    void createGachaBoxes();
    void createGachaButtons();
    CCNikButton* createGachaButtons(bool isFree, int price, bool locked);
    CCNikButton* createGachaButtons(bool isFree, int price, bool locked, int badge);
    CCNikButton* createGachaPromotionButton(const Promotion& promotion);
    void showAcquiredPet();
    void showAcquiredItem();
    void spinForPet(const char* type);
    void openGachaBox(const char* animName, SEL_CallFunc selector);
    void showGachaBoxes();
    bool isFreeBoxAvailable();
    bool isBarnFull();
    
    void _updateFreeButtonIdleState();
    void onEndDialog();
    void buyAnimalWithPromotion();

public:

    CCNikButton *backButton;

    CCNikButton *premiumButton;
    CCNikButton *freeButton;
    CCNikButton *normalButton;
    
    ResourceProgressButton  *m_gemButton;
    
    std::vector<GDPetVO> m_gotPets;
    std::vector<GDItemVO> m_gotItems;
    
    static GachaPanel* create();
    virtual bool init();
    virtual void onExit();
    
    ~GachaPanel();
    
    void setDelegate(GachaPanelDelegate *delegate){ m_delegate = delegate; };
    
    void animateStarLights();

    void updateUI(unsigned int numGems);
    void update(float delta);
    
    static bool isPremiumBoxUnlocked();
    static int premiumBoxUnlockLevel();
    
#pragma mark - Core functions
    
    void presentPanel(CCNode *onNode);
    void dismissPanel();
    
    void disableButtons();
    void enableButtons();
    
    void setFreeSpinEnabled(bool enabled);
    void setFreeSpinButtonEnabled(bool enabled);
    
    void setNormalSpinButtonEnabled(bool enabled) { m_normalSpinBtnEnabled = enabled; }
    void setPremiumSpinButtonEnabled(bool enabled) { m_premiumSpinBtnEnabled = enabled; }
    void setBackButtonEnabled(bool enabled) { m_backBtnEnabled = enabled; }
    
#pragma mark - Button callback
    
    void onFreeBtnPressed(CCObject *sender);
    void onNormalBtnPressed(cocos2d::CCObject *sender);
    void onPremiumBtnPressed(cocos2d::CCObject *sender);
    void onBackBtnPressed(cocos2d::CCObject *sender);
    void onGemsBarPressed(cocos2d::CCObject *sender);
    static void onAlertDialog(void *sender_, bool confirm_);
    static void onIAPSuccess(void* sender);

    virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    
#pragma mark - Backend Callback
    
    static void onGachaReturn(void* sender_, std::string resp_json_str);

    void paticleRandom();

    void onResultDismissed();
    
    float getBoxFreeScale() { return m_gachaBoxFree->getScale(); };
    float getBoxNormalScale() { return m_gachaBoxNormal->getScale(); };
    float getBoxPremiumScale() { return m_gachaBoxPremium->getScale(); };
    
};


#endif /* defined(__Cheetah__GachaPanel__) */
