#pragma once

#include "AbstractEventListener.h"

class GameController {
    private:
    GameController() {}
    virtual ~GameController() {}

    static list < AbstractEventListener* > _LISTENERS;

    static bool _HANDLE_UPPERCASE;
    static bool _HANDLE_KEYREPEAT;

    static map < int, int > _KEYMAPPINGS;
    static map < int, int > _DISALLOWED_KEYS;

    public:
    static const int RESULT_EMPTY = 0x00000000;
    static const int RESULT_EXIT = 0x00000001;
    static const int RESULT_CLICK = 0x00000002;

    static const int RESULT_RETURN = 0x00000004;

	static int controlEvents() {
	    int result = RESULT_EMPTY;
	    SDL_Event event;
	    while ( SDL_PollEvent( &event ) ) {
            switch ( event.type ) {
                case SDL_QUIT: {
                    return RESULT_EXIT;
                }
                case SDL_KEYDOWN: {
                    if ( event.key.repeat == 0 || _HANDLE_KEYREPEAT ) {
                        int key = _getUppercaseEventKey( event );
                        if ( _isAllowed( key ) ) {
                            Keys::setKey( key, Keys::STATE_PRESSED );
                            for ( list < AbstractEventListener* >::iterator i = _LISTENERS.begin(); i != _LISTENERS.end(); i++ ) {
                                result |= ( *i ) -> catchKeyPushEvent( key );
                            }
                        }
                    }
                    break;
                }
                case SDL_KEYUP: {
                    if ( event.key.repeat == 0 || _HANDLE_KEYREPEAT ) {
                        int key = _getUppercaseEventKey( event );
                        if ( _isAllowed( key ) ) {
                            Keys::setKey( key, Keys::STATE_RELEASED );
                            for ( list < AbstractEventListener* >::iterator i = _LISTENERS.begin(); i != _LISTENERS.end(); i++ ) {
                                result |= ( *i ) -> catchKeyReleaseEvent( key );
                            }
                        }
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    Keys::setMouseButton( event.button.button, Keys::STATE_PRESSED );
                    for ( list < AbstractEventListener* >::iterator i = _LISTENERS.begin(); i != _LISTENERS.end(); i++ ) {
                        result |= ( *i ) -> catchMousePushEvent( event.button.button );
                    }
                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    Keys::setMouseButton( event.button.button, Keys::STATE_RELEASED );
                    for ( list < AbstractEventListener* >::iterator i = _LISTENERS.begin(); i != _LISTENERS.end(); i++ ) {
                        result |= ( *i ) -> catchMouseReleaseEvent( event.button.button );
                    }
                    result |= RESULT_CLICK;
                    break;
                }
                case SDL_MOUSEWHEEL: {
                    for ( list < AbstractEventListener* >::iterator i = _LISTENERS.begin(); i != _LISTENERS.end(); i++ ) {
                        result |= ( *i ) -> catchMouseScrollEvent( event.wheel.y );
                    }
                    break;
                }
                case SDL_MOUSEMOTION: {
                    Keys::setMouse( event.motion.x, event.motion.y );
                    break;
                }
            }
	    }
        for ( list < AbstractEventListener* >::iterator i = _LISTENERS.begin(); i != _LISTENERS.end(); i++ ) {
            result |= ( *i ) -> onRegularTick();
	    }
	    return result;
	}

	static void addEventListener( AbstractEventListener* listener ) {
	    if ( listener ) {
            _LISTENERS.push_back( listener );
	    }
	}

	static void clearEventListenerList() {
	    _LISTENERS.clear();
    }

    static void setHandleUppercase( bool value ) {
        _HANDLE_UPPERCASE = value;
    }

    static void setHandleKeyRepeat( bool value ) {
        _HANDLE_KEYREPEAT = value;
    }

    static void initKeyMappings() {
        _setKeyMapping( '1', '!' );
        _setKeyMapping( '2', '@' );
        _setKeyMapping( '3', '#' );
        _setKeyMapping( '4', '$' );
        _setKeyMapping( '5', '%' );
        _setKeyMapping( '6', '^' );
        _setKeyMapping( '7', '&' );
        _setKeyMapping( '8', '*' );
        _setKeyMapping( '9', '(' );
        _setKeyMapping( '0', ')' );
        _setKeyMapping( '`', '~' );
        _setKeyMapping( '-', '_' );
        _setKeyMapping( '=', '+' );
        _setKeyMapping( '[', '{' );
        _setKeyMapping( ']', '}' );
        _setKeyMapping( '\\', '|' );
        _setKeyMapping( ';', ':' );
        _setKeyMapping( '\'', '"' );
        _setKeyMapping( ',', '<' );
        _setKeyMapping( '.', '>' );
        _setKeyMapping( '/', '?' );
    }

    static void setDisallowedKeys( const char* keyString ) {
        setDisallowedKeys( string( keyString ) );
    }

    static void setDisallowedKeys( string keyString ) {
        _DISALLOWED_KEYS.clear();
        for ( int i = 0; i < ( int ) keyString.length(); i++ ) {
            int id = ( int ) keyString[ i ];
            //printf( "Set disallowed key: %d [%c]\n", id, ( char ) id );
            _DISALLOWED_KEYS[ id ] = id;
        }
    }

    private:

    inline static void _setKeyMapping( int key, int upperValue ) {
        _KEYMAPPINGS[ key ] = upperValue;
    }

    inline static bool _isAllowed( int key ) {
        if ( _DISALLOWED_KEYS.empty() ) {
            return true;
        }
        key &= 0x000000FF;
        map < int, int >::iterator i = _DISALLOWED_KEYS.find( key );
        if ( i != _DISALLOWED_KEYS.end() ) {
            //printf( "Pressed disallowed key: [%c]\n", ( char ) key );
            return false;
        }
        return ( key >= 32 || key == 8 || key == 27 || key == 13 ) && ( key < 127 );
    }

    inline static int _getUppercaseEventKey( SDL_Event & event ) {
        int key = event.key.keysym.sym;
        if ( _HANDLE_UPPERCASE ) {
            if (
                ( event.key.keysym.mod & KMOD_LSHIFT ) ||
                ( event.key.keysym.mod & KMOD_RSHIFT )
            ) {
                map < int, int >::iterator i = _KEYMAPPINGS.find( key );
                if ( i != _KEYMAPPINGS.end() ) {
                    return i -> second;
                }
                if ( !( event.key.keysym.sym & KMOD_CAPS ) ) {
                    return toupper( key );
                }
            } else if ( event.key.keysym.mod & KMOD_CAPS ) {
                return toupper( key );
            }
        }
        return key;
    }

};

list < AbstractEventListener* > GameController::_LISTENERS;
bool GameController::_HANDLE_UPPERCASE = true;
bool GameController::_HANDLE_KEYREPEAT = true;
map < int, int > GameController::_KEYMAPPINGS;
map < int, int > GameController::_DISALLOWED_KEYS;

