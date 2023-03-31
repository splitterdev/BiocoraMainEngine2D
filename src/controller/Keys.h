#pragma once

class Keys {
    private:
    Keys() {}
    virtual ~Keys() {}

    static int _mouseX;
    static int _mouseY;
    static map < int, char > _keyStates;
    static map < int, char > _mouseStates;

    public:

    static const int STATE_RELEASED = 0;
    static const int STATE_PRESSED = 1;

    static const unsigned int SHIFT_LEFT = 0x400000E1;
    static const unsigned int SHIFT_RIGHT = 0x400000E5;
    static const unsigned int CTRL_LEFT = 0x400000E0;
    static const unsigned int CTRL_RIGHT = 0x400000E4;
    static const unsigned int ALT_LEFT = 0x400000E2;
    static const unsigned int ALT_RIGHT = 0x400000E6;
    static const unsigned int CAPS_LOCK = 0x40000039;
    static const unsigned int NUM_LOCK = 0x40000053;
    static const unsigned int SCROLL_LOCK = 0x40000047;

    static inline void setKey( int key, char state ) {
        _keyStates[ key ] = state;
    }

    static inline bool isPressed( int key ) {
        map < int, char >::iterator i = _keyStates.find( key );
        if ( i != _keyStates.end() ) {
            return ( i -> second ) != 0;
        }
        return false;
    }

    static inline void setMouse( int mouseX, int mouseY ) {
        _mouseX = mouseX;
        _mouseY = mouseY;
    }

    static inline int getMouseX() {
        return _mouseX;
    }

    static inline int getMouseY() {
        return _mouseY;
    }

    static inline void setMouseButton( int mouseButton, char state ) {
        _mouseStates[ mouseButton ] = state;
    }

    static inline bool isMousePressed( int key ) {
        map < int, char >::iterator i = _mouseStates.find( key );
        if ( i != _mouseStates.end() ) {
            return ( i -> second ) != 0;
        }
        return false;
    }

    static void resetInput() {
        _keyStates.clear();
        _mouseStates.clear();
    }

};

int Keys::_mouseX = 0;
int Keys::_mouseY = 0;
map < int, char > Keys::_keyStates;
map < int, char > Keys::_mouseStates;

