int WINDOW_WIDTH = 1024;
int WINDOW_HEIGHT = 768;
int WINDOW_FULLSCREEN = 0;

int EXIT_GAME_SIGNAL = 0;

#include "src/Core.h"

#include "src/Utils.h"
#include "src/Drawing.h"
#include "src/Script.h"
#include "src/DataModel.h"
#include "src/Cache.h"
#include "src/Ui.h"
#include "src/GamePlay.h"
#include "src/Controller.h"

#include "src/drawing/DisplayUnit.h"
#include "src/PerspectiveCustom.h"

#include "src/MainSetupCoreLoader.h"

Map* GLOBAL_LOADED_MAP = NULL;

void testFillMap( Uint32 textureId ) {
    if ( !GLOBAL_LOADED_MAP ) {
        return;
    }
    GLOBAL_LOADED_MAP -> beginUpdate();
    for ( int x = 0; x < GLOBAL_LOADED_MAP -> getWidth(); x++ ) {
        for ( int y = 0; y < GLOBAL_LOADED_MAP -> getHeight(); y++ ) {
            if ( GLOBAL_LOADED_MAP -> setField( x, y, textureId ) ) {
                GLOBAL_LOADED_MAP -> updateDisplayField( x, y, Cache::getFileAccessor() -> getTexture( textureId ) -> getTexture() );
            }
        }
    }
    GLOBAL_LOADED_MAP -> endUpdate();
}

class TextureSelector {
    private:
    TextureSelector() {
    }
    virtual ~TextureSelector() {
    }

    public:

    static void updateByCache() {
        _texIds = Cache::getFileAccessor() -> getMapTextureIds();
    }

    static int getId() {
        if ( ( _textureId >= 0 ) && ( _textureId < int( _texIds.size() ) ) ) {
            return _texIds[ _textureId ];
        }
        return 0;
    }

    static void nextId( int dir ) {
        _textureId += dir;
        if ( _textureId < 0 ) {
            _textureId = _texIds.size() - 1;
        }
        if ( _textureId >= int( _texIds.size() ) ) {
            _textureId = 0;
        }
    }

    static void selectByPosition( Map* mapInstance, int x, int y ) {
        if ( !mapInstance ) {
            return;
        }
        Uint32 picked;
        if ( mapInstance -> getField( x, y, picked ) ) {
            for ( int i = 0; i < ( int ) _texIds.size(); i++ ) {
                if ( ( Uint32 ) _texIds[ i ] == picked ) {
                    _textureId = i;
                    return;
                }
            }
        }
    }

    private:

    static int _textureId;
    static vector < int > _texIds;

};

int TextureSelector::_textureId = 0;
vector < int > TextureSelector::_texIds;

class MapUpdater {
    private:
    MapUpdater() {}
    virtual ~MapUpdater() {}

    public:

    static void undo( Map* mapInstance ) {
        if ( ( !_recordedUpdates.empty() ) && ( mapInstance ) ) {
            list < SDL_Rect > rList = _recordedUpdates.front();
            _recordedUpdates.pop_front();
            _recordedUndos.push_front( rList );
            _applyUpdate( mapInstance, rList, true );
        }
    }

    static void redo( Map* mapInstance ) {
        if ( ( !_recordedUndos.empty() ) && ( mapInstance ) ) {
            list < SDL_Rect > rList = _recordedUndos.front();
            _recordedUndos.pop_front();
            _recordedUpdates.push_front( rList );
            _applyUpdate( mapInstance, rList, false );
        }
    }

    static void updateMapField( Map* mapInstance, int x, int y, int texId ) {
        if ( !mapInstance ) {
            return;
        }
        Texture* tex = Cache::getFileAccessor() -> getTexture( texId );
        if ( tex ) {
            Uint32 prevTexId;
            if ( mapInstance -> getField( x, y, prevTexId ) ) {
                if ( prevTexId != ( Uint32 ) texId ) {
                    mapInstance -> beginUpdate();
                    mapInstance -> setField( x, y, texId );
                    mapInstance -> updateDisplayField( x, y, tex -> getTexture() );
                    mapInstance -> endUpdate();
                    _recordUpdate( x, y, prevTexId, texId );
                }
            }
        }
    }

    static void fillUpdateMap( Map* mapInstance, int x, int y, int texId ) {
        if ( !mapInstance ) {
            return;
        }
        Texture* tex = Cache::getFileAccessor() -> getTexture( texId );
        if ( !tex ) {
            return;
        }
        list < SDL_Rect > stk;
        Uint32 vStart;
        if ( !mapInstance -> getField( x, y, vStart ) ) {
            return;
        }
        if ( vStart == ( Uint32 ) texId ) {
            return;
        }
        mapInstance -> beginUpdate();
        stk.push_back( Draw::getSDLRect( x, y, 0, 0 ) );
        while ( !stk.empty() ) {
            SDL_Rect pos = stk.front();
            stk.pop_front();
            Uint32 vNext;
            if ( mapInstance -> getField( pos.x, pos.y, vNext ) ) {
                if ( vStart == vNext ) {
                    mapInstance -> setField( pos.x, pos.y, texId );
                    mapInstance -> updateDisplayField( pos.x, pos.y, tex -> getTexture() );
                    stk.push_back( Draw::getSDLRect( pos.x - 1, pos.y, 0, 0 ) );
                    stk.push_back( Draw::getSDLRect( pos.x + 1, pos.y, 0, 0 ) );
                    stk.push_back( Draw::getSDLRect( pos.x, pos.y - 1, 0, 0 ) );
                    stk.push_back( Draw::getSDLRect( pos.x, pos.y + 1, 0, 0 ) );
                    _recordUpdate( pos.x, pos.y, vNext, texId );
                }
            }
        }
        mapInstance -> endUpdate();
    }

    static void beginNewUpdate() {
        _recordedUpdates.push_front( list < SDL_Rect > () );
        _recordedUndos.clear();
        _changed = true;
    }

    static void setChange() {
        _changed = true;
    }

    static void disableChange() {
        _changed = false;
    }

    static bool anyChange() {
        return _changed;
    }

    private:

    static void _recordUpdate( int x, int y, int texBefore, int currentTex ) {
        if ( _recordedUpdates.empty() ) {
            return;
        }
        list < SDL_Rect > rcts = _recordedUpdates.front();
        _recordedUpdates.pop_front();
        SDL_Rect r = Draw::getSDLRect( x, y, texBefore, currentTex );
        rcts.push_back( r );
        _recordedUpdates.push_front( rcts );
        _changed = true;
    }

    static void _applyUpdate( Map* mapInstance, list < SDL_Rect > & rList, bool isUndo ) {
        mapInstance -> beginUpdate();
        for ( list < SDL_Rect >::iterator i = rList.begin(); i != rList.end(); i++ ) {
            SDL_Rect update = ( *i );
            int x = update.x;
            int y = update.y;
            int texId = update.w;
            if ( !isUndo ) {
                texId = update.h;
            }
            Texture* tex = Cache::getFileAccessor() -> getTexture( texId );
            if ( tex ) {
                mapInstance -> setField( x, y, texId );
                mapInstance -> updateDisplayField( x, y, tex -> getTexture() );
            }
        }
        mapInstance -> endUpdate();
        _changed = true;
    }

    static list < list < SDL_Rect > > _recordedUpdates;
    static list < list < SDL_Rect > > _recordedUndos;
    static bool _changed;

};

list < list < SDL_Rect > > MapUpdater::_recordedUpdates;
list < list < SDL_Rect > > MapUpdater::_recordedUndos;
bool MapUpdater::_changed = false;

string mapPath = "data/map/map/mapEditCreated_1.dat";
int mapBlockWidth = 64;
int mapBlockHeight = 64;

bool saveGlobalMap( bool quit ) {
    if ( GLOBAL_LOADED_MAP ) {
        bool ret = false;
        if ( GLOBAL_LOADED_MAP -> saveMap( mapPath ) ) {
            MapUpdater::disableChange();
            if ( quit ) {
                delete GLOBAL_LOADED_MAP;
                GLOBAL_LOADED_MAP = NULL;
            }
            ret = true;
        }
        return ret;
    }
    return true;
}

vector < string > helpStrings;

void initHelpStrings() {
    helpStrings.push_back( "Press ESC to quit this screen" );
    helpStrings.push_back( "Use WSAD or arrows to move, LMB to update pointed block" );
    helpStrings.push_back( "Q/E - change texture ID" );
    helpStrings.push_back( "RMB - pick texture from pointed block" );
    helpStrings.push_back( "P - fill with selected texture" );
    helpStrings.push_back( "U/R - undo/redo" );
    helpStrings.push_back( "X - select start point" );
    helpStrings.push_back( "Z/C - change spawn ID, V - reset" );
    helpStrings.push_back( "B/M - change script ID, N - reset" );
    helpStrings.push_back( "[/] - change camera speed movement" );
    helpStrings.push_back( "F2 - save map" );
    helpStrings.push_back( "BACKSPACE - quit" );
}

bool parseArgs( int argc, char** argv ) {
    if ( argc < 2 ) {
        printf( "Error: no arguments\n" );
        return false;
    }
    mapPath = string( argv[ 1 ] );
    FILE* handle = fopen( mapPath.c_str(), "rb" );
    if ( handle ) {
        printf( "Map exists.\n" );
        fclose( handle );
    } else {
        printf( "Map does not exist, will create new one.\n" );
        if ( argc <= 4 ) {
            if ( !StringUtils::toInteger( argv[ 2 ], &mapBlockWidth ) ) {
                printf( "Error parsing argument: \"%s\" must be an integer.\n", argv[ 2 ] );
                return false;
            }
            if ( !StringUtils::toInteger( argv[ 3 ], &mapBlockHeight ) ) {
                printf( "Error parsing argument: \"%s\" must be an integer.\n", argv[ 3 ] );
                return false;
            }
            printf( "Using size: %d/%d\n", mapBlockWidth, mapBlockHeight );
        } else {
            printf( "Warning: Using default size: %d/%d\n", mapBlockWidth, mapBlockHeight );
        }
    }
    return true;
}



int main( int argc, char** argv ) {
    if ( !parseArgs( argc, argv ) ) {
        printf( "General error on startup.\n" );
        //getchar(); // Press any key to exit.
        exit( 0 );
    }
    initHelpStrings();
    MainSetupCoreLoader::loadCoreMapEdit();

    GLOBAL_LOADED_MAP = Map::Loader::load( mapPath, Cache::getFileAccessor() -> getTextureSet() );
    if ( !GLOBAL_LOADED_MAP ) {
        printf( "Map can't be loaded, creating...\n" );
        GLOBAL_LOADED_MAP = Map::createMap( mapBlockWidth, mapBlockHeight );
        testFillMap( 53 );
        if ( !GLOBAL_LOADED_MAP ) {
            printf( "ERROR: map can't be created!\n" );
        }
    } else {
        printf( "Map loaded.\n" );
    }

    Vector2D cameraPosition = Vector2D( 0.0, 0.0 );
    double CAMSPEED = 4.0;
    double CAMSPEEDMIN = 2.0;
    double CAMSPEEDMAX = 16.0;

    TextureSelector::updateByCache();
    bool exitPrompt = false;
    bool saveError = false;
    bool helpScreen = false;

    Text::setSize( Text::SIZE_MAPEDIT );

    bool done = false;
    while ( !done ) {
        if ( exitPrompt ) {
            SDL_Event event;
            while ( SDL_PollEvent( &event ) ) {
                switch ( event.type ) {
                    case SDL_QUIT: {
                        // this isn't a joke
                        done = true;
                        break;
                    }
                    case SDL_KEYDOWN: {
                        if ( event.key.keysym.sym == SDLK_BACKSPACE ) {
                            done = true;
                        }
                        if ( event.key.keysym.sym == SDLK_F2 ) {
                            if ( saveGlobalMap( true ) ) {
                                done = true;
                            }
                        }
                        if ( event.key.keysym.sym == SDLK_ESCAPE ) {
                            exitPrompt = false;
                        }
                        break;
                    }
                }
            }
            Renderer::begin();
            Text::Color( 255, 255, 255 );
            Text::Write( Renderer::getRenderer(), 4, 4, "Map has changed. Press F2 to save or ESC to cancel." );
            if ( saveError ) {
                Text::Color( 160, 0, 0 );
                Text::Write( Renderer::getRenderer(), 4, 28, "ERROR: Map cannot be saved!" );
            }
            Renderer::end();
            continue;
        }
        if ( helpScreen ) {
            SDL_Event event;
            while ( SDL_PollEvent( &event ) ) {
                switch ( event.type ) {
                    case SDL_QUIT: {
                        if ( MapUpdater::anyChange() ) {
                            exitPrompt = true;
                        } else {
                            done = true;
                        }
                        break;
                    }
                    case SDL_KEYDOWN: {
                        if ( event.key.keysym.sym == SDLK_BACKSPACE ) {
                            if ( MapUpdater::anyChange() ) {
                                exitPrompt = true;
                            } else {
                                done = true;
                            }
                        }
                        if ( event.key.keysym.sym == SDLK_ESCAPE ) {
                            helpScreen = false;
                        }
                        break;
                    }
                }
            }
            Renderer::begin();
            Text::Color( 255, 255, 255 );
            for ( int i = 0; i < int( helpStrings.size() ); i++ ) {
                Text::Write( Renderer::getRenderer(), 4, 4 + i * 24, "%s", helpStrings[ i ].c_str() );
            }
            Renderer::end();
            continue;
        }
        saveError = false;
        int mousePX = cameraPosition.x + Keys::getMouseX() - ( WINDOW_WIDTH >> 1 );
        int mousePY = cameraPosition.y + Keys::getMouseY() - ( WINDOW_HEIGHT >> 1 );
        int mousePXB = mousePX / Map::BLOCK_SIZE;
        int mousePYB = mousePY / Map::BLOCK_SIZE;
        SDL_Event event;
        while ( SDL_PollEvent( &event ) ) {
            switch ( event.type ) {
                case SDL_QUIT: {
                    if ( MapUpdater::anyChange() ) {
                        exitPrompt = true;
                    } else {
                        done = true;
                    }
                    break;
                }
                case SDL_KEYDOWN: {
                    Keys::setKey( event.key.keysym.sym, Keys::STATE_PRESSED );
                    if ( event.key.keysym.sym == SDLK_LEFTBRACKET ) {
                        CAMSPEED /= M_SQRT2;
                        if ( CAMSPEED < CAMSPEEDMIN ) {
                            CAMSPEED = CAMSPEEDMIN;
                        }
                    }
                    if ( event.key.keysym.sym == SDLK_RIGHTBRACKET ) {
                        CAMSPEED *= M_SQRT2;
                        if ( CAMSPEED > CAMSPEEDMAX ) {
                            CAMSPEED = CAMSPEEDMAX;
                        }
                    }
                    if ( event.key.keysym.sym == SDLK_x ) {
                        GLOBAL_LOADED_MAP -> setParameter( Map::PARAM_MAP_START_X, mousePX );
                        GLOBAL_LOADED_MAP -> setParameter( Map::PARAM_MAP_START_Y, mousePY );
                    }
                    if ( event.key.keysym.sym == SDLK_z ) {
                        double spawnId = GLOBAL_LOADED_MAP -> getParameter( Map::PARAM_MAP_SPAWN_ID );
                        if ( !isnan( spawnId ) ) {
                            spawnId = 0.0;
                        }
                        spawnId -= 1;
                        GLOBAL_LOADED_MAP -> setParameter( Map::PARAM_MAP_SPAWN_ID, spawnId );
                    }
                    if ( event.key.keysym.sym == SDLK_c ) {
                        double spawnId = GLOBAL_LOADED_MAP -> getParameter( Map::PARAM_MAP_SPAWN_ID );
                        if ( !isnan( spawnId ) ) {
                            spawnId = 0.0;
                        }
                        spawnId += 1;
                        GLOBAL_LOADED_MAP -> setParameter( Map::PARAM_MAP_SPAWN_ID, spawnId );
                    }
                    if ( event.key.keysym.sym == SDLK_v ) {
                        GLOBAL_LOADED_MAP -> setParameter( Map::PARAM_MAP_SPAWN_ID, 0.0 );
                    }
                    if ( event.key.keysym.sym == SDLK_b ) {
                        double scriptId = GLOBAL_LOADED_MAP -> getParameter( Map::PARAM_MAP_SCRIPT_ID );
                        if ( isnan( scriptId ) ) {
                            scriptId = 0.0;
                        }
                        scriptId -= 1;
                        GLOBAL_LOADED_MAP -> setParameter( Map::PARAM_MAP_SCRIPT_ID, scriptId );
                    }
                    if ( event.key.keysym.sym == SDLK_m ) {
                        double scriptId = GLOBAL_LOADED_MAP -> getParameter( Map::PARAM_MAP_SCRIPT_ID );
                        if ( isnan( scriptId ) ) {
                            scriptId = 0.0;
                        }
                        scriptId += 1;
                        GLOBAL_LOADED_MAP -> setParameter( Map::PARAM_MAP_SCRIPT_ID, scriptId );
                    }
                    if ( event.key.keysym.sym == SDLK_n ) {
                        GLOBAL_LOADED_MAP -> setParameter( Map::PARAM_MAP_SCRIPT_ID, 0.0 );
                    }
                    if ( event.key.keysym.sym == SDLK_q ) {
                        TextureSelector::nextId( -1 );
                    }
                    if ( event.key.keysym.sym == SDLK_e ) {
                        TextureSelector::nextId( 1 );
                    }
                    if ( event.key.keysym.sym == SDLK_p ) {
                        if ( !Keys::isMousePressed( SDL_BUTTON_LEFT ) ) {
                            MapUpdater::beginNewUpdate();
                            MapUpdater::fillUpdateMap( GLOBAL_LOADED_MAP, mousePXB, mousePYB, TextureSelector::getId() );
                        }
                    }
                    if ( event.key.keysym.sym == SDLK_u ) {
                        MapUpdater::undo( GLOBAL_LOADED_MAP );
                    }
                    if ( event.key.keysym.sym == SDLK_r ) {
                        MapUpdater::redo( GLOBAL_LOADED_MAP );
                    }
                    if ( event.key.keysym.sym == SDLK_BACKSPACE ) {
                        if ( MapUpdater::anyChange() ) {
                            exitPrompt = true;
                        } else {
                            done = true;
                        }
                    }
                    if ( event.key.keysym.sym == SDLK_F1 ) {
                        helpScreen = true;
                    }
                    if ( event.key.keysym.sym == SDLK_F2 ) {
                        saveGlobalMap( false );
                    }
                    break;
                }
                case SDL_KEYUP: {
                    Keys::setKey( event.key.keysym.sym, Keys::STATE_RELEASED );
                    break;
                }
                case SDL_MOUSEMOTION: {
                    Keys::setMouse( event.motion.x, event.motion.y );
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    Keys::setMouseButton( event.button.button, Keys::STATE_PRESSED );
                    if ( event.button.button == SDL_BUTTON_LEFT ) {
                        MapUpdater::beginNewUpdate();
                    }
                    if ( event.button.button == SDL_BUTTON_RIGHT ) {
                        TextureSelector::selectByPosition( GLOBAL_LOADED_MAP, mousePXB, mousePYB );
                    }
                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    Keys::setMouseButton( event.button.button, Keys::STATE_RELEASED );
                    break;
                }
            }
        }
        if ( Keys::isPressed( SDLK_UP ) || Keys::isPressed( SDLK_w ) ) {
            cameraPosition.y -= CAMSPEED;
        }
        if ( Keys::isPressed( SDLK_DOWN ) || Keys::isPressed( SDLK_s ) ) {
            cameraPosition.y += CAMSPEED;
        }
        if ( Keys::isPressed( SDLK_LEFT ) || Keys::isPressed( SDLK_a ) ) {
            cameraPosition.x -= CAMSPEED;
        }
        if ( Keys::isPressed( SDLK_RIGHT ) || Keys::isPressed( SDLK_d ) ) {
            cameraPosition.x += CAMSPEED;
        }
        if ( Keys::isMousePressed( SDL_BUTTON_LEFT ) ) {
            MapUpdater::updateMapField( GLOBAL_LOADED_MAP, mousePXB, mousePYB, TextureSelector::getId() );
        }
        Camera::setNoOffset( 0, 0, &cameraPosition, Cache::gameTime() );
        Renderer::begin();
        DisplayUnit::displayMap( GLOBAL_LOADED_MAP );
        int startX = GLOBAL_LOADED_MAP -> getParameter( Map::PARAM_MAP_START_X );
        int startY = GLOBAL_LOADED_MAP -> getParameter( Map::PARAM_MAP_START_Y );
        Draw::sprite( startX, startY, Cache::getFileAccessor() -> getTexture( 1 ), 0.0, 0.25 );
        Texture* blockTex = Cache::getFileAccessor() -> getTexture( TextureSelector::getId() );
        if ( blockTex ) {
            Draw::block( mousePXB * Map::BLOCK_SIZE, mousePYB * Map::BLOCK_SIZE, blockTex, Map::BLOCK_SIZE, Map::BLOCK_SIZE );
            // indicator helper
            SDL_SetRenderDrawColor( Renderer::getRenderer(), 0, 255, 0, 255 );
            SDL_Rect blockRect = Draw::getSDLRect( mousePXB * Map::BLOCK_SIZE - Camera::X, mousePYB * Map::BLOCK_SIZE - Camera::Y, Map::BLOCK_SIZE, Map::BLOCK_SIZE );
            SDL_RenderDrawRect( Renderer::getRenderer(), &blockRect );
        }
        // hud manual bg
        SDL_SetRenderDrawColor( Renderer::getRenderer(), 32, 32, 32, 128 );
        SDL_Rect rect = Draw::getSDLRect( 0, 0, WINDOW_WIDTH * 0.4, WINDOW_HEIGHT * 0.2 );
        SDL_RenderFillRect( Renderer::getRenderer(), &rect );
        Text::Color( 255, 255, 255 );
        Text::Write( Renderer::getRenderer(), 4, 4, "Position: %d / %d [block %d / %d]",
            mousePX,
            mousePY,
            mousePXB,
            mousePYB
        );
        double spawnId = GLOBAL_LOADED_MAP -> getParameter( Map::PARAM_MAP_SPAWN_ID );
        if ( isnan( spawnId ) ) {
            spawnId = 0.0;
        }
        double scriptId = GLOBAL_LOADED_MAP -> getParameter( Map::PARAM_MAP_SCRIPT_ID );
        if ( isnan( scriptId ) ) {
            scriptId = 0.0;
        }
        Text::Write( Renderer::getRenderer(), 4, 28, "Spawn ID: %d", int( spawnId ) );
        Text::Write( Renderer::getRenderer(), 4, 52, "Script ID: %d", int( scriptId ) );
        Text::Write( Renderer::getRenderer(), 4, 76, "Texture ID: %d", TextureSelector::getId() );
        Renderer::end();
    }

    int endStatus = MainSetupCoreLoader::exitGameNoSave();
    //printf( "Press any key to exit.\n" );
    //getchar();
    return endStatus;
}
