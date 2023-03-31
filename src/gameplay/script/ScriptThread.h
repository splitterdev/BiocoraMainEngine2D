#pragma once

class ScriptPool;

class ScriptThread {
    friend class ScriptPool;
    public:
    ScriptThread( int id ) {
        _pool = NULL;
        _scriptId = id;
        _accumulator = NAN;
        reset();
    }
    virtual ~ScriptThread() {
        removeThreadContent();
    }

    bool addInstruction( int factoryKey, vector < double > & params ) {
        ScriptInstruction* instruction = ScriptInstructionFactory::createInstruction( factoryKey, params );
        if ( instruction ) {
            instruction -> _motherThread = this;
            _instructions.push_back( instruction );
            return true;
        }
        return false;
    }

    bool nextInstruction() {
        if ( !_done ) {
            if ( _currentIndex < threadSize() ) {
                _currentIndex++;
            }
            if ( _currentIndex >= threadSize() ) {
                _done = true;
            }
        }
        return !_done;
    }

    bool jumpToInstruction( int index ) {
        if ( ( index < 0 || ( index >= threadSize() ) ) ) {
            return false;
        }
        _currentIndex = index;
        return true;
    }

    inline void reset() {
        _currentIndex = 0;
        _done = false;
    }

    inline ScriptInstruction* getCurrentInstruction() {
        if ( ( _currentIndex < 0 || ( _currentIndex >= threadSize() ) ) ) {
            return NULL;
        }
        return _instructions[ _currentIndex ];
    }

    inline int getCurrentIndex() {
        return _currentIndex;
    }

    inline int threadSize() {
        return int( _instructions.size() );
    }

    inline bool isDone() {
        return _done;
    }

    inline void finish() {
        _currentIndex = threadSize();
        _done = true;
    }

    void removeThreadContent() {
        reset();
        for ( vector < ScriptInstruction* >::iterator i = _instructions.begin(); i != _instructions.end(); i++ ) {
            ScriptInstruction* instruction = ( *i );
            if ( instruction ) {
                instruction -> _motherThread = NULL;
                delete instruction;
            }
        }
        _instructions.clear();
    }

    inline ScriptPool* getPool() {
        return _pool;
    }

    inline int getId() {
        return _scriptId;
    }

    inline double getAccumulatorValue() {
        return _accumulator;
    }

    inline void setAccumulatorValue( double value ) {
        _accumulator = value;
    }

    double getVariable( int variableId ) {
        map < int, double >::iterator i = _registers.find( variableId );
        if ( i != _registers.end() ) {
            return i -> second;
        }
        return NAN;
    }

    void setVariable( int variableId, double value ) {
        _registers[ variableId ] = value;
    }

    void pushToStack( double value ) {
        _varStack.push_front( value );
    }

    double popFromStack() {
        if ( _varStack.empty() ) {
            return NAN;
        }
        double value = _varStack.front();
        _varStack.pop_front();
        return value;
    }

    private:

    int _scriptId;
    vector < ScriptInstruction* > _instructions;
    ScriptPool* _pool;
    int _currentIndex;
    bool _done;

    /// variables

    double _accumulator;
    map < int, double > _registers;
    list < double > _varStack;

};


