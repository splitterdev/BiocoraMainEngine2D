#pragma once

class SpawnLine {
    public:
    SpawnLine( double timeInterval ) {
        _lastSpawn = 0.0;
        _timer = timeInterval;
        _fullChance = 0.0;
    }
    virtual ~SpawnLine() {
    }

    /**
        Parses input string to SpawnLine item.
        Parser expects the following string as input:
        Timer(s);ID1,Chance1;ID2,Chance2;...;IDN,ChanceN
        During the spawn, which occurs every x seconds
    */
    static SpawnLine* parse( string str ) {
        vector < string > splits = StringUtils::split( str, ';' );
        if ( splits.size() > 0 ) {
            double timer = StringUtils::toDouble( splits[ 0 ] );
            if ( !isnan( timer ) ) {
                SpawnLine* spawnLine = new SpawnLine( timer * 1000.0 );
                map < int, double > spawnedItems;
                for ( int i = 1; i < int( splits.size() ); i++ ) {
                    int itemId;
                    double itemChance;
                    vector < string > splitsSub = StringUtils::split( splits[ i ], ',' );
                    if ( splitsSub.size() == 2 ) {
                        if ( StringUtils::toInteger( splitsSub[ 0 ], &itemId ) && StringUtils::toDouble( splitsSub[ 1 ], &itemChance ) ) {
                            spawnLine -> _fullChance += itemChance;
                            spawnedItems[ itemId ] = spawnLine -> _fullChance;
                        }
                    } else {
                        ParseException( splits[ i ] + string( " does not contain proper count of elements" ), i );
                    }
                }
                spawnLine -> _spawnedItems = spawnedItems;
                return spawnLine;
            } else {
                throw ParseException( splits[ 0 ] + string( " is not a proper number." ), -1 );
            }
        }
        return NULL;
    }

    /**
        Returns time interval in milliseconds.
    */
    inline double getTimer() {
        return _timer;
    }

    inline bool canSpawn( double currentTimeMS ) {
        return ( _lastSpawn + _timer ) <= currentTimeMS;
    }

    inline void updateTimer( double currentTimeMS ) {
        _lastSpawn = currentTimeMS;
    }

    int getSpawnedItem() {
        double r = MathUtils::random() * _fullChance;
        for ( map < int, double >::iterator i = _spawnedItems.begin(); i != _spawnedItems.end(); i++ ) {
            if ( r <= i -> second ) {
                return i -> first;
            }
        }
        return -1;
    }

    inline void reset() {
        _lastSpawn = 0.0;
    }

    private:

    double _lastSpawn;
    double _timer;
    map < int, double > _spawnedItems;
    double _fullChance;

};


