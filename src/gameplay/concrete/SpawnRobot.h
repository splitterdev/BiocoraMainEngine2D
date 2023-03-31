#pragma once

class SpawnRobot : public GamePlayRobot {
    public:
    SpawnRobot() : GamePlayRobot() {
    }
    virtual ~SpawnRobot() {}

    virtual int onTick() {
        GameStateHolder* gameStateHolder = Cache::getGameStateHolder();
        Map* currentMap = gameStateHolder -> getMap();
        if ( currentMap ) {
            double spawnId = currentMap -> getParameter( Map::PARAM_MAP_SPAWN_ID );
            if ( !isnan( spawnId ) && ( spawnId >= 0 ) ) {
                SpawnMachine* machine = Cache::getFileAccessor() -> getSpawnMachine( spawnId );
                if ( machine ) {
                    list < int > spawnedIds = machine -> getSpawnedIds( Cache::gameTime() );
                    for ( list < int >::iterator i = spawnedIds.begin(); i != spawnedIds.end(); i++ ) {
                        Cache::getGameStateHolder() -> add( EntitySpawner::spawn( *i ) );
                    }
                }
            }
        }
        return GameStateProcessor::EVENT_RESULT_NONE;
    }

};


