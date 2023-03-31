#pragma once

class WeaponType : public InteractiveType {
    public:
    WeaponType( int id ) : InteractiveType( id ) {
    }
    virtual ~WeaponType() {
    }

    virtual InteractiveObject* createObjectInstance( int assignedId );

    static const int ATTRIBUTE_TEXTURE_ID = 0;
    static const int ATTRIBUTE_BULLET_ID = 1;
    static const int ATTRIBUTE_SHOOT_SOUND_ID = 2;
    static const int ATTRIBUTE_DAMAGE_MIN = 3;
    static const int ATTRIBUTE_DAMAGE_MAX = 4;
    static const int ATTRIBUTE_PRECISION = 5;
    static const int ATTRIBUTE_DELAY = 6;
    static const int ATTRIBUTE_BUCK = 7;
    static const int ATTRIBUTE_CLIP_SIZE = 8;
    static const int ATTRIBUTE_CLIP_CURRENT = 9;
    static const int ATTRIBUTE_RELOAD_TIME = 10;
    static const int ATTRIBUTE_BULLET_SPEED = 11;
    static const int ATTRIBUTE_SHOOT_QUAKE_POWER = 12;
    static const int ATTRIBUTE_SHOOT_QUAKE_DURATION = 13;
    static const int ATTRIBUTE_DEATH_PUSH = 14;
    static const int ATTRIBUTE_FIRE_TEXTURE_ID = 15;
    static const int ATTRIBUTE_INFINITY_AMMO = 16;
    static const int ATTRIBUTE_CURSOR_AFFECTION = 17;
    static const int ATTRIBUTE_NAME = 18;
    static const int ATTRIBUTE_ENERGY_CONSUMPTION = 19;
    static const int ATTRIBUTE_HEAT_MULTIPLIER = 20;
    static const int ATTRIBUTE_ENERGY_CONVERSION_VALUE = 21;
    static const int ATTRIBUTE_ENERGY_UPGRADE_VALUE = 22;
    static const int ATTRIBUTE_BULLET_SPREAD = 23;

    static const int ATTRIBUTE_NEXT_SHOOT_AVAILABLE = 100;
    static const int ATTRIBUTE_RELOAD_TIME_CURRENT = 101;
    static const int ATTRIBUTE_RELOAD_TIME_STARTED = 102;
    static const int ATTRIBUTE_UPGRADE_LEVEL = 103;

};


