#pragma once

class ArmorObject : public InteractiveObject {
    public:
    ArmorObject( int id, int typeId ) : InteractiveObject( id, typeId ) {
    }
    virtual ~ArmorObject() {
    }
    virtual void setCalculatedAttributes() {
    }

    inline int getTextureId() {
        return getParameter( ArmorType::ATTRIBUTE_TEXTURE_ID );
    }

    inline string getTypeName() {
        double nameId = getParameter( ArmorType::ATTRIBUTE_NAME );
        if ( !isnan( nameId ) ) {
            return LangStrings::getString( ( int ) nameId );
        }
        return "???";
    }

    private:
};

InteractiveObject* ArmorType::createObjectInstance( int assignedId ) {
        return new ArmorObject( assignedId, getId() );
    }
