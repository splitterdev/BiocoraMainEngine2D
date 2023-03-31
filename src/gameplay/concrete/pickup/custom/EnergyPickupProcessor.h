#pragma once

class EnergyPickupProcessor : public PickupProcessor {
    public:
    EnergyPickupProcessor() : PickupProcessor() {
    }
    virtual ~EnergyPickupProcessor() {
    }

    virtual bool onPick( EntityObject* picker, PickupObject* pickup ) {
        double currentEnergy = picker -> getParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT );
        double currentMaxEnergy = picker -> getParameter( EntityType::ATTRIBUTE_ENERGY_MAX );
        //printf( "Health Value: %g\n", currentHp / currentMaxHp );
        if ( currentEnergy >= currentMaxEnergy ) {
            return false;
        }
        currentEnergy += pickup -> getCustomValue( 0 );
        //printf( "Health Value 2: %g\n", currentHp / currentMaxHp );
        if ( currentEnergy > currentMaxEnergy ) {
            currentEnergy = currentMaxEnergy;
        }
        picker -> setParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT, currentEnergy );
        return true;
    }

};


