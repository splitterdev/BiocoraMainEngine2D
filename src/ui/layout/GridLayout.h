#pragma once

class GridLayout : public Layout {
    public:
    GridLayout( ComponentProperties properties, int x, int y ) : Layout( properties ) {
        _table = NULL;
        _x = 0;
        _y = 0;
        resize( x, y );
    }
    virtual ~GridLayout() {
        if ( _table ) {
            free( _table );
        }
    }

    bool addComponent( Component* component, int x, int y, int w, int h ) {
        int64_t* item = _item( x, y, w, h, true );
        if ( item ) {
            if ( Component::addComponent( component ) ) {
                ComponentProperties myProp = getProperties();
                ComponentProperties properties = component -> getProperties();
                properties.x = myProp.x + ( double( x ) / double( _x ) * myProp.w );
                properties.y = myProp.y + ( double( y ) / double( _y ) * myProp.h );
                properties.w = myProp.w / double( _x ) * double( w );
                properties.h = myProp.h / double( _y ) * double( h );
                component -> setProperties( properties );
                Layout* layout = dynamic_cast < Layout* > ( component );
                if ( layout ) {
                    layout -> rescaleContent();
                }
                for ( int i = x; i < x + w; i++ ) {
                    for ( int j = y; j < y + h; j++ ) {
                        item = _item( i, j, 1, 1, false );
                        if ( item ) {
                            ( *item ) = component -> getID();
                        }
                    }
                }
                return true;
            }
        }
        return false;
    }

    inline bool addComponent( Component* component, int x, int y ) {
        return addComponent( component, x, y, 1, 1 );
    }

    bool resize( int x, int y ) {
        if ( ( x < 0 ) || ( y < 0 ) ) {
            return false;
        }
        if ( ( x == 0 ) || ( y == 0 ) ) {
            free( _table );
            _table = NULL;
            _x = 0;
            _y = 0;
            return true;
        }
        int64_t* newTable = ( int64_t* ) calloc( sizeof( int64_t ), x * y );
        if ( _table ) {
            int realRows = min( x, _x );
            int realCols = min( y, _y );
            for ( int r = 0; r < realRows; r++ ) {
                for ( int c = 0; c < realCols; c++ ) {
                    newTable[ r * x + c ] = _table[ r * _x + c ];
                }
            }
            free( _table );
        }
        _table = newTable;
        _x = x;
        _y = y;
        return true;
    }

    Component* getComponent( int x, int y ) {
        int64_t* item = _item( x, y, 1, 1, false );
        if ( item ) {
            return Component::getComponent( *item );
        }
        return NULL;
    }

    bool isOccupied( int x, int y ) {
        return ( getComponent( x, y ) != NULL );
    }

    Component* deleteComponent( int x, int y ) {
        Component* c = getComponent( x, y );
        if ( c ) {
            for ( int i = 0; i < _x; i++ ) {
                for ( int j = 0; j < _y; j++ ) {
                    int64_t* item = _item( i, j, 1, 1, false );
                    if ( item ) {
                        if ( ( *item ) == c -> getID() ) {
                            ( *item ) = 0;
                        }
                    }
                }
            }
            c -> unlinkParent();
        }
        return c;
    }

    Component* deleteComponent( int64_t id ) {
        for ( int i = 0; i < _x; i++ ) {
            for ( int j = 0; j < _y; j++ ) {
                int64_t* item = _item( i, j, 1, 1, false );
                if ( item ) {
                    if ( ( *item ) == id ) {
                        ( *item ) = 0;
                    }
                }
            }
        }
        Component* c = Component::getComponent( id );
        if ( c ) {
            c -> unlinkParent();
        }
        return c;
    }

    void rescaleContent() {
        ComponentProperties myProp = getProperties();
        vector < int64_t > children = getChildComponents();
        for ( vector < int64_t >::iterator i = children.begin(); i != children.end(); i++ ) {
            Component* c = Component::getComponent( *i );
            if ( c ) {
                int x = 1;
                int y = 1;
                int w = 1;
                int h = 1;
                _findItem( *i, x, y, w, h );
                //printf( "(%g/%g) - Gridlayout item %d: x = %d, y = %d, w = %d, h = %d\n", myProp.x, myProp.y, int( getID() ), x, y, w, h );
                ComponentProperties properties = c -> getProperties();
                properties.x = myProp.x + ( double( x ) / double( _x ) * myProp.w );
                properties.y = myProp.y + ( double( y ) / double( _y ) * myProp.h );
                properties.w = myProp.w / double( _x ) * double( w );
                properties.h = myProp.h / double( _y ) * double( h );
                c -> setProperties( properties );
                Layout* l = dynamic_cast < Layout* > ( c );
                if ( l ) {
                    l -> rescaleContent();
                }
            }
        }
    }

    private:

    void _findItem( int64_t id, int & x, int & y, int & w, int & h ) {
        for ( int j = 0; j < _x; j++ ) {
            for ( int k = 0; k < _y; k++ ) {
                int64_t* item = _item( j, k, 1, 1, false );
                if ( item ) {
                    if ( ( *item ) == id ) {
                        x = j;
                        y = k;
                        for ( int l = x + 1; l < _x; l++ ) {
                            int64_t* item2 = _item( l, y, 1, 1, false );
                            if ( item2 ) {
                                if ( ( *item2 ) != id ) {
                                    break;
                                } else {
                                    w++;
                                }
                            }
                        }
                        for ( int m = y + 1; m < _y; m++ ) {
                            int64_t* item2 = _item( x, m, 1, 1, false );
                            if ( item2 ) {
                                if ( ( *item2 ) != id ) {
                                    break;
                                } else {
                                    h++;
                                }
                            }
                        }
                        return;
                    }
                }
            }
        }
    }

    int64_t* _item( int x, int y, int w, int h, bool getFree ) {
        if ( ( w > 0 ) && ( h > 0 ) ) {
            if ( _table && ( x >= 0 ) && ( x + w <= _x ) && ( y >= 0 ) && ( y + h <= _y ) ) {
                if ( getFree ) {
                    for ( int i = x; i < x + w; i++ ) {
                        for ( int j = y; j < y + h; j++ ) {
                            if ( *( _table + j * _x + i ) != 0 ) {
                                return NULL;
                            }
                        }
                    }
                }
                return ( _table + y * _x + x );
            }
        }
        return NULL;
    }

    int64_t* _table;
    int _x;
    int _y;

};


