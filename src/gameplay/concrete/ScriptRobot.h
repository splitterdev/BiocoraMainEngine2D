#pragma once

class ScriptRobot : public GamePlayRobot {
    public:
    ScriptRobot() : GamePlayRobot() {

    }
    virtual ~ScriptRobot() {}

    virtual int onTick() {
        GameStateHolder* gameStateHolder = Cache::getGameStateHolder();
        if ( gameStateHolder -> getPlayer() ) {
            ScriptPool* activePool = gameStateHolder -> getActiveScriptPool();
            if ( !activePool ) {
                Map* currentMap = gameStateHolder -> getMap();
                if ( currentMap ) {
                    double scriptId = currentMap -> getParameter( Map::PARAM_MAP_SCRIPT_ID );
                    if ( !isnan( scriptId ) && ( scriptId >= 0 ) ) {
                        ScriptThread* thread = Cache::getFileAccessor() -> getScript( scriptId );
                        if ( thread ) {
                            activePool = new ScriptPool();
                            activePool -> addScriptThread( thread );
                            gameStateHolder -> setActiveScriptPool( activePool );
                        }
                    }
                }
            }
            if ( activePool ) {
                activePool -> executeTick();
            }
            QuestManager::getDefault() -> processTick();
        }
        return GameStateProcessor::EVENT_RESULT_NONE;
    }

};


