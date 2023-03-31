#pragma once

#include <string>
#include <string.h>

using namespace std;

class Exception {
    public:
    Exception( string s ) {
        _s = s;
    }

    Exception( const char* s ) {
        _s = string( s );
    }

    virtual ~Exception() {
    }

    string cause() {
        return _s;
    }

    virtual string getClassName() {
        return "Exception";
    }

    string fullMessage() {
        return string( "[" ) + getClassName() + string( "] Exception occurred: " ) + _s;
    }

    void log() {
        printf( "%s\n", fullMessage().c_str() );
    }

    private:
    string _s;

};

#include "ParseException.h"


