#pragma once

class TimedDisplay {
    private:
    TimedDisplay() {}
    public:
    ~TimedDisplay() {}

    static const int LOAD_TYPE_REGULAR = 0;
    static const int LOAD_TYPE_BURST = 1;

    static void clear() {
        for ( map < Uint32, list < TimedDisplay* > >::iterator i = _displayTimeMap.begin(); i != _displayTimeMap.end(); i++ ) {
            list < TimedDisplay* > displayList = i -> second;
            _dispose( displayList );
        }
        _displayTimeMap.clear();
    }

    static TimedDisplay* createRegular( int textureId, Vector2D position, double rotation, double scale, Uint32 msTimeout ) {
        TimedDisplay* timed = new TimedDisplay();
        timed -> _type = LOAD_TYPE_REGULAR;
        timed -> _textureId = textureId;
        timed -> _position = position;
        timed -> _rotation = rotation;
        timed -> _scale = scale;
        _addToMap( Cache::gameTime() + msTimeout, timed );
        return timed;
    }

    static TimedDisplay* createBurst( Vector2D position, double rangeMin, double rangeMax, Uint32 msRefreshTime, Uint32 msTimeout ) {
        TimedDisplay* timed = new TimedDisplay();
        timed -> _type = LOAD_TYPE_BURST;
        timed -> _textureId = -1;
        timed -> _position = position;
        timed -> _rangeMin = rangeMin;
        timed -> _rangeMax = rangeMax;
        timed -> _msRefreshTime = msRefreshTime;
        timed -> _msLastRefreshTime = 0;
        _addToMap( Cache::gameTime() + msTimeout, timed );
        return timed;
    }

    void display( FileAccessor* accessor ) {
        if ( _type == LOAD_TYPE_REGULAR ) {
            Texture* texture = accessor -> getTexture( _textureId );
            if ( texture ) {
                double rotation = _rotation;
                if ( isnan( rotation ) ) {
                    rotation = MathUtils::random() * 360.0;
                }
                Draw::sprite( _position.x, _position.y, texture, rotation, _scale );
            }
        } else if ( _type == LOAD_TYPE_BURST ) {
            _drawBurst( _position, _rangeMin, _rangeMax );
        }
    }

    static void displayAll() {
        double msTime = Cache::gameTime();
        bool cleaned = false;
        do {
            if ( _displayTimeMap.empty() ) {
                break;
            }
            map < Uint32, list < TimedDisplay* > >::iterator cleaner = _displayTimeMap.begin();
            if ( cleaner -> first < msTime ) {
                _dispose( cleaner -> second );
                _displayTimeMap.erase( cleaner );
            } else {
                cleaned = true;
            }
        } while ( !cleaned );
        FileAccessor* accessor = Cache::getFileAccessor();
        for ( map < Uint32, list < TimedDisplay* > >::iterator i = _displayTimeMap.begin(); i != _displayTimeMap.end(); i++ ) {
            list < TimedDisplay* > displayList = i -> second;
            for ( list < TimedDisplay* >::iterator j = displayList.begin(); j != displayList.end(); j++ ) {
                ( *j ) -> display( accessor );
            }
        }
    }

    private:

    void _drawBurst( Vector2D position, double rangeMin, double rangeMax ) {
        uint32_t t = SDL_GetTicks();
        if ( _msLastRefreshTime + _msRefreshTime <= t ) {
            _msLastRefreshTime = t;
            list < Vector2D > targets; // mock
            for ( int i = 0; i < 16; i++ ) {
                PolarVector2D target; // Vector2D( Camera::X + Keys::getMouseX(), Camera::Y + Keys::getMouseY() );
                target.r = rangeMin + MathUtils::random() * ( rangeMax - rangeMin );
                target.a = MathUtils::random() * 2.0 * M_PI;
                Vector2D cartTarget = Vector2D( target );
                cartTarget.x += position.x;
                cartTarget.y += position.y;
                Vector2D meanPosition = position;
                double checkerLimit = 100.0;
                for ( double k = 0.0; k <= 1.0; k += 1.0 / checkerLimit ) {
                    Vector2D nowPosition = position.balance( cartTarget, k );
                    Uint32 field;
                    if ( Cache::getGameStateHolder() -> getMap() -> getField( nowPosition.x / Map::BLOCK_SIZE, nowPosition.y / Map::BLOCK_SIZE, field ) ) {
                        if ( ( Cache::getFileAccessor() -> getBlockInfo( field ) ).isSolid() ) {
                            break;
                        }
                    }
                    meanPosition = nowPosition;
                }
                targets.push_back( meanPosition );
            }
            Vector2D sectorDev = Vector2D( -0.15, 0.15 );
            Vector2D sectorLenDev = Vector2D( 0.95, 1.05 );
            Vector2D sectorCount = Vector2D( 6, 10 );
            Vector2D lineCount = Vector2D( 1, 3 );
            _genLines = LightStruckHelper::electroBurst( position, targets, sectorDev, sectorLenDev, sectorCount, lineCount, false );
        }

        double a = double( t - _msLastRefreshTime ) / double ( _msRefreshTime );
        if ( a < 0.0 ) {
            a = 0.0;
        }
        a = ( 1.0 - a ) * 160.0;
        Draw::lines( _genLines, 192, 255, 255, a );
    }

    static void _addToMap( Uint32 time, TimedDisplay* timed ) {
        list < TimedDisplay* > displayList;
        map < Uint32, list < TimedDisplay* > >::iterator i = _displayTimeMap.find( time );
        if ( i != _displayTimeMap.end() ) {
            displayList = i -> second;
        }
        displayList.push_back( timed );
        _displayTimeMap[ time ] = displayList;
    }

    static void _dispose( list < TimedDisplay* > & displayList ) {
        for ( list < TimedDisplay* >::iterator j = displayList.begin(); j != displayList.end(); j++ ) {
            delete ( *j );
        }
    }

    static map < Uint32, list < TimedDisplay* > > _displayTimeMap;

    int _type;
    int _textureId;
    Vector2D _position;
    double _rotation;
    double _scale;

    // for burst
    double _rangeMin;
    double _rangeMax;
    Uint32 _msLastRefreshTime;
    Uint32 _msRefreshTime;
    list < list < Vector2D > > _genLines;

};

map < Uint32, list < TimedDisplay* > > TimedDisplay::_displayTimeMap;

