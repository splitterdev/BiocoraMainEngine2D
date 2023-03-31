#pragma once

class PickupCreator {
    private:
    PickupCreator() {}
    virtual ~PickupCreator() {}

    public:
    static PickupObject* newPickup( int pickupType, Vector2D position ) {
        PickupType* type = Cache::getFileAccessor() -> getPickupType( pickupType );
        if ( type ) {
            PickupObject* pickup = type -> createObject < PickupObject > ( Cache::getGameStateHolder() -> nextPickupId() );
            pickup -> setPosition( position );
            pickup -> setCreateTime( Cache::gameTime() );
            return pickup;
        }
        return NULL;
    }

};



