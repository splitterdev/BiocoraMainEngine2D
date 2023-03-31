#pragma once

class ArmorType : public InteractiveType {
    public:
    ArmorType( int id ) : InteractiveType( id ) {
    }
    virtual ~ArmorType() {
    }

    virtual InteractiveObject* createObjectInstance( int assignedId );

    static const int ATTRIBUTE_HP = 0;
    static const int ATTRIBUTE_TEXTURE_ID = 1;
    static const int ATTRIBUTE_NAME = 2;
    static const int ATTRIBUTE_ENERGY_CONVERSION_VALUE = 3;
    static const int ATTRIBUTE_ENERGY_UPGRADE_VALUE = 4;

};


