#pragma once

class ScriptSetSpawnInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptSetSpawnInstruction* create() {
            return new ScriptSetSpawnInstruction();
        }
    };

    ScriptSetSpawnInstruction() : ScriptInstruction() {}
    virtual ~ScriptSetSpawnInstruction() {}

    virtual void perform() {
        Map* currentMap = Cache::getGameStateHolder() -> getMap();
        double spawnIdParamIndex = 0;
        ScriptThread* thread = getThread();
        if ( currentMap && getParameter( 0, &spawnIdParamIndex ) ) {
            currentMap -> setParameter( Map::PARAM_MAP_SPAWN_ID, spawnIdParamIndex );
            thread -> nextInstruction();
        }
    }

};



