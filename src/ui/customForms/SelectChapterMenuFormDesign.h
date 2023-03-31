#pragma once

class SelectChapterMenuFormDesign : public FormDesignFactory {
    public:
    SelectChapterMenuFormDesign() : FormDesignFactory() {
    }
    virtual ~SelectChapterMenuFormDesign() {
    }

    virtual Form* build( FormBuildParameters parameters ) {
        Form* form = new Form( parameters );

        form -> addComponent( getLevelSelectorComponent( fetchLevels( 1023 ) ) );

        return form;
    }

    protected:

    /*class SelectorItem {
        public:
        SelectorItem() {
        }
        virtual ~SelectorItem() {
        }
        int chapterId;
        int levelId;
        ComponentProperties properties;
    };*/

    /*class LevelSelectableList : public SelectableList {
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
    };*/

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

