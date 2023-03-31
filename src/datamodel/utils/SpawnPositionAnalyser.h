#pragma once

class SpawnPositionAnalyser {
    public:
    SpawnPositionAnalyser( Map* linkedMap ) {
        _linkedMap = linkedMap;
        _calculatedForFields = NULL;
        _accessor = NULL;
    }
    virtual ~SpawnPositionAnalyser() {
        if ( _calculatedForFields ) {
            for ( int i = 0; i < _linkedMap -> getWidth(); i++ ) {
                for ( int j = 0; j < _linkedMap -> getHeight(); j++ ) {
                    vector < Vector2D > * pos = _calculatedForFields[
                        _getIndex( i, j )
                    ];
                    if ( pos ) {
                        delete pos;
                    }
                }
            }
            free( _calculatedForFields );
        }
    }

    static SpawnPositionAnalyser* create( Map* fromMap, double rangeMin, double rangeMax, FileAccessor* accessor ) {
        if ( fromMap ) {
            SpawnPositionAnalyser* analyser = new SpawnPositionAnalyser( fromMap );
            fromMap -> setAnalyser( analyser );
            analyser -> _accessor = accessor;
            return analyser;
        }
        return NULL;
    }

    inline Map* getMap() {
        return _linkedMap;
    }

    bool getNextSpawnPositionFor( Vector2D playerPosition, Vector2D & spawnPosition ) {
        Vector2D blockPosition = playerPosition;
        blockPosition.x /= double( Map::BLOCK_SIZE );
        blockPosition.y /= double( Map::BLOCK_SIZE );
        if ( validate( blockPosition ) ) {
            double wWh = WINDOW_WIDTH >> 1;
            double wHh = WINDOW_HEIGHT >> 1;
            double windowSq = sqrt( wWh * wWh + wHh * wHh );
            double radius = ceil( windowSq * ( 1.0 + Camera::OFFSET_POWER ) / Map::BLOCK_SIZE ) + 1.0;
            vector < Vector2D > availablePositions = _getAvailablePositionVector( blockPosition.x, blockPosition.y, radius );
            if ( !availablePositions.empty() ) {
                vector < Vector2D >::iterator i = availablePositions.begin();
                int vSize = availablePositions.size();
                if ( vSize > 0 ) {
                    i += ( rand() % vSize );
                    spawnPosition = ( *i );
                    spawnPosition.x = ( spawnPosition.x * Map::BLOCK_SIZE ) + rand() % Map::BLOCK_SIZE;
                    spawnPosition.y = ( spawnPosition.y * Map::BLOCK_SIZE ) + rand() % Map::BLOCK_SIZE;
                    return true;
                }
            }
        }
        return false;
    }

    void displayDebug( int x, int y, double radius ) {
        vector < Vector2D > pos = _getAvailablePositionVector( x, y, radius );
        SDL_SetRenderDrawColor( Renderer::getRenderer(), 0, 255, 0, 128 );
        for ( vector < Vector2D >::iterator i = pos.begin(); i != pos.end(); i++ ) {
            Vector2D vec = ( *i );
            SDL_Rect r;
            r.x = vec.x * Map::BLOCK_SIZE - Camera::X;
            r.y = vec.y * Map::BLOCK_SIZE - Camera::Y;
            r.w = Map::BLOCK_SIZE;
            r.h = Map::BLOCK_SIZE;
            SDL_RenderFillRect( Renderer::getRenderer(), &r );
        }
    }

    private:

    inline int _getIndex( int x, int y ) {
        return y * _linkedMap -> getWidth() + x;
    }

    inline bool validate( Vector2D p ) {
        return ( ( p.x >= 0 ) && ( p.y >= 0 ) && ( p.x < _linkedMap -> getWidth() ) && ( p.y < _linkedMap -> getHeight() ) );
    }

    inline list < Vector2D > _analyseField( int x, int y, double rMin, double rMax, FileAccessor* accessor ) {
        list < Vector2D > posList;
        int xMin = x - rMax;
        int yMin = y - rMax;
        int xMax = x + rMax;
        int yMax = y + rMax;
        Vector2D center = Vector2D( x, y );
        for ( int i = xMin; i <= xMax; i++ ) {
            for ( int j = yMin; j <= yMax; j++ ) {
                MAP_FIELD_TYPE field;
                if ( _linkedMap -> getField( i, j, field ) ) {
                    if ( !( accessor -> getBlockInfo( field ) ).isSolid() ) {
                        Vector2D pos = Vector2D( i, j );
                        double dist = pos.dist2D( center );
                        if ( ( dist >= rMin ) && ( dist <= rMax ) ) {
                            posList.push_back( pos );
                        }
                    }
                }
            }
        }
        return posList;
    }

    inline vector < Vector2D > _getAvailablePositionVector( int x, int y, double radius ) {
        if ( !_accessor ) {
            return vector < Vector2D > ();
        }
        vector < Vector2D > positions;
        PolarVector2D polarSeeker;
        polarSeeker.r = radius;
        static const double fullCircle = 2.0 * M_PI;
        double circleLength = fullCircle * radius;
        double circleStep = ( 1.0 / circleLength ) * fullCircle;
        for ( double i = 0.0; i <= fullCircle; i += circleStep ) {
            polarSeeker.a = i;
            Vector2D pos = Vector2D( polarSeeker );
            pos.x = floor( pos.x + x );
            pos.y = floor( pos.y + y );
            MAP_FIELD_TYPE field;
            if ( _linkedMap -> getField( pos.x, pos.y, field ) ) {
                if ( !( _accessor -> getBlockInfo( field ) ).isSolid() ) {
                    positions.push_back( pos );
                }
            }
        }
        return positions;
    }

    Map* _linkedMap;
    vector < Vector2D > ** _calculatedForFields;

    FileAccessor* _accessor;

};

