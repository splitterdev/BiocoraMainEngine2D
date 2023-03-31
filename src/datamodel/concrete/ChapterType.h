#pragma once

class ChapterType : public InteractiveType {
    public:
    ChapterType( int id ) : InteractiveType( id ) {
    }
    virtual ~ChapterType() {
    }

    static const int ATTRIBUTE_CHAPTER_ID = 0;
    static const int ATTRIBUTE_LEVEL = 1;
    static const int ATTRIBUTE_NAME = 2;
    static const int ATTRIBUTE_LEVEL_NAME = 3;

    inline int getChapterId() {
        return getParameter( ATTRIBUTE_CHAPTER_ID );
    }

    virtual InteractiveObject* createObjectInstance( int assignedId ) {
        return NULL;
    }

    inline string getLoadedPath() {
        return _loadedPath;
    }

    inline void setLoadedPath( string loadedPath ) {
        _loadedPath = loadedPath;
    }

    inline int getName() {
        return getParameter( ATTRIBUTE_NAME );
    }

    inline int getLevelName( int orderLevel ) {
        map < int, double > values;
        if ( getMultiValuedParameter( ATTRIBUTE_LEVEL_NAME, values ) ) {
            map < int, double >::iterator i = values.find( orderLevel );
            if ( i != values.end() ) {
                return i -> second;
            }
        }
        return -1;
    }

    inline vector < int > getLevels() {
        map < int, double > values;
        vector < int > levels;
        if ( getMultiValuedParameter( ATTRIBUTE_LEVEL, values ) ) {
            for ( map < int, double >::iterator i = values.begin(); i != values.end(); i++ ) {
                double levelId = i -> second;
                if ( !isnan( levelId ) ) {
                    levels.push_back( int( levelId ) );
                }
            }
        }
        return levels;
    }

    map < int, int > getLevelIdMap() {
        map < int, double > values;
        map < int, int > levels;
        if ( getMultiValuedParameter( ATTRIBUTE_LEVEL, values ) ) {
            for ( map < int, double >::iterator i = values.begin(); i != values.end(); i++ ) {
                double levelId = i -> second;
                if ( !isnan( levelId ) ) {
                    levels[ i -> first ] = int( levelId );
                }
            }
        }
        return levels;
    }

    int getLevel( int levelMappingId ) {
        map < int, double > values;
        if ( getMultiValuedParameter( ATTRIBUTE_LEVEL, values ) ) {
            map < int, double >::iterator i = values.find( levelMappingId );
            if ( i != values.end() ) {
                if ( !isnan( i -> second ) ) {
                    return i -> second;
                }
            }
        }
        return -1;
    }

    bool isLastLevelInChapter( int levelMappingId ) {
        map < int, double > values;
        if ( getMultiValuedParameter( ATTRIBUTE_LEVEL, values ) ) {
            map < int, double >::reverse_iterator i = values.rbegin();
            if ( i != values.rend() ) {
                return ( levelMappingId == i -> first );
            }
        }
        return false;
    }

    int getFirstLevelId() {
        map < int, double > values;
        if ( getMultiValuedParameter( ATTRIBUTE_LEVEL, values ) ) {
            map < int, double >::iterator i = values.begin();
            if ( i != values.end() ) {
                return i -> first;
            }
        }
        return -1;
    }

    int getLevelAfter( int levelMappingId ) {
        map < int, double > values;
        if ( getMultiValuedParameter( ATTRIBUTE_LEVEL, values ) ) {
            /*for ( map < int, double >::iterator i = values.begin(); i != values.end(); i++ ) {
                if ( int( i -> second ) == levelId ) {
                    i++;
                    if ( i == values.end() ) {
                        return -1;
                    }
                    return i -> second;
                }
            }*/
            map < int, double >::iterator i = values.find( levelMappingId );
            if ( i == values.end() ) {
                return -1;
            }
            i++;
            if ( i != values.end() ) {
                if ( !isnan( i -> second ) ) {
                    return i -> first;
                }
            }
        }
        return -1;
    }

    private:

    string _loadedPath;

};


