#pragma once

class PickupType : public InteractiveType {
    public:
    PickupType( int id ) : InteractiveType( id ) {
    }
    virtual ~PickupType() {
    }

    virtual InteractiveObject* createObjectInstance( int assignedId );

    static const int ATTRIBUTE_TEXTURE_ID = 0;
    static const int ATTRIBUTE_PICKUP_TYPE = 1;
    static const int ATTRIBUTE_PICK_RANGE = 2;
    static const int ATTRIBUTE_COLLECT_SOUND = 3;
    static const int ATTRIBUTE_CUSTOM_VALUE = 4;
    static const int ATTRIBUTE_TIP_HOVER = 5;

    static const int ATTRIBUTE_POSITION_X = 101;
    static const int ATTRIBUTE_POSITION_Y = 102;

    static const int ATTRIBUTE_CREATE_TIME = 1001;

};


