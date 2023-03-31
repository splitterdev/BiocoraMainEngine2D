#pragma once

class Adjuster : public AbstractSelector {
    public:

    enum ValueChangeMode {
        ADD,
        MULTIPLY,
        POWER
    };

    Adjuster( ComponentProperties properties, double* valuePointer, double valueChange, ValueChangeMode mode ) : AbstractSelector( properties ) {
        _valuePointer = valuePointer;
        _valueChange = valueChange;
        _mode = mode;
        _fmt = string( "%g" );
        _upperLimit = NAN;
        _lowerLimit = NAN;
    }
    Adjuster( ComponentProperties properties, double* valuePointer, double valueChange, ValueChangeMode mode, double startValue ) : AbstractSelector( properties ) {
        _valuePointer = valuePointer;
        _valueChange = valueChange;
        _mode = mode;
        _fmt = string( "%g" );
        _upperLimit = NAN;
        _lowerLimit = NAN;
        setValue( startValue );
    }
    virtual ~Adjuster() {
    }

    double getValue() {
        if ( _valuePointer ) {
            return *_valuePointer;
        }
        return NAN;
    }

    double getValueChange() {
        return _valueChange;
    }

    ValueChangeMode getValueChangeMode() {
        return _mode;
    }

    void setValue( double value ) {
        if ( _valuePointer ) {
            ( *_valuePointer ) = value;
            _textUpdate();
        }
    }

    void setValueChange( double valueChange ) {
        _valueChange = valueChange;
    }

    void setValueChangeMode( ValueChangeMode mode ) {
        _mode = mode;
    }

    string getTextFormat() {
        return _fmt;
    }

    void setTextFormat( string fmt ) {
        _fmt = fmt;
    }

    double getUpperLimit() {
        return _upperLimit;
    }

    void setUpperLimit( double limit ) {
        _upperLimit = limit;
    }

    double getLowerLimit() {
        return _lowerLimit;
    }

    void setLowerLimit( double limit ) {
        _lowerLimit = limit;
    }

    protected:

    virtual void afterRebuild() {
        _textUpdate();
    }

    virtual void onIncreaseValue() {
        if ( _valuePointer ) {
            switch ( _mode ) {
                case ADD: {
                    ( *_valuePointer ) = ( *_valuePointer ) + _valueChange;
                    break;
                }
                case MULTIPLY: {
                    ( *_valuePointer ) = ( *_valuePointer ) * _valueChange;
                    break;
                }
                case POWER: {
                    ( *_valuePointer ) = pow( ( *_valuePointer ), _valueChange );
                    break;
                }
            }
            _checkLimits();
            _textUpdate();
        }
    }

    virtual void onDecreaseValue() {
        if ( _valuePointer ) {
            switch ( _mode ) {
                case ADD: {
                    ( *_valuePointer ) = ( *_valuePointer ) - _valueChange;
                    break;
                }
                case MULTIPLY: {
                    ( *_valuePointer ) = ( *_valuePointer ) / _valueChange;
                    break;
                }
                case POWER: {
                    ( *_valuePointer ) = pow( ( *_valuePointer ), 1.0 / _valueChange );
                    break;
                }
            }
            _checkLimits();
            _textUpdate();
        }
    }

    private:

    inline void _textUpdate() {
        StaticText* text = getTextDisplay();
        if ( text ) {
            char* value = ( char* ) malloc( 256 );
            snprintf( value, 255, _fmt.c_str(), ( *_valuePointer ) );
            text -> setText( string( value ) );
            free( value );
        }
    }

    inline void _checkLimits() {
        if ( !isnan( _upperLimit ) ) {
            if ( ( *_valuePointer ) > _upperLimit ) {
                ( *_valuePointer ) = _upperLimit;
            }
        }
        if ( !isnan( _lowerLimit ) ) {
            if ( ( *_valuePointer ) < _lowerLimit ) {
                ( *_valuePointer ) = _lowerLimit;
            }
        }
        if ( fabs( *_valuePointer ) < 1E-11 ) {
            ( *_valuePointer ) = 0.0;
        }
    }

    double* _valuePointer;
    double _valueChange;

    string _fmt;

    double _upperLimit;
    double _lowerLimit;

    ValueChangeMode _mode;

};

