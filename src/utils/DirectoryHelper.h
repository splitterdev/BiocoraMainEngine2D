#pragma once

class DirectoryHelper {
    private:
    DirectoryHelper() {}
    virtual ~DirectoryHelper() {}

    public:

    static list < string > listAllFiles( string dir, bool recursive ) {
        return _listAllFiles( dir, "", "*.*", recursive );
    }

    static list < string > listAllFiles( string dir, const char* searchRegex, bool recursive ) {
        return _listAllFiles( dir, "", searchRegex, recursive );
    }

    private:

    static list < string > _listAllFiles( string dir, string glueDir, const char* searchRegex, bool recursive ) {
        WIN32_FIND_DATA ffd;
        string regexDir = dir + string( searchRegex );
        HANDLE hFind = FindFirstFile( regexDir.c_str(), &ffd );
        if ( INVALID_HANDLE_VALUE == hFind ) {
            return list < string > ();
        }
        list < string > dirFiles;
        do {
            if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                if ( ( strcmp( ffd.cFileName, "." ) != 0 && strcmp( ffd.cFileName, ".." ) != 0 ) && ( recursive ) ) {
                    list < string > subList = _listAllFiles( dir + string( ffd.cFileName ) + "/", glueDir + string( ffd.cFileName ) + "/", searchRegex, recursive );
                    for ( list < string >::iterator i = subList.begin(); i != subList.end(); i++ ) {
                        dirFiles.push_back( *i );
                    }
                }
            } else {
                dirFiles.push_back( glueDir + string( ffd.cFileName ) );
            }
        } while ( FindNextFile( hFind, &ffd ) != 0 );
        FindClose( hFind );
        return dirFiles;
    }

};


