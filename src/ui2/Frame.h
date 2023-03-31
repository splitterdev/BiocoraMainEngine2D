#pragma once

class FrameHolder;

class Frame {
    public:
    Frame( int id, FrameHolder* holder ) {
        _holder = holder;
        _rootComponent = NULL;
    }
    virtual ~Frame() {
        if ( _rootComponent ) {
            delete _rootComponent;
        }
    }

    FrameHolder* getHolder() {
        return _holder;
    }

    void display() {
        if ( _rootComponent ) {
            _rootComponent -> display( 0.0, 0.0, WINDOW_WIDTH, WINDOW_HEIGHT );
        }
    }

    void click( int x, int y ) {
        if ( _rootComponent ) {
            _rootComponent -> click( x, y );
        }
    }

    void setRootComponent( Component* component ) {
        _rootComponent = component;
    }

    private:

    FrameHolder* _holder;
    Component* _rootComponent;

};
