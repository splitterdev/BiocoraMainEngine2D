#pragma once

class TableLayout : public Layout {
    public:
    TableLayout( ComponentProperties properties, int x, int y ) : Layout( properties ) {
        _table = NULL;
        _x = 0;
        _y = 0;
        resize( x, y );
    }
    virtual ~TableLayout() {
        if ( _table ) {
            free( _table );
        }
    }

    vector < Component* > getRow( int row ) {
        vector < Component* > vec;
        for ( int i = 0; i < _x; i++ ) {
            int64_t* item = _item( i, row );
            Component* queried = NULL;
            if ( item ) {
                queried = Component::getComponent( *item );
            }
            vec.push_back( queried );
        }
        return vec;
    }

    vector < Component* > getColumn( int col ) {
        vector < Component* > vec;
        for ( int i = 0; i < _y; i++ ) {
            int64_t* item = _item( col, i );
            Component* queried = NULL;
            if ( item ) {
                queried = Component::getComponent( *item );
            }
            vec.push_back( queried );
        }
        return vec;
    }

    bool addComponent( Component* component, int x, int y ) {
        int64_t* item = _item( x, y );
        if ( item ) {
            if ( Component::addComponent( component ) ) {
                ComponentProperties myProp = getProperties();
                ComponentProperties properties = component -> getProperties();
                properties.x = myProp.x + ( double( x ) / double( _x ) * myProp.w );
                properties.y = myProp.y + ( double( y ) / double( _y ) * myProp.h );
                properties.w = myProp.w / double( _x );
                properties.h = myProp.h / double( _y );
                component -> setProperties( properties );
                Layout* layout = dynamic_cast < Layout* > ( component );
                if ( layout ) {
                    layout -> rescaleContent();
                }
                ( *item ) = component -> getID();
                return true;
            }
        }
        return false;
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
        int64_t* item = _item( x, y );
        if ( item ) {
            return Component::getComponent( *item );
        }
        return NULL;
    }

    Component* deleteComponent( int x, int y ) {
        Component* c = getComponent( x, y );
        if ( c ) {
            int64_t* item = _item( x, y );
            ( *item ) = 0;
            c -> unlinkParent();
        }
        return c;
    }

    void rescaleContent() {
        ComponentProperties myProp = getProperties();
        for ( int x = 0; x < _x; x++ ) {
            for ( int y = 0; y < _y; y++ ) {
                int64_t* item = _item( x, y );
                if ( item ) {
                    Component* c = Component::getComponent( *item );
                    if ( c ) {
                        ComponentProperties properties = c -> getProperties();
                        properties.x = myProp.x + ( double( x ) / double( _x ) * myProp.w );
                        properties.y = myProp.y + ( double( y ) / double( _y ) * myProp.h );
                        properties.w = myProp.w / double( _x );
                        properties.h = myProp.h / double( _y );
                        c -> setProperties( properties );
                        Layout* l = dynamic_cast < Layout* > ( c );
                        if ( l ) {
                            l -> rescaleContent();
                        }
                    }
                }
            }
        }
    }

    private:

    int64_t* _item( int x, int y ) {
        if ( _table && ( x >= 0 ) && ( x < _x ) && ( y >= 0 ) && ( y < _y ) ) {
            return ( _table + y * _x + x );
        }
        return NULL;
    }

    int64_t* _table;
    int _x;
    int _y;

};


