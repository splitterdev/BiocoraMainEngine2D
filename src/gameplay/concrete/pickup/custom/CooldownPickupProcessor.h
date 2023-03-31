#pragma once

class CooldownPickupProcessor : public PickupProcessor {
    public:
    CooldownPickupProcessor() : PickupProcessor() {
    }
    virtual ~CooldownPickupProcessor() {
    }

    virtual bool onPick( EntityObject* picker, PickupObject* pickup ) {
        double heatCurrent = picker -> getParameter( EntityType::ATTRIBUTE_HEAT_CURRENT );
        if ( heatCurrent <= 0.0 ) {
            return false;
        }
        double dissipation = pickup -> getCustomValue( 0 );
        if ( heatCurrent < dissipation ) {
            heatCurrent = 0.0;
        } else {
            heatCurrent -= dissipation;
        }
        picker -> setParameter( EntityType::ATTRIBUTE_HEAT_CURRENT, heatCurrent );
        return true;
    }

};


