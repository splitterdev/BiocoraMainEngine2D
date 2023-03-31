#pragma once

bool INVENTORY_FORCE_REFRESH_SIGNAL = false;

class PickupProcessor {
    public:
    PickupProcessor() {
    }
    virtual ~PickupProcessor() {
    }

    virtual bool onPick( EntityObject* picker, PickupObject* pickup ) = 0;

    static bool processPickup( EntityObject* picker, PickupObject* pickup ) {
        Vector2D pickerPosition = picker -> getPosition();
        Vector2D pickupPosition = pickup -> getPosition();
        bool returned = false;
        if ( pickerPosition.dist2D( pickupPosition ) < pickup -> getPickRange() * double( Map::BLOCK_SIZE ) ) {
            int pickupProcessorId = pickup -> getPickupType();
            map < int, PickupProcessor* >::iterator processor = _processors.find( pickupProcessorId );
            if ( processor != _processors.end() ) {
                returned = processor -> second -> onPick( picker, pickup );
            } else {
                returned = true;
            }
        }
        if ( returned ) {
            INVENTORY_FORCE_REFRESH_SIGNAL = true;
            Sound* collectSound = Cache::getFileAccessor() -> getSound( pickup -> getCollectSoundId() );
            if ( collectSound ) {
                collectSound -> play();
            }
        }
        return returned;
    }

    static map < int, PickupProcessor* > mountProcessors();

    private:

    static map < int, PickupProcessor* > _processors;

};

/// Pickup processor mapping

const int PICKUP_PROCESSOR_ID_EXPERIENCE = 0;
const int PICKUP_PROCESSOR_ID_HEALTH = 1;
const int PICKUP_PROCESSOR_ID_ENERGY = 2;
const int PICKUP_PROCESSOR_ID_COOLDOWN = 3;
const int PICKUP_PROCESSOR_ID_WEAPON_BLUEPRINT = 4;

#include "custom/HealthPickupProcessor.h"
#include "custom/ExpPickupProcessor.h"
#include "custom/EnergyPickupProcessor.h"
#include "custom/CooldownPickupProcessor.h"
#include "custom/WeaponBlueprintPickupProcessor.h"

map < int, PickupProcessor* > PickupProcessor::mountProcessors() {
    map < int, PickupProcessor* > processorMapping;
    processorMapping[ PICKUP_PROCESSOR_ID_HEALTH ] = new HealthPickupProcessor();
    processorMapping[ PICKUP_PROCESSOR_ID_EXPERIENCE ] = new ExpPickupProcessor();
    processorMapping[ PICKUP_PROCESSOR_ID_ENERGY ] = new EnergyPickupProcessor();
    processorMapping[ PICKUP_PROCESSOR_ID_COOLDOWN ] = new CooldownPickupProcessor();
    processorMapping[ PICKUP_PROCESSOR_ID_WEAPON_BLUEPRINT ] = new WeaponBlueprintPickupProcessor();
    return processorMapping;
}

map < int, PickupProcessor* > PickupProcessor::_processors = PickupProcessor::mountProcessors();

