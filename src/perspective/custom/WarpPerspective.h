#pragma once

class WarpPerspective : public Perspective {
    public:
    WarpPerspective() : Perspective( PERSPECTIVE_WARP_ID ) {
        _timeWarp = 0;
    }
    virtual ~WarpPerspective() {
    }

    virtual void onCreate() {
        setTimeWarp( 300 );
    }

    virtual void onDestroy() {
    }

    virtual FormBundle* buildInterface() {
        return NULL;
    }

    virtual bool onTick() {
        SDL_Event event;
        while ( SDL_PollEvent( &event ) ) {
            switch ( event.type ) {
                case SDL_QUIT: {
                    EXIT_GAME_SIGNAL = 1;
                    return true;
                }
            }
        }
        Renderer::begin();
        SDL_RenderClear( Renderer::getRenderer() );
        Text::Color( 128, 128, 128 );
        Text::CenterWrite( Renderer::getRenderer(), "Deployment in progress..." );
        Renderer::end();
        if ( SDL_GetTicks() > _timeWarp ) {
            // Change to loading
            Perspective::ChangeTo( PERSPECTIVE_LOADING_ID );
        }
        return true;
    }

    void setTimeWarp( Uint32 msTime ) {
        _timeWarp = SDL_GetTicks() + msTime;
    }

    private:

    Uint32 _timeWarp;

};



