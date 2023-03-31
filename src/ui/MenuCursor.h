#pragma once

class MenuCursor {
    private:
    MenuCursor() {}
    virtual ~MenuCursor() {}

    static Texture* _cursorTex;
    static bool _loaded;

    public:

    static const int TEXTURE_ID = 5;
    static const double ROTATION_SPEED = 0.5;

    static void display() {
        if ( !_loaded ) {
            _cursorTex = Cache::getFileAccessor() -> getTexture( TEXTURE_ID );
            if ( !_cursorTex ) {
                SDL_ShowCursor( 1 );
            }
            _loaded = true;
        }
        if ( _cursorTex ) {
            double rotation = fmod( Cache::globalTime() * 360.0 * ROTATION_SPEED, 360.0 );
            Draw::spriteStatic( Keys::getMouseX(), Keys::getMouseY(), _cursorTex, rotation, 1.0 );
        }
    }

};

Texture* MenuCursor::_cursorTex = NULL;
bool MenuCursor::_loaded = false;
