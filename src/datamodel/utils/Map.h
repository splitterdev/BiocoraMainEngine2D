#pragma once

#define MAP_FIELD_TYPE Uint32

class SpawnPositionAnalyser;

class Map {
    protected:
    Map( int width, int height ) {
        _w = width;
        _h = height;
        _data = ( MAP_FIELD_TYPE* ) calloc( _w * _h, sizeof( MAP_FIELD_TYPE ) );
        _trashField = SDL_CreateTexture( Renderer::getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, _w * BLOCK_SIZE, _h * BLOCK_SIZE );
        _positionAnalyser = NULL;
        updateWidthAndHeightParams();
    }
    public:

    class Loader {
        private:
        Loader() {}
        virtual ~Loader() {}
        public:

        static Map* loadOld( string path, map < int, Texture* > textureSet ) {
            FILE* handle = fopen( path.c_str(), "r" );
            if ( handle ) {
                int w = -1;
                int h = -1;
                Map* loadedMap = NULL;
                char* buffer = ( char* ) malloc( 32 );
                if ( fgets( buffer, 31, handle ) ) {
                    sscanf( buffer, "%d", &w );
                }
                if ( fgets( buffer, 31, handle ) ) {
                    sscanf( buffer, "%d", &h );
                }
                loadedMap = Map::createMap( w, h );
                if ( loadedMap ) {
                    buffer = ( char* ) realloc( buffer, w + 16 );
                    loadedMap -> beginUpdate();
                    for ( int j = 0; j < h; j++ ) {
                        if ( fgets( buffer, w + 15, handle ) ) {
                            for ( int i = 0; i < w; i++ ) {
                                loadedMap -> setField( i, j, buffer[ i ] );
                                map < int, Texture* >::iterator texI = textureSet.find( buffer[ i ] );
                                if ( texI != textureSet.end() ) {
                                    Texture* tex = texI -> second;
                                    if ( tex ) {
                                        loadedMap -> updateDisplayField( i, j, tex -> getTexture() );
                                    }
                                } else {
                                    printf( "Error Map: can't find texture %d\n", buffer[ i ] );
                                }
                            }
                        }
                    }
                    loadedMap -> endUpdate();
                }
                free( buffer );
                fclose( handle );
                return loadedMap;
            }
            return NULL;
        }

        static Map* load( string path, map < int, Texture* > textureSet ) {
            FILE* handle = fopen( path.c_str(), "rb" );
            if ( handle ) {
                Map* loadedMap = NULL;
                char* buffer = ( char* ) malloc( 4096 );
                try {
                    int paramCount = 0;
                    map < int, double > params;
                    int width = -1;
                    int height = -1;
                    if ( fread( &paramCount, sizeof( paramCount ), 1, handle ) != 1 ) {
                        throw Exception( "Map read error at: parameter count" );
                    }
                    for ( int i = 0; i < paramCount; i++ ) {
                        int key;
                        double value;
                        if ( fread( &key, sizeof( key ), 1, handle ) != 1 ) {
                            sprintf( buffer, "Map read error at: value #%d", i );
                            throw Exception( buffer );
                        }
                        if ( fread( &value, sizeof( value ), 1, handle ) != 1 ) {
                            sprintf( buffer, "Map read error at: value #%d", i );
                            throw Exception( buffer );
                        }
                        params[ key ] = value;
                        if ( key == PARAM_MAP_WIDTH ) {
                            width = value;
                        }
                        if ( key == PARAM_MAP_HEIGHT ) {
                            height = value;
                        }
                    }
                    loadedMap = createMap( width, height );
                    if ( loadedMap ) {
                        loadedMap -> setParameters( params );
                        for ( int i = 0; i < loadedMap -> _w; i++ ) {
                            for ( int j = 0; j < loadedMap -> _h; j++ ) {
                                MAP_FIELD_TYPE field;
                                if ( fread( &field, sizeof( field ), 1, handle ) != 1 ) {
                                    sprintf( buffer, "Map read error at: field %d / %d", i, j );
                                    throw Exception( buffer );
                                }
                                loadedMap -> setField( i, j, field );
                            }
                        }
                        loadedMap -> beginUpdate();
                        for ( int i = 0; i < loadedMap -> _w; i++ ) {
                            for ( int j = 0; j < loadedMap -> _h; j++ ) {
                                MAP_FIELD_TYPE field;
                                if ( !loadedMap -> getField( i, j, field ) ) {
                                    continue;
                                }
                                map < int, Texture* >::iterator texI = textureSet.find( field );
                                if ( texI != textureSet.end() ) {
                                    Texture* tex = texI -> second;
                                    if ( tex ) {
                                        loadedMap -> updateDisplayField( i, j, tex -> getTexture() );
                                    }
                                } else {
                                    sprintf( buffer, "Error Map: can't find texture %d\n", field );
                                    throw Exception( buffer );
                                }
                            }
                        }
                        loadedMap -> endUpdate();
                        loadedMap -> initBlockHpForEachField();
                    } else {
                        sprintf( buffer, "Can't create map of dimensions: %d, %d\n", width, height );
                        throw Exception( buffer );
                    }
                } catch ( Exception e ) {
                    e.log();
                    if ( loadedMap ) {
                        delete loadedMap;
                        loadedMap = NULL;
                    }
                }
                free( buffer );
                fclose( handle );
                return loadedMap;
            }
            return NULL;
        }

    };

    bool saveMap( string path ) {
        FILE* handle = fopen( path.c_str(), "wb" );
        if ( handle ) {
            /// TODO save me
            int paramCount = _parameters.size();
            fwrite( &paramCount, sizeof( paramCount ), 1, handle );
            for ( map < int, double >::iterator i = _parameters.begin(); i != _parameters.end(); i++ ) {
                int key = i -> first;
                double value = i -> second;
                fwrite( &key, sizeof( key ), 1, handle );
                fwrite( &value, sizeof( value ), 1, handle );
            }
            for ( int i = 0; i < _w; i++ ) {
                for ( int j = 0; j < _h; j++ ) {
                    MAP_FIELD_TYPE field;
                    /*if (  != 1 ) {
                        sprintf( buffer, "Map read error at: field %d / %d", i, j );
                        throw Exception( buffer );
                    }*/
                    if ( getField( i, j, field ) ) {
                        fwrite( &field, sizeof( field ), 1, handle );
                    }
                }
            }
            fclose( handle );
            return true;
        }
        return false;
    }

    double getParameter( int key ) {
        map < int, double >::iterator i = _parameters.find( key );
        if ( i != _parameters.end() ) {
            return i -> second;
        }
        return NAN;
    }

    void setParameter( int key, double value ) {
        _parameters[ key ] = value;
        if ( key == PARAM_MAP_WIDTH ) {
            _w = value;
        } else if ( key == PARAM_MAP_HEIGHT ) {
            _h = value;
        }
    }

    void setParameters( map < int, double > params ) {
        _parameters = params;
    }

    static const int BLOCK_SIZE = 32;

    virtual ~Map() {
        if ( _data ) {
            free( _data );
        }
        if ( _trashField ) {
            SDL_DestroyTexture( _trashField );
        }
    }

    static Map* createMap( int width, int height ) {
        if ( ( width > 0 ) && ( height > 0 ) ) {
            return new Map( width, height );
        }
        return NULL;
    }

    bool setField( int x, int y, MAP_FIELD_TYPE value ) {
        MAP_FIELD_TYPE* ptr = _getFieldPtr( x, y );
        if ( ptr ) {
            ( *ptr ) = value;
            return true;
        }
        return false;
    }

    bool getField( int x, int y, MAP_FIELD_TYPE & value ) {
        MAP_FIELD_TYPE* ptr = _getFieldPtr( x, y );
        if ( ptr ) {
            value = ( *ptr );
            return true;
        }
        return false;
    }

    inline SDL_Texture* getDisplayTexture() {
        return _trashField;
    }

    void updateDisplayField( int blockX, int blockY, SDL_Texture* texture ) {
        SDL_Rect r;
        r.x = blockX * BLOCK_SIZE;
        r.y = blockY * BLOCK_SIZE;
        r.w = BLOCK_SIZE;
        r.h = BLOCK_SIZE;
        SDL_RenderCopy( Renderer::getRenderer(), texture, NULL, &r );
        _updateGradient( blockX, blockY );
    }

    inline void beginUpdate() {
        SDL_SetRenderTarget( Renderer::getRenderer(), _trashField );
    }

    inline void endUpdate() {
        SDL_SetRenderTarget( Renderer::getRenderer(), Renderer::getMainTexture() );
    }

    inline int getDumpTexWidth() {
        return _w * BLOCK_SIZE;
    }

    inline int getDumpTexHeight() {
        return _h * BLOCK_SIZE;
    }

    inline int getWidth() {
        return _w;
    }

    inline int getHeight() {
        return _h;
    }

    inline void setAnalyser( SpawnPositionAnalyser* analyser ) {
        _positionAnalyser = analyser;
    }

    inline SpawnPositionAnalyser* getAnalyser() {
        return _positionAnalyser;
    }

    inline void updateWidthAndHeightParams() {
        setParameter( PARAM_MAP_WIDTH, _w );
        setParameter( PARAM_MAP_HEIGHT, _h );
    }

    inline double getBlockHp( int blockX, int blockY ) {
        Uint64 key = ( ( ( Uint64 ) blockX & 0x0FFFFFFF ) << 32 ) + ( ( Uint64 ) blockY & 0x0FFFFFFF );
        map < Uint64, double >::iterator i = _blockHps.find( key );
        if ( i != _blockHps.end() ) {
            return i -> second;
        }
        return NAN;
    }

    void initBlockHpForEachField();

    void initBlockHp( int blockX, int blockY, BlockInfo info ) {
        if ( info.isDestructible() ) {
            setBlockHp( blockX, blockY, info.getBlockHp() );
        } else {
            setBlockHp( blockX, blockY, NAN );
        }
    }

    inline void setBlockHp( int blockX, int blockY, double value ) {
        Uint64 key = ( ( ( Uint64 ) blockX & 0x0FFFFFFF ) << 32 ) + ( ( Uint64 ) blockY & 0x0FFFFFFF );
        if ( isnan( value ) ) {
            map < Uint64, double >::iterator i = _blockHps.find( key );
            if ( i != _blockHps.end() ) {
                _blockHps.erase( i );
            }
            return;
        }
        _blockHps[ key ] = value;
    }

    static const int PARAM_MAP_WIDTH = 0;
    static const int PARAM_MAP_HEIGHT = 1;
    static const int PARAM_MAP_START_X = 2;
    static const int PARAM_MAP_START_Y = 3;
    static const int PARAM_MAP_SPAWN_ID = 4;
    static const int PARAM_MAP_SCRIPT_ID = 5;

    static void Init() {
        if ( _gradientTextureMapInitialized ) {
            return;
        }
        SDL_SetRenderDrawBlendMode( Renderer::getRenderer(), SDL_BLENDMODE_BLEND );
        _loadRotated( 0, "data/textures/systemBlend/blank.bmp", 0 );
        _loadRotated( 1, "data/textures/systemBlend/cornerLight.bmp", 0 );
        _loadRotated( 2, "data/textures/systemBlend/cornerLight.bmp", 1 );
        _loadRotated( 3, "data/textures/systemBlend/half.bmp", 0 );
        _loadRotated( 4, "data/textures/systemBlend/cornerLight.bmp", 2 );
        _loadRotated( 5, "data/textures/systemBlend/doubleCorner.bmp", 0 );
        _loadRotated( 6, "data/textures/systemBlend/half90.bmp", 1 );
        _loadRotated( 7, "data/textures/systemBlend/cornerDark.bmp", 1 );
        _loadRotated( 8, "data/textures/systemBlend/cornerLight.bmp", 3 );
        _loadRotated( 9, "data/textures/systemBlend/half90.bmp", 0 );
        _loadRotated( 10, "data/textures/systemBlend/doubleCorner.bmp", 1 );
        _loadRotated( 11, "data/textures/systemBlend/cornerDark.bmp", 0 );
        _loadRotated( 12, "data/textures/systemBlend/half.bmp", 3 );
        _loadRotated( 13, "data/textures/systemBlend/cornerDark.bmp", 3 );
        _loadRotated( 14, "data/textures/systemBlend/cornerDark.bmp", 2 );
        _loadRotated( 15, "data/textures/systemBlend/full.bmp", 0 );
        /*for ( int i = 0; i < 256; i++ ) {
            _createGradientTexture( i );
        }*/
        _gradientTextureMapInitialized = true;
    }

    static void disableGradientFields() {
        _GRADIENT_FIELDS = false;
    }

    static const Vector2D GRADIENT_DIR[ 4 ];
    static const Vector2D GRADIENT_DIR_CHECK[ 4 ][ 3 ];

    private:

    MAP_FIELD_TYPE* _getFieldPtr( int x, int y ) {
        if ( ( x >= 0 ) && ( y >= 0 ) && ( x < _w ) && ( y < _h ) && ( _data ) ) {
            return _data + y * _w + x;
        }
        return NULL;
    }

    void _updateGradient( int blockX, int blockY );

    SDL_Texture* _getGradientTextureFor( int blockX, int blockY );

    static void _loadRotated( int id, const char* path, int rotation ) {
        SDL_Surface* ld = SDL_LoadBMP( path );
        if ( ld ) {
            SDL_Texture* ldTex = SDL_CreateTextureFromSurface( Renderer::getRenderer(), ld );
            if ( ldTex ) {
                SDL_Texture* ldTexRot = SDL_CreateTexture( Renderer::getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BLOCK_SIZE, BLOCK_SIZE );
                if ( ldTexRot ) {
                    SDL_SetTextureBlendMode( ldTex, SDL_BLENDMODE_BLEND );
                    SDL_SetTextureBlendMode( ldTexRot, SDL_BLENDMODE_BLEND );
                    SDL_Texture* target = SDL_GetRenderTarget( Renderer::getRenderer() );
                    SDL_SetRenderTarget( Renderer::getRenderer(), ldTexRot );
                    //SDL_RenderClear( Renderer::getRenderer() );
                    int flip = SDL_FLIP_NONE;
                    switch ( rotation & 0x03 ) {
                        case 1: flip = SDL_FLIP_HORIZONTAL; break;
                        case 2: flip = SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL; break;
                        case 3: flip = SDL_FLIP_VERTICAL; break;
                    }
                    SDL_RenderCopyEx( Renderer::getRenderer(), ldTex, NULL, NULL, 0, NULL, ( SDL_RendererFlip ) flip );
                    //SDL_RenderPresent( Renderer::getRenderer() );
                    SDL_SetRenderTarget( Renderer::getRenderer(), target );
                    SDL_Texture* prevTexture = _getGradientTexture( id );
                    if ( prevTexture ) {
                        SDL_DestroyTexture( prevTexture );
                    }
                    _gradientTextureMap[ id ] = ldTexRot;
                }
                SDL_DestroyTexture( ldTex );
            }
            SDL_FreeSurface( ld );
        }
    }

    static void _createGradientTexture( int flagId ) {
        SDL_Surface* gradientSurface = SDL_CreateRGBSurface( SDL_SWSURFACE, BLOCK_SIZE, BLOCK_SIZE, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 );
        if ( !gradientSurface ) {
            return;
        }
        SDL_FillRect( gradientSurface, NULL, 0x00000000 );
        _fillGradientSurface( flagId, gradientSurface );
        SDL_Texture* gradientTexture = SDL_CreateTextureFromSurface( Renderer::getRenderer(), gradientSurface );
        if ( gradientTexture ) {
            SDL_Texture* prevGradientTexture = _getGradientTexture( flagId );
            if ( prevGradientTexture ) {
                SDL_DestroyTexture( prevGradientTexture );
            }
            _gradientTextureMap[ flagId ] = gradientTexture;
        }
        /// TODO REMOVE!
        //char name[ 256 ];
        //sprintf( name, "trash/surface_gen_gradient_%d.bmp", flagId );
        //SDL_SaveBMP( gradientSurface, name );
        SDL_FreeSurface( gradientSurface );
    }

    static inline void _fillGradientSurface( int flagId, SDL_Surface* surface ) {
        /*for ( int i = 0; i < 8; i++ ) {
            int fillBlack = ( flagId >> ( 7 - i ) ) & 0x01;
            if ( fillBlack ) {
                _fillGradientSurfaceDir( GRADIENT_DIR[ i ][ 0 ], GRADIENT_DIR[ i ][ 1 ], surface, 0.25 );
            }
        }*/
        /*for ( int ix = 0; ix < BLOCK_SIZE; ix++ ) {
            for ( int iy = 0; iy < BLOCK_SIZE; iy++ ) {
                Uint32 alpha = _getPixelAlpha( ix, iy, surface );
                _setPixelAlpha( ix, iy, surface, 255 - alpha );
            }
        }*/
    }

    static Uint32* _pixelPtr32( int x, int y, SDL_Surface* surface ) {
        return ( Uint32* )( ( Uint8* ) surface -> pixels + y * surface -> pitch + x * surface -> format -> BytesPerPixel );
    }

    static inline Uint32 _getPixelAlpha( int x, int y, SDL_Surface* surface ) {
        return ( ( *_pixelPtr32( x, y, surface ) ) >> 24 ) & 0x000000FF;
    }

    static inline void _setPixelAlpha( int x, int y, SDL_Surface* surface, Uint32 alpha ) {
        Uint32* ptr = _pixelPtr32( x, y, surface );
        ( *ptr ) = ( ( *ptr ) & 0x00FFFFFF ) + ( ( alpha << 24 ) & 0xFF000000 );
    }

    static inline void _fillGradientSurfaceDir( int x, int y, SDL_Surface* surface, double gradientRatio ) {
        for ( int ix = 0; ix < BLOCK_SIZE; ix++ ) {
            for ( int iy = 0; iy < BLOCK_SIZE; iy++ ) {
                double predictedAlphaX = ( double( ix ) / double( BLOCK_SIZE ) - 0.5 ) * double( x ) / gradientRatio;
                double predictedAlphaY = ( double( iy ) / double( BLOCK_SIZE ) - 0.5 ) * double( y ) / gradientRatio;
                double predictedAlpha = max( predictedAlphaX, predictedAlphaY ); //  + 0.125
                if ( predictedAlpha < - 0.5 ) {
                    predictedAlpha = - 0.5;
                } else if ( predictedAlpha > 0.5 ) {
                    predictedAlpha = 0.5;
                }
                Uint32 predictedAlphaFinal = ( Uint32 )( ( predictedAlpha + 0.5 ) * 255.0 );
                Uint32 existingAlpha = _getPixelAlpha( ix, iy, surface );
                if ( existingAlpha < predictedAlphaFinal ) {
                    _setPixelAlpha( ix, iy, surface, predictedAlphaFinal );
                }
            }
        }
    }

    static SDL_Texture* _getGradientTexture( int flagId ) {
        map < int, SDL_Texture* >::iterator i = _gradientTextureMap.find( flagId );
        if ( i != _gradientTextureMap.end() ) {
            return i -> second;
        }
        return NULL;
    }

    static void _disposeGradientTextures() {
        for ( map < int, SDL_Texture* >::iterator i = _gradientTextureMap.begin(); i != _gradientTextureMap.end(); i++ ) {
            if ( i -> second ) {
                SDL_DestroyTexture( i -> second );
            }
        }
        _gradientTextureMap.clear();
    }

    static map < int, SDL_Texture* > _gradientTextureMap;
    static bool _gradientTextureMapInitialized;
    static bool _GRADIENT_FIELDS;

    map < int, double > _parameters;
    int _w;
    int _h;
    MAP_FIELD_TYPE* _data;
    SDL_Texture* _trashField;
    SpawnPositionAnalyser* _positionAnalyser;
    map < Uint64, double > _blockHps;

};

map < int, SDL_Texture* > Map::_gradientTextureMap;
bool Map::_gradientTextureMapInitialized = false;
bool Map::_GRADIENT_FIELDS = true;
const Vector2D Map::GRADIENT_DIR[ 4 ] = { Vector2D( -1, -1 ), Vector2D( 1, -1 ), Vector2D( 1, 1 ), Vector2D( -1, 1 ) };
const Vector2D Map::GRADIENT_DIR_CHECK[ 4 ][ 3 ] = {
    { Vector2D( -1, 0 ), Vector2D( -1, -1 ), Vector2D( 0, -1 ) },
    { Vector2D( 1, 0 ), Vector2D( 1, -1 ), Vector2D( 0, -1 ) },
    { Vector2D( 1, 0 ), Vector2D( 1, 1 ), Vector2D( 0, 1 ) },
    { Vector2D( -1, 0 ), Vector2D( -1, 1 ), Vector2D( 0, 1 ) }
};

