#pragma once

/** CUSTOM DEFINITIONS */

#define PERSPECTIVE_LEVEL_ID 1
#define PERSPECTIVE_LOADING_ID 2
#define PERSPECTIVE_MENU_ID 3
#define PERSPECTIVE_WARP_ID 4

int FPS = 0;

/** CUSTOM DEFINITIONS END */

class Perspective {
    public:

    class PerspectiveEvent {
        public:
        PerspectiveEvent() { }
        PerspectiveEvent( int e_code ) {
            code = e_code;
            for ( int i = 0; i < 16; i++ ) {
                arg[ i ] = 0;
            }
        }
        PerspectiveEvent argument( int index, int value ) {
            arg[ index ] = value;
            return *this;
        }
        int code;
        int arg[ 16 ];
    };

    protected:

    Perspective( int perspectiveID ) {
        _perspectiveID = perspectiveID;
        _q = new list < PerspectiveEvent > ();
    }

    public:
    virtual ~Perspective() {
        if ( _q ) {
            delete _q;
        }
    }

    static Perspective* Setup( Perspective* perspective ) {
        if ( !perspective ) {
            return NULL;
        }
        map < int, Perspective* >::iterator found = _map.find( perspective -> _perspectiveID );
        if ( found == _map.end() ) {
            pair < int, Perspective* > p;
            p.first = perspective -> _perspectiveID;
            p.second = perspective;
            _map.insert( p );
            return perspective;
        }
        return NULL;
    }

    static Perspective* Overwrite( Perspective* perspective ) {
        map < int, Perspective* >::iterator found = _map.find( perspective -> _perspectiveID );
        if ( found != _map.end() ) {
            if ( perspective == found -> second ) {
                return perspective;
            }
            if ( _current == found -> second ) {
                _current = perspective;
            }
            if ( found -> second ) {
                delete found -> second;
            }
            _map.erase( found );
        }
        if ( perspective ) {
            pair < int, Perspective* > p;
            p.first = perspective -> _perspectiveID;
            p.second = perspective;
            _map.insert( p );
        }
        return perspective;
    }

    virtual void onCreate() = 0;
    virtual void onDestroy() = 0;
    virtual bool onTick() = 0;

    int getPerspectiveID() {
        return _perspectiveID;
    }

    static Perspective* Current() {
        return _current;
    }

    static void ChangeTo( int npID ) {
        _nextPerspectiveChangeID = npID;
    }

    static bool tick() {
        //FPS = _fpsCounter();
        if ( _nextPerspectiveChangeID >= 0 ) {
            _performChangePerspective( _nextPerspectiveChangeID );
            _nextPerspectiveChangeID = -1;
        }
        if ( _current ) {
            bool retVal = _current -> onTick();
            return retVal;
        }
        RaiseException( "ERROR: NO PERSPECTIVE" );
        return false;
    }

    static Perspective* GetPerspective( int id ) {
        map < int, Perspective* >::iterator found = _map.find( id );
        if ( found != _map.end() ) {
            return found -> second;
        }
        return NULL;
    }

    static void RaiseException( const char* fmt, ... ) {
        char buffer[ 4096 ];
        va_list args;
        va_start( args, fmt );
        vsnprintf( buffer, 4095, fmt, args );
        va_end( args );
        throw Exception( buffer );
    }

    void sendEvent( PerspectiveEvent event ) {
        _q -> push_back( event );
    }

    bool getNextEvent( PerspectiveEvent* event ) {
        if ( !_q -> empty() ) {
            if ( event ) {
                ( *event ) = _q -> front();
            }
            _q -> pop_front();
            return true;
        }
        return false;
    }

    virtual FormBundle* buildInterface() = 0;

    void buildInterfaceOnCreate() {
        _interfaceBundle = buildInterface();
        FormCache::setActiveBundle( _interfaceBundle );
    }

    void disposeInterface() {
        if ( _interfaceBundle ) {
            delete _interfaceBundle;
            _interfaceBundle = NULL;
        }
        FormCache::setActiveBundle( NULL );
    }

    void drawInterface() {
        FormCache::displayActiveBundle();
    }

    private:

    int _perspectiveID;

    list < PerspectiveEvent >* _q;

    FormBundle* _interfaceBundle;

    static Perspective* _current;
    static map < int, Perspective* > _map;
    static int _nextPerspectiveChangeID;

    static void _performChangePerspective( int npID ) {
        if ( _current ) {
            _current -> onDestroy();
            _current -> disposeInterface();
            _current = NULL;
        }
        map < int, Perspective* >::iterator found = _map.find( npID );
        if ( found != _map.end() ) {
            _current = found -> second;
        }
        Keys::resetInput();
        if ( _current ) {
            _current -> buildInterfaceOnCreate();
            _current -> onCreate();
        }
    }

    int _fpsCounter() {
        static int cnt = 0;
        static int fpss = 0;
        static int t = SDL_GetTicks();
        cnt++;
        if ( int( SDL_GetTicks() ) > t + 1000 ) {
            t = SDL_GetTicks();
            fpss = cnt;
            cnt = 0;
        }
        return fpss;
    }

};

Perspective* Perspective::_current = NULL;
map < int, Perspective* > Perspective::_map;
int Perspective::_nextPerspectiveChangeID = -1;


