#pragma once

class MainSetupCoreLoader {
    private:
    MainSetupCoreLoader() {}
    virtual ~MainSetupCoreLoader() {}

    static SDL_Window* _GAME_WINDOW;
    static bool _INITIALIZED;

    static void _readProperties( const char* path ) {
        Properties* properties = Properties::read( path );
        if ( properties ) {
            properties -> getInteger( string( "WINDOW_WIDTH" ), &WINDOW_WIDTH );
            properties -> getInteger( string( "WINDOW_HEIGHT" ), &WINDOW_HEIGHT );
            properties -> getInteger( string( "WINDOW_FULLSCREEN" ), &WINDOW_FULLSCREEN );
            properties -> getInteger( string( "FONT_SIZE_MENU" ), &Text::SIZE_MENU );
            properties -> getInteger( string( "FONT_SIZE_GAME" ), &Text::SIZE_GAME );
            printf( "Properties read.\n" );
            delete properties;
        }
    }

    static void _readSettings( const char* path ) {
        Properties* properties = Properties::read( path );
        if ( properties ) {
            int characterId = 0;
            if ( properties -> getInteger( string( "PROFILE_ID" ), &characterId ) ) {
                printf( "Profile ID set.\n" );
                PlayerManager::setProfileId( characterId );
            }
            printf( "Settings read.\n" );
            delete properties;
        }
    }

    static void _saveSettings( const char* path ) {
        Properties* properties = new Properties();
        properties -> set( string( "PROFILE_ID" ), StringUtils::toString( PlayerManager::getProfileId() ) );
        if ( properties -> write( path ) ) {
            printf( "Settings saved.\n" );
        } else {
            printf( "ERROR: settings were not saved.\n" );
        }
        delete properties;
    }

    public:

    static void loadCore() {
        if ( _INITIALIZED ) {
            return;
        }
        GameController::initKeyMappings();
        EntityStatCalculator::Init();
        _readProperties( "data/GameProperties.dat" );
        _readSettings( "data/Settings.dat" );
        srand( time( NULL ) );
        SDL_Init( SDL_INIT_VIDEO );
        SDL_ShowCursor( SDL_DISABLE );
        Text::Init( "data/font/Font.ttf" );
        FormCache::Initialize();
        bool fullscreen = ( WINDOW_FULLSCREEN != 0 );
        _GAME_WINDOW = SDL_CreateWindow( "Biocora Primal", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, ( fullscreen ? SDL_WINDOW_FULLSCREEN : 0 ) );
        Sound::Init();
        Renderer::Init( _GAME_WINDOW );
        Cache::setFileAccessor( new FileAccessor() );
        LogicProcessorInitializer::InitProcessor();
        Cache::getFileAccessor() -> load();
        LangStrings::Init( FileAccessor::loadPaths( "data/lang/baseLang.dat" ) );
        FlagmanController::Init();
        MouseUIController::Init();
        AutoTalkerConfig::init( "data/gamedata/AutoTalkerConfig.dat" );
        DialogReader::readDialogs( "data/gamedata/DialogConfig.dat" );
        ScriptEngine::Init();
        Map::Init();
        _INITIALIZED = true;
    }

    static void loadCoreMapEdit() {
        if ( _INITIALIZED ) {
            return;
        }
        srand( time( NULL ) );
        SDL_Init( SDL_INIT_VIDEO );
        Text::Init( "data/font/Font.ttf" );
        bool fullscreen = false;
        _GAME_WINDOW = SDL_CreateWindow( "Biocora Primal", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, ( fullscreen ? SDL_WINDOW_FULLSCREEN : 0 ) );
        Renderer::Init( _GAME_WINDOW );
        Cache::setFileAccessor( new FileAccessor() );
        Cache::getFileAccessor() -> loadTextures( "data/textures/Textures.txt", "data/textures/MapTextures.txt" );
        Map::Init();
        Map::disableGradientFields();
        _INITIALIZED = true;
    }

    static SDL_Window* getWindow() {
        return _GAME_WINDOW;
    }

    static int exitGame() {
        if ( !_INITIALIZED ) {
            return -1;
        }
        _saveSettings( "data/Settings.dat" );
        delete Cache::getFileAccessor();
        FlagmanController::dispose();
        MouseUIController::dispose();
        Renderer::dispose();
        SDL_DestroyWindow( _GAME_WINDOW );
        return 0;
    }

    static int exitGameNoSave() {
        if ( !_INITIALIZED ) {
            return -1;
        }
        //delete Cache::getFileAccessor();
        FlagmanController::dispose();
        MouseUIController::dispose();
        Renderer::dispose();
        SDL_DestroyWindow( _GAME_WINDOW );
        return 0;
    }
};

SDL_Window* MainSetupCoreLoader::_GAME_WINDOW = NULL;
bool MainSetupCoreLoader::_INITIALIZED = false;

