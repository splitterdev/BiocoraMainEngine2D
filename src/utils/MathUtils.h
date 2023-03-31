#pragma once

class MathUtils {
    private:
    MathUtils() {}
    virtual ~MathUtils() {}
    public:

    static double random() {
        double r = 0.0;
        for ( int i = 0; i < 10; i++ ) {
            r = ( r + double( rand() % 10 ) ) / 10.0;
        }
        return r;
    }

    static int random( int a, int b ) {
        if ( a >= b ) {
            return a;
        }
        return a + ( rand() % ( b - a ) );
    }

    static uint32_t randomInteger() {
        uint32_t number = 0;
        for ( unsigned int i = 0; i < sizeof( uint32_t ); i++ ) {
            number = ( number << 8 ) + ( rand() & 0xFF );
        }
        return number;
    }

    static uint64_t randomLong() {
        uint64_t number = 0;
        for ( unsigned int i = 0; i < sizeof( uint64_t ); i++ ) {
            number = ( number << 8 ) + ( rand() & 0xFF );
        }
        return number;
    }

    static double randomValueFromMap( map < int, double > & m ) {
        int s = m.size();
        if ( s > 0 ) {
            s = int( double( s ) * random() );
            map < int, double >::iterator i = m.begin();
            for ( int j = 0; j < s; j++ ) {
                i++;
            }
            return i -> second;
        }
        return NAN;
    }

    static int roundToInt( double value ) {
        return ( int ) lround( value );
    }
};

