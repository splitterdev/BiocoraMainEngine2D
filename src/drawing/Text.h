#pragma once

#include <SDL2/SDL_ttf.h>

class Text {
    private:
    Text() {
    }
    ~Text() {
    }

    static inline SDL_Surface* ___renderText( char* buffer, SDL_Color & textColor ) {
        map < int, TTF_Font* >::iterator i = _font.find( _size );
        if ( i != _font.end() ) {
            TTF_Font* font = i -> second;
            if ( font ) {
                //return TTF_RenderUTF8_Solid( font, buffer, textColor );
                return TTF_RenderUTF8_Blended( font, buffer, textColor );
            }
        }
        return SDL_CreateRGBSurface( 0, 1, 1, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 );
    }

    static void _loadFont( const char* fontPath, int size ) {
        TTF_Font* font = TTF_OpenFont( fontPath, size );
        if ( font ) {
            map < int, TTF_Font* >::iterator i = _font.find( size );
            if ( i != _font.end() ) {
                if ( i -> second ) {
                    TTF_CloseFont( i -> second );
                }
            }
            _font[ size ] = font;
        }
    }

    public:

    static int SIZE_MAPEDIT;
    static int SIZE_MENU;
    static int SIZE_GAME;

    static void Init( const char* fontPath ) {
        TTF_Init();
        /*if ( _font ) {
            TTF_CloseFont( _font );
        }*/
        _loadFont( fontPath, SIZE_MENU );
        _loadFont( fontPath, SIZE_GAME );
        _loadFont( fontPath, SIZE_MAPEDIT );
        _size = SIZE_GAME;
        Color( 255, 255, 255 );
    }

    static void Write( SDL_Surface* surface, int x, int y, const char* fmt, ... ) {
        char buffer[ 4096 ];
        va_list args;
        va_start( args, fmt );
        vsnprintf( buffer, 4095, fmt, args );
        va_end( args );
        SDL_Surface* tx;
        tx = ___renderText( buffer, _textColor );
        SDL_Rect textPos;
        textPos.x = x;
        textPos.y = y;
        SDL_BlitSurface( tx, 0, surface, &textPos );
        SDL_FreeSurface( tx );
    }

    static SDL_Surface* RenderOnly( const char* fmt, ... ) {
        char buffer[ 4096 ];
        va_list args;
        va_start( args, fmt );
        vsnprintf( buffer, 4095, fmt, args );
        va_end( args );
        //printf( "Rendering [%s]\n", buffer );
        /*SDL_Color color;
        color.r = 1 - _textColor.r;
        color.b = 1 - _textColor.g;
        color.g = 1 - _textColor.b;
        color.a = 0;*/
        SDL_Surface* tx = ___renderText( buffer, _textColor );
        //printf( "BPP = %d\n", tx -> format -> BytesPerPixel );
        //SDL_Surface* conv = SDL_ConvertSurfaceFormat( tx, SDL_PIXELFORMAT_RGBA32, 0 );
        //printf( "BPP_Converted = %d\n", conv -> format -> BytesPerPixel );
        //SDL_FreeSurface( tx );
        /*SDL_SetColorKey( tx, SDL_TRUE, SDL_MapRGB( tx -> format, 0, 0, 0 ) );
        SDL_Surface* back = SDL_CreateRGBSurface( 0, tx -> w, tx -> h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 );
        SDL_FillRect( back, NULL, SDL_MapRGBA( back -> format, 0, 0, 0, 0 ) );
        SDL_BlitSurface( tx, NULL, back, NULL );
        SDL_FreeSurface( tx );*/
        /*int wh = conv -> w * tx -> h;
        Uint32* pixels = ( Uint32* ) conv -> pixels;
        //Uint32 color = ( _
        for ( int i = 0; i < wh; i++ ) {
            if ( ( pixels[ i ] & 0x00FFFFFF ) == 0 ) {
                //printf( "set\n" );
                pixels[ i ] = 0x00000000; // xBGR
            }
        }*/
        return tx;//conv;
    }

    static SDL_Surface* MultilineRenderOnly( list < string > lines ) {
        list < SDL_Surface* > surfaces;
        int w = -1;
        int h = 0;
        for ( list < string >::iterator i = lines.begin(); i != lines.end(); i++ ) {
            SDL_Surface* surface = RenderOnly( i -> c_str() );
            if ( surface -> w > w ) {
                w = surface -> w;
            }
            h += surface -> h;
            surfaces.push_back( surface );
        }
        if ( ( w > 0 ) && ( h > 0 ) ) {
            SDL_Surface* text = SDL_CreateRGBSurface( 0, w, h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 );
            SDL_Rect rect;
            rect.x = 0;
            rect.y = 0;
            for ( list < SDL_Surface* >::iterator i = surfaces.begin(); i != surfaces.end(); i++ ) {
                SDL_Surface* iSurface = ( *i );
                SDL_BlitSurface( iSurface, NULL, text, &rect );
                rect.y += iSurface -> h;
                SDL_FreeSurface( iSurface );
            }
            return text;
        }
        return NULL;
    }

    static void Write( SDL_Renderer* renderer, int x, int y, const char* fmt, ... ) {
        char buffer[ 4096 ];
        va_list args;
        va_start( args, fmt );
        vsnprintf( buffer, 4095, fmt, args );
        va_end( args );
        SDL_Surface* tx;
        tx = ___renderText( buffer, _textColor );
        SDL_Rect textPos;
        textPos.x = x;
        textPos.y = y;
        textPos.w = tx -> w;
        textPos.h = tx -> h;
        SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, tx );
        SDL_FreeSurface( tx );
        SDL_RenderCopy( renderer, texture, 0, &textPos );
        SDL_DestroyTexture( texture );
    }

    static void Write( SDL_Renderer* renderer, SDL_Rect textPos, int opacity, const char* fmt, ... ) {
        char buffer[ 4096 ];
        va_list args;
        va_start( args, fmt );
        vsnprintf( buffer, 4095, fmt, args );
        va_end( args );
        SDL_Surface* tx;
        tx = ___renderText( buffer, _textColor );
        SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, tx );
        SDL_FreeSurface( tx );
        SDL_SetTextureAlphaMod( texture, opacity );
        SDL_RenderCopy( renderer, texture, 0, &textPos );
        SDL_DestroyTexture( texture );
    }

    static void WriteCenterAt( SDL_Renderer* renderer, int x, int y, unsigned int bgColor, const char* fmt, ... ) {
        char buffer[ 4096 ];
        va_list args;
        va_start( args, fmt );
        vsnprintf( buffer, 4095, fmt, args );
        va_end( args );
        SDL_Surface* tx;
        tx = ___renderText( buffer, _textColor );
        SDL_Rect textPos;
        textPos.w = tx -> w;
        textPos.h = tx -> h;
        textPos.x = x - ( textPos.w >> 1 );
        textPos.y = y - ( textPos.h >> 1 );
        SDL_Rect bgPos;
        bgPos.x = textPos.x - 8;
        bgPos.y = textPos.y - 8;
        bgPos.w = tx -> w + 16;
        bgPos.h = tx -> h + 16;
        SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, tx );
        SDL_FreeSurface( tx );
        if ( bgColor > 0 ) {
            Uint8 oldColor[ 4 ];
            SDL_GetRenderDrawColor( renderer, oldColor, oldColor + 1, oldColor + 2, oldColor + 3 );
            SDL_SetRenderDrawColor( renderer, ( bgColor >> 16 ) & 0xFF, ( bgColor >> 8 ) & 0xFF, bgColor & 0xFF, ( bgColor >> 24 ) & 0xFF );
            SDL_RenderFillRect( renderer, &bgPos );
            SDL_SetRenderDrawColor( renderer, oldColor[ 0 ], oldColor[ 1 ], oldColor[ 2 ], oldColor[ 3 ] );
        }
        SDL_RenderCopy( renderer, texture, 0, &textPos );
        SDL_DestroyTexture( texture );
    }

    static void WriteRight( SDL_Renderer* renderer, int x, int y, const char* fmt, ... ) {
        char buffer[ 4096 ];
        va_list args;
        va_start( args, fmt );
        vsnprintf( buffer, 4095, fmt, args );
        va_end( args );
        SDL_Surface* tx;
        tx = ___renderText( buffer, _textColor );
        SDL_Rect textPos;
        textPos.x = x - tx -> w;
        textPos.y = y;
        textPos.w = tx -> w;
        textPos.h = tx -> h;
        SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, tx );
        SDL_FreeSurface( tx );
        SDL_RenderCopy( renderer, texture, 0, &textPos );
        SDL_DestroyTexture( texture );
    }

    static void WriteWithBackground( SDL_Renderer* renderer, int x, int y, double scale, double horizontalScale, const char* fmt, ... ) {
        char buffer[ 4096 ];
        va_list args;
        va_start( args, fmt );
        vsnprintf( buffer, 4095, fmt, args );
        va_end( args );
        SDL_Surface* tx;
        tx = ___renderText( buffer, _textColor );
        SDL_Rect textPos;
        textPos.x = x + _margin;
        textPos.y = y + _margin;
        textPos.w = tx -> w * scale * horizontalScale - ( _margin << 1 );
        textPos.h = tx -> h * scale - ( _margin << 1 );
        SDL_Rect bgPos;
        bgPos.x = x;
        bgPos.y = y;
        bgPos.w = tx -> w * scale * horizontalScale;
        bgPos.h = tx -> h * scale;
        SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, tx );
        SDL_FreeSurface( tx );
        Uint8 oldColor[ 4 ];
        SDL_GetRenderDrawColor( renderer, oldColor, oldColor + 1, oldColor + 2, oldColor + 3 );
        SDL_SetRenderDrawColor( renderer, ( _bgColor >> 16 ) & 0xFF, ( _bgColor >> 8 ) & 0xFF, _bgColor & 0xFF, ( _bgColor >> 24 ) & 0xFF );
        SDL_RenderFillRect( renderer, &bgPos );
        SDL_SetRenderDrawColor( renderer, oldColor[ 0 ], oldColor[ 1 ], oldColor[ 2 ], oldColor[ 3 ] );
        SDL_RenderCopy( renderer, texture, 0, &textPos );
        SDL_DestroyTexture( texture );
    }

    static void CenterWrite( SDL_Surface* surface, const char* fmt, ... ) {
        char buffer[ 4096 ];
        va_list args;
        va_start( args, fmt );
        vsnprintf( buffer, 4095, fmt, args );
        va_end( args );
        SDL_Surface* tx;
        tx = ___renderText( buffer, _textColor );
        SDL_Rect textPos;
        textPos.x = ( surface -> w >> 1 ) - ( tx -> w >> 1 );
        textPos.y = ( surface -> h >> 1 ) - ( tx -> h >> 1 );
        SDL_BlitSurface( tx, 0, surface, &textPos );
        SDL_FreeSurface( tx );
    }

    static void CenterWrite( SDL_Renderer* renderer, const char* fmt, ... ) {
        char buffer[ 4096 ];
        va_list args;
        va_start( args, fmt );
        vsnprintf( buffer, 4095, fmt, args );
        va_end( args );
        SDL_Surface* tx;
        tx = ___renderText( buffer, _textColor );
        SDL_Rect textPos;
        textPos.x = ( WINDOW_WIDTH >> 1 ) - ( tx -> w >> 1 );
        textPos.y = ( WINDOW_HEIGHT >> 1 ) - ( tx -> h >> 1 );
        textPos.w = tx -> w;
        textPos.h = tx -> h;
        SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, tx );
        if ( texture ) {
            SDL_RenderCopy( renderer, texture, NULL, &textPos );
            SDL_DestroyTexture( texture );
        }
        SDL_FreeSurface( tx );
    }

    static void Color( int r, int g, int b ) {
        _textColor.r = r;
        _textColor.g = g;
        _textColor.b = b;
        _textColor.a = 255;
    }

    static void Color( int r, int g, int b, int a ) {
        _textColor.r = r;
        _textColor.g = g;
        _textColor.b = b;
        _textColor.a = a;
    }

    static void Color( Uint32 color ) {
        _textColor.r = ( color >> 16 ) & 0x000000FF;
        _textColor.g = ( color >> 8 ) & 0x000000FF;
        _textColor.b = ( color ) & 0x000000FF;
        _textColor.a = ( color >> 24 ) & 0x000000FF;
    }

    static void setSize( int size ) {
        _size = size;
    }

    static int getSize() {
        return _size;
    }

    private:

    static map < int, TTF_Font* > _font;
    static SDL_Color _textColor;
    static Uint32 _bgColor;
    static int _margin;

    static int _size;

};

map < int, TTF_Font* > Text::_font;
SDL_Color Text::_textColor;
Uint32 Text::_bgColor = 0x80404040;
int Text::_margin = 1;

int Text::SIZE_MAPEDIT = 20;
int Text::SIZE_MENU = 40;
int Text::SIZE_GAME = 150;

int Text::_size;

