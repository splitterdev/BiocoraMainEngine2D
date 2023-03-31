#pragma once

class SectorCollision {
    private:
    SectorCollision() {
    }
    public:
    virtual ~SectorCollision() {
        for ( int i = 0; i < _w; i++ ) {
            for ( int j = 0; j < _h; j++ ) {
                Item* item = _fields[ _getIndex( i, j ) ];
                if ( item ) {
                    delete item;
                }
            }
        }
        free( _fields );
    }

    class Item {
        public:
        Item() {
            _map = map < int, InteractiveObject* > ();
        }
        virtual ~Item() {
        }
        inline void addObject( int id, InteractiveObject* object ) {
            _map[ id ] = object;
        }
        inline map < int, InteractiveObject* > getObjects() {
            return _map;
        }
        private:
        map < int, InteractiveObject* > _map;
    };

    static SectorCollision* create( int blockX, int blockY, int blockSize ) {
        if ( ( blockX > 0 ) && ( blockY > 0 ) && ( blockSize > 0 ) ) {
            SectorCollision* s = new SectorCollision();
            s -> _blockSize = blockSize;
            s -> _fields = ( Item** ) calloc( blockX * blockY * sizeof( Item* ), 1 );
            s -> _w = blockX;
            s -> _h = blockY;
            return s;
        }
        return NULL;
    }

    map < int, InteractiveObject* > getObjectsFromSectorRange( Vector2D position ) {
        int indexX = position.x / double( _blockSize );
        int indexY = position.y / double( _blockSize );
        if ( ( indexX >= 0 ) && ( indexX < _w ) && ( indexY >= 0 ) && ( indexY < _h ) ) {
            Item* item = _fields[ _getIndex( indexX, indexY ) ];
            if ( item ) {
                return item -> getObjects();
            }
        }
        return map < int, InteractiveObject* > ();
    }

    void addObjectToSectorRange( int id, InteractiveObject* object, Vector2D position, double radius ) {
        int indexX1 = ( position.x - radius ) / double( _blockSize );
        int indexY1 = ( position.y - radius ) / double( _blockSize );
        int indexX2 = ( position.x + radius ) / double( _blockSize );
        int indexY2 = ( position.y + radius ) / double( _blockSize );
        for ( int i = indexX1; i <= indexX2; i++ ) {
            for ( int j = indexY1; j <= indexY2; j++ ) {
                if ( ( i >= 0 ) && ( i < _w ) && ( j >= 0 ) && ( j < _h ) ) {
                    _add( i, j, id, object );
                }
            }
        }
    }

    private:

    inline void _add( int x, int y, int id, InteractiveObject* object ) {
        int i = _getIndex( x, y );
        Item* objects = _fields[ i ];
        if ( !objects ) {
            objects = new Item();
        }
        objects -> addObject( id, object );
        _fields[ i ] = objects;
    }

    inline int _getIndex( int x, int y ) {
        return y * _w + x;
    }

    int _w;
    int _h;
    Item** _fields;
    int _blockSize;

};


