#pragma once

class ParticleType : public InteractiveType {
    public:
    ParticleType( int id ) : InteractiveType( id ) {
    }
    virtual ~ParticleType() {
    }

    virtual InteractiveObject* createObjectInstance( int assignedId );

    static const int ATTRIBUTE_TEXTURE_ID = 0;
    static const int ATTRIBUTE_MASS = 1;
    static const int ATTRIBUTE_COLLISION_TEXTURE_ID = 2;
    static const int ATTRIBUTE_COLLISION_SOUND_ID = 3;

    static const int ATTRIBUTE_POSITION_X = 101;
    static const int ATTRIBUTE_POSITION_Y = 102;
    static const int ATTRIBUTE_MOVEMENT_X = 103;
    static const int ATTRIBUTE_MOVEMENT_Y = 104;
    static const int ATTRIBUTE_HEIGHT = 105;
    static const int ATTRIBUTE_HEIGHT_DERV = 106;
    static const int ATTRIBUTE_ROTATION = 107;
    static const int ATTRIBUTE_ROTATION_SPEED = 108;

};


