#pragma once

/*
class LevelSelectorFormDesign : public FormDesignFactory {
    public:
    LevelSelectorFormDesign() : FormDesignFactory() {
    }
    virtual ~LevelSelectorFormDesign() {
    }

    class StartGameButton : public UiButton {
        public:
        StartGameButton( int chapterId, int levelId, ComponentProperties properties ) : UiButton( "", properties, 1024 ) {
            string chapterName = _getChapterName( chapterId );
            string levelName = _getLevelName( chapterId, levelId );
            string text = string( "Level " ) + chapterName + string( " / " ) + levelName;
            setText( text );
            _chapterId = chapterId;
            _levelId = levelId;
        }
        virtual ~StartGameButton() {
        }
        virtual void onClick( double x, double y ) {
            LoadingPerspective* loading = dynamic_cast < LoadingPerspective* > ( Perspective::GetPerspective( PERSPECTIVE_LOADING_ID ) );
            if ( loading ) {
                loading -> setLoadingData( _chapterId, _levelId );
            }
            Perspective::ChangeTo( PERSPECTIVE_WARP_ID );
        }

        private:

        string _getChapterName( int chapterId ) {
            ChapterType* type = Cache::getFileAccessor() -> getChapter( chapterId );
            if ( type ) {
                return LangStrings::getString( type -> getName() );
            }
            return "CH_INVALID";
        }

        string _getLevelName( int chapterId, int levelId ) {
            ChapterType* type = Cache::getFileAccessor() -> getChapter( chapterId );
            if ( type ) {
                return LangStrings::getString( type -> getLevelName( levelId ) );
            }
            return "LV_INVALID";
        }

        int _chapterId;
        int _levelId;

    };

    virtual Form* build( FormBuildParameters parameters ) {
        Form* form = new Form( parameters );

        form -> addComponent( getLevelSelectorComponent( fetchLevels( 1023 ) ) );

        return form;
    }

    protected:

    class SelectorItem {
        public:
        SelectorItem() {
        }
        virtual ~SelectorItem() {
        }
        int chapterId;
        int levelId;
        ComponentProperties properties;
    };

    class LevelSelectableList : public SelectableList {
        public:
        LevelSelectableList( int visibleRows, vector < SelectorItem > items, ComponentProperties properties ) : SelectableList( visibleRows, items.size(), properties ) {
            _items = items;
        }
        virtual ~LevelSelectableList() {
        }
        private:
        vector < SelectorItem > _items;
        protected:
        virtual Component* buildData( int rowId ) {
            if ( ( rowId < 0 ) || ( rowId >= int( _items.size() ) ) ) {
                return NULL;
            }
            SelectorItem item = _items[ rowId ];
            return new StartGameButton( item.chapterId, item.levelId, item.properties );
        }
        virtual int getArrowTextureID() {
            return 1024;
        }
    };

    Component* getLevelSelectorComponent( vector < SelectorItem > items ) {
        ComponentProperties properties;
        properties.x = 0.1;
        properties.y = 0.1;
        properties.w = 0.3;
        properties.h = 0.8;
        return new LevelSelectableList( 8, items, properties );
    }

    static vector < SelectorItem > fetchLevels( int backgroundImage ) {
        vector < SelectorItem > items;
        ComponentProperties properties;
        properties.backgroundImage = backgroundImage;
        vector < ChapterType* > chapters = Cache::getFileAccessor() -> getAllChapters();
        for ( vector < ChapterType* >::iterator i = chapters.begin(); i != chapters.end(); i++ ) {
            ChapterType* chapter = ( *i );
            int chapterId = chapter -> getChapterId();
            map < int, int > levels = chapter -> getLevelIdMap();
            for ( map < int, int >::iterator j = levels.begin(); j != levels.end(); j++ ) {
                int levelId = j -> first;
                SelectorItem item;
                item.chapterId = chapterId;
                item.levelId = levelId;
                item.properties = properties;
                items.push_back( item );
            }
        }
        return items;
    }

};
*/
