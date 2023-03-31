#pragma once

class EditText : public TextButton {
    public:
    EditText( string label, int textLength, ComponentProperties properties ) : TextButton( label, properties ) {
        _init( label.c_str(), textLength );
        textCenter( false );
    }
    EditText( const char* label, int textLength, ComponentProperties properties ) : TextButton( label, properties ) {
        _init( label, textLength );
        textCenter( false );
    }
    virtual ~EditText() {
        if ( _text ) {
            free( _text );
        }
    }

    virtual void onClick( double x, double y ) {
        setFocus( getID() );
    }

    virtual bool onPress( double x, double y ) {
        EditText* edit = dynamic_cast < EditText* > ( Component::getComponent( Component::getFocus() ) );
        if ( ( edit ) && ( edit != this ) ) {
            edit -> _text[ edit -> _cursor ] = 32;
            edit -> setText( edit -> _text );
        }
        setText( string( _text ) );
        return TextButton::onPress( x, y );
    }

    virtual bool onRelease( double x, double y ) {
        if ( isFocused() ) {
            _text[ _cursor ] = '_';
        } else {
            _text[ _cursor ] = 32;
        }
        setText( string( _text ) );
        return TextButton::onRelease( x, y );
    }

    virtual void onReturn() {
        printf( "\"%s\"\n", getText().c_str() );
    }

    virtual void onKeyInput( int k ) {
        if ( !isFocused() ) {
            return;
        }
        if ( k == 27 ) {
            _text[ _cursor ] = 32;
            Component::setFocus( 0 );
        } else if ( k == 13 ) {
            onReturn();
        } else if ( ( k < 32 ) && ( k != 8 ) ) {
            return;
        } else if ( ( k == 8 ) || ( k == 127 ) ) {
            if ( _cursor > 0 ) {
                _text[ _cursor ] = 32;
                _cursor--;
                _text[ _cursor ] = '_';
            }
        } else if ( ( k >= 32 ) && ( k <= 255 ) ) {
            if ( _cursor < _textLength ) {
                _text[ _cursor ] = char( k );
                _cursor++;
                _text[ _cursor ] = '_';
            }
        }
        setText( string( _text ) );
    }

    virtual string getText() {
        char* t = ( char* ) malloc( _cursor + 1 );
        memcpy( t, _text, _cursor );
        t[ _cursor ] = 0;
        string s = string( t );
        free( t );
        return s;
    }

    virtual void setText( string label ) {
        string text = label.substr( 0, _textLength + 1 );
        TextButton::setText( text );
    }

    private:

    void _init( const char* text, int textLength ) {
        _textLength = textLength;
        _text = ( char* ) malloc( textLength + 2 );
        memset( _text, 32, textLength );
        _text[ textLength ] = 32;
        _text[ textLength + 1 ] = 0;
        _cursor = strlen( text );
        memcpy( _text, text, _cursor );
        setText( string( _text ) );
    }

    char* _text;
    int _textLength;
    int _cursor;

};


