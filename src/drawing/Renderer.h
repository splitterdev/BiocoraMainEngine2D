#pragma once

class Renderer {
    private:
    Renderer() {}
    ~Renderer() {}

    static SDL_Renderer* _DEFAULT_RENDERER;
    //static SDL_Texture* _MAIN_TARGET_TEXTURE;

    public:

    static SDL_Renderer* getRenderer() {
        return _DEFAULT_RENDERER;
    }

    static void begin() {
        //SDL_SetRenderTarget( _DEFAULT_RENDERER, _MAIN_TARGET_TEXTURE );
        SDL_SetRenderDrawColor( _DEFAULT_RENDERER, 0, 0, 0, 255 );
        SDL_RenderClear( _DEFAULT_RENDERER );
    }

    static void end() {
        SDL_RenderPresent( _DEFAULT_RENDERER );
        /*SDL_SetRenderTarget( _DEFAULT_RENDERER, NULL );
        SDL_SetRenderDrawColor( _DEFAULT_RENDERER, 0, 0, 0, 255 );
        SDL_RenderClear( _DEFAULT_RENDERER );
        SDL_RenderCopy( _DEFAULT_RENDERER, _MAIN_TARGET_TEXTURE, NULL, NULL );
        SDL_RenderPresent( _DEFAULT_RENDERER );*/
    }

    static void dispose() {
        if ( _DEFAULT_RENDERER ) {
            SDL_DestroyRenderer( _DEFAULT_RENDERER );
            _DEFAULT_RENDERER = NULL;
        }
        /*if ( _MAIN_TARGET_TEXTURE ) {
            SDL_DestroyTexture( _MAIN_TARGET_TEXTURE );
            _MAIN_TARGET_TEXTURE = NULL;
        }*/
    }

    static void Init( SDL_Window* window ) {
        dispose();
        _DEFAULT_RENDERER = SDL_CreateRenderer( window, 0, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED );
        SDL_SetRenderDrawBlendMode( _DEFAULT_RENDERER, SDL_BLENDMODE_BLEND );
        //_MAIN_TARGET_TEXTURE = SDL_CreateTexture( _DEFAULT_RENDERER, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT );
        //SDL_SetTextureBlendMode( _MAIN_TARGET_TEXTURE, SDL_BLENDMODE_BLEND );
    }

    /*static void setOpacity( int opacity ) {
        SDL_SetRenderDrawColor( _DEFAULT_RENDERER, 255, 255, 255, opacity );
    }*/

    static SDL_Texture* getMainTexture() {
        return NULL;//_MAIN_TARGET_TEXTURE;
    }

};

SDL_Renderer* Renderer::_DEFAULT_RENDERER = NULL;
//SDL_Texture* Renderer::_MAIN_TARGET_TEXTURE = NULL;

