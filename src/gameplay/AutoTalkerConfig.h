#pragma once

class AutoTalkerConfig {
    private:
    AutoTalkerConfig() {
    }
    virtual ~AutoTalkerConfig() {
    }
    public:

    static inline void init( string path ) {
        init( path.c_str() );
    }

    static void init( const char* path ) {
        FILE* handle = fopen( path, "r" );
        if ( handle ) {
            int BUFFER_LEN = 2048;
            char* buffer = ( char* ) malloc( BUFFER_LEN );
            while ( fgets( buffer, BUFFER_LEN - 3, handle ) ) {
                try {
                    vector < string > splits = StringUtils::split( buffer, ':' );
                    if ( splits.size() == 6 ) {
                        int messageId;
                        double minHpLevel;
                        double maxHpLevel;
                        int messageLevel;
                        double minGenTime;
                        double maxGenTime;
                        if ( !StringUtils::toInteger( splits[ 0 ], &messageId ) ) {
                            throw 1;
                        }
                        if ( !StringUtils::toDouble( splits[ 1 ], &minHpLevel ) ) {
                            throw 2;
                        }
                        if ( !StringUtils::toDouble( splits[ 2 ], &maxHpLevel ) ) {
                            throw 3;
                        }
                        if ( !StringUtils::toInteger( splits[ 3 ], &messageLevel ) ) {
                            throw 4;
                        }
                        if ( !StringUtils::toDouble( splits[ 4 ], &minGenTime ) ) {
                            throw 5;
                        }
                        if ( !StringUtils::toDouble( splits[ 5 ], &maxGenTime ) ) {
                            throw 6;
                        }
                        AutoTalkerConfig* atc = new AutoTalkerConfig();
                        atc -> _messageId = messageId;
                        atc -> _minHpLevel = minHpLevel;
                        atc -> _maxHpLevel = maxHpLevel;
                        atc -> _messageLevel = messageLevel;
                        atc -> _minGenTime = minGenTime;
                        atc -> _maxGenTime = maxGenTime;
                        _config.push_back( atc );
                    } else {
                        throw 0;
                    }
                } catch ( int inv ) {
                    if ( inv > 0 ) {
                        printf( "ERROR / AutoTalkerConfig::init() / invalid line: \"%s\" (invalid argument %d)\n", buffer, inv );
                    } else {
                        printf( "ERROR / AutoTalkerConfig::init() / invalid line: \"%s\"\n", buffer );
                    }
                }
            }
            fclose( handle );
        }
    }

    static void autoTalkClearInit() {
        double currentTime = Cache::gameTime();
        for ( list < AutoTalkerConfig* >::iterator i = _config.begin(); i != _config.end(); i++ ) {
            ( *i ) -> _setNextFireTimeFrom( currentTime );
        }
    }

    static void autoTalk() {
        EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
        if ( player ) {
            for ( list < AutoTalkerConfig* >::iterator i = _config.begin(); i != _config.end(); i++ ) {
                ( *i ) -> _talk( player );
            }
        }
    }

    private:

    inline void _talk( EntityObject* player ) {
        double hpLevel = player -> getCurrentHp() / player -> getCurrentMaxHp();
        double currentTime = Cache::gameTime();
        if ( currentTime > _fireTime ) {
            _setNextFireTimeFrom( currentTime );
            if ( ( hpLevel >= _minHpLevel ) && ( hpLevel <= _maxHpLevel ) ) {
                string message = _getFormattedMessage();
                MessageManager::get() -> addMessage( _messageLevel, message );
            }
        }
    }

    inline string _getFormattedMessage() {
        string message = LangStrings::getString( _messageId );
        // @ - random hex address (32 bit)
        vector < string > splitsAt = StringUtils::split( message, '@' );
        if ( splitsAt.size() > 0 ) {
            message = splitsAt[ 0 ];
            for ( int i = 1; i < int( splitsAt.size() ); i++ ) {
                message += StringUtils::toHex( MathUtils::randomInteger() ) + splitsAt[ i ];
            }
        }
        // # - random hex address (64 bit)
        vector < string > splitsHash = StringUtils::split( message, '#' );
        if ( splitsHash.size() > 0 ) {
            message = splitsHash[ 0 ];
            for ( int i = 1; i < int( splitsHash.size() ); i++ ) {
                message += StringUtils::toHex( MathUtils::randomLong() ) + splitsHash[ i ];
            }
        }
        // $ - random number (a, b)
        vector < string > splitsDollar = StringUtils::split( message, '$' );
        if ( splitsDollar.size() > 0 ) {
            message = "";
            for ( int i = 0; i < int( splitsDollar.size() ); i++ ) {
                vector < string > numbers = StringUtils::split( splitsDollar[ i ], ':' );
                bool numberPart = false;
                int a = 0;
                int b = 0;
                if ( numbers.size() == 2 ) {
                    if ( StringUtils::toInteger( numbers[ 0 ], &a ) && StringUtils::toInteger( numbers[ 1 ], &b ) ) {
                        numberPart = true;
                    }
                }
                if ( numberPart ) {
                    message += StringUtils::toString( MathUtils::random( a, b ) );
                } else {
                    message += splitsDollar[ i ];
                }
            }
        }
        // message formatted
        return message;
    }

    inline void _setNextFireTimeFrom( double currentTime ) {
        _fireTime = currentTime + _minGenTime + MathUtils::random() * ( _maxGenTime - _minGenTime );
    }

    int _messageId;
    double _minHpLevel;
    double _maxHpLevel;
    int _messageLevel;
    double _minGenTime;
    double _maxGenTime;

    double _fireTime;

    static list < AutoTalkerConfig* > _config;

};

list < AutoTalkerConfig* > AutoTalkerConfig::_config;

