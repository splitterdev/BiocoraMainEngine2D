#pragma once

class StaticText : public TextButton {
    public:
    StaticText( string label, ComponentProperties properties ) : TextButton( label, properties ) {
    }
    StaticText( const char* label, ComponentProperties properties ) : TextButton( label, properties ) {
    }
    virtual ~StaticText() {
    }
    virtual void onClick( double x, double y ) {
    }
};


