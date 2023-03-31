#pragma once

class EnergyHeatSystemRobot : public GamePlayRobot {
    public:
    EnergyHeatSystemRobot() : GamePlayRobot() {
    }
    virtual ~EnergyHeatSystemRobot() {}

    virtual int onTick() {
        /*GameStateHolder* gameStateHolder = Cache::getGameStateHolder();
        GameStateHolder::Container < EntityObject > container = gameStateHolder -> getEntityContainer();
        while ( container.hasMore() ) {
            EntityObject* object = container.nextObject();
            processObject( object );
        }*/
        EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
        processObject( player );
        return GameStateProcessor::EVENT_RESULT_NONE;
    }

    void processObject( EntityObject* object ) {
        if ( object ) {
            object -> dissipateHeat( Cache::getTimeLastDifferenceMultiplier() );
            object -> restoreEnergy( Cache::getTimeLastDifferenceMultiplier() );
            object -> refillHealthBuffer( Cache::getTimeLastDifferenceMultiplier() );
            AutoTalkerConfig::autoTalk();
        }
    }

};



