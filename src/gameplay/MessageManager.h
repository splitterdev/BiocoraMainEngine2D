#pragma once

class MessageManager {
    private:
    MessageManager( int maxSize ) {
        _maxSize = maxSize;
    }
    virtual ~MessageManager() {}

    static MessageManager* _DEF_INST;

    static const int DEFAULT_MAX_SIZE = 10;

    class _MessageInfo {
        public:
        _MessageInfo() {
            text = "";
            level = LEVEL_LOG;
        }
        virtual ~_MessageInfo() {}
        string text;
        int level;
    };

    list < _MessageInfo > _formattedMessages;
    int _maxSize;

    static const int _LEVEL_NUM = 5;
    static Uint32 _LEVEL_COLOR[];
    static const char* _LEVEL_HEADER[];

    public:

    static const int LEVEL_LOG = 0;
    static const int LEVEL_NOTICE = 1;
    static const int LEVEL_WARN = 2;
    static const int LEVEL_ERROR = 3;
    static const int LEVEL_FATAL = 4;

    static MessageManager* get() {
        if ( !_DEF_INST ) {
            _DEF_INST = new MessageManager( DEFAULT_MAX_SIZE );
        }
        return _DEF_INST;
    }

    void clear() {
        _formattedMessages.clear();
    }

    void addMessage( string message ) {
        if ( ( int ) _formattedMessages.size() >= _maxSize ) {
            _formattedMessages.pop_back();
        }
        _MessageInfo msgInfo;
        msgInfo.text = message;
        _formattedMessages.push_front( msgInfo );
    }

    void addMessage( int level, string message ) {
        if ( ( int ) _formattedMessages.size() >= _maxSize ) {
            _formattedMessages.pop_back();
        }
        _MessageInfo msgInfo;
        msgInfo.text = message;
        msgInfo.level = level;
        _formattedMessages.push_front( msgInfo );
    }

    void display( int x, int y, int lineOffset ) {
        int a = y;
        double scale = 1.0;
        double horizontalScale = 0.75;
        for ( list < _MessageInfo >::iterator i = _formattedMessages.begin(); i != _formattedMessages.end(); i++ ) {
            _MessageInfo info = ( *i );
            const char* levelHeader = NULL;
            if ( info.level >= 0 && info.level < _LEVEL_NUM ) {
                Uint32 c = _LEVEL_COLOR[ info.level ];
                Text::Color( ( c >> 16 ) & 0xFF, ( c >> 8 ) & 0xFF, c & 0xFF );
                levelHeader = _LEVEL_HEADER[ info.level ];
            } else {
                Text::Color( 254, 254, 254 );
            }
            if ( levelHeader ) {
                Text::WriteWithBackground( Renderer::getRenderer(), x, a, scale, horizontalScale, "%s %s", levelHeader, info.text.c_str() );
            } else {
                Text::WriteWithBackground( Renderer::getRenderer(), x, a, scale, horizontalScale, "%s", info.text.c_str() );
            }
            a += double( lineOffset ) * scale;
        }
        Text::Color( 254, 254, 254 );
    }

    int getMaxSize() {
        return _maxSize;
    }

};

MessageManager* MessageManager::_DEF_INST = NULL;
Uint32 MessageManager::_LEVEL_COLOR[ _LEVEL_NUM ] = { 0x00808080, 0x00FEFEFE, 0x00FEFE00, 0x00FE0000, 0x00FE0080 };
const char* MessageManager::_LEVEL_HEADER[ _LEVEL_NUM ] = { "[Verbose]", "[Notice]", "[WARN]", "[ERROR]", "[FATAL]" };

