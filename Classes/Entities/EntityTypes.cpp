//
//  EntityTypes.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/19/2557 BE.
//
//

#include "EntityTypes.h"

#include "Building.h"
#include "Decoration.h"

EntityType getEntityType(void *entity_) {
    
    if (Building *_building = (Building*)entity_) {
        if (typeid(*_building) == typeid(Building)) {
            
            if (_building->bVo.type == kBuildingHQ) {
                return ET_BUILDING_HQ;
            }else if (_building->bVo.type == kBuildingUncleHouse) {
                return ET_BUILDING_UNCLE_HOUSE;
            }else if (_building->bVo.type == kBuildingFoodProduction) {
                return ET_BUILDING_PRODUCER;
            }else if (_building->bVo.type == kBuildingFoodStorage) {
                return ET_BUILDING_FOOD_STORAGE;
            }else if (_building->bVo.type == kBuildingItem) {
                return ET_BUILDING_INVENTORY;
            } else if (_building->bVo.type == kBuildingAssociation) {
                return ET_BUILDING_ASSOCIATION;
            } else if (_building->bVo.type == kBuildingFusion) {
                return ET_BUILDING_MARKET;
            } else if (_building->bVo.type == kBuildingTicket) {
                return ET_BUILDING_TICKETBOOTH;
            } else if (_building->bVo.type == kBuildingBank) {
                return ET_BUILDING_COIN_STORAGE;
            } else if (_building->bVo.type == kBuildingHabitat || _building->bVo.type == kBuildingHabitatPremium) {
                return ET_BUILDING_HABITAT;
            } else if (_building->bVo.type == kBuildingAnimal) {
                return ET_BUILDING_ANIMAL;
            } else if (_building->bVo.type == kBuildingTraining) {
                return ET_BUILDING_TRAINING;
            } else if (_building->bVo.type == kBuildingGarage) {
                return ET_BUILDING_EXPLORER;
            } else if (_building->bVo.type == kBuildingLibrary) {
                return ET_BUILDING_LIBRARY;
            }
            
        }
    }
    
    if (Decoration *_decoration = (Decoration*)entity_) {
        if (typeid(*_decoration) == typeid(Decoration)) {
            if (_decoration->dVo.decoration_type == kDecorationTypeUnwanted) {
                return ET_UNWANTED_TREE;
            } else if (_decoration->dVo.decoration_type == kDecorationTypeGeneral ||
                       _decoration->dVo.decoration_type == kDecorationTypeFlag) {
                return ET_DECORATION_REMOVABLE;
            }
        }
    }
    
    return ET_UNKNOWN;
    
}