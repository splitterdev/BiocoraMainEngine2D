#pragma once

#include <utility>

class StringUtils {
    private:
    StringUtils() {}
    virtual ~StringUtils() {}
    public:

    inline static vector < string > split( string input, char delimiter ) {
        return split( input.c_str(), delimiter );
    }

    static vector < string > split( const char* input, char delimiter ) {
        vector < string > v;
        string vs = "";
        for ( int i = 0; i < int( strlen( input ) ); i++ ) {
            if ( input[ i ] == '\n' ) {
                continue;
            }
            if ( input[ i ] == delimiter ) {
                v.push_back( vs );
                vs = "";
            } else {
                vs += input[ i ];
            }
        }
        //if ( vs.size() > 0 ) {
            v.push_back( vs );
        //}
        return v;
    }

    static string trimCopy( string str ) {
        trim( str );
        return str;
    }

    static void trim( string & str ) {
        if( str.empty() ) {
            return;
        }
        const char* pStr = str.c_str();
        size_t front = 0;
        while ( front < str.length() && isspace( int( pStr[ front ] ) ) ) {
            ++front;
        }
        size_t back = str.length();
        while ( back > front && isspace( int( pStr[ back - 1 ] ) ) ) {
            --back;
        }
        if ( 0 == front ) {
            if ( back < str.length() ) {
                str.resize( back - front );
            }
        } else if ( back <= front ) {
            str.clear();
        } else {
            str = str.substr( front, back - front );
        }
    }

    static void trim( char* str ) {
        unsigned int length = strlen( str );
        if( length == 0 ) {
            return;
        }
        size_t front = 0;
        while ( front < length && isspace( int( str[ front ] ) ) ) {
            ++front;
        }
        size_t back = length;
        while ( back > front && isspace( int( str[ back - 1 ] ) ) ) {
            --back;
        }
        if ( 0 == front ) {
            if ( back < length ) {
                str[ back ] = 0;
            }
        } else if ( back <= front ) {
            str[ 0 ] = 0;
        } else {
            memcpy( str, str + front, back - front );
            str[ back ] = 0;
        }
    }

    static string extractFromCopy( string str, char mark ) {
        extractFrom( str, mark );
        return str;
    }

    static void extractFrom( string & str, char mark ) {
        if( str.empty() ) {
            return;
        }
        const char* pStr = str.c_str();
        size_t front = 0;
        while ( front < str.length() && ( pStr[ front ] != mark ) ) {
            ++front;
        }
        size_t back = str.length();
        while ( back > front && ( pStr[ back - 1 ] != mark ) ) {
            --back;
        }
        if ( front - 1 < back ) {
            str = str.substr( front + 1, back - front - 2 );
        } else {
            str.clear();
        }
    }

    inline static bool toDouble( const char* str, double* retValue ) {
        double value = toDouble( str );
        if ( isnan( value ) ) {
            return false;
        }
        if ( retValue ) {
            ( *retValue ) = value;
        }
        return true;
    }

    static double toDouble( const char* Pstr ) {
        unsigned int length = strlen( Pstr );
        if ( length == 0 ) {
            return NAN;
        }
        double value = 0.0;
        double afterCommaMultiplier = 10.0;
        bool commaFound = false;
        double overallMultiplier = 1.0;
        unsigned int startFrom = 0;
        if ( *Pstr == '-' ) {
            overallMultiplier = - 1.0;
            startFrom = 1;
        }
        static const char* INF_STRING = "INF";
        static const unsigned int INF_STRLEN = strlen( INF_STRING );
        bool cmp = true;
        for ( unsigned int i = 0; i < INF_STRLEN; i++ ) {
            if ( Pstr[ startFrom + i ] != INF_STRING[ i ] ) {
                cmp = false;
                break;
            }
        }
        if ( cmp ) {
            return INFINITY * overallMultiplier;
        }
        for ( unsigned int i = startFrom; i < length; i++ ) {
            char digit = Pstr[ i ];
            if ( digit >= '0' && digit <= '9' ) {
                if ( commaFound ) {
                    value += double( digit - '0' ) / afterCommaMultiplier;
                    afterCommaMultiplier *= 10.0;
                } else {
                    value = ( value * 10.0 ) + double( digit - '0' );
                }
            } else if ( digit == '.' ) {
                if ( !commaFound ) {
                    commaFound = true;
                } else {
                    return NAN;
                }
            } else {
                return NAN;
            }
        }
        return value * overallMultiplier;
    }

    static bool toInteger( const char* Pstr, int* retValue ) {
        unsigned int length = strlen( Pstr );
        if ( length == 0 ) {
            return false;
        }
        int value = 0;
        int overallMultiplier = 1;
        unsigned int startFrom = 0;
        if ( *Pstr == '-' ) {
            overallMultiplier = - 1;
            startFrom = 1;
        }
        for ( unsigned int i = startFrom; i < length; i++ ) {
            char digit = Pstr[ i ];
            if ( digit >= '0' && digit <= '9' ) {
                value = ( value * 10 ) + ( digit - '0' );
            } else {
                return false;
            }
        }
        if ( retValue ) {
            ( *retValue ) = value * overallMultiplier;
        }
        return true;
    }

    static bool toIntegerFromHex( const char* Pstr, Uint32* retValue ) {
        unsigned int length = strlen( Pstr );
        if ( length == 0 ) {
            return false;
        }
        Uint32 value = 0;
        unsigned int startFrom = 0;
        for ( unsigned int i = startFrom; i < length; i++ ) {
            char digit = toupper( Pstr[ i ] );
            if ( digit >= '0' && digit <= '9' ) {
                value = ( value * 16 ) + ( digit - '0' );
            } else if ( digit >= 'A' && digit <= 'F' ) {
                value = ( value * 16 ) + 10 + ( digit - 'A' );
            } else {
                return false;
            }
        }
        if ( retValue ) {
            ( *retValue ) = value;
        }
        return true;
    }

    static bool toIntegerFromHex64( const char* Pstr, Uint64* retValue ) {
        unsigned int length = strlen( Pstr );
        if ( length == 0 ) {
            return false;
        }
        Uint64 value = 0;
        unsigned int startFrom = 0;
        for ( unsigned int i = startFrom; i < length; i++ ) {
            char digit = toupper( Pstr[ i ] );
            if ( digit >= '0' && digit <= '9' ) {
                value = ( value * 16 ) + ( digit - '0' );
            } else if ( digit >= 'A' && digit <= 'F' ) {
                value = ( value * 16 ) + 10 + ( digit - 'A' );
            } else {
                return false;
            }
        }
        if ( retValue ) {
            ( *retValue ) = value;
        }
        return true;
    }

    inline static bool toDouble( const string & str, double* retValue ) {
        return toDouble( str.c_str(), retValue );
    }

    inline static double toDouble( const string & str ) {
        return toDouble( str.c_str() );
    }

    inline static bool toInteger( const string & str, int* retValue ) {
        return toInteger( str.c_str(), retValue );
    }

    inline static bool toIntegerFromHex( const string & str, Uint32* retValue ) {
        return toIntegerFromHex( str.c_str(), retValue );
    }

    inline static bool toIntegerFromHex64( const string & str, Uint64* retValue ) {
        return toIntegerFromHex64( str.c_str(), retValue );
    }

    inline static string toString( const int & value ) {
        char buffer[ ___BUFFER_SIZE_CONV___INT ]; // non-static == thread-safe
        return string( itoa( value, buffer, 10 ) );
    }

    inline static string toString( const long & value ) {
        char buffer[ ___BUFFER_SIZE_CONV___LNG ]; // non-static == thread-safe
        return string( ltoa( value, buffer, 10 ) );
    }

    inline static string toString( const double & value ) {
        char buffer[ ___BUFFER_SIZE_CONV___DBL ]; // non-static == thread-safe
        snprintf( buffer, ___BUFFER_SIZE_CONV___DBL - 1, "%lf", value );
        return string( buffer );
    }

    inline static string toString( const double & value, const char* fmt ) {
        char buffer[ ___BUFFER_SIZE_CONV___DBL ]; // non-static == thread-safe
        snprintf( buffer, ___BUFFER_SIZE_CONV___DBL - 1, fmt, value );
        return string( buffer );
    }

    inline static string toHex( const uint32_t & value ) {
        char buffer[ ___BUFFER_SIZE_CONV___INT_HEX ]; // non-static == thread-safe
        sprintf( buffer, "%08X", value );
        return string( buffer );
    }

    inline static string toHex( const uint64_t & value ) {
        char buffer[ ___BUFFER_SIZE_CONV___LNG_HEX ]; // non-static == thread-safe
        sprintf( buffer, "%08X%08X", int( ( value >> 32 ) & 0xFFFFFFFF ), int( value & 0xFFFFFFFF ) );
        return string( buffer );
    }

    inline static bool endsWith( string const &fullString, string const &ending ) {
        if ( fullString.length() >= ending.length() ) {
            return ( 0 == fullString.compare( fullString.length() - ending.length(), ending.length(), ending ) );
        } else {
            return false;
        }
    }

    inline static void replace( string & str, char oldChar, char newChar ) {
        int len = str.length();
        for ( int i = 0; i < len; i++ ) {
            if ( str[ i ] == oldChar ) {
                str[ i ] = newChar;
            }
        }
    }

    inline static void replace( char* str, char oldChar, char newChar ) {
        int len = strlen( str );
        for ( int i = 0; i < len; i++ ) {
            if ( str[ i ] == oldChar ) {
                str[ i ] = newChar;
            }
        }
    }

    private:

    static const int ___BUFFER_SIZE_CONV___INT = 12;
    static const int ___BUFFER_SIZE_CONV___INT_HEX = 9;
    static const int ___BUFFER_SIZE_CONV___LNG = 22;
    static const int ___BUFFER_SIZE_CONV___LNG_HEX = 17;
    static const int ___BUFFER_SIZE_CONV___DBL = 256;

};


