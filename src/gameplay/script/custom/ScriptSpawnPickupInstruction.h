#pragma once

class ScriptSpawnPickupInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptSpawnPickupInstruction* create() {
            return new ScriptSpawnPickupInstruction();
        }
    };

    ScriptSpawnPickupInstruction() : ScriptInstruction() {}
    virtual ~ScriptSpawnPickupInstruction() {}

    virtual void perform() {
        Vector2D position;
        double pickupId = 0;
        ScriptThread* thread = getThread();
        if ( thread && getParameter( 0, &( position.x ) ) && getParameter( 1, &( position.y ) ) && getParameter( 2, &pickupId ) ) {
            thread -> setAccumulatorValue( -1 );
            if ( !isnan( pickupId ) ) {
                PickupObject* pickup = PickupCreator::newPickup( pickupId, position );
                if ( pickup ) {
                    Cache::getGameStateHolder() -> add( pickup );
                    thread -> setAccumulatorValue( pickup -> getId() );
                }
            }
            thread -> nextInstruction();
        }
    }

};



