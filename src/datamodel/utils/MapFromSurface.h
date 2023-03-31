#pragma once

class MapFromSurface {
    private:
    MapFromSurface() {
    }
    virtual ~MapFromSurface() {
    }

    inline static Uint32* _getPixelPtr( int x, int y, SDL_Surface* surface ) {
        return ( Uint32* )( ( Uint8* ) surface -> pixels + y * surface -> pitch + x * surface -> format -> BytesPerPixel );
    }

    inline static int _getWallcodeForField( int x, int y, SDL_Surface* surface ) {
        int code = 0x00;
        int offsetArray[ 4 ][ 2 ] = { { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 } };
        for ( int i = 0; i < 4; i++ ) {
            int ix = x + offsetArray[ i ][ 0 ];
            int iy = y + offsetArray[ i ][ 1 ];
            if ( ( ix < 0 ) || ( iy < 0 ) || ( ix >= surface -> w ) || ( iy >= surface -> h ) ) {
                code = ( code << 1 ) | 0x01;
                continue;
            }
            Uint32* pixelPtr = _getPixelPtr( ix, iy, surface );
            Uint32 pixel = ( *pixelPtr ) & 0x00FFFFFF;
            if ( pixel == COLOR_WALL ) {
                code = ( code << 1 ) | 0x01;
            } else {
                code = ( code << 1 );
            }
        }
        return code;
    }

    inline static int _getVisibleWallId( int code ) {
        map < int, int >::iterator i = _codeToWallIdMapping.find( code );
        if ( i != _codeToWallIdMapping.end() ) {
            return VISIBLE_WALL_IDENTIFIERS[ i -> second ];
        }
        return WALL;
    }

    static const int _DOOR_INDICATOR_VERTICAL = 0;
    static const int _DOOR_INDICATOR_HORIZONTAL = 1;

    inline static int _getVisibleDoorId( int x, int y, SDL_Surface* surface ) {
        int offsetArray[ 4 ][ 3 ] = { { 0, -1, _DOOR_INDICATOR_VERTICAL }, { 1, 0, _DOOR_INDICATOR_HORIZONTAL }, { 0, 1, _DOOR_INDICATOR_VERTICAL }, { -1, 0, _DOOR_INDICATOR_HORIZONTAL } };
        for ( int i = 0; i < 4; i++ ) {
            int ix = x + offsetArray[ i ][ 0 ];
            int iy = y + offsetArray[ i ][ 1 ];
            if ( ( ix < 0 ) || ( iy < 0 ) || ( ix >= surface -> w ) || ( iy >= surface -> h ) ) {
                continue;
            }
            Uint32* pixelPtr = _getPixelPtr( ix, iy, surface );
            Uint32 pixel = ( *pixelPtr ) & 0x00FFFFFF;
            if ( pixel == COLOR_DOOR ) {
                return offsetArray[ i ][ 2 ];
            }
        }
        return -1;
    }

    inline static int _checkTIntersection( Map* convertedMap, int x, int y, int code ) {
        switch ( code ) {
            case _T_INTER_CHECK_VERTICAL: {
                int offsetArray[ 2 ][ 2 ] = { { -1, 0 }, { 1, 0 } };
                for ( int i = 0; i < 2; i++ ) {
                    int ix = x + offsetArray[ i ][ 0 ];
                    int iy = y + offsetArray[ i ][ 1 ];
                    MAP_FIELD_TYPE field;
                    if ( convertedMap -> getField( ix, iy, field ) ) {
                        if ( ( int ) field == VISIBLE_WALL_IDENTIFIERS[ _WALL_ID_INDEX_HORIZONTAL ] || ( int ) field == VISIBLE_WALL_IDENTIFIERS[ _WALL_ID_INDEX_CROSS ] ) {
                            return true;
                        }
                    }
                }
                return false;
            }
            case _T_INTER_CHECK_HORIZONTAL: {
                for ( int i = 0; i < 2; i++ ) {
                    int offsetArray[ 2 ][ 2 ] = { { 0, -1 }, { 0, 1 } };
                    int ix = x + offsetArray[ i ][ 0 ];
                    int iy = y + offsetArray[ i ][ 1 ];
                    MAP_FIELD_TYPE field;
                    if ( convertedMap -> getField( ix, iy, field ) ) {
                        if ( ( int ) field == VISIBLE_WALL_IDENTIFIERS[ _WALL_ID_INDEX_VERTICAL ] || ( int ) field == VISIBLE_WALL_IDENTIFIERS[ _WALL_ID_INDEX_CROSS ] ) {
                            return true;
                        }
                    }
                }
                return false;
            }
            case _T_INTER_CHECK_CROSS: {
                return true;
            }
        }
        return false;
    }

    inline static int _getVisibleWallIdForIntersection( Map* convertedMap, int x, int y, int code ) {
        map < int, int >::iterator i = _codeToWallIdMapping.find( code );
        if ( i != _codeToWallIdMapping.end() ) {
            if ( _checkTIntersection( convertedMap, x, y, i -> second ) ) {
                return VISIBLE_WALL_IDENTIFIERS[ _WALL_ID_INDEX_CROSS ];
            }
        }
        return -1; // aka "don't set"
    }

    inline static int _getVisibleWallIdForDiagonal( int x, int y, SDL_Surface* surface ) {
        int floorCount = 0;
        int diagonalOffsetArray[ 4 ][ 2 ] = { { 1, -1 }, { 1, 1 }, { -1, 1 }, { -1, -1 } };
        for ( int i = 0; i < 4; i++ ) {
            int ix = x + diagonalOffsetArray[ i ][ 0 ];
            int iy = y + diagonalOffsetArray[ i ][ 1 ];
            if ( ( ix < 0 ) || ( iy < 0 ) || ( ix >= surface -> w ) || ( iy >= surface -> h ) ) {
                continue;
            }
            Uint32* pixelPtr = _getPixelPtr( ix, iy, surface );
            Uint32 pixel = ( *pixelPtr ) & 0x00FFFFFF;
            if ( pixel == COLOR_FLOOR || pixel == COLOR_SPAWN || pixel == COLOR_GLASS || pixel == COLOR_DOOR ) {
                floorCount++;
            }
        }
        if ( floorCount > 0 ) {
            return VISIBLE_WALL_IDENTIFIERS[ _WALL_ID_INDEX_CROSS ];
        }
        return WALL;
    }

    static const int _WALL_ID_INDEX_VERTICAL = 0;
    static const int _WALL_ID_INDEX_HORIZONTAL = 1;
    static const int _WALL_ID_INDEX_CROSS = 2;

    static const int _T_INTER_CHECK_VERTICAL = 0;
    static const int _T_INTER_CHECK_HORIZONTAL = 1;
    static const int _T_INTER_CHECK_CROSS = 2;

    inline static void _initCodeMap() {
        // SECOND PASS
        _codeToWallIdMapping[ 0x00 ] = _WALL_ID_INDEX_CROSS; // 0000
        _codeToWallIdMapping[ 0x01 ] = _WALL_ID_INDEX_CROSS; // 0001
        _codeToWallIdMapping[ 0x02 ] = _WALL_ID_INDEX_CROSS; // 0010
        _codeToWallIdMapping[ 0x03 ] = _WALL_ID_INDEX_CROSS; // 0011
        _codeToWallIdMapping[ 0x04 ] = _WALL_ID_INDEX_CROSS; // 0100
        _codeToWallIdMapping[ 0x05 ] = _WALL_ID_INDEX_HORIZONTAL; // 0101
        _codeToWallIdMapping[ 0x06 ] = _WALL_ID_INDEX_CROSS; // 0110
        _codeToWallIdMapping[ 0x07 ] = _WALL_ID_INDEX_HORIZONTAL; // 0111
        _codeToWallIdMapping[ 0x08 ] = _WALL_ID_INDEX_CROSS; // 1000
        _codeToWallIdMapping[ 0x09 ] = _WALL_ID_INDEX_CROSS; // 1001
        _codeToWallIdMapping[ 0x0A ] = _WALL_ID_INDEX_VERTICAL; // 1010
        _codeToWallIdMapping[ 0x0B ] = _WALL_ID_INDEX_VERTICAL; // 1011
        _codeToWallIdMapping[ 0x0C ] = _WALL_ID_INDEX_CROSS; // 1100
        _codeToWallIdMapping[ 0x0D ] = _WALL_ID_INDEX_HORIZONTAL; // 1101
        _codeToWallIdMapping[ 0x0E ] = _WALL_ID_INDEX_VERTICAL; // 1110
        // 1111 - use default wall / another processing
        // THIRD PASS
        // 0000 - not a T
        // 0001 - not a T
        // 0010 - not a T
        // 0011 - not a T
        // 0100 - not a T
        // 0101 - not a T
        // 0110 - not a T
        _codeToWallIdMappingTIntersections[ 0x07 ] = _T_INTER_CHECK_HORIZONTAL; // 0111
        // 1000 - not a T
        // 1001 - not a T
        // 1010 - not a T
        _codeToWallIdMappingTIntersections[ 0x0B ] = _T_INTER_CHECK_VERTICAL; // 1011
        // 1100 - not a T
        _codeToWallIdMappingTIntersections[ 0x0D ] = _T_INTER_CHECK_HORIZONTAL; // 1101
        _codeToWallIdMappingTIntersections[ 0x0E ] = _T_INTER_CHECK_VERTICAL; // 1110
        _codeToWallIdMappingTIntersections[ 0x0F ] = _T_INTER_CHECK_CROSS; // 1111
    }

    static map < int, int > _codeToWallIdMapping;
    static map < int, int > _codeToWallIdMappingTIntersections;
    static int WALL;
    static int FLOOR;
    static int GLASS;
    static vector < int > VISIBLE_WALL_IDENTIFIERS;
    static vector < int > VISIBLE_DOOR_IDENTIFIERS;

    public:

    static bool Init( const char* path ) {
        _initCodeMap();
        FILE* handle = fopen( path, "r" );
        if ( handle ) {
            int wallVertical;
            int wallHorizontal;
            int wallCross;
            int doorV;
            int doorH;
            bool ret = true;
            if ( fscanf( handle, "%d %d %d %d %d %d %d %d", &WALL, &FLOOR, &wallVertical, &wallHorizontal, &wallCross, &doorV, &doorH, &GLASS ) == 8 ) {
                VISIBLE_WALL_IDENTIFIERS.push_back( wallVertical );
                VISIBLE_WALL_IDENTIFIERS.push_back( wallHorizontal );
                VISIBLE_WALL_IDENTIFIERS.push_back( wallCross );
                VISIBLE_DOOR_IDENTIFIERS.push_back( doorV );
                VISIBLE_DOOR_IDENTIFIERS.push_back( doorH );
            } else {
                ret = false;
            }
            fclose( handle );
            return ret;
        }
        return false;
    }

    static const int COLOR_WALL = 0x00000000;
    static const int COLOR_FLOOR = 0x00FFFFFF;
    static const int COLOR_SPAWN = 0x0000FF00;
    static const int COLOR_DOOR = 0x00FF0000;
    static const int COLOR_GLASS = 0x000000FF;

    static Map* convert( SDL_Surface* surface ) {
        if ( !surface ) {
            return NULL;
        }
        int w = surface -> w;
        int h = surface -> h;
        Map* convertedMap = Map::createMap( w, h );
        if ( !convertedMap ) {
            return NULL;
        }
        for ( int i = 0; i < w; i++ ) {
            for ( int j = 0; j < h; j++ ) {
                Uint32* pixelPtr = _getPixelPtr( i, j, surface );
                Uint32 pixel = ( *pixelPtr ) & 0x00FFFFFF;
                if ( pixel == COLOR_WALL || pixel == COLOR_DOOR ) {
                    convertedMap -> setField( i, j, WALL );
                }
                if ( pixel == COLOR_FLOOR ) {
                    convertedMap -> setField( i, j, FLOOR );
                }
                if ( pixel == COLOR_GLASS ) {
                    convertedMap -> setField( i, j, GLASS );
                }
                if ( pixel == COLOR_SPAWN ) {
                    convertedMap -> setField( i, j, FLOOR );
                    convertedMap -> Map::setParameter( Map::PARAM_MAP_START_X, double( i * Map::BLOCK_SIZE ) + 0.5 );
                    convertedMap -> Map::setParameter( Map::PARAM_MAP_START_Y, double( j * Map::BLOCK_SIZE ) + 0.5 );
                }
            }
        }
        for ( int i = 0; i < w; i++ ) {
            for ( int j = 0; j < h; j++ ) {
                Uint32* pixelPtr = _getPixelPtr( i, j, surface );
                Uint32 pixel = ( *pixelPtr ) & 0x00FFFFFF;
                if ( pixel == COLOR_FLOOR || pixel == COLOR_SPAWN || pixel == COLOR_GLASS || pixel == COLOR_DOOR ) {
                    continue;
                }
                int wallCode = _getWallcodeForField( i, j, surface );
                int visibleWallId;
                if ( wallCode == 0x0F ) {
                    visibleWallId = _getVisibleWallIdForDiagonal( i, j, surface );
                } else {
                    visibleWallId = _getVisibleWallId( wallCode );
                }
                convertedMap -> setField( i, j, visibleWallId );
            }
        }
        for ( int i = 0; i < w; i++ ) {
            for ( int j = 0; j < h; j++ ) {
                Uint32* pixelPtr = _getPixelPtr( i, j, surface );
                Uint32 pixel = ( *pixelPtr ) & 0x00FFFFFF;
                if ( pixel == COLOR_FLOOR || pixel == COLOR_SPAWN || pixel == COLOR_GLASS || pixel == COLOR_DOOR ) {
                    continue;
                }
                int wallCode = _getWallcodeForField( i, j, surface );
                int visibleWallId = _getVisibleWallIdForIntersection( convertedMap, i, j, wallCode );
                if ( visibleWallId > 0 ) {
                    convertedMap -> setField( i, j, visibleWallId );
                }
            }
        }
        for ( int i = 0; i < w; i++ ) {
            for ( int j = 0; j < h; j++ ) {
                Uint32* pixelPtr = _getPixelPtr( i, j, surface );
                Uint32 pixel = ( *pixelPtr ) & 0x00FFFFFF;
                if ( pixel == COLOR_DOOR ) {
                    int visibleDoorId = _getVisibleDoorId( i, j, surface );
                    if ( visibleDoorId >= 0 ) {
                        convertedMap -> setField( i, j, VISIBLE_DOOR_IDENTIFIERS[ visibleDoorId ] );
                    } else {
                        int wallCode = _getWallcodeForField( i, j, surface );
                        int visibleWallId = _getVisibleWallId( wallCode );
                        if ( visibleWallId == VISIBLE_WALL_IDENTIFIERS[ _WALL_ID_INDEX_VERTICAL ] ) {
                            convertedMap -> setField( i, j, VISIBLE_DOOR_IDENTIFIERS[ _DOOR_INDICATOR_VERTICAL ] );
                        } else {
                            convertedMap -> setField( i, j, VISIBLE_DOOR_IDENTIFIERS[ _DOOR_INDICATOR_HORIZONTAL ] );
                        }
                    }
                }
            }
        }
        convertedMap -> Map::setParameter( Map::PARAM_MAP_SPAWN_ID, 0 );
        convertedMap -> Map::setParameter( Map::PARAM_MAP_SCRIPT_ID, 0 );
        return convertedMap;
    }

    static void processMap( string input, string output ) {
        SDL_Surface* surface = SDL_LoadBMP( input.c_str() );
        Map* m = MapFromSurface::convert( surface );
        SDL_FreeSurface( surface );
        if ( m ) {
            m -> saveMap( output.c_str() );
            delete m;
        }
    }

    static void processMaps( string rootDir ) {
        /// TODO process all from given directory, non-recursive
    }

};

map < int, int > MapFromSurface::_codeToWallIdMapping;
map < int, int > MapFromSurface::_codeToWallIdMappingTIntersections;
int MapFromSurface::WALL = -1;
int MapFromSurface::FLOOR = -1;
int MapFromSurface::GLASS = -1;
vector < int > MapFromSurface::VISIBLE_WALL_IDENTIFIERS;
vector < int > MapFromSurface::VISIBLE_DOOR_IDENTIFIERS;

