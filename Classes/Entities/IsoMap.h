//
//  IsoMap.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/29/56 BE.
//
//

#ifndef __Cheetah__IsoMap__
#define __Cheetah__IsoMap__

#define __Cheetah__IsoMap__Debug false

#include "cocos2d.h"
#include <cocos-ext.h>
#include <spine/spine-cocos2dx.h>

#define ISO_TILE_SIZE       120
#define ISO_TILE_COL        ISO_TILE_SIZE
#define ISO_TILE_ROW        ISO_TILE_SIZE
#define ISO_TILE_NUM        ISO_TILE_COL*ISO_TILE_ROW
#define ISO_TRACK_NUM       ISO_TILE_NUM/4
#define ISO_TILE_GRASS_VARY 5
#define ISO_UNAVAILABLE_I   -1
#define FIND_AVAILABLE_SPACE_RECURSIVE_MAX 3

#define CLOUD_Z_INDEX       500

#include "PlaceHolder.h"
#include "Pet.h"

//#define IMPLEMENT_LOCK_ZONE
#ifdef IMPLEMENT_LOCK_ZONE
#include "ShadowInactiveArea.h"
#endif

#include "TrackGlue.h"
#include "../Extensions/CCNikScrollView.h"

enum IsoMapView {
    IMV_PLAYABLE,
    IMV_VIEWONLY,
    IMV_RACING
};

struct SearchPath {
    
    cocos2d::CCPoint coord;
    int count;
    int trackno;
    
    SearchPath(cocos2d::CCPoint coord_, int count_, int trackno_);
    
};

class IsoMap: public cocos2d::CCLayer {
    
    int                             m_legalTileSize;
    
    IsoMapView                      m_viewType;
    
protected:
    
    cocos2d::CCSprite*              m_bgSprite[4][4];
    cocos2d::CCSpriteBatchNode*     m_tilesBatch;
    cocos2d::CCLayer*               m_layerTrack;
    
    cocos2d::CCSprite*              m_trackStart;
    cocos2d::CCSprite*              m_trackDirection;
    
    cocos2d::CCSize                 m_allLegalTileSize;
    cocos2d::CCPoint                m_allLegalTilePos;

#ifdef IMPLEMENT_LOCK_ZONE
    ShadowInactiveArea              *m_shadowGrid;
#endif
    
    //////////////////////////////////////////////////////////////
    // Logic functions
    bool _validateTrackCompletion();
    int _travelMap(int map[], int prepos, int curpos, int count);
    
    Track*  _findNearestTrack(cocos2d::CCPoint coord, int no, int avoidIndex);
    Track*  _findNearestTrack(cocos2d::CCPoint coord, int no);
    
    void _printTrackMap();
    
    void _setLegalTile(int legalTileSize_);

    bool                            m_petsAreSleepingCheck;

public:
    
    void                            *preferenceRootParent;
    void                            *mapPlayerState;
    
    //////////////////////////////////////////////////////////////
    // Static Helper Functions
    static cocos2d::CCPoint CoordToIso(cocos2d::CCPoint pt, cocos2d::CCSize size, cocos2d::CCPoint shift);
    static cocos2d::CCPoint IsoToCoord(cocos2d::CCPoint pt, cocos2d::CCSize size, cocos2d::CCPoint shift);
    static cocos2d::CCPoint IsoToCoord2(cocos2d::CCPoint pt, cocos2d::CCSize size, cocos2d::CCPoint shift);

    static cocos2d::CCPoint getCoordByMapIndex(int mapIndex_);
    static cocos2d::CCPoint getPositionByMapIndex(int mapIndex_, cocos2d::CCSize size_, cocos2d::CCPoint shift_);
    static cocos2d::CCPoint getPositionByMapIndexes(int *mapIndexes_, cocos2d::CCSize size_, cocos2d::CCPoint shift_);
    
    static int getTileLeftFrom(int tileIndex);
    static int getTileRightFrom(int tileIndex);
    static int getTileUpFrom(int tileIndex);
    static int getTileDownFrom(int tileIndex);
    
    static cocos2d::CCPoint getCoordByTrackIndex(int trackIndex);
    static cocos2d::CCPoint getPositionByTrackIndex(int trackIndex, cocos2d::CCSize size, cocos2d::CCPoint shift);
    static int getTileIndexByCoord(cocos2d::CCPoint coord);
    static int getTileIndexByPosition(cocos2d::CCPoint position_, cocos2d::CCSize size_, cocos2d::CCPoint shift_);
//    static void getTileIndexesByPosition(cocos2d::CCPoint position,
//                                         cocos2d::CCSize size,
//                                         cocos2d::CCPoint shift,
//                                         int *tiles,
//                                         int colum,
//                                         int row);
    static int getTrackIndexByCoord(cocos2d::CCPoint coord);
    static int getTrackIndexByPosition(cocos2d::CCPoint position, cocos2d::CCSize size, cocos2d::CCPoint shift);
    


    static cocos2d::CCPoint getSnapGridPosition(cocos2d::CCPoint pt, cocos2d::CCSize size, cocos2d::CCPoint shift, cocos2d::CCSize gridsize);
    static cocos2d::CCPoint getSnapQuadPosition(cocos2d::CCPoint pt, cocos2d::CCSize size, cocos2d::CCPoint shift, cocos2d::CCSize gridsize);
    
    //////////////////////////////////////////////////////////////
    // Objects in Iso Map
    
    Building                        *reservedDummy;
    Building                        *reservedDummyForTrack;
    void                            *tilesMap[ISO_TILE_NUM];
    int                             tilesLegal[ISO_TILE_NUM];
    
    int                             tilesTrack[ISO_TRACK_NUM];
    int                             tilesObstacle[ISO_TRACK_NUM];
    std::vector<TrackGlue*>         trackGlues;

    CCNikScrollView                 *scrollView;
    
    bool isTrackComplete();
    // param: force = system will find other track to set as start point if given track couldn't be set as start track,
    bool setAsStartTrack(Track* track);
    bool setAsStartTrackIfLegal(Track* track);
    bool updateTrackGlue();
    void switchTrackGlue();
    void removeTrackGlue();
    
    //////////////////////////////////////////////////////////////
    // State variables
//    bool lockCamera;
    
    //////////////////////////////////////////////////////////////
    // Core Functions
    
    virtual bool init();
    bool initWithPreferenceRootParent(void *preferenceRootParent_);
    CREATE_FUNC(IsoMap);
    virtual ~IsoMap();
    
    bool composeMap(void* playerState_, IsoMapView viewType_, void *preferenceRootParent_);
    void disposeMap();
    
    void addPetToItsOwnHabitat();
//    void addPetToItsOwnHabitat(std::string buid);
    void purgeAllPetsFromHabitat();
    
    Track* getTrackAt(int index_);
    Track* getTrackAtIndex(int index);
    Track* getObstacleAtIndex(int index);
    int getNBuilding();
    int getNTrack();
    int getNObstacle();
    
    Track* getNearestTrack(cocos2d::CCPoint coord, int no, int avoidIndex);
    void getTrackMapper(int trackIndex, int *trackMapper);
    
    int analystTrackDirection();
    bool getAvailableSpaceFor(GDBuildingVO buildingVo,
                              GDBuildingVO attachToBuildingVo,
                              int firstTileOfBuildingAttachTo,
                              int *tileIndexes);
    bool getAvailableSpaceFor(GDBuildingVO buildingVo,
                              GDBuildingVO attachToBuildingVo,
                              int firstTileOfBuildingAttachTo,
                              int *tileIndexes,
                              int recursiveLevel);
    bool checkAndAssignAvailableSpace(GDBuildingVO buildingVo,
                                      int firstTile,
                                      int *tileIndexes);
    bool isTileFree(int tileIndex);
    cocos2d::CCPoint getPositionOfAvailableSpaceFor(void *entity);
    cocos2d::CCPoint getPositionOfAvailableSpaceFor(GDBuildingVO vo);
    
#pragma mark - Buildings
    int calZOrderForBuilding(Building* building);
    bool addBuilding(Building *building_);
    bool addBuilding(Building *building_, bool update_backend_);
    bool addBuilding(Building *building_, BuildingBuildType buildType, bool update_backend);
    bool purgeBuilding(Building *building_);
    void updateBuildings();
    void focuzOnBuilding(const char *building_type);
    void focuzOnBuilding(const char *building_type, int index);
    
#pragma mark - Tracks
    bool addTrack(Track* track);
    bool applyTrack(Track* track);
    bool purgeTrack(PlayerTrackVO ptvo);
    bool purgeTrack(Track* track);
    bool removeTrack(Track* track);
    
    bool addTrackGlue(Track* track1, Track* track2);
    
    bool addDecoration(Decoration *decoration_);
    bool addDecoration(Decoration *decoration_, bool fixed_);
    bool addDecoration(Decoration *decoration_, bool fixed_, bool update_backend_, bool instant_);
    bool purgeDecoration(Decoration *decoration_);
    
    bool addPetToBuilding(std::string puid, std::string buid);
    bool areAllPetsSleeping();
    
//    bool updateTrainedResult();
    
    int getStartTrackIndex();
    int getLegalTileSize();
    cocos2d::CCSize getAllLegalTileSize();
    cocos2d::CCPoint getAllLegalTilePos();
    void *getMapPlayerState();
    
    void setLockCamera(bool lock);
    

    
#pragma mark - callback functions
    
    static void onBuildingUpdated(void *sender, std::string respn_str);
    static void onDecorationUpdated(void *sender, std::string respn_str);
    
};

#endif /* defined(__Cheetah__IsoMap__) */
