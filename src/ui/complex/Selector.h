#pragma once

class Selector : public AbstractSelector {
    public:
    Selector( ComponentProperties properties ) : AbstractSelector( properties ) {
        _current = 0;
        _loop = true;
    }
    virtual ~Selector() {
    }

    inline vector < string > getValues() {
        return _strings;
    }

    void setValues( list < string >* strings ) {
        _strings.clear();
        if ( strings ) {
            for ( list < string >::iterator i = strings -> begin(); i != strings -> end(); i++ ) {
                _strings.push_back( *i );
            }
        }
        _textUpdate();
    }

    void setValues( vector < string >* strings ) {
        if ( !strings ) {
            _strings.clear();
        } else {
            _strings = ( *strings );
        }
        _textUpdate();
    }

    inline bool getLoopOnBorders() {
        return _loop;
    }

    inline void setLoopOnBorders( bool loop ) {
        _loop = loop;
    }

    inline int getOptionCount() {
        return _strings.size();
    }

    inline int getCurrentOptionIndex() {
        return _current;
    }

    void setCurrentOptionIndex( int optionIndex ) {
        if ( ( _current >= 0 ) && ( _current < int( _strings.size() ) ) ) {
            _current = optionIndex;
            _textUpdate();
        }
    }

    string getCurrentOptionValue() {
        if ( ( _current >= 0 ) && ( _current < int( _strings.size() ) ) ) {
            return _strings[ _current ];
        }
        return string();
    }

    void setCurrentOptionValue( string optionValue ) {
        if ( ( _current >= 0 ) && ( _current < int( _strings.size() ) ) ) {
            _strings[ _current ] = optionValue;
            _textUpdate();
        }
    }

    protected:

    virtual void afterRebuild() {
        _textUpdate();
    }

    virtual void onIncreaseValue() {
        int s = _strings.size();
        if ( s > 1 ) {
            int next = _current + 1;
            if ( next >= s ) {
                if ( _loop ) {
                    _current = 0;
                }
            } else {
                _current = next;
            }
        }
        _textUpdate();
    }

    virtual void onDecreaseValue() {
        int s = _strings.size();
        if ( s > 1 ) {
            int next = _current - 1;
            if ( next < 0 ) {
                if ( _loop ) {
                    _current = s - 1;
                }
            } else {
                _current = next;
            }
        }
        _textUpdate();
    }

    private:

    inline void _textUpdate() {
        StaticText* text = getTextDisplay();
        if ( ( text ) && ( _current >= 0 ) && ( _current < int( _strings.size() ) ) ) {
            text -> setText( _strings[ _current ] );
        }
    }

    vector < string > _strings;
    int _current;

    bool _loop;

};


