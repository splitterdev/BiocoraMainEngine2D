#pragma once

class InteractiveType;

class InteractiveObject {
    friend class InteractiveType;
    public:
    InteractiveObject( int id, int typeId ) {
        _id = id;
        _typeId = typeId;
    }
    virtual ~InteractiveObject() {
    }

    inline double getParameter( int paramId ) {
        map < int, double >::iterator i = _parameters.find( paramId );
        if ( i != _parameters.end() ) {
            return i -> second;
        }
        return NAN;
    }

    inline void setParameter( int paramId, double value ) {
        _parameters[ paramId ] = value;
    }

    inline string getStringParameter( int paramId ) {
        map < int, string >::iterator i = _stringParameters.find( paramId );
        if ( i != _stringParameters.end() ) {
            return i -> second;
        }
        return "";
    }

    inline void setStringParameter( int paramId, string value ) {
        _stringParameters[ paramId ] = value;
    }

    inline bool getMultiValuedParameter( int paramId, map < int, double > & value ) {
        map < int, map < int, double > >::iterator i = _multiValuedParameters.find( paramId );
        if ( i != _multiValuedParameters.end() ) {
            value = i -> second;
            return true;
        }
        return false;
    }

    inline void setMultiValuedParameter( int paramId, map < int, double > value ) {
        _multiValuedParameters[ paramId ] = value;
    }

    inline bool getStringMultiValuedParameter( int paramId, map < int, string > & value ) {
        map < int, map < int, string > >::iterator i = _multiValuedStringParameters.find( paramId );
        if ( i != _multiValuedStringParameters.end() ) {
            value = i -> second;
            return true;
        }
        return false;
    }

    inline void setStringMultiValuedParameter( int paramId, map < int, string > value ) {
        _multiValuedStringParameters[ paramId ] = value;
    }

    inline bool getValueFromMultiValued( int paramId, int keyId, double & value ) {
        map < int, double > valueMap;
        if ( getMultiValuedParameter( paramId, valueMap ) ) {
            map < int, double >::iterator i = valueMap.find( keyId );
            if ( i != valueMap.end() ) {
                value = i -> second;
                return true;
            }
        }
        return false;
    }

    inline void setValueFromMultiValued( int paramId, int keyId, double value ) {
        map < int, double > valueMap;
        if ( !getMultiValuedParameter( paramId, valueMap ) ) {
            valueMap = map < int, double > ();
        }
        valueMap[ keyId ] = value;
        setMultiValuedParameter( paramId, valueMap );
    }

    inline int getId() {
        return _id;
    }

    inline int getTypeId() {
        return _typeId;
    }

    virtual void setCalculatedAttributes() = 0;

    void printAttributes() {
        printf( "Parameters:\n" );
        for ( map < int, double >::iterator i = _parameters.begin(); i != _parameters.end(); i++ ) {
            printf( " > %d = %g\n", i -> first, i -> second );
        }
        printf( "String parameters:\n" );
        for ( map < int, string >::iterator i = _stringParameters.begin(); i != _stringParameters.end(); i++ ) {
            printf( " > %d = %s\n", i -> first, ( i -> second ).c_str() );
        }
        printf( "Multivalued parameters:\n" );
        for ( map < int, map < int, double > >::iterator i = _multiValuedParameters.begin(); i != _multiValuedParameters.end(); i++ ) {
            printf( " > %d = [ ", i -> first );
            map < int, double > v = i -> second;
            for ( map < int, double >::iterator j = v.begin(); j != v.end(); j++ ) {
                printf( "|%d = %g| ", j -> first, j -> second );
            }
            printf( "]\n" );
        }
        printf( "\n" );
    }

    private:

    int _id;
    int _typeId;
    map < int, double > _parameters;
    map < int, string > _stringParameters;
    map < int, map < int, double > > _multiValuedParameters;
    map < int, map < int, string > > _multiValuedStringParameters;

};


