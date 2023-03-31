#pragma once

#include "LogicCallResult.h"

class Map;

#define LOGIC_SOLDIER 1

class Logic {
    public:
    Logic() {
        clearData();
    }
    virtual ~Logic() {
    }
    virtual LogicCallResult onCall() = 0;

    protected:

    EntityObject* getCaller() {
        return _caller;
    }

    Map* getMap() {
        return _map;
    }

    int getGameTime() {
        return _gameTime;
    }

    public:

    static const int FLAG_NONE = 0x00000000;
    static const int FLAG_PLAYER_WALK = 0x00000001;
    static const int FLAG_PLAYER_RUN = 0x00000002;
    static const int FLAG_PLAYER_SHOOT = 0x00000004;
    static const int FLAG_ENTITY_DAMAGED = 0x00000008;

    static const double MAX_ENTITY_DISTANCE = 100.0;

    void setData( EntityObject* caller, Map* map, int gameTime ) {
        _caller = caller;
        _map = map;
        _gameTime = gameTime;
    }

    inline void clearData() {
        setData( NULL, NULL, -1 );
    }

    private:

    EntityObject* _caller;
    Map* _map;
    int _gameTime;

};

