#pragma once

class PickupObject : public InteractiveObject {
    public:
    PickupObject( int id, int typeId ) : InteractiveObject( id, typeId ) {
    }
    virtual ~PickupObject() {
    }
    virtual void setCalculatedAttributes() {
    }

    inline int getTextureId() {
        return getParameter( PickupType::ATTRIBUTE_TEXTURE_ID );
    }

    inline void setTextureId( int textureId ) {
        setParameter( PickupType::ATTRIBUTE_TEXTURE_ID, textureId );
    }

    inline Vector2D getPosition() {
        return Vector2D(
            getParameter( PickupType::ATTRIBUTE_POSITION_X ),
            getParameter( PickupType::ATTRIBUTE_POSITION_Y )
        );
    }

    inline void setPosition( double x, double y ) {
        setParameter( PickupType::ATTRIBUTE_POSITION_X, x );
        setParameter( PickupType::ATTRIBUTE_POSITION_Y, y );
    }

    inline void setPosition( Vector2D position ) {
        setParameter( PickupType::ATTRIBUTE_POSITION_X, position.x );
        setParameter( PickupType::ATTRIBUTE_POSITION_Y, position.y );
    }

    inline double getPickRange() {
        return getParameter( PickupType::ATTRIBUTE_PICK_RANGE );
    }

    inline int getPickupType() {
        return getParameter( PickupType::ATTRIBUTE_PICKUP_TYPE );
    }

    inline double getCreateTime() {
        return getParameter( PickupType::ATTRIBUTE_CREATE_TIME );
    }

    inline void setCreateTime( double time ) {
        setParameter( PickupType::ATTRIBUTE_CREATE_TIME, time );
    }

    inline int getCollectSoundId() {
        return getParameter( PickupType::ATTRIBUTE_COLLECT_SOUND );
    }

    inline string getTip() {
        int tipId = getParameter( PickupType::ATTRIBUTE_TIP_HOVER );
        if ( tipId >= 0 ) {
            return LangStrings::getString( tipId );
        }
        return string( "" );
    }

    /// Custom values - use as parameters

    inline double getCustomValue( int key ) {
        map < int, double > mvParam;
        if ( getMultiValuedParameter( PickupType::ATTRIBUTE_CUSTOM_VALUE, mvParam ) ) {
            map < int, double >::iterator i = mvParam.find( key );
            if ( i != mvParam.end() ) {
                return i -> second;
            }
        }
        return 0.0;
    }

};

InteractiveObject* PickupType::createObjectInstance( int assignedId ) {
        return new PickupObject( assignedId, getId() );
    }

