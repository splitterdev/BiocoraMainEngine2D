#pragma once

class DialogReader {
    private:
    DialogReader() {}
    virtual ~DialogReader() {}

    class _Page {
        public:
        _Page() {
            senderId = -1;
            senderAvatar = -1;
            messageId = -1;
            textColor = 0xFFFFFFFF;
            waitTimeBefore = 0.0;
            isOwnerMessage = 0;
            passTime = 0.0;
            started = false;
        }
        virtual ~_Page() {
        }

        int senderId;
        int senderAvatar;
        int messageId;
        Uint32 textColor;
        double waitTimeBefore;
        int isOwnerMessage;
        int soundId; // reserved for eventual voicing, unused yet

        double passTime;
        bool started;

        bool processPage() {
            double t = Cache::gameTime();
            if ( !started ) {
                started = true;
                passTime = t + waitTimeBefore;
            }
            if ( isOwnerMessage && ( !CryptChatEngine::getInstance() -> isActive() ) ) {
                passTime = t + waitTimeBefore;
                return false;
            }
            //printf( "Time: %g, passTime: %g\n", t, passTime );
            if ( passTime <= t ) {
                if ( isOwnerMessage ) {
                    CryptChatEngine::getInstance() -> sendMessage( senderId, senderAvatar, messageId, textColor, CryptChatEngine::OWNER );
                } else {
                    CryptChatEngine::getInstance() -> sendMessage( senderId, senderAvatar, messageId, textColor, CryptChatEngine::GUEST );
                }
                started = false;
                return true;
            }
            return false;
        }
    };

    static _Page* _doParseLine( string line ) {
        vector < string > splits = StringUtils::split( line, ',' );
        if ( splits.size() != 6 ) {
            throw Exception( string( "Error: line has wrong number of parameters \"" ) + line + string( "\"" ) );
        }
        for ( int i = 0; i < 6; i++ ) {
            string param = splits[ i ];
            StringUtils::trim( param );
            splits[ i ] = param;
        }
        _Page* page = new _Page();
        try {
            if ( !StringUtils::toInteger( splits[ 0 ], &( page -> senderId ) ) ) {
                throw Exception( string( "Invalid senderId " ) + splits[ 0 ] );
            }
            if ( !StringUtils::toInteger( splits[ 1 ], &( page -> senderAvatar ) ) ) {
                throw Exception( string( "Invalid senderAvatar " ) + splits[ 1 ] );
            }
            if ( !StringUtils::toInteger( splits[ 2 ], &( page -> messageId ) ) ) {
                throw Exception( string( "Invalid messageId " ) + splits[ 2 ] );
            }
            if ( !StringUtils::toIntegerFromHex( splits[ 3 ], &( page -> textColor ) ) ) {
                throw Exception( string( "Invalid textColor " ) + splits[ 3 ] );
            }
            page -> textColor = 0xFF000000 | page -> textColor;
            if ( !StringUtils::toDouble( splits[ 4 ], &( page -> waitTimeBefore ) ) ) {
                throw Exception( string( "Invalid waitTimeBefore " ) + splits[ 4 ] );
            }
            if ( !StringUtils::toInteger( splits[ 5 ], &( page -> isOwnerMessage ) ) ) {
                throw Exception( string( "Invalid isOwnerMessage " ) + splits[ 5 ] );
            }
        } catch ( Exception e ) {
            delete page;
            throw e;
        }
        return page;
    }

    public:

    class Dialog {
        public:
        Dialog() {
            _cursor = 0;
        }
        virtual ~Dialog() {
            deletePages();
        }

        void addPage( _Page* page ) {
            if ( page ) {
                _pages.push_back( page );
            }
        }

        bool processDialog() {
            if ( _cursor < int( _pages.size() ) ) {
                _Page* page = _pages[ _cursor ];
                if ( page -> processPage() ) {
                    _cursor++;
                }
                return false;
            }
            return true;
        }

        void resetDialog() {
            if ( _cursor < int( _pages.size() ) ) {
                _Page* page = _pages[ _cursor ];
                page -> started = false;
            }
            _cursor = 0;
        }

        void deletePages() {
            for ( vector < _Page* >::iterator i = _pages.begin(); i != _pages.end(); i++ ) {
                delete ( *i );
            }
            _pages.clear();
        }

        inline int pageCount() {
            return _pages.size();
        }

        private:

        int _cursor;

        vector < _Page* > _pages;
    };

    static bool readDialogs( const char* path ) {
        FILE* handle = fopen( path, "r" );
        if ( handle ) {
            int BUFFER_SIZE = 4096;
            char* buffer = ( char* ) malloc( BUFFER_SIZE );
            int dialogId = -1;
            Dialog* currentDialog = NULL;
            bool ret = true;
            try {
                _doRead( &currentDialog, dialogId, buffer, BUFFER_SIZE, handle );
            } catch ( Exception e ) {
                e.log();
                ret = false;
            }
            free( buffer );
            fclose( handle );
            return ret;
        }
        return false;
    }

    static Dialog* getDialog( int dialogId ) {
        map < int, Dialog* >::iterator i = _dialogs.find( dialogId );
        if ( i != _dialogs.end() ) {
            return i -> second;
        }
        return NULL;
    }

    static void deleteDialogs() {
        for ( map < int, Dialog* >::iterator i = _dialogs.begin(); i != _dialogs.end(); i++ ) {
            if ( i -> second ) {
                delete i -> second;
            }
        }
        _dialogs.clear();
    }

    private:

    static void _doRead( Dialog** currentDialog, int & dialogId, char* buffer, int bufferSize, FILE* handle ) {
        while ( !feof( handle ) ) {
            if ( fgets( buffer, bufferSize - 2, handle ) ) {
                string line = string( buffer );
                StringUtils::trim( line );
                StringUtils::replace( line, '\n', 0 );
                if ( line.size() > 0 ) { // not empty
                    if ( line[ 0 ] != '#' ) { // not a comment
                        if ( sscanf( line.c_str(), "DIALOG %d", &dialogId ) == 1 ) {
                            if ( getDialog( dialogId ) ) {
                                throw Exception( string( "Duplicate dialog ID: line \"" ) + StringUtils::toString( dialogId ) + string( "\"" ) );
                            }
                            ( *currentDialog ) = new Dialog();
                            _dialogs[ dialogId ] = ( *currentDialog );
                        } else if ( *currentDialog ) {
                            _Page* page = _doParseLine( line );
                            ( *currentDialog ) -> addPage( page );
                            printf( "Dialog #%d pages count: %d\n", dialogId, ( *currentDialog ) -> pageCount() );
                        } else {
                            throw Exception( "Dialog not set for process line" );
                        }
                    }
                }
            }
        }
    }

    static map < int, Dialog* > _dialogs;

};

map < int, DialogReader::Dialog* > DialogReader::_dialogs;

