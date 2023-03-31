#pragma once

class ScriptThread;
class ScriptInstructionFactory;

class ScriptInstruction {
    friend class ScriptThread;
    friend class ScriptInstructionFactory;
    public:
    ScriptInstruction() {
        _motherThread = NULL;
        _factoryId = -1;
    }
    virtual ~ScriptInstruction() {
        /*if ( _motherThread ) {
            /// TODO remove from thread
        }*/
    }

    inline vector < double > getParameters() {
        return _parameters;
    }

    bool getParameter( int id, double * value ) {
        if ( ( id >= 0 ) && ( id < parameterCount() ) ) {
            if ( value ) {
                ( *value ) = _parameters[ id ];
            }
            return true;
        }
        return false;
    }

    inline int addParameter( const double value ) {
        int paramId = parameterCount();
        _parameters.push_back( value );
        return paramId;
    }

    inline bool setParameter( int id, const double value ) {
        if ( ( id >= 0 ) && ( id < parameterCount() ) ) {
            _parameters[ id ] = value;
            return true;
        }
        return false;
    }

    inline int parameterCount() {
        return int( _parameters.size() );
    }

    virtual void perform() = 0;

    inline ScriptThread* getThread() {
        return _motherThread;
    }

    inline map < int, double > getResultMap() {
        return _result;
    }

    inline void clearResultMap() {
        _result.clear();
    }

    bool getResult( int key, double * value ) {
        map < int, double >::iterator i = _result.find( key );
        if ( i != _result.end() ) {
            if ( value ) {
                ( *value ) = i -> second;
            }
            return true;
        }
        return false;
    }

    inline int getResultMapSize() {
        return int( _result.size() );
    }

    inline void setResult( int key, double value ) {
        _result[ key ] = value;
    }

    inline int getFactoryKey() {
        return _factoryId;
    }

    private:
    vector < double > _parameters;
    map < int, double > _result;
    ScriptThread* _motherThread;
    int _factoryId;

};


