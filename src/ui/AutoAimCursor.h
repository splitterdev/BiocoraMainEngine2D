#pragma once

class AutoAimCursor {
    private:
    AutoAimCursor() {}
    virtual ~AutoAimCursor() {}

    static Texture* _cursorTex;
    static bool _loaded;
    static bool _isEnabled;
    static bool _lastPositionValid;
    static Vector2D _lastPosition;

    public:

    static const int TEXTURE_ID = 6;

    static void display() {
        if ( !_isEnabled ) {
            return;
        }
        if ( !_loaded ) {
            _cursorTex = Cache::getFileAccessor() -> getTexture( TEXTURE_ID );
            if ( !_cursorTex ) {
                SDL_ShowCursor( 1 );
            }
            _loaded = true;
        }
        _lastPositionValid = false;
        if ( _cursorTex ) {
            EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
            if ( player ) {
                Vector2D position = Vector2D( Camera::X + Keys::getMouseX(), Camera::Y + Keys::getMouseY() );
                EntityObject* nearest = AttackHelper::getNearestEntityToPosition( position, player, 500.0 );
                if ( nearest ) {
                    Vector2D pos = nearest -> getPosition();
                    double timeSeconds = Cache::gameTime() / 1000.0 * 1.0;
                    Draw::sprite( pos.x, pos.y, _cursorTex, timeSeconds * 360.0, sin( timeSeconds * 2.0 * M_PI ) * 0.3 + 1.25 );
                    _lastPosition = pos;
                    _lastPositionValid = true;
                }
            }
        }
    }

    static bool isEnabled() {
        return _isEnabled;
    }

    static void setEnabled( bool enabled ) {
        _isEnabled = enabled;
    }

    static bool getAutoAimTarget( Vector2D* target ) {
        if ( _isEnabled && _lastPositionValid ) {
            if ( target ) {
                ( *target ) = _lastPosition;
            }
            return true;
        }
        return false;
    }

};

Texture* AutoAimCursor::_cursorTex = NULL;
bool AutoAimCursor::_loaded = false;
bool AutoAimCursor::_isEnabled = true;
bool AutoAimCursor::_lastPositionValid = false;
Vector2D AutoAimCursor::_lastPosition;

