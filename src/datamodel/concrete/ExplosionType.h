#pragma once

class ExplosionType : public InteractiveType {
    public:
    ExplosionType( int id ) : InteractiveType( id ) {
    }
    virtual ~ExplosionType() {
    }

    static const int ATTRIBUTE_TEXTURE_ID = 0;
    static const int ATTRIBUTE_RANGE_MIN = 1;
    static const int ATTRIBUTE_RANGE_MAX = 2;
    static const int ATTRIBUTE_DAMAGE_MIN = 3;
    static const int ATTRIBUTE_DAMAGE_MAX = 4;
    static const int ATTRIBUTE_DEATH_PUSH = 5;
    static const int ATTRIBUTE_DURATION = 6;
    static const int ATTRIBUTE_SOUND_ID = 7;
    static const int ATTRIBUTE_QUAKE_POWER = 8;
    static const int ATTRIBUTE_LOAD_TYPE = 9;

    inline int getTextureId() {
        map < int, double > values;
        if ( getMultiValuedParameter( ATTRIBUTE_TEXTURE_ID, values ) ) {
            return _randomFromMap( values );
        }
        return -1.0;
    }

    inline double getTextureId( int id ) {
        map < int, double > values;
        if ( getMultiValuedParameter( ATTRIBUTE_TEXTURE_ID, values ) ) {
            map < int, double >::iterator i = values.find( id );
            if ( i != values.end() ) {
                return i -> second;
            }
        }
        return -1.0;
    }

    inline double getRange() {
        double mN = getParameter( ATTRIBUTE_RANGE_MIN );
        double mX = getParameter( ATTRIBUTE_RANGE_MAX );
        return ( mN + MathUtils::random() * ( mX - mN ) ) * double( Map::BLOCK_SIZE );
    }

    inline double getDamage() {
        double mN = getParameter( ATTRIBUTE_DAMAGE_MIN );
        double mX = getParameter( ATTRIBUTE_DAMAGE_MAX );
        return mN + MathUtils::random() * ( mX - mN );
    }

    inline double getDeathPush() {
        return getParameter( ATTRIBUTE_DEATH_PUSH ) * double( Map::BLOCK_SIZE );
    }

    inline double getDuration() {
        return getParameter( ATTRIBUTE_DURATION ) * 1000.0;
    }

    inline double getQuakePower() {
        return getParameter( ATTRIBUTE_QUAKE_POWER );
    }

    inline int getSoundId() {
        map < int, double > values;
        if ( getMultiValuedParameter( ATTRIBUTE_SOUND_ID, values ) ) {
            return _randomFromMap( values );
        }
        return -1;
    }

    virtual InteractiveObject* createObjectInstance( int assignedId ) {
        return NULL;
    }

    inline double getLoadType() {
        return getParameter( ATTRIBUTE_LOAD_TYPE );
    }

    private:

    inline static double _randomFromMap( map < int, double > & m ) {
        map < int, double >::iterator i = m.begin();
        if ( i == m.end() ) {
            return NAN;
        }
        int r = rand() % m.size();
        for ( int j = 0; j < r; j++ ) {
            i++;
        }
        return i -> second;
    }

};

