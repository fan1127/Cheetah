//
//  PlayerHelperPetVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 6/30/2557 BE.
//
//

#ifndef Cheetah_PlayerHelperPetVO_h
#define Cheetah_PlayerHelperPetVO_h

#include <stdio.h>

struct PlayerHelperPetVO {
    
    std::string     uuid;
    std::string     name;
    std::string     photo;
    std::string     pet_type;
    unsigned int    pet_top_speed;
    unsigned int    pet_corner_speed;
    unsigned int    pet_jumping;
    int             pet_horse_power;
    bool            isFriend;
    std::map<std::string, int> abilities;
    
    PlayerHelperPetVO() {
        
        uuid                = "";
        name                = "";
        photo               = "";
        pet_type            = "";
        pet_top_speed       = 0;
        pet_corner_speed    = 0;
        pet_jumping         = 0;
        pet_horse_power     = 0;
        isFriend            = false;
        abilities.clear();
        
    }
    
    PlayerHelperPetVO(std::string uuid_, std::string name_, std::string photo_, std::string pet_type_, int pet_top_speed_, int pet_corner_speed_, int pet_jumping_, bool isFriend_) {
        
        uuid                = uuid_;
        name                = name_;
        photo               = photo_;
        pet_type            = pet_type_;
        pet_top_speed       = pet_top_speed_;
        pet_corner_speed    = pet_corner_speed_;
        pet_jumping         = pet_jumping_;
        pet_horse_power     = pet_top_speed_ + pet_corner_speed_ + pet_jumping_;
        isFriend            = isFriend_;
        abilities.clear();
        
    }
    
    void copy(PlayerHelperPetVO vo_) {
        
        uuid                = vo_.uuid;
        name                = vo_.name;
        photo               = vo_.photo;
        pet_type            = vo_.pet_type;
        pet_top_speed       = vo_.pet_top_speed;
        pet_corner_speed    = vo_.pet_corner_speed;
        pet_jumping         = vo_.pet_jumping;
        pet_horse_power     = vo_.pet_horse_power;
        isFriend            = vo_.isFriend;
        abilities.clear();
        for (std::map<std::string, int>::iterator it = vo_.abilities.begin(); it != vo_.abilities.end(); ++it) {
            abilities[(*it).first] = (*it).second;
        }
        
    }
    
};

#endif
