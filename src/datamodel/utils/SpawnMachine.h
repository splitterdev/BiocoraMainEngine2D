#pragma once

class SpawnMachine {
    public:
    SpawnMachine() {
    }
    virtual ~SpawnMachine() {
        for ( list < SpawnLine* >::iterator i = _lines.begin(); i != _lines.end(); i++ ) {
            if ( *i ) {
                delete ( *i );
            }
        }
    }

    static SpawnMachine* parse( string str ) {
        SpawnMachine* machine = new SpawnMachine();
        try {
            vector < string > splits = StringUtils::split( str, '|' );
            for ( vector < string >::iterator i = splits.begin(); i != splits.end(); i++ ) {
                SpawnLine* line = SpawnLine::parse( *i );
                if ( line ) {
                    machine -> addLine( line );
                }
            }
        } catch ( Exception e ) {
            delete machine;
            throw e;
        }
        return machine;
    }

    void addLine( SpawnLine* line ) {
        _lines.push_back( line );
    }

    list < int > getSpawnedIds( double currentTimeMS ) {
        list < int > spawned;
        for ( list < SpawnLine* >::iterator i = _lines.begin(); i != _lines.end(); i++ ) {
            SpawnLine* line = ( *i );
            if ( line ) {
                if ( line -> canSpawn( currentTimeMS ) ) {
                    int id = line -> getSpawnedItem();
                    if ( id >= 0 ) {
                        spawned.push_back( id );
                        line -> updateTimer( currentTimeMS );
                    }
                }
            }
        }
        return spawned;
    }

    void reset() {
        for ( list < SpawnLine* >::iterator i = _lines.begin(); i != _lines.end(); i++ ) {
            SpawnLine* line = ( *i );
            if ( line ) {
                line -> reset();
            }
        }
    }

    private:

    list < SpawnLine* > _lines;

};


