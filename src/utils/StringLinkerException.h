#pragma once

class StringLinkerException : public Exception {
    public:
    StringLinkerException( string s ) : Exception( s ) {
    }
    StringLinkerException( const char* s ) : Exception( s ) {
    }
    virtual ~StringLinkerException() {
    }

    virtual string getClassName() {
        return "StringLinkerException";
    }

    private:
};

