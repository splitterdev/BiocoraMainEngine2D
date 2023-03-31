#pragma once

class ScriptParser {
    public:

    class ScriptInput {
        private:
        ScriptInput() {
            _factoryKey = -1;
            _params = vector < double > ();
            _valid = false;
        }

        public:
        ScriptInput( int factoryKey, vector < double > & params, vector < string > & labels ) {
            _factoryKey = factoryKey;
            _params = params;
            _labels = labels;
            _valid = true;
        }
        virtual ~ScriptInput() {
        }

        inline int getFactoryKey() {
            return _factoryKey;
        }

        inline vector < double > getParams() {
            return _params;
        }

        inline vector < string > getLabels() {
            return _labels;
        }

        inline bool isValid() {
            return _valid;
        }

        inline void invalidate() {
            _valid = false;
        }

        static inline ScriptInput invalid() {
            return ScriptInput();
        }

        private:

        int _factoryKey;
        vector < double > _params;
        vector < string > _labels;
        bool _valid;

    };

    class ParseException : public Exception {
        public:
        ParseException( string input, string message ) : Exception( string( "[SCRIPT] Parse error at line \"" ) + input + string( "\": " ) + message ) {}
        ParseException( string input, const char* message ) : Exception( string( "[SCRIPT] Parse error at line \"" ) + input + string( "\": " ) + string( message ) ) {}
        virtual ~ParseException() {}
    };

    class LinkingException : public Exception {
        public:
        LinkingException( string label ) : Exception( string( "[SCRIPT] Linking error: label \"" ) + label + string( "\" not found" ) ) {}
        virtual ~LinkingException() {}
    };

    ScriptParser() {}
    virtual ~ScriptParser() {}

    static inline ScriptThread* fromFile( int scriptId, string path ) {
        return fromFile( scriptId, path.c_str() );
    }

    /*static map < int, ScriptThread* > fromCombinedFile( const char* path ) {
        FILE* handle = fopen( path, "r" );
        Exception thrown = Exception( "" );
        bool isThrown = false;
        if ( handle ) {
            int BUFFER_SIZE = 64 * 1024;
            char* buffer = ( char* ) malloc( BUFFER_SIZE );
            ScriptParser* parser = new ScriptParser();
            ScriptThread* thread = NULL;
            int scriptId = -1;
            try {
                while ( fgets( buffer, BUFFER_SIZE - 2, handle ) ) {
                    string line = string( buffer );
                    if ( line. ) {
                        new ScriptThread( scriptId );
                    } else {
                        parser -> parse( line );
                    }
                }
                parser -> pushToThread( thread );
            } catch ( Exception e ) {
                thrown = e;
                isThrown = true;
            }
            free( buffer );
            fclose( handle );
            delete parser;
            if ( isThrown ) {
                delete thread;
                throw thrown;
            }
            return thread;
        }
        return NULL;
    }*/

    static ScriptThread* fromFile( int scriptId, const char* path ) {
        FILE* handle = fopen( path, "r" );
        Exception thrown = Exception( "" );
        bool isThrown = false;
        if ( handle ) {
            int BUFFER_SIZE = 64 * 1024;
            char* buffer = ( char* ) malloc( BUFFER_SIZE );
            ScriptParser* parser = new ScriptParser();
            ScriptThread* thread = new ScriptThread( scriptId );
            try {
                while ( fgets( buffer, BUFFER_SIZE - 2, handle ) ) {
                    parser -> parse( string( buffer ) );
                }
                parser -> pushToThread( thread );
            } catch ( Exception e ) {
                thrown = e;
                isThrown = true;
            }
            free( buffer );
            fclose( handle );
            delete parser;
            if ( isThrown ) {
                delete thread;
                throw thrown;
            }
            return thread;
        }
        return NULL;
    }

    void pushToThread( ScriptThread* thread ) {
        if ( !thread ) {
            return;
        }
        for ( list < ScriptInput >::iterator i = _inputCache.begin(); i != _inputCache.end(); i++ ) {
            ScriptInput input = ( *i );
            if ( input.isValid() ) {
                int key = input.getFactoryKey();
                vector < double > params = input.getParams();
                vector < string > labels = input.getLabels();
                for ( int i = 0; i < int( labels.size() ); i++ ) {
                    string label = labels[ i ];
                    if ( label.size() > 0 ) {
                        params[ i ] = _getJumpIndex( label );
                    }
                }
                if ( !thread -> addInstruction( key, params ) ) {
                    throw Exception( string( "[SCRIPT] Parse error (push): general engine failure at factory key " ) + StringUtils::toString( key ) );
                }
            } else {
                throw Exception( string( "[SCRIPT] Parse error (push): invalid instruction detected" ) );
            }
        }
        _inputCache.clear();
    }

    void parse( string line ) {
        vector < string > inputs = StringUtils::split( line, ';' );
        for ( int i = 0; i < int( inputs.size() ); i++ ) {
            ScriptInput input = _parseSingleInstruction( inputs[ i ] );
            if ( input.isValid() ) {
                _inputCache.push_back( input );
            }
        }
    }

    static void setParseKeywordFactoryKey( string keyword, int factoryId ) {
        int prevFactoryId;
        if ( getParseKeywordFactoryKey( keyword, &prevFactoryId ) ) {
            throw Exception(
                string( "Duplicate keyword mapping: \"" )
                + keyword
                + string( "\" - previous factory key: " )
                + StringUtils::toString( prevFactoryId )
                + string( ", attempted: " )
                + StringUtils::toString( factoryId )
            );
        }
        _parseKeyMap[ keyword ] = factoryId;
    }

    static bool getParseKeywordFactoryKey( string keyword, int * factoryId ) {
        map < string, int >::iterator i = _parseKeyMap.find( keyword );
        if ( i != _parseKeyMap.end() ) {
            if ( factoryId ) {
                ( *factoryId ) = i -> second;
            }
            return true;
        }
        return false;
    }

    private:

    ScriptInput _parseSingleInstruction( string input ) {
        if ( input.length() == 0 ) {
            return ScriptInput::invalid(); // nothing to parse
        }
        string commentCheck = string( input.c_str() );
        StringUtils::trim( commentCheck );
        if ( commentCheck[ 0 ] == '#' ) {
            // this is a comment, skip
            return ScriptInput::invalid();
        }
        vector < string > inputKP = StringUtils::split( input, '(' );
        if ( inputKP.size() != 2 ) {
            if ( input[ 0 ] == ':' ) {
                _jumpLabels[ input.substr( 1 ) ] = int( _inputCache.size() );
                return ScriptInput::invalid();
            }
            throw ParseException( input, "not a statement" );
        }

        string keyword = inputKP[ 0 ];
        StringUtils::trim( keyword );
        int insKey;
        if ( !getParseKeywordFactoryKey( keyword, &insKey ) ) {
            throw ParseException( input, string( "keyword \"" ) + keyword + string( "\" not registered" ) );
        }

        string params = inputKP[ 1 ];
        StringUtils::replace( params, ')', ' ' );
        vector < string > paramList = StringUtils::split( params, ',' );
        vector < double > insParams;
        vector < string > insLabels;
        for ( int i = 0; i < int( paramList.size() ); i++ ) {
            double value = NAN;
            string param = paramList[ i ];
            StringUtils::trim( param );
            bool raiseError = true;
            if ( StringUtils::toDouble( param, &value ) ) {
                insParams.push_back( value );
                insLabels.push_back( "" );
                raiseError = false;
            } else if ( param.size() == 0 ) {
                // no arguments for call, just skip...
                raiseError = false;
            } else if ( param[ 0 ] == ':' ) {
                insParams.push_back( 0.0 );
                insLabels.push_back( param.substr( 1 ) );
                raiseError = false;
            }
            if ( raiseError ) {
                throw ParseException( input, string( "cannot parse value \"" ) + param + string( "\"" ) );
            }
        }

        return ScriptInput( insKey, insParams, insLabels );
    }

    int _getJumpIndex( string label ) {
        map < string, int >::iterator i = _jumpLabels.find( label );
        if ( i != _jumpLabels.end() ) {
            return i -> second;
        }
        throw LinkingException( label );
    }

    static map < string, int > _parseKeyMap;

    list < ScriptInput > _inputCache;
    map < string, int > _jumpLabels;

};

map < string, int > ScriptParser::_parseKeyMap;

