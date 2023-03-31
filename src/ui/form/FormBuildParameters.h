#pragma once

class FormBuildParameters {
    public:
    FormBuildParameters() {
    }
    virtual ~FormBuildParameters() {
    }

    inline double get( int paramID ) {
        map < int, double >::iterator i = _params.find( paramID );
        if ( i != _params.end() ) {
            return i -> second;
        }
        return NAN;
    }

    inline void set( int paramID, double paramValue ) {
        _params[ paramID ] = paramValue;
    }

    inline double update( int paramID, double paramValue ) {
        double previous = get( paramID );
        set( paramID, paramValue );
        return previous;
    }

    inline void clear() {
        _params.clear();
    }

    private:
    map < int, double > _params;
};


