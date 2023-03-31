#pragma once

class HealthPickupProcessor : public PickupProcessor {
    public:
    HealthPickupProcessor() : PickupProcessor() {
    }
    virtual ~HealthPickupProcessor() {
    }

    virtual bool onPick( EntityObject* picker, PickupObject* pickup ) {
        double currentHp = picker -> getCurrentHp();
        double currentMaxHp = picker -> getCurrentMaxHp();
        //printf( "Health Value: %g\n", currentHp / currentMaxHp );
        if ( currentHp >= currentMaxHp ) {
            return false;
        }
        currentHp += pickup -> getCustomValue( 0 );
        //printf( "Health Value 2: %g\n", currentHp / currentMaxHp );
        if ( currentHp > currentMaxHp ) {
            currentHp = currentMaxHp;
        }
        picker -> setCurrentHp( currentHp );
        return true;
    }

};


