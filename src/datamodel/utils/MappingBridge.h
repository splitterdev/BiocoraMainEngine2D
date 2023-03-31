#pragma once

class MappingBridge {
    public:
    MappingBridge() {
    }
    virtual ~MappingBridge() {
        for ( map < int, vector < int > * >::iterator i = _attributeMapping.begin(); i != _attributeMapping.end(); i++ ) {
            if ( i -> second ) {
                delete i -> second;
            }
        }
    }

    void addMapping( int srcAttributeId, int dstAttributeId ) {
        vector < int > * mappingVector = _getMapping( srcAttributeId );
        if ( !mappingVector ) {
            mappingVector = new vector < int > ();
        }
        mappingVector -> push_back( dstAttributeId );
        _attributeMapping[ srcAttributeId ] = mappingVector;
    }

    void copyAttributes( map < int, double > src, map < int, double > * dst ) {
        for ( map < int, double >::iterator i = src.begin(); i != src.end(); i++ ) {
            int srcId = i -> first;
            vector < int > * mapping = _getMapping( srcId );
            if ( mapping ) {
                for ( vector < int >::iterator j = mapping -> begin(); j != mapping -> end(); j++ ) {
                    pair < int, double > p;
                    p.first = srcId;
                    p.second = i -> second;
                    dst -> insert( p );
                }
            }
        }
    }

    private:

    inline vector < int > * _getMapping( int srcAttributeId ) {
        map < int, vector < int >* >::iterator i = _attributeMapping.find( srcAttributeId );
        if ( i != _attributeMapping.end() ) {
            return i -> second;
        }
        return NULL;
    }

    map < int, vector < int >* > _attributeMapping;

};


