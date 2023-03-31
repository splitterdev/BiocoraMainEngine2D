#pragma once

#include <sys/time.h>

class AbstractSelector : public ComplexComponent {
    private:

    class _ButtonChange : public Button {
        public:
        _ButtonChange( ComponentProperties properties, AbstractSelector* selector ) : Button( properties ) {
            _selector = selector;
            _lastPressed = -1;
            _lastPressedCounter = 0;
            _pressed = false;
            _pressedOutState = false;
        }
        virtual ~_ButtonChange() {}

        virtual void onValueChange( AbstractSelector* selector ) = 0;

        bool onPress( double x, double y ) {
            _pressed = ENABLE_SCROLL;
            if ( _pressed ) {
                if ( _selector ) {
                    onValueChange( _selector );
                }
                _lastPressed = _timeNow();
                _lastPressedCounter = SCROLL_INIT_DELAY;
            }
            return Button::onPress( x, y );
        }

        bool onRelease( double x, double y ) {
            _pressed = false;
            _pressedOutState = false;
            return Button::onRelease( x, y );
        }

        void onClick( double x, double y ) {
            if ( _selector && !ENABLE_SCROLL ) {
                onValueChange( _selector );
            }
        }

        void onTick() {
            if ( _pressed && ( Component::getPressedID() == getID() ) ) {
                int64_t now = _timeNow();
                if ( now > _lastPressed + _lastPressedCounter ) {
                    _lastPressedCounter = SCROLL_DELAY;
                    _lastPressed = now;
                    if ( _selector ) {
                        onValueChange( _selector );
                    }
                }
            }
        }

        void onMouseMoveOut( double x, double y ) {
            _pressedOutState = _pressed;
            _pressed = false;
        }

        void onMouseMoveIn( double x, double y ) {
            _pressed = _pressedOutState;
        }

        private:
        AbstractSelector* _selector;
        int64_t _lastPressed;
        int64_t _lastPressedCounter;
        bool _pressed;
        bool _pressedOutState;
    };

    class _ButtonUp : public _ButtonChange {
        public:
        _ButtonUp( ComponentProperties properties, AbstractSelector* selector ) : _ButtonChange( properties, selector ) {
        }
        virtual ~_ButtonUp() {}

        virtual void onValueChange( AbstractSelector* selector ) {
            selector -> onIncreaseValue();
        }

    };

    class _ButtonDown : public _ButtonChange {
        public:
        _ButtonDown( ComponentProperties properties, AbstractSelector* selector ) : _ButtonChange( properties, selector ) {
        }
        virtual ~_ButtonDown() {}

        virtual void onValueChange( AbstractSelector* selector ) {
            selector -> onDecreaseValue();
        }

    };

    static int64_t _timeNow() {
        struct timeval tim;
        gettimeofday( &tim, NULL );
        int64_t ts = tim.tv_sec;
        int64_t tus = tim.tv_usec;
        return ts * 1000 + tus / 1000;
    }

    public:
    AbstractSelector( ComponentProperties properties ) : ComplexComponent( properties ) {
        _staticText = NULL;
    }
    virtual ~AbstractSelector() {
    }

    static const bool ENABLE_SCROLL = true;
    static const int SCROLL_INIT_DELAY = 250;
    static const int SCROLL_DELAY = 90;

    protected:

    virtual void onIncreaseValue() = 0;
    virtual void onDecreaseValue() = 0;

    virtual list < Component* > buildStructure() {
        list < Component* > components;
        _staticText = new StaticText( "", getTextComponentProperties() );
        VectorLayout* layout = new VectorLayout( getProperties() );
        components.push_back( layout );
        Component* arrowDecrease = new _ButtonDown( getButtonDecreaseComponentProperties(), this );
        Component* arrowIncrease = new _ButtonUp( getButtonIncreaseComponentProperties(), this );
        if ( !layout -> addComponent( arrowDecrease ) ) {
            delete arrowDecrease;
        }
        if ( !layout -> addComponent( _staticText ) ) {
            delete _staticText;
            _staticText = NULL;
        }
        if ( !layout -> addComponent( arrowIncrease ) ) {
            delete arrowIncrease;
        }
        layout -> setOrientation( VectorLayout::HORIZONTAL );
        return components;
    }

    virtual ComponentProperties getButtonIncreaseComponentProperties() {
        ComponentProperties properties;
        properties.backgroundImage = 100;
        properties.rotation = 3;
        properties.weight = 0.2;
        return properties;
    }

    virtual ComponentProperties getButtonDecreaseComponentProperties() {
        ComponentProperties properties;
        properties.backgroundImage = 100;
        properties.rotation = 1;
        properties.weight = 0.2;
        return properties;
    }

    virtual ComponentProperties getTextComponentProperties() {
        return ComponentProperties();
    }

    StaticText* getTextDisplay() {
        return _staticText;
    }

    private:

    StaticText* _staticText;

};


