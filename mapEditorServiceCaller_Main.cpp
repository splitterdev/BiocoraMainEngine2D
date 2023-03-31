#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "src/servicecaller/resource.h"

#include <math.h>
#include <stdlib.h>
#include <string>
using namespace std;
#include <list>
#include <vector>
#include <map>

#include <SDL2/SDL.h>

#include "src/utils/StringUtils.h"
#include "src/utils/DirectoryHelper.h"

class SpawnPositionAnalyser {
    public:
    SpawnPositionAnalyser() {}
    virtual ~SpawnPositionAnalyser() {}
};

HINSTANCE hInst;
HWND MAIN_HWND;

string ME_SERVICE_LOCATION = "mapEdit.exe";
string ME_WORK_CAT = "mapEditor.WorkingDirectory/";

HWND getElementById( int ID ) {
    return GetDlgItem( MAIN_HWND, ID );
}

string getValueFromEdit( int editId ) {
    HWND edit = getElementById( editId );
    int CHARBUFFER_SIZE = 4096;
    char* charBuffer = ( char* ) malloc( CHARBUFFER_SIZE + 2 );
    GetWindowText( edit, charBuffer, CHARBUFFER_SIZE );
    string s = string( charBuffer );
    free( charBuffer );
    return s;
}

int getSelection( int listId ) {
    HWND l = getElementById( listId );
    return SendMessage( l, LB_GETCURSEL, 0, 0 );
}

string getValueFromList( int listId, int index ) {
    HWND l = getElementById( listId );
    int CHARBUFFER_SIZE = 4096;
    char* charBuffer = ( char* ) malloc( CHARBUFFER_SIZE + 2 );
    string retS = string( "?" );
    if ( SendMessage( l, LB_GETTEXT, index, ( LPARAM ) charBuffer ) != LB_ERR ) {
        retS = string( charBuffer );
    }
    free( charBuffer );
    return retS;
}

void setListData( int listId, list < string > data ) {
    HWND l = getElementById( listId );
    SendMessage( l, LB_RESETCONTENT, 0, 0 );
    for ( list < string >::iterator i = data.begin(); i != data.end(); i++ ) {
        string dataItem = ( *i );
        if ( SendMessage( l, LB_ADDSTRING, 0, ( LPARAM ) dataItem.c_str() ) == LB_ERR ) {
            return;
        }
    }
}

void setText( int editId, string text ) {
    HWND hwnd = getElementById( editId );
    SetWindowText( hwnd, text.c_str() );
}

void callMEService( string args ) {
    string servCall = "start " + ME_SERVICE_LOCATION + " " + args;
    printf( "Calling \"%s\"...\n", servCall.c_str() );
    system( servCall.c_str() );
}

void mkdirs( string dirPath ) {
    if ( dirPath.find( "/../" ) != string::npos || dirPath.find( "../" ) == 0 ) {
        return;
    }
    vector < string > dirs = StringUtils::split( dirPath, '/' );
    string dirLoc = "";
    for ( vector < string >::iterator i = dirs.begin(); i != dirs.end(); i++ ) {
        string dir = ( *i );
        if ( dir.length() == 0 ) {
            return;
        }
        dirLoc += dir + "/";
        printf( "mkdirs for \"%s\"\n", dirLoc.c_str() );
        CreateDirectory( dirLoc.c_str(), NULL );
    }
}

string getFixedDir( string dir ) {
    if ( dir.empty() ) {
        return "/";
    }
    unsigned int LAST_CHAR = dir.find_last_of( '/' );
    unsigned int LAST_CHAR_2 = dir.find_last_of( '\\' );
    if ( ( LAST_CHAR != dir.length() - 1 ) && ( LAST_CHAR_2 != dir.length() - 1 ) ) {
        return dir + "/";
    }
    return dir;
}

void loadInfo( string path ) {
    FILE* handle = fopen( path.c_str(), "r" );
    if ( handle ) {
        char* buffer = ( char* ) malloc( 4096 );
        if ( fgets( buffer, 4094, handle ) ) {
            ME_WORK_CAT = string( buffer );
            unsigned int a = ME_WORK_CAT.find_last_of( "\n" );
            if ( a != string::npos ) {
                ME_WORK_CAT = ME_WORK_CAT.substr( 0, a );
            }
            ME_WORK_CAT = getFixedDir( ME_WORK_CAT );
            setText( EDIT_WORKINGDIR, ME_WORK_CAT );
        }
        fclose( handle );
        free( buffer );
    }
}

void saveInfo( string path ) {
    FILE* handle = fopen( path.c_str(), "w" );
    if ( handle ) {
        fprintf( handle, "%s", ME_WORK_CAT.c_str() );
        fclose( handle );
    }
}

string STD_PATH_CONFIG = ".mapEditConfig.dat";

BOOL CALLBACK DlgMain( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
    switch( uMsg ) {
        case WM_INITDIALOG: {
            MAIN_HWND = hwndDlg;
            loadInfo( STD_PATH_CONFIG );
            mkdirs( ME_WORK_CAT );
            setText( EDIT_WORKINGDIR, ME_WORK_CAT.c_str() );
            list < string > files = DirectoryHelper::listAllFiles( ME_WORK_CAT, true );
            setListData( LIST_MAPS, files );
            return TRUE;
        }
        case WM_CLOSE: {
            saveInfo( STD_PATH_CONFIG );
            EndDialog( hwndDlg, 0 );
            return TRUE;
        }
        case WM_COMMAND: {
            //printf( "DEBUG: LOWORD(wParam) = %d\n", LOWORD( wParam ) );
            switch( LOWORD( wParam ) ) {
                case BUTTON_LOAD_MAP: {
                    int selId = getSelection( LIST_MAPS );
                    string mapFileRel = getValueFromList( LIST_MAPS, selId );
                    string mapPath = ME_WORK_CAT + mapFileRel;//getValueFromEdit( EDIT_MAP_PATH );
                    callMEService( "\"" + mapPath + "\"" );
                    break;
                }
                case BUTTON_CREATE_MAP: {
                    string mapPath = ME_WORK_CAT + getValueFromEdit( EDIT_MAP_PATH );
                    string mapW = getValueFromEdit( EDIT_MAP_WIDTH );
                    string mapH = getValueFromEdit( EDIT_MAP_HEIGHT );
                    callMEService( "\"" + mapPath + "\"" + " " + mapW + " " + mapH );
                    break;
                }
                case LIST_PARAMETERS: {
                    break;
                }
                case BUTTON_RELOAD_PARAM: {
                    break;
                }
                case BUTTON_PERSIST_PARAM: {
                    break;
                }
                case LIST_MAPS: {
                    break;
                }
                case BUTTON_REFRESH_MAPS: {
                    list < string > files = DirectoryHelper::listAllFiles( ME_WORK_CAT, true );
                    setListData( LIST_MAPS, files );
                    break;
                }
                case EDIT_WORKINGDIR: {
                    break;
                }
                case BUTTON_UPDATE_WORKINGDIR: {
                    ME_WORK_CAT = getFixedDir( getValueFromEdit( EDIT_WORKINGDIR ) );
                    mkdirs( ME_WORK_CAT );
                    list < string > files = DirectoryHelper::listAllFiles( ME_WORK_CAT, true );
                    setListData( LIST_MAPS, files );
                    break;
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ) {
    hInst = hInstance;
    return DialogBox( hInst, MAKEINTRESOURCE( DLG_MAIN ), NULL, ( DLGPROC ) DlgMain );
}

