#pragma once

class Texture {
    public:
    Texture() {
        setTexture( NULL );
        setWidth( -1 );
        setHeight( -1 );
    }
    Texture( SDL_Texture* texture, int width, int height ) {
        setTexture( texture );
        setWidth( width );
        setHeight( height );
    }
    Texture( SDL_Surface* surface ) {
        int width = -1;
        int height = -1;
        SDL_Texture* texture = SDL_CreateTextureFromSurface( Renderer::getRenderer(), surface );
        if ( surface ) {
            width = surface -> w;
            height = surface -> h;
        }
        setTexture( texture );
        setWidth( width );
        setHeight( height );
    }
    virtual ~Texture() {
        if ( _texture ) {
            SDL_DestroyTexture( _texture );
        }
    }

    inline SDL_Texture* getTexture() {
        return _texture;
    }

    inline int getWidth() {
        return _w;
    }

    inline int getHeight() {
        return _h;
    }

    inline void setTexture( SDL_Texture* texture ) {
        _texture = texture;
        if ( _texture ) {
            SDL_SetTextureBlendMode( _texture, SDL_BLENDMODE_BLEND );
        }
    }

    inline void setWidth( int width ) {
        _w = width;
    }

    inline void setHeight( int height ) {
        _h = height;
    }

    private:
    SDL_Texture* _texture;
    int _w;
    int _h;
};


