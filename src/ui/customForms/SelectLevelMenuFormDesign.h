#pragma once

class SelectLevelMenuFormDesign : public FormDesignFactory {
    public:
    SelectLevelMenuFormDesign() : FormDesignFactory() {
    }
    virtual ~SelectLevelMenuFormDesign() {
    }

    virtual Form* build( FormBuildParameters parameters ) {
        Form* form = new Form( parameters );

        form -> addComponent( getLevelSelectorComponent( fetchLevels() ) );

        form -> addComponent( getBackFormButton() );

        return form;
    }

    protected:

    Component* getLevelSelectorComponent( vector < SelectorItem > items ) {
        ComponentProperties properties;
        properties.x = 0.2;
        properties.y = 0.1;
        properties.w = 0.6;
        properties.h = 0.6;
        return new LevelSelectableList( 6, items, properties );
    }

    Component* getBackFormButton() {
        ComponentProperties properties;
        properties.x = 0.25;
        properties.y = 0.75;
        properties.w = 0.5;
        properties.h = 0.1;
        return new BackUiButton( LangStrings::getString( LangStringsID::MENU_BACK ), properties );
    }

    static vector < SelectorItem > fetchLevels() {
        vector < SelectorItem > items;
        ComponentProperties properties;
        EntityType* character = Cache::getFileAccessor() -> getCharacter( PlayerManager::getProfileId() );
        if ( !character ) {
            printf( "Profile of id#%d not found\n", PlayerManager::getProfileId() );
            return items;
        }
        vector < ChapterType* > chapters = Cache::getFileAccessor() -> getAllChapters();
        for ( vector < ChapterType* >::iterator i = chapters.begin(); i != chapters.end(); i++ ) {
            ChapterType* chapter = ( *i );
            int chapterId = chapter -> getChapterId();
            int maxLevel = character -> getLevelProgressForChapter( chapterId );
            map < int, int > levels = chapter -> getLevelIdMap();
            for ( map < int, int >::iterator j = levels.begin(); j != levels.end(); j++ ) {
                int levelId = j -> first;
                if ( maxLevel >= levelId ) {
                    SelectorItem item;
                    item.chapterId = chapterId;
                    item.levelId = levelId;
                    item.properties = properties;
                    items.push_back( item );
                }
            }
        }
        return items;
    }

};

