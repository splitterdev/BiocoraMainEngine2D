#pragma once

#include "Logic.h"

class LogicProcessor {
    private:
    LogicProcessor() {
    }
    ~LogicProcessor() {
    }
    public:

    static LogicCallResult call( int logicID, EntityObject* caller, Map* mapInstance, int gameTime ) {
        LogicCallResult result = LogicCallResult::NotFound();
        map < int, Logic* >::iterator found = _LOGIC_CALL_MAP.find( logicID );
        if ( found != _LOGIC_CALL_MAP.end() ) {
            Logic* logic = found -> second;
            logic -> setData( caller, mapInstance, gameTime );
            result = logic -> onCall();
            logic -> clearData();
        }
        return result;
    }

    static void addLogic( int id, Logic* logic ) {
        _LOGIC_CALL_MAP[ id ] = logic;
    }

    private:
    static map < int, Logic* > _LOGIC_CALL_MAP;
};

map < int, Logic* > LogicProcessor::_LOGIC_CALL_MAP;

