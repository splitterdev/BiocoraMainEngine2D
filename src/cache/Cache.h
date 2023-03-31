#pragma once

class FileAccessor;
class GameStateHolder;
class GameStateProcessor;
class EventDelegateQueue;

class Cache {
    private:
    Cache() {}
    virtual ~Cache() {}

    static FileAccessor* _ACTIVE_FILEACCESSOR;
    static GameStateHolder* _ACTIVE_GAMESTATEHOLDER;
    static GameStateProcessor* _ACTIVE_GAMESTATEPROCESSOR;
    static EventDelegateQueue* _ACTIVE_EVENTDELEGATEQUEUE;
    static double _GAME_TIME;
    static double _LAST_TIME_DIFF_MUL;

    public:

    /// FILE ACCESSOR

    static inline FileAccessor* getFileAccessor() {
        return _ACTIVE_FILEACCESSOR;
    }

    static inline void setFileAccessor( FileAccessor* accessor ) {
        _ACTIVE_FILEACCESSOR = accessor;
    }

    static inline FileAccessor* unmountFileAccessor() {
        FileAccessor* handle = _ACTIVE_FILEACCESSOR;
        _ACTIVE_FILEACCESSOR = NULL;
        return handle;
    }

    /// GAME STATE HOLDER

    static inline GameStateHolder* getGameStateHolder() {
        return _ACTIVE_GAMESTATEHOLDER;
    }

    static inline void setGameStateHolder( GameStateHolder* stateHolder ) {
        _ACTIVE_GAMESTATEHOLDER = stateHolder;
    }

    static inline GameStateHolder* unmountGameStateHolder() {
        GameStateHolder* handle = _ACTIVE_GAMESTATEHOLDER;
        _ACTIVE_GAMESTATEHOLDER = NULL;
        return handle;
    }

    static inline void dropGameStateHolder() {
        if ( _ACTIVE_GAMESTATEHOLDER ) {
            delete _ACTIVE_GAMESTATEHOLDER;
            _ACTIVE_GAMESTATEHOLDER = NULL;
        }
    }

    /// GAME STATE PROCESSOR

    static inline GameStateProcessor* getGameStateProcessor() {
        return _ACTIVE_GAMESTATEPROCESSOR;
    }

    static inline void setGameStateProcessor( GameStateProcessor* stateProcessor ) {
        _ACTIVE_GAMESTATEPROCESSOR = stateProcessor;
    }

    static inline GameStateProcessor* unmountGameStateProcessor() {
        GameStateProcessor* handle = _ACTIVE_GAMESTATEPROCESSOR;
        _ACTIVE_GAMESTATEPROCESSOR = NULL;
        return handle;
    }

    /// EVENT DELEGATE QUEUE

    static inline EventDelegateQueue* getEventQueue() {
        return _ACTIVE_EVENTDELEGATEQUEUE;
    }

    /// TIMER

    /**
        Gets game time in milliseconds.
        @return Game time in milliseconds.
    */
    static inline double gameTime() {
        return _GAME_TIME;
    }

    /**
        Gets global time. Will cause game gaps when on hold.
        @return Global time since program start (SDL_GetTicks()).
    */
    static inline double globalTime() {
        return ( ( double ) SDL_GetTicks() ) / 1000.0;
    }

    /**
        Sets game time in milliseconds.
        @param newTime Time to set in milliseconds.
    */
    static inline void setGameTime( double newTime ) {
        _GAME_TIME = newTime;
    }

    /**
        Gets game last time difference multiplier in milliseconds.
        @return Game last time difference multiplier in milliseconds.
    */
    static inline double getTimeLastDifferenceMultiplier() {
        return _LAST_TIME_DIFF_MUL;
    }

    /**
        Sets game last time difference multiplier in milliseconds.
        @param newTime Last time difference multiplier to set in milliseconds.
    */
    static inline void setTimeLastDifferenceMultiplier( double diffMul ) {
        _LAST_TIME_DIFF_MUL = diffMul;
    }

    static const double DEFAULT_FRAME_RATE;

};

FileAccessor* Cache::_ACTIVE_FILEACCESSOR = NULL;
GameStateHolder* Cache::_ACTIVE_GAMESTATEHOLDER = NULL;
GameStateProcessor* Cache::_ACTIVE_GAMESTATEPROCESSOR = NULL;
EventDelegateQueue* Cache::_ACTIVE_EVENTDELEGATEQUEUE = new EventDelegateQueue();
double Cache::_GAME_TIME = 0.0;
double Cache::_LAST_TIME_DIFF_MUL = 1.0;
const double Cache::DEFAULT_FRAME_RATE = 60.0;


void Map::initBlockHpForEachField() {
        for ( int y = 0; y < getHeight(); y++ ) {
            for ( int x = 0; x < getWidth(); x++ ) {
                MAP_FIELD_TYPE mainField;
                if ( !getField( x, y, mainField ) ) {
                    continue;
                }
                BlockInfo mainInfo = Cache::getFileAccessor() -> getBlockInfo( mainField );
                initBlockHp( x, y, mainInfo );
            }
        }
    }

void Map::_updateGradient( int blockX, int blockY ) {
        if ( !_GRADIENT_FIELDS ) {
            return;
        }
        MAP_FIELD_TYPE mainField;
        if ( !getField( blockX, blockY, mainField ) ) {
            return;
        }
        BlockInfo mainInfo = Cache::getFileAccessor() -> getBlockInfo( mainField );
        if ( !mainInfo.isSolid() || mainInfo.isTransparentView() ) {
            return;
        }
        //const int DIR_INSERT[ 4 ][ 2 ] = { { -1, -1 }, { 1, -1 }, { 1, 1 }, { -1, 1 } };
        /*int flagId = 0x00000000;
        for ( int i = 0; i < 8; i++ ) {
            flagId = ( flagId << 1 );
            MAP_FIELD_TYPE otherField;
            if ( getField( blockX + Map::GRADIENT_DIR[ i ][ 0 ], blockY + Map::GRADIENT_DIR[ i ][ 1 ], otherField ) ) {
                BlockInfo otherInfo = Cache::getFileAccessor() -> getBlockInfo( otherField );
                if ( otherInfo.isSolid() ) {
                    flagId |= 0x00000001;
                }
            }
        }*/

        SDL_Texture* gradientTex = _getGradientTextureFor( blockX, blockY );
        if ( gradientTex ) {
            //printf( "Gradient copy\n" );
            SDL_Rect rect;
            rect.x = blockX * BLOCK_SIZE;
            rect.y = blockY * BLOCK_SIZE;
            rect.w = BLOCK_SIZE;
            rect.h = BLOCK_SIZE;
            SDL_RenderCopy( Renderer::getRenderer(), gradientTex, NULL, &rect );
        }
    }

SDL_Texture* Map::_getGradientTextureFor( int blockX, int blockY ) {
        bool debug = false;
        if ( blockX == 5 && blockY == 7 ) {
            debug = true;
        }
        int flagId = 0x00000000;
        for ( int i = 0; i < 4; i++ ) {
            flagId = ( flagId >> 1 );
            bool lightAccess = false;
            for ( int j = 0; j < 3; j++ ) {
                MAP_FIELD_TYPE otherField;
                if ( debug ) {
                    printf( " > Check: %d/%d -> sub %d/%d\n", int( blockX + GRADIENT_DIR[ i ].x ), int( blockY + GRADIENT_DIR[ i ].y ), int( blockX + Map::GRADIENT_DIR_CHECK[ i ][ j ].x ), int( blockY + Map::GRADIENT_DIR_CHECK[ i ][ j ].y ) );
                }
                if ( getField( blockX + Map::GRADIENT_DIR_CHECK[ i ][ j ].x, blockY + Map::GRADIENT_DIR_CHECK[ i ][ j ].y, otherField ) ) {
                    if ( debug ) {
                        printf( "   OtherInfo get: %d\n", int( otherField ) );
                    }
                    BlockInfo otherInfo = Cache::getFileAccessor() -> getBlockInfo( otherField );
                    if ( !otherInfo.isSolid() || otherInfo.isTransparentView() ) {
                        if ( debug ) {
                            printf( "   Light access\n" );
                        }
                        lightAccess = true;
                        break;
                    } else {
                        if ( debug ) {
                            printf( "   No light access\n" );
                        }
                    }
                }
            }
            if ( !lightAccess ) {
                flagId |= 0x00000008;
            }
        }
        if ( debug ) {
            printf( " > FlagId = %d\n", flagId );
        }
        return Map::_getGradientTexture( flagId );
    }



