#pragma once

class Camera {
    private:
    Camera() {}
    ~Camera() {}
    public:

    static const double OFFSET_POWER = 0.25;

    static void set( int mouseX, int mouseY, Vector2D* boundPosition, double msCurrentTime ) {
        if ( boundPosition ) {
            _position = ( *boundPosition );
        }
        double offsetX = mouseX - ( WINDOW_WIDTH >> 1 );
        double offsetY = mouseY - ( WINDOW_HEIGHT >> 1 );
        if ( Enabled ) {
            _lastOffsetX = offsetX;
            _lastOffsetY = offsetY;
        } else {
            offsetX = _lastOffsetX;
            offsetY = _lastOffsetY;
        }
        offsetX *= OFFSET_POWER;
        offsetY *= OFFSET_POWER;
        Vector2D quakeOffset = getQuakeOffset( msCurrentTime );
        X = _position.x + offsetX - ( WINDOW_WIDTH >> 1 ) + quakeOffset.x;
        Y = _position.y + offsetY - ( WINDOW_HEIGHT >> 1 ) + quakeOffset.y;
    }

    static void setNoOffset( int mouseX, int mouseY, Vector2D* boundPosition, double msCurrentTime ) {
        if ( boundPosition ) {
            _position = ( *boundPosition );
        }
        /*double offsetX = mouseX - ( WINDOW_WIDTH >> 1 );
        double offsetY = mouseY - ( WINDOW_HEIGHT >> 1 );
        offsetX *= 0;//OFFSET_POWER;
        offsetY *= 0;//OFFSET_POWER;*/
        Vector2D quakeOffset = getQuakeOffset( msCurrentTime );
        X = _position.x - ( WINDOW_WIDTH >> 1 ) + quakeOffset.x;
        Y = _position.y - ( WINDOW_HEIGHT >> 1 ) + quakeOffset.y;
    }

    static Vector2D getQuakeOffset( double msCurrentTime ) {
        bool cleaned = false;
        do {
            if ( _quakeMap.empty() ) {
                break;
            }
            map < Uint32, list < Quake* > >::iterator cleaner = _quakeMap.begin();
            if ( cleaner -> first < msCurrentTime ) {
                _dispose( cleaner -> second );
                _quakeMap.erase( cleaner );
            } else {
                cleaned = true;
            }
        } while ( !cleaned );
        int quakeMaxPower = 0;
        for ( map < Uint32, list < Quake* > >::iterator i = _quakeMap.begin(); i != _quakeMap.end(); i++ ) {
            list < Quake* > quakeList = i -> second;
            for ( list < Quake* >::iterator j = quakeList.begin(); j != quakeList.end(); j++ ) {
                Quake* quake = ( *j );
                int quakePower = quake -> power;
                if ( quakePower > quakeMaxPower ) {
                    quakeMaxPower = quakePower;
                }
            }
        }
        PolarVector2D quakeOffset;
        quakeOffset.r = quakeMaxPower;
        quakeOffset.a = MathUtils::random() * M_PI * 2.0;
        return Vector2D( quakeOffset );
    }

    static void createQuake( int power, int duration, double msCurrentTime ) {
        Quake* quake = new Quake( power );
        _addToMap( msCurrentTime + duration, quake );
    }

    class Quake {
        public:
        Quake( int initialPower ) {
            power = initialPower;
        }
        ~Quake() {}
        int power;
    };

    static void clearQuakes() {
        for ( map < Uint32, list < Quake* > >::iterator i = _quakeMap.begin(); i != _quakeMap.end(); i++ ) {
            _dispose( i -> second );
        }
        _quakeMap.clear();
    }

    static double X;
    static double Y;
    static bool Enabled;

    private:

    static void _addToMap( Uint32 time, Quake* quake ) {
        list < Quake* > quakeList;
        map < Uint32, list < Quake* > >::iterator i = _quakeMap.find( time );
        if ( i != _quakeMap.end() ) {
            quakeList = i -> second;
        }
        quakeList.push_back( quake );
        _quakeMap[ time ] = quakeList;
    }

    static void _dispose( list < Quake* > & quakeList ) {
        for ( list < Quake* >::iterator j = quakeList.begin(); j != quakeList.end(); j++ ) {
            delete ( *j );
        }
    }

    static map < Uint32, list < Quake* > > _quakeMap;

    static Vector2D _position;
    static double _lastOffsetX;
    static double _lastOffsetY;

};

double Camera::X = 0.0;
double Camera::Y = 0.0;
bool Camera::Enabled = true;
double Camera::_lastOffsetX = 0.0;
double Camera::_lastOffsetY = 0.0;
Vector2D Camera::_position = Vector2D( 0.0, 0.0 );
map < Uint32, list < Camera::Quake* > > Camera::_quakeMap;

