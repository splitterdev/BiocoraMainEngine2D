#pragma once

class BulletType : public InteractiveType {
    public:
    BulletType( int id ) : InteractiveType( id ) {
    }
    virtual ~BulletType() {
    }

    virtual InteractiveObject* createObjectInstance( int assignedId );

    static const int DISPLAY_TYPE_LINE = 0;
    static const int DISPLAY_TYPE_ELECTRO_JOINED = 1;
    static const int DISPLAY_TYPE_ELECTRO_OPEN = 2;

    static const int ATTRIBUTE_TEXTURE_ID = 0;
    static const int ATTRIBUTE_MAX_CAPACITY = 1;
    static const int ATTRIBUTE_COST = 2;
    static const int ATTRIBUTE_BOX_SIZE = 3;
    static const int ATTRIBUTE_CORPSE_TEXTURE_ID = 4;
    static const int ATTRIBUTE_INVENTORY_TEXTURE_ID = 5;
    static const int ATTRIBUTE_NAME = 6;
    static const int ATTRIBUTE_COLOR_R = 7;
    static const int ATTRIBUTE_COLOR_G = 8;
    static const int ATTRIBUTE_COLOR_B = 9;
    static const int ATTRIBUTE_COLOR_A = 10;
    static const int ATTRIBUTE_TYPE = 11;
    static const int ATTRIBUTE_EXPLOSION_ON_DEATH_ID = 12;
    static const int ATTRIBUTE_AUTO_AIM = 13;
    static const int ATTRIBUTE_IGNORE_WALLS = 14;
    static const int ATTRIBUTE_IS_MATERIAL = 15;
    static const int ATTRIBUTE_RANGE = 16;
    static const int ATTRIBUTE_POSITION_X = 101;
    static const int ATTRIBUTE_POSITION_Y = 102;
    static const int ATTRIBUTE_LAST_POSITION_X = 103;
    static const int ATTRIBUTE_LAST_POSITION_Y = 104;
    static const int ATTRIBUTE_TARGET_X = 105;
    static const int ATTRIBUTE_TARGET_Y = 106;
    static const int ATTRIBUTE_MOTHER_ENTITY = 107;
    static const int ATTRIBUTE_VELOCITY = 108;
    static const int ATTRIBUTE_DAMAGE = 109;
    static const int ATTRIBUTE_MOVEMENT_X = 110;
    static const int ATTRIBUTE_MOVEMENT_Y = 111;
    static const int ATTRIBUTE_DEATH_PUSH = 112;
    static const int ATTRIBUTE_IS_IN_TRANSPARENT_MATERIAL = 113;
    static const int ATTRIBUTE_TRAVEL_DISTANCE = 114;

    inline bool isAutoAim() {
        return ( getParameter( BulletType::ATTRIBUTE_AUTO_AIM ) > 0.5 );
    }
};


