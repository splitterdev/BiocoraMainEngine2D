#pragma once

class Button : public Component {
    public:
    Button( ComponentProperties properties ) : Component( properties ) {
    }
    virtual ~Button() {
    }

    /*virtual bool onPress( double x, double y ) {
        return Component::onPress( x, y );
    }*/

    virtual bool onRelease( double x, double y ) {
        if ( Component::getPressedID() == getID() ) {
            onClick( x, y );
        }
        return Component::onRelease( x, y );
    }

    virtual void onClick( double x, double y ) = 0;

    virtual void onKeyInput( int k ) {
    }

    virtual void onDisplay() {
        displayBackground();
    }

    private:

    static int64_t _PRESSED_ID;

};

int64_t Button::_PRESSED_ID = 0;

