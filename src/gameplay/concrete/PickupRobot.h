#pragma once

#include "pickup/PickupProcessor.h"

const double DEFAULT_PICKUP_REMOVE_TIME = 120.0 * 1000.0; // we have 2 min to pick up something, or it disappears in the logic below

class PickupRobot : public GamePlayRobot {
    public:
    PickupRobot() : GamePlayRobot() {
    }
    virtual ~PickupRobot() {}

    virtual int onTick() {
        GameStateHolder* gameStateHolder = Cache::getGameStateHolder();
        GameStateHolder::Container < PickupObject > pickupContainer = gameStateHolder -> getPickupContainer();
        EntityObject* player = gameStateHolder -> getPlayer();
        if ( player ) {
            while ( pickupContainer.hasMore() ) {
                PickupObject* pickup = pickupContainer.nextObject();
                if ( pickup ) {
                    bool collected = PickupProcessor::processPickup( player, pickup );
                    if ( collected ) {
                        pickupContainer.removeObject();
                        delete pickup;
                    } else {
                        if ( pickup -> getCreateTime() + DEFAULT_PICKUP_REMOVE_TIME < Cache::gameTime() ) {
                            pickupContainer.removeObject();
                            delete pickup;
                        }
                    }
                }
            }
        }
        return GameStateProcessor::EVENT_RESULT_NONE;
    }

};

