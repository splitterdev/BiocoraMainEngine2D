#pragma once

class GameStateProcessor {
    public:

    static const int EVENT_RESULT_NONE = 0;

    class Schema {
        public:
        Schema() {
        }
        virtual ~Schema() {
            clearSchema();
        }

        void clearSchema() {
            for ( list < GamePlayRobot* >::iterator i = _robots.begin(); i != _robots.end(); i++ ) {
                if ( *i ) {
                    delete ( *i );
                }
            }
            _robots.clear();
        }

        inline void addRobot( GamePlayRobot* robot ) {
            if ( robot ) {
                _robots.push_back( robot );
            }
        }

        inline list < GamePlayRobot* > getRobots() {
            return _robots;
        }

        private:
        list < GamePlayRobot* > _robots;
    };

    GameStateProcessor() {
        _currentSchema = NULL;
    }

    virtual ~GameStateProcessor() {
        setGamePlay( NULL );
    }

    void setGamePlay( Schema* newSchema ) {
        if ( _currentSchema ) {
            delete _currentSchema;
        }
        _currentSchema = newSchema;
    }

    inline void gamePlayTick() {
        if ( _currentSchema ) {
            list < GamePlayRobot* > robots = _currentSchema -> getRobots();
            for ( list < GamePlayRobot* >::iterator i = robots.begin(); i != robots.end(); i++ ) {
                GamePlayRobot* robot = ( *i );
                if ( robot ) {
                    int eventResult = robot -> onTick();
                    switch ( eventResult ) {
                        case EVENT_RESULT_NONE: {
                            break;
                        }
                    }
                }
            }
        }
    }

    private:

    Schema* _currentSchema;

};


