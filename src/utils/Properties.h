#pragma once

class Properties {
    public:

    static const char DEFAULT_FILE_DELIMITER = '=';

    Properties() {
        _fileDelimiter = DEFAULT_FILE_DELIMITER;
    }
    virtual ~Properties() {
    }

    static inline Properties* read( string path ) {
        return read( path.c_str(), DEFAULT_FILE_DELIMITER );
    }

    static inline Properties* read( const char* path ) {
        return read( path, DEFAULT_FILE_DELIMITER );
    }

    static inline Properties* read( string path, char fileDelimiter ) {
        return read( path.c_str(), fileDelimiter );
    }

    static Properties* read( const char* path, char fileDelimiter ) {
        FILE* handle = fopen( path, "r" );
        if ( handle ) {
            Properties* properties = new Properties();
            properties -> setFileDelimiter( fileDelimiter );
            static const int BUFFER_SIZE = 65536;
            char* buffer = ( char* ) malloc( BUFFER_SIZE );
            int lineNumber = 1;
            try {
                while ( !feof( handle ) ) {
                    if ( fgets( buffer, BUFFER_SIZE - 1, handle ) ) {
                        string bufferStr = string( buffer );
                        unsigned int delimiterAt = bufferStr.find_first_of( fileDelimiter );
                        if ( delimiterAt != string::npos ) {
                            /*
                            // disable multi-delimiter
                            if ( delimiterAt != bufferStr.find_last_of( fileDelimiter ) ) {
                                throw ParseException( "More than one delimiter", lineNumber );
                            }
                            */
                            string key = bufferStr.substr( 0, delimiterAt );
                            string value = bufferStr.substr( delimiterAt + 1, bufferStr.size() - 1 );
                            _prepareKey( key, lineNumber );
                            _prepareValue( value, lineNumber );
                            if ( key.empty() ) {
                                throw ParseException( "Key cannot be empty", lineNumber );
                            }
                            string subKey;
                            if ( _isMultiValueKey( key, subKey ) ) {
                                properties -> addToMultiValued( key, subKey, value );
                            } else {
                                properties -> set( key, value );
                            }
                        }
                    } else {
                        break;
                    }
                    lineNumber++;
                }
            } catch ( ParseException e ) {
                delete properties;
                fclose( handle );
                throw e;
            }
            fclose( handle );
            return properties;
        }
        return NULL;
    }

    bool write( string path ) {
        return write( path.c_str() );
    }

    bool write( const char* path ) {
        FILE* handle = fopen( path, "w" );
        if ( handle ) {
            for ( map < string, string >::iterator i = _properties.begin(); i != _properties.end(); i++ ) {
                fprintf( handle, "%s = %s\n", ( i -> first ).c_str(), ( i -> second ).c_str() );
            }
            for ( map < string, map < string, string > >::iterator i = _multiValuedProperties.begin(); i != _multiValuedProperties.end(); i++ ) {
                map < string, string > v = i -> second;
                for ( map < string, string >::iterator j = v.begin(); j != v.end(); j++ ) {
                    fprintf( handle, "%s[ %s ] = %s\n", ( i -> first ).c_str(), ( j -> first ).c_str(), ( j -> second ).c_str() );
                }
            }
            fclose( handle );
            return true;
        }
        return false;
    }

    inline void set( string key, string value ) {
        _properties[ key ] = value;
    }

    inline void setMultiValued( string key, map < string, string > value ) {
        _multiValuedProperties[ key ] = value;
    }

    inline void addToMultiValued( string attributeKey, string parameterKey, string value ) {
        map < string, string > mValue;
        if ( !getMultiValued( attributeKey, mValue ) ) {
            mValue = map < string, string >();
        }
        mValue[ parameterKey ] = value;
        setMultiValued( attributeKey, mValue );
    }

    inline int count() {
        return _properties.size();
    }

    bool get( string key, string* retValue ) {
        map < string, string >::iterator i = _properties.find( key );
        if ( i != _properties.end() ) {
            if ( retValue ) {
                ( *retValue ) = i -> second;
            }
            return true;
        }
        return false;
    }

    inline bool get( string key, string & retValue ) {
        return get( key, &retValue );
    }

    inline bool getInteger( string key, int * value ) {
        string valueStr;
        if ( get( key, valueStr ) ) {
            return StringUtils::toInteger( valueStr, value );
        }
        return false;
    }

    inline bool getDouble( string key, double * value ) {
        string valueStr;
        if ( get( key, valueStr ) ) {
            return StringUtils::toDouble( valueStr, value );
        }
        return false;
    }

    inline bool getMultiValued( string key, map < string, string > & value ) {
        map < string, map < string, string > >::iterator i = _multiValuedProperties.find( key );
        if ( i != _multiValuedProperties.end() ) {
            value = i -> second;
            return true;
        }
        return false;
    }

    inline void setFileDelimiter( char fileDelimiter ) {
        _fileDelimiter = fileDelimiter;
    }

    inline char getFileDelimiter( char fileDelimiter ) {
        return _fileDelimiter;
    }

    inline map < string, string > getAll() {
        return _properties;
    }

    inline map < string, map < string, string > > getAllMultiValued() {
        return _multiValuedProperties;
    }

    private:

    static void _prepareKey( string & str, int lineNumber ) {
        StringUtils::trim( str );
        /*if ( ( str[ 0 ] != '"' ) || ( str[ str.size() - 1 ] != '"' ) ) {
            throw ParseException( "Unexpected character in key", lineNumber );
        }
        if ( str.size() < 2 ) {
            throw ParseException( "Invalid marks", lineNumber );
        }
        str = str.substr( 1, str.length() - 2 );*/
    }

    static void _prepareValue( string & str, int lineNumber ) {
        StringUtils::trim( str );
        /*if ( ( str[ 0 ] != '"' ) || ( str[ str.size() - 1 ] != '"' ) ) {
            throw ParseException( "Unexpected character in value", lineNumber );
        }
        if ( str.size() < 2 ) {
            throw ParseException( "Invalid marks", lineNumber );
        }
        str = str.substr( 1, str.length() - 2 );*/
    }

    static bool _isMultiValueKey( string & key, string & subKey ) {
        unsigned int openBracket = key.find_last_of( '[' );
        unsigned int closeBracket = key[ key.size() - 1 ];
        if ( ( openBracket != string::npos ) && ( closeBracket == ']' ) ) {
            subKey = key.substr( openBracket + 1, key.length() - openBracket - 2 );
            key = key.substr( 0, openBracket );
            StringUtils::trim( key );
            StringUtils::trim( subKey );
            return true;
        }
        return false;
    }

    map < string, string > _properties;
    map < string, map < string, string > > _multiValuedProperties;
    char _fileDelimiter;

};


