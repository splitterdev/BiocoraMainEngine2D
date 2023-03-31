#pragma once

class ParseException : public Exception {
    public:
    ParseException( string s, int line ) : Exception( _formatException( s.c_str(), line ) ) {
    }
    ParseException( const char* s, int line ) : Exception( _formatException( s, line ) ) {
    }
    virtual ~ParseException() {
    }

    virtual string getClassName() {
        return "ParseException";
    }

    private:

    string _formatException( const char* s, int line ) {
        if ( line >= 0 ) {
            char* buffer = ( char* ) malloc( strlen( s ) + 64 );
            sprintf( buffer, "[line %d] %s", line, s );
            string formattedException = string( buffer );
            free( buffer );
            return formattedException;
        }
        return string( s );
    }

};
