#pragma once

const int SOUND_DEFAULT_MENU = 8;

const int LOGO_TEXTURE_ID = 15;

class MenuPerspective : public Perspective {
    public:
    MenuPerspective() : Perspective( PERSPECTIVE_MENU_ID ) {
    }
    virtual ~MenuPerspective() {
    }

    virtual void onCreate() {
        GameController::addEventListener( MouseUIController::getController() );
        GameController::setHandleUppercase( true );
        GameController::setHandleKeyRepeat( true );
        GameController::setDisallowedKeys( "=\"\'" );
        createCalibration();
    }

    virtual void onDestroy() {
        Sound* sound = Cache::getFileAccessor() -> getSoundStream( SOUND_DEFAULT_MENU );
        if ( sound ) {
            if ( sound -> isPlaying() ) {
                sound -> stop();
            }
        }
        GameController::clearEventListenerList();
    }

    virtual FormBundle* buildInterface() {
        FormBundleBuilder builder = FormBundleBuilder()
            .with( FORM_MAIN_MENU )
            .with( FORM_SELECT_CHAPTER_MENU )
            .with( FORM_PLAY_MENU )
            .with( FORM_NEW_GAME_MENU )
            .with( FORM_SELECT_LEVEL_MENU )
            .with( FORM_PROFILES_MENU )
            .with( FORM_NEW_PROFILE_MENU )
            .with( FORM_DELETE_PROFILE_MENU )
            .with( FORM_STATS_MENU )
            .with( FORM_EXIT_CONFIRMATION_MENU )
            .startFrom( FORM_MAIN_MENU );
        return builder.build();
    }

    virtual bool onTick() {
        int controllerResult = GameController::controlEvents();
        if ( controllerResult == GameController::RESULT_EXIT ) {
            return false;
        }
        Sound* sound = Cache::getFileAccessor() -> getSoundStream( SOUND_DEFAULT_MENU );
        if ( sound ) {
            if ( !sound -> isPlaying() ) {
                sound -> play();
            }
        }
        Renderer::begin();
        DisplayUnit::displayMenuBackground();
        _drawLogo( LOGO_TEXTURE_ID );
        drawInterface();
        /*vector < double > inputs;
        inputs.push_back( 1.0 );
        inputs.push_back( double( Keys::getMouseX() ) / double( WINDOW_WIDTH ) );
        inputs.push_back( double( Keys::getMouseY() ) / double( WINDOW_HEIGHT ) );
        _calibration -> calculateFor( inputs );
        vector < double > outputValues = _calibration -> getOutputs();
        for ( int i = 0; i < int( outputValues.size() ); i++ ) {
            if ( fabs( outputValues[ i ] - _outputValues[ i ] ) < 0.1 ) {
                Text::Color( 0, 200, 0 );
            } else {
                Text::Color( 200, 0, 0 );
            }
            Text::Write( Renderer::getRenderer(), 4, 4 + 30 * i, "[%d] = %lg", i, outputValues[ i ] );
        }*/
        MenuCursor::display();
        Renderer::end();
        return true;
    }

    void createCalibration() {
        vector < int > layerNodeCount;
        layerNodeCount.push_back( 3 );
        layerNodeCount.push_back( 3 );
        _calibration = WeaponCalibration::create( 3, layerNodeCount );
        vector < double > inputs;
        inputs.push_back( 1.0 );
        inputs.push_back( MathUtils::random() );
        inputs.push_back( MathUtils::random() );
        _calibration -> calculateFor( inputs );
        _outputValues = _calibration -> getOutputs();
    }

    private:

    void _drawLogo( int textureId ) {
        Texture* texture = Cache::getFileAccessor() -> getTexture( textureId );
        if ( texture ) {
            int pxOutSize = 0.15 * WINDOW_WIDTH;
            SDL_SetTextureAlphaMod( texture -> getTexture(), 96 );
            Draw::scaled( 0.75 * WINDOW_WIDTH, 0.05 * WINDOW_HEIGHT, pxOutSize, pxOutSize, texture );
            SDL_SetTextureAlphaMod( texture -> getTexture(), 255 );
        }
    }

    WeaponCalibration* _calibration;
    vector < double > _outputValues;

};

