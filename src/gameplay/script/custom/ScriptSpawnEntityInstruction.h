#pragma once

class ScriptSpawnEntityInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptSpawnEntityInstruction* create() {
            return new ScriptSpawnEntityInstruction();
        }
    };

    ScriptSpawnEntityInstruction() : ScriptInstruction() {}
    virtual ~ScriptSpawnEntityInstruction() {}

    virtual void perform() {
        Vector2D position;
        double entityId = 0;
        ScriptThread* thread = getThread();
        if ( thread && getParameter( 0, &( position.x ) ) && getParameter( 1, &( position.y ) ) && getParameter( 2, &entityId ) ) {
            thread -> setAccumulatorValue( -1 );
            if ( !isnan( entityId ) ) {
                EntityObject* entity = EntitySpawner::spawnAt( entityId, position.x, position.y );
                if ( entity ) {
                    Cache::getGameStateHolder() -> add( entity );
                    thread -> setAccumulatorValue( entity -> getId() );
                }
            }
            thread -> nextInstruction();
        }
    }

};


