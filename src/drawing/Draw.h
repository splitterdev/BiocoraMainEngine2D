#pragma once

class Draw {
    private:
    Draw() {}
    virtual ~Draw() {}
    public:

    static void plainCentered( int x, int y, int w, int h, SDL_Texture* generatedTexture ) {
        SDL_Rect r = getSDLRect( x - ( w >> 1 ), y - ( h >> 1 ), w, h );
        SDL_RenderCopy( Renderer::getRenderer(), generatedTexture, NULL, &r );
    }

    static void plain( int x, int y, Texture* texture ) {
        SDL_Rect r = getSDLRect( x, y, texture -> getWidth(), texture -> getHeight() );
        SDL_RenderCopy( Renderer::getRenderer(), texture -> getTexture(), NULL, &r );
    }

    static void progressBar( int x, int y, Texture* texture, double rotation, double scale, double progress ) {
        double w = scale * texture -> getWidth();
        double h = scale * texture -> getHeight();
        double hProgressR = h * progress;
        SDL_Rect r = getSDLRect( x - w / 2.0, y - h / 2.0 + hProgressR, w, h - hProgressR );
        double hProgressS = texture -> getHeight() * progress;
        SDL_Rect s = getSDLRect( 0, hProgressS, texture -> getWidth(), texture -> getHeight() - hProgressS );
        SDL_RenderCopyEx( Renderer::getRenderer(), texture -> getTexture(), &s, &r, rotation, NULL, SDL_FLIP_NONE );
    }

    static void scaled( int x, int y, int outW, int outH, Texture* texture ) {
        SDL_Rect r = getSDLRect( x, y, outW, outH );
        SDL_RenderCopy( Renderer::getRenderer(), texture -> getTexture(), NULL, &r );
    }

    static void sprite( int x, int y, Texture* texture, double rotation, double scale ) {
        double w = scale * texture -> getWidth();
        double h = scale * texture -> getHeight();
        SDL_Rect r = getSDLRect( x - w / 2.0 - Camera::X, y - w / 2.0 - Camera::Y, w, h );
        SDL_RenderCopyEx( Renderer::getRenderer(), texture -> getTexture(), NULL, &r, rotation, NULL, SDL_FLIP_NONE );
    }

    static void block( int x, int y, Texture* texture, int pxWidth, int pxHeight ) {
        SDL_Rect r = getSDLRect( x - Camera::X, y - Camera::Y, pxWidth, pxHeight );
        SDL_RenderCopyEx( Renderer::getRenderer(), texture -> getTexture(), NULL, &r, 0.0, NULL, SDL_FLIP_NONE );
    }

    static void spriteStatic( int x, int y, Texture* texture, double rotation, double scale ) {
        double w = scale * texture -> getWidth();
        double h = scale * texture -> getHeight();
        SDL_Rect r = getSDLRect( x - w / 2.0, y - w / 2.0, w, h );
        SDL_RenderCopyEx( Renderer::getRenderer(), texture -> getTexture(), NULL, &r, rotation, NULL, SDL_FLIP_NONE );
    }

    static void targetSprite( int x, int y, int tx, int ty, Texture* texture, double scale ) {
        int dx = - ( x - tx );
        int dy = y - ty;
        double rotation = atan2( dx, dy ) * 180.0 /  M_PI;
        double w = scale * texture -> getWidth();
        double h = scale * texture -> getHeight();
        SDL_Rect r = getSDLRect( x - w / 2.0 - Camera::X, y - w / 2.0 - Camera::Y, w, h );
        SDL_RenderCopyEx( Renderer::getRenderer(), texture -> getTexture(), NULL, &r, rotation, NULL, SDL_FLIP_NONE );
    }

    static SDL_Rect getSDLRect( int x, int y ) {
        SDL_Rect r;
        r.x = x;
        r.y = y;
        return r;
    }

    static SDL_Rect getSDLRect( int x, int y, int w, int h ) {
        SDL_Rect r;
        r.x = x;
        r.y = y;
        r.w = w;
        r.h = h;
        return r;
    }

    static void line( list < Vector2D > vectorLine, int r, int g, int b, int a ) {
        SDL_Renderer* renderer = Renderer::getRenderer();
        SDL_SetRenderDrawBlendMode( renderer, SDL_BLENDMODE_BLEND );
        SDL_SetRenderDrawColor( renderer, r, g, b, a );
        SDL_Point* points = ( SDL_Point* ) malloc( ( vectorLine.size() ) * sizeof( SDL_Point ) );
        int counter = 0;
        for ( list < Vector2D >::iterator i = vectorLine.begin(); i != vectorLine.end(); i++ ) {
            SDL_Point point;
            point.x = ( *i ).x - Camera::X;
            point.y = ( *i ).y - Camera::Y;
            points[ counter ] = point;
            counter++;
        }
        SDL_RenderDrawLines( renderer, points, vectorLine.size() );
    }

    inline static void lines( list < list < Vector2D > > vectorLines, int r, int g, int b, int a ) {
        for ( list < list < Vector2D > >::iterator i = vectorLines.begin(); i != vectorLines.end(); i++ ) {
            line( *i, r, g, b, a );
        }
    }

    static SDL_Rect getSDLTextureRectCentered( Texture* texture, SDL_Rect fitRect ) {
        int tWidth = texture -> getWidth();
        int tHeight = texture -> getHeight();
        SDL_Rect newRect = fitRect;
        double aspect = double( tHeight ) / double( tWidth );
        if ( tWidth > tHeight ) {
            double nextH = newRect.h * aspect;
            newRect.y += ( newRect.h - nextH ) / 2.0;
            newRect.h = nextH;
        } else {
            double nextW = newRect.w / aspect;
            newRect.x += ( newRect.w - nextW ) / 2.0;
            newRect.w = nextW;
        }
        return newRect;
    }

};

