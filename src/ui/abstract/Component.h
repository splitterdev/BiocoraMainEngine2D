#pragma once

/**
    Enable reusing IDs by Component class
*/
// #define REUSE_ID

class Component {
    public:
    Component( ComponentProperties componentProperties ) {
        _id = _GENERATEID();
        _parent_id = 0;
        _refCount = 0;
        _COMPONENTS[ _id ] = this;
        _properties = componentProperties;
        _mouseInside = false;
    }
    virtual ~Component() {
        for ( map < int64_t, int64_t >::iterator i = _children.begin(); i != _children.end(); i++ ) {
            Component* child = getComponent( i -> second );
            if ( child ) {
                if ( child -> _refCount <= 0 ) {
                    delete child;
                }
            }
        }
        map < int64_t, Component* >::iterator j = _COMPONENTS.find( _id );
        if ( j != _COMPONENTS.end() ) {
            _COMPONENTS.erase( j );
            #ifdef REUSE_ID
            _REUSABLEIDS.push_back( _id );
            #endif // REUSE_ID
        }
        if ( _FOCUS_ID == _id ) {
            _FOCUS_ID = 0;
        }
    }

    inline Component* getParent() {
        return getComponent( _parent_id );
    }

    Component* getRootParent() {
        Component* parent = NULL;
        Component* higherparent = getParent();
        while ( higherparent ) {
            parent = higherparent;
            higherparent = higherparent -> getParent();
        }
        return parent;
    }

    bool addComponent( Component* component ) {
        if ( component ) {
            if ( component -> _parent_id == 0 ) {
                _children[ component -> _id ] = component -> _id;
                component -> _parent_id = _id;
                _displayOrderedComponents.push_back( component -> _id );
                onAddComponent( component );
                return true;
            }
        }
        return false;
    }

    void display() {
        if ( _properties.visible ) {
            onDisplay();
            //for ( map < int64_t, int64_t >::iterator i = _children.begin(); i != _children.end(); i++ ) {
            for ( list < int64_t >::iterator i = _displayOrderedComponents.begin(); i != _displayOrderedComponents.end(); i++ ) {
                Component* child = getComponent( *i );
                if ( child ) {
                    child -> display();
                }
            }
        }
    }

    bool press( double x, double y ) {
        if ( !_properties.enabled ) {
            return false;
        }
        for ( map < int64_t, int64_t >::iterator i = _children.begin(); i != _children.end(); i++ ) {
            Component* child = getComponent( i -> second );
            if ( child ) {
                if ( child -> press( x, y ) ) {
                    return true;
                }
            }
        }
        if ( mouseOver( x, y ) ) {
            bool pr = onPress( x, y );
            setFocus( _id );
            return pr;
        }
        return false;
    }

    bool release( double x, double y ) {
        if ( !_properties.enabled ) {
            return false;
        }
        for ( map < int64_t, int64_t >::iterator i = _children.begin(); i != _children.end(); i++ ) {
            Component* child = getComponent( i -> second );
            if ( child ) {
                if ( child -> release( x, y ) ) {
                    return true;
                }
            }
        }
        if ( mouseOver( x, y ) ) {
            bool rl = onRelease( x, y );
            if ( !isFocused() ) {
                setFocus( 0 );
            }
            return rl;
        }
        return false;
    }

    void mouseTick( double x, double y ) {
        if ( !_properties.enabled ) {
            return;
        }
        bool mouseWasInside = _mouseInside;
        _mouseInside = mouseOver( x, y );
        if ( mouseWasInside != _mouseInside ) {
            if ( _mouseInside ) {
                onMouseMoveIn( x, y );
            } else {
                onMouseMoveOut( x, y );
            }
        }
        if ( _mouseInside ) {
            onMouseOver( x, y );
        }
        for ( map < int64_t, int64_t >::iterator i = _children.begin(); i != _children.end(); i++ ) {
            Component* child = getComponent( i -> second );
            if ( child ) {
                child -> mouseTick( x, y );
            }
        }
    }

    void tick() {
        onTick();
        for ( map < int64_t, int64_t >::iterator i = _children.begin(); i != _children.end(); i++ ) {
            Component* child = getComponent( i -> second );
            if ( child ) {
                child -> tick();
            }
        }
    }

    void keyInput( int k ) {
        if ( !_properties.enabled ) {
            return;
        }
        onKeyInput( k );
        for ( map < int64_t, int64_t >::iterator i = _children.begin(); i != _children.end(); i++ ) {
            Component* child = getComponent( i -> second );
            if ( child ) {
                child -> keyInput( k );
            }
        }
    }

    bool unlinkParent() {
        Component* parent = getComponent( _parent_id );
        if ( parent ) {
            _parent_id = 0;
            parent -> _unlinkChild( _id );
            return true;
        }
        return false;
    }

    ComponentProperties getProperties() {
        return _properties;
    }

    void setProperties( ComponentProperties properties ) {
        _properties = properties;
        //_properties.center();
    }

    inline int64_t getID() {
        return _id;
    }

    static Component* getComponent( int64_t id ) {
        if ( id ) {
            map < int64_t, Component* >::iterator i = _COMPONENTS.find( id );
            if ( i != _COMPONENTS.end() ) {
                return i -> second;
            }
        }
        return NULL;
    }

    Component* getChildComponent( int64_t id ) {
        if ( id ) {
            map < int64_t, int64_t >::iterator i = _children.find( id );
            if ( i != _children.end() ) {
                return getComponent( i -> second );
            }
        }
        return NULL;
    }

    Component* deleteChildComponent( int64_t id ) {
        Component* c = getChildComponent( id );
        if ( c ) {
            c -> unlinkParent();
            return c;
        }
        return NULL;
    }

    vector < int64_t > getChildComponents() {
        vector < int64_t > vec;
        for ( map < int64_t, int64_t >::iterator i = _children.begin(); i != _children.end(); i++ ) {
            vec.push_back( i -> second );
        }
        return vec;
    }

    static list < Component* > unwrap( Component* component ) {
        vector < int64_t > vec = component -> getChildComponents();
        list < Component* > unwrapped;
        for ( vector < int64_t >::iterator i = vec.begin(); i != vec.end(); i++ ) {
            Component* childComponent = getComponent( *i );
            if ( childComponent ) {
                childComponent -> unlinkParent();
                unwrapped.push_back( childComponent );
            }
        }
        return unwrapped;
    }

    virtual void reset() {
        _mouseInside = false;
    }

    virtual void onAddComponent( Component* component ) {
    }

    /*static void grindUnused() {
        int total = _COMPONENTS.size();
        int reusable = _REUSABLEIDS.size();
        int unused = 0;
        int linked = 0;
        int err = 0;
        for ( map < int64_t, Component* >::iterator i = _COMPONENTS.begin(); i != _COMPONENTS.end(); i++ ) {
            Component* c = i -> second;
            if ( c ) {
                if ( c -> _parent_id == 0 ) {
                    unused++;
                } else {
                    linked++;
                }
            } else {
                err++;
            }
        }
        //printf( "Component state: total %d, linked %d, unused %d, error %d, reusable %d\n", total, linked, unused, err, reusable );
    }*/

    protected:

    virtual bool onPress( double x, double y ) {
        _PRESSED_ID = _id;
        return true;
    }

    virtual bool onRelease( double x, double y ) {
        _PRESSED_ID = 0;
        return true;
    }

    virtual void onTick() {
    }

    virtual void onDisplay() = 0;

    virtual void onMouseOver( double x, double y ) {
    }

    inline bool mouseOver( double x, double y ) {
        return ( ( x >= _properties.x ) && ( y >= _properties.y )
                 && ( x < _properties.x + _properties.w ) && ( y < _properties.y + _properties.h ) );
    }

    virtual void onMouseMoveIn( double x, double y ) {
    }

    virtual void onMouseMoveOut( double x, double y ) {
    }

    virtual void onKeyInput( int k ) {
    }

    void displayBackground() {
        displayBackground( NULL, NULL );
    }

    void displayBackground( ComponentProperties* prop ) {
        displayBackground( NULL, prop );
    }

    void displayBackground( Texture* customTexture, ComponentProperties* prop ) {
        ComponentProperties* p = prop;
        if ( !p ) {
            p = &_properties;
        }
        Texture* texture = customTexture != NULL ? customTexture : Cache::getFileAccessor() -> getTexture( p -> backgroundImage );
        SDL_Rect dst;
        double calcW = p -> w * WINDOW_WIDTH;
        double calcH = p -> h * WINDOW_HEIGHT;
        dst.w = calcW * p -> scale;
        dst.h = calcH * p -> scale;
        dst.x = p -> x * WINDOW_WIDTH - ( dst.w - calcW ) / 2.0;
        dst.y = p -> y * WINDOW_HEIGHT - ( dst.h - calcH ) / 2.0;
        if ( texture ) {
            /*SDL_Point center;
            center.x = dst.x + ( dst.w >> 1 );
            center.y = dst.y + ( dst.h >> 1 );*/
            SDL_RenderCopyEx( Renderer::getRenderer(), texture -> getTexture(), NULL, &dst, p -> rotation, NULL, SDL_FLIP_NONE );
            //printf( "Display %d %d %d %d\n", dst.x, dst.y, dst.w, dst.h );
        } else {
            Uint8 oldR;
            Uint8 oldG;
            Uint8 oldB;
            Uint8 oldA;
            Uint32 color = p -> fillColor;
            SDL_GetRenderDrawColor( Renderer::getRenderer(), &oldR, &oldG, &oldB, &oldA );
            SDL_SetRenderDrawColor(
                Renderer::getRenderer(),
                ( color >> 16 ) & 0xFF,
                ( color >> 8 ) & 0xFF,
                ( color ) & 0xFF,
                ( color >> 24 ) & 0xFF
            );
            SDL_RenderFillRect( Renderer::getRenderer(), &dst );
            SDL_SetRenderDrawColor( Renderer::getRenderer(), oldR, oldG, oldB, oldA );
        }
        /*glPushAttrib( GL_ALL_ATTRIB_BITS );
        ComponentProperties* p = prop;
        if ( !p ) {
            p = &_properties;
        }
        Texture* texture = customTexture != NULL ? customTexture : TextureHolder::getGlobalHolder() -> get( p -> backgroundImage );
        if ( texture ) {
            glEnable( GL_TEXTURE_2D );
            texture -> glUseImage();
            //Camera::SetColorAlpha( 1.0, 1.0, 1.0, p -> alpha ); // if uses Camera module
            glColor4d( 1.0, 1.0, 1.0, p -> alpha ); // otherwise
        } else {
            glDisable( GL_TEXTURE_2D );
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            unsigned int c = p -> fillColor;
            glColor4ub( ( c >> 16 ) & 0x000000FF, ( c >> 8 ) & 0x000000FF, c & 0x000000FF, ( c >> 24 ) & 0x000000FF );
        }
        glDisable( GL_DEPTH_TEST );
        glMatrixMode( GL_PROJECTION );
        glPushMatrix();
        glLoadIdentity();
        //double screenX = FrameUnit::getScreenWidth();
        //double screenY = FrameUnit::getScreenHeight();
        //glOrtho( - screenX, screenX, - screenY, screenY, 0.0, 1.0 );
        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glLoadIdentity();
        double xx = p -> x;
        double yy = p -> y;
        double ww = p -> w;
        double hh = p -> h;
        double xd = ( xx + p -> x_translation ) * 2.0 - 1.0;
        double yd = ( yy + p -> y_translation ) * 2.0 - 1.0;
        //double xc = ( p -> center_x ) * 2.0 - 1.0;
        //double yc = ( p -> center_y ) * 2.0 - 1.0;
        double wd = xd + ww * 2.0;
        double hd = yd + hh * 2.0;
        //glTranslated( xc, - yc, 0.0 );
        //glRotated( p -> rotation, 0.0, 0.0, 1.0 );
        //glTranslated( - xc, yc, 0.0 );
        int z = 0.5;
        if ( texture ) {
            double tx = 0.0;
            double ty = 0.0;
            double tw = 1.0;
            double th = 1.0;
            double TEX_INDEXES[ 4 ][ 2 ] = {
                { tx, ty },
                { tx + tw, ty },
                { tx + tw, ty + th },
                { tx, ty + th }
            };
            int startTex = p -> rotation;
            glBegin( GL_QUADS );
                glTexCoord2d( TEX_INDEXES[ ( startTex ) % 4 ][ 0 ], TEX_INDEXES[ ( startTex ) % 4 ][ 1 ] );
                glVertex3d( xd, - yd, z );
                glTexCoord2d( TEX_INDEXES[ ( startTex + 1 ) % 4 ][ 0 ], TEX_INDEXES[ ( startTex + 1 ) % 4 ][ 1 ] );
                glVertex3d( wd, - yd, z );
                glTexCoord2d( TEX_INDEXES[ ( startTex + 2 ) % 4 ][ 0 ], TEX_INDEXES[ ( startTex + 2 ) % 4 ][ 1 ] );
                glVertex3d( wd, - hd, z );
                glTexCoord2d( TEX_INDEXES[ ( startTex + 3 ) % 4 ][ 0 ], TEX_INDEXES[ ( startTex + 3 ) % 4 ][ 1 ] );
                glVertex3d( xd, - hd, z );
            glEnd();
        } else {
            glBegin( GL_QUADS );
                glVertex3d( xd, - yd, z );
                glVertex3d( wd, - yd, z );
                glVertex3d( wd, - hd, z );
                glVertex3d( xd, - hd, z );
            glEnd();
        }
        glMatrixMode( GL_PROJECTION );
        glPopMatrix();
        glMatrixMode( GL_MODELVIEW );
        glPopMatrix();
        glPopAttrib();*/
    }

    static inline int64_t getFocus() {
        return _FOCUS_ID;
    }

    static inline void setFocus( int64_t componentID ) {
        _FOCUS_ID = componentID;
    }

    inline bool isFocused() {
        return ( _FOCUS_ID == _id );
    }

    inline static int64_t getPressedID() {
        return _PRESSED_ID;
    }

    inline static void setPressedID( int64_t id ) {
        _PRESSED_ID = id;
    }

    void bringToFront( Component* child ) {
        if ( child ) {
            int64_t id = child -> _id;
            for ( list < int64_t >::iterator j = _displayOrderedComponents.begin(); j != _displayOrderedComponents.end(); j++ ) {
                if ( ( *j ) == id ) {
                    _displayOrderedComponents.erase( j );
                    break;
                }
            }
            _displayOrderedComponents.push_back( id );
        }
    }

    void bringToFront() {
        Component* parent = getParent();
        if ( parent ) {
            parent -> bringToFront( this );
        }
    }

    private:

    int64_t _id;

    int64_t _parent_id;
    int64_t _refCount;

    bool _mouseInside;

    ComponentProperties _properties;

    map < int64_t, int64_t > _children;
    list < int64_t > _displayOrderedComponents;

    void _unlinkChild( int64_t id ) {
        map < int64_t, int64_t >::iterator i = _children.find( id );
        if ( i != _children.end() ) {
            for ( list < int64_t >::iterator j = _displayOrderedComponents.begin(); j != _displayOrderedComponents.end(); j++ ) {
                if ( ( *j ) == id ) {
                    _displayOrderedComponents.erase( j );
                    break;
                }
            }
            _children.erase( i );
        }
    }

    static int64_t _ID_GEN;
    static int64_t _FOCUS_ID;
    static list < int64_t > _REUSABLEIDS;

    inline static int64_t _GENERATEID() {
        #ifdef REUSE_ID
        if ( !_REUSABLEIDS.empty() ) {
            int64_t id = _REUSABLEIDS.front();
            _REUSABLEIDS.pop_front();
            return id;
        }
        #endif // REUSE_ID
        int64_t id = _ID_GEN;
        _ID_GEN++;
        return id;
    }

    static map < int64_t, Component* > _COMPONENTS;

    static int64_t _PRESSED_ID;

};

int64_t Component::_ID_GEN = 1;
int64_t Component::_FOCUS_ID = 0;
int64_t Component::_PRESSED_ID = 0;
list < int64_t > Component::_REUSABLEIDS;
map < int64_t, Component* > Component::_COMPONENTS;
