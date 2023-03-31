#pragma once

class Component {
    public:
    Component( int textureId, double x, double y, double w, double h ) {
        setTextureId( textureId );
        setPositionAndDimensions( x, y, w, h );
        _parent = NULL;
    }
    virtual ~Component() {
        for ( map < Component*, Component* >::iterator i = _subComponents.begin(); i != _subComponents.end(); i++ ) {
            delete i -> second;
        }
    }

    virtual void onDisplay( double x, double y, double w, double h ) = 0;

    virtual void onClick() {
    }

    Component* mouseOver( double x, double y ) {
        for ( map < Component*, Component* >::iterator i = _subComponents.begin(); i != _subComponents.end(); i++ ) {
            Component* check = i -> first;
            Component* over = check -> mouseOver( ( x - check -> _x ) / check -> _w, ( y - check -> _y ) / check -> _h );
            if ( over ) {
                return over;
            }
        }
        if ( ( x >= 0.0 ) && ( y >= 0.0 ) && ( x <= 1.0 ) && ( y <= 1.0 ) ) {
            return this;
        }
        return NULL;
    }

    bool click( int x, int y ) {
        double xS = double( x ) / double( WINDOW_WIDTH );
        double yS = double( y ) / double( WINDOW_HEIGHT );
        Component* clickedComponent = mouseOver( ( xS - _x ) / _w, ( yS - _y ) / _h );
        if ( clickedComponent ) {
            clickedComponent -> onClick();
            return true;
        }
        return false;
    }

    void display( double x, double y, double w, double h ) {
        onDisplay( x, y, w, h );
        for ( map < Component*, Component* >::iterator i = _subComponents.begin(); i != _subComponents.end(); i++ ) {
            SDL_Rect rect = Draw::getSDLRect(
                _x * w + x,
                _y * h + y,
                w * _w,
                h * _h
            );
            i -> first -> display( rect.x, rect.y, rect.w, rect.h );
        }
    }

    void displayBackground( int windowPosX, int windowPosY, int windowWidth, int windowHeight ) {
        Texture* texture = Cache::getFileAccessor() -> getTexture( _textureId );
        if ( texture ) {
            SDL_Rect rect = Draw::getSDLRect(
                windowPosX + _x * windowWidth,
                windowPosY + _y * windowHeight,
                _w * windowWidth,
                _h * windowHeight
            );
            SDL_RenderCopy( Renderer::getRenderer(), texture -> getTexture(), NULL, &rect );
        }
    }

    virtual void addSubComponent( Component* component ) {
        if ( component ) {
            _subComponents[ component ] = component;
            component -> _parent = this;
        }
    }

    virtual bool removeSubComponent( Component* component ) {
        if ( component ) {
            map < Component*, Component* >::iterator i = _subComponents.find( component );
            if ( i != _subComponents.end() ) {
                delete i -> second;
                _subComponents.erase( i );
                return true;
            }
        }
        return false;
    }

    inline int getTextureId() {
        return _textureId;
    }

    inline void setTextureId( int textureId ) {
        _textureId = textureId;
    }

    inline double getX() {
        return _x;
    }

    inline double getY() {
        return _y;
    }

    inline double getWidth() {
        return _w;
    }

    inline double getHeight() {
        return _h;
    }

    inline void setPosition( double x, double y ) {
        _x = x;
        _y = y;
    }

    inline void setDimensions( double w, double h ) {
        _w = w;
        _h = h;
    }

    inline void setPositionAndDimensions( double x, double y, double w, double h ) {
        _x = x;
        _y = y;
        _w = w;
        _h = h;
    }

    private:

    int _textureId;
    double _x;
    double _y;
    double _w;
    double _h;

    map < Component*, Component* > _subComponents;
    Component* _parent;

};


