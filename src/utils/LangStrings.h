#pragma once

#include "LangStringsException.h"

class LangStrings {
    private:
    LangStrings( map < int, string > strings ) {
        _stringsMap = strings;
    }
    ~LangStrings() {
    }

    public:

    const static int STRING_BUFFER_SIZE = 64 * 1024;
    const static char* STRING_LANG_PATH_DEFAULT;

    static bool setLanguage( const char* lang ) {
        char* path = ( char* ) malloc( 512 );
        snprintf( path, 511, "%s/%s.dat", STRING_LANG_PATH_DEFAULT, lang );
        FILE* handle = fopen( path, "r" );
        if ( handle ) {
            map < int, string > stringMap;
            char* rBuffer = ( char* ) malloc( STRING_BUFFER_SIZE );
            try {
                while ( !feof( handle ) ) {
                    if ( fgets( rBuffer, STRING_BUFFER_SIZE, handle ) ) {
                        int s_id;
                        if ( sscanf( rBuffer, "%d", &s_id ) == 1 ) {
                            int f;
                            int l;
                            string rStr = string( rBuffer );
                            if ( _findFirstAndLastQM( rStr, &f, &l, '"' ) ) {
                                string sub = rStr.substr( f + 1, l - f - 1 );
                                stringMap[ s_id ] = sub;
                            } else {
                                char msg[ 4096 ];
                                snprintf( msg, 4095, "LangStrings::setLanguage( FILE: %s ) NO QUOTATION MARKS FOUND IN STRING %d\n", path, s_id );
                                throw LangStringsException( msg );
                            }
                        } else {
                            char msg[ 4096 ];
                            snprintf( msg, 4095, "LangStrings::setLanguage( FILE: %s ) NO ID FOUND IN LINE: %s\n", path, rBuffer );
                            throw LangStringsException( msg );
                        }
                    } else {
                        break;
                    }
                }
            } catch ( LangStringsException lse ) {
                fclose( handle );
                free( rBuffer );
                throw lse;
            }
            free( rBuffer );
            fclose( handle );
            if ( _strings ) {
                delete _strings;
                _strings = NULL;
            }
            _strings = new LangStrings( stringMap );
            free( path );
            return true;
        }
        free( path );
        return false;
    }

    static string getString( int id ) {
        if ( _strings ) {
            map < int, string >::iterator found = ( _strings -> _stringsMap ).find( id );
            if ( found != ( _strings -> _stringsMap ).end() ) {
                return found -> second;
            }
        }
        return string( "???" );
    }

    //static void Init( const char* path ) {
        //_prefixes = StringLinker::FromFile( path );
    //}

    static void Init( map < int, string > pathes ) {
        _prefixes = pathes;
        if ( !_prefixes.empty() ) {
            try {
                setLanguage( ( _prefixes.begin() -> second ).c_str() );
            } catch ( Exception e ) {
                printf( "[LSE] %s\n", e.fullMessage().c_str() );
                throw e;
            }
        }
    }

    /*static bool selectLanguage( int langID ) {
        if ( _prefixes ) {
            string prefix = _prefixes -> getString( langID );
            if ( !prefix.empty() ) {
                return setLanguage( prefix.c_str() );
            }
        }
        return false;
    }

    static bool nextLanguage() {
        if ( !_prefixes ) {
            return false;
        }
        _langIndex++;
        if ( _langIndex >= _prefixes -> entryCount() ) {
            _langIndex = 0;
        }
        return selectLanguage( _prefixes -> getEntryID( _langIndex ) );
    }

    static bool prevLanguage() {
        if ( !_prefixes ) {
            return false;
        }
        _langIndex--;
        if ( _langIndex < 0 ) {
            _langIndex = _prefixes -> entryCount() - 1;
        }
        return selectLanguage( _prefixes -> getEntryID( _langIndex ) );
    }*/

    private:

    static bool _findFirstAndLastQM( string & s, int* f, int* l, char mark ) {
        int64_t ff = s.find_first_of( mark );
        int64_t ll = s.find_last_of( mark );
        if ( ( ff != string::npos ) && ( ll != string::npos ) && ( ff != ll ) ) {
            ( *f ) = ff;
            ( *l ) = ll;
            return true;
        }
        return false;
    }

    static LangStrings* _strings;
    //static StringLinker* _prefixes;
    static map < int, string > _prefixes;
    static int _langIndex;

    map < int, string > _stringsMap;

};

LangStrings* LangStrings::_strings = NULL;
//StringLinker* LangStrings::_prefixes = NULL;
map < int, string > LangStrings::_prefixes;

const char* LangStrings::STRING_LANG_PATH_DEFAULT = "data/lang";

class LangStringsID {
    private:
    LangStringsID() {
    }
    ~LangStringsID() {
    }
    public:

    static const int MENU_EXIT = 500;
    static const int MENU_PLAY = 501;
    static const int MENU_NEW_GAME = 502;
    static const int MENU_SELECT_LEVEL = 503;
    static const int MENU_BACK = 504;
    static const int MENU_YES = 505;
    static const int MENU_NO = 506;
    static const int MENU_NEW_PROFILE = 507;
    static const int MENU_DELETE_PROFILE = 508;
    static const int MENU_SELECT_PROFILE = 509;
    static const int MENU_RESUME = 510;
    static const int MENU_DROP_GAME = 511;
    static const int MENU_PREV_PROFILE = 512;
    static const int MENU_NEXT_PROFILE = 513;
    static const int MENU_CONFIRM = 514;
    static const int MENU_AVAILABLE_POINTS = 515;
    static const int MENU_STATS = 516;

    static const int CHARACTER_STAT_HP = 801;
    static const int CHARACTER_STAT_ENERGY = 802;

    static const int INVENTORY_CHANGE = 1000;
    static const int INVENTORY_CONVERT = 1001;
    static const int INVENTORY_UPGRADE = 1002;
    static const int INVENTORY_TAKEOFF = 1003;

    static const int MESSAGE_LOG_DEATH = 100001;
    static const int MESSAGE_LOG_DAMAGE_LOW_HP = 100002;
    static const int MESSAGE_LOG_DAMAGE_MEDIUM_HP = 100003;
    static const int MESSAGE_LOG_DAMAGE_HIGH_HP = 100004;
    static const int MESSAGE_LOG_AUTOBURST = 100005;
    static const int MESSAGE_LOG_OVERHEAT_WARN_1 = 100006;
    static const int MESSAGE_LOG_OVERHEAT_WARN_2 = 100007;
    static const int MESSAGE_LOG_OVERHEAT_GONE = 100008;
    static const int MESSAGE_LOG_BEGIN_LEVEL = 100009;
    static const int MESSAGE_LOG_CANNOT_UPGRADE = 100010;
    static const int MESSAGE_LOG_UPGRADE_SUCCESS = 100011;
    static const int MESSAGE_LOG_UPGRADE_FAIL = 100012;
    static const int MESSAGE_LOG_HBU_ENABLED = 100013;
    static const int MESSAGE_LOG_HBU_DISABLED = 100014;
    static const int MESSAGE_LOG_HBU_USED = 100015;
    static const int MESSAGE_LOG_HBU_USED_FAIL = 100016;
    static const int MESSAGE_LOG_HBU_USED_NOT_NECESSARY = 100017;
    static const int MESSAGE_LOG_LEVEL_ADVANCE = 100018;
    static const int MESSAGE_LOG_AAS_ENABLED = 100019;
    static const int MESSAGE_LOG_AAS_DISABLED = 100020;

};


