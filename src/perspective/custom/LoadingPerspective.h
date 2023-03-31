#pragma once

class LoadingPerspective : public Perspective {
    public:
    LoadingPerspective() : Perspective( PERSPECTIVE_LOADING_ID ) {
        _loadingChapter = -1;
        _loadingLevel = -1;
        _loadingDataIsSet = false;
    }
    virtual ~LoadingPerspective() {
    }

    virtual void onCreate() {
    }

    virtual void onDestroy() {
    }

    virtual FormBundle* buildInterface() {
        return NULL;
    }

    virtual bool onTick() {
        // Reset game time
        Cache::setGameTime( 0.0 );
        // Initialize processor
        GameStateProcessor* processor = Cache::getGameStateProcessor();
        if ( processor ) {
            printf( "RM processor... " );
            delete processor;
            printf( "DONE\n" );
        }
        processor = new GameStateProcessor();
        Cache::setGameStateProcessor( processor );
        GameStateProcessorSchemaBuilder* builder = GameStateProcessorSchemaBuilder::create();
        processor -> setGamePlay( builder -> asSinglePlayerCampaign() -> build() );
        delete builder;
        // Load level
        if ( _loadingDataIsSet ) {
            ChapterType* chapter = Cache::getFileAccessor() -> getChapter( _loadingChapter );
            if ( chapter ) {
                int levelId = chapter -> getLevel( _loadingLevel );
                GameStateLoader::loadLevel( levelId );
            }
        }
        // Change to gamePlay
        AutoTalkerConfig::autoTalkClearInit();
        MessageManager::get() -> clear();
        TimedTask::removeAll();
        TimedTask::add( new MessageTimedTask( MessageManager::LEVEL_LOG, LangStrings::getString( LangStringsID::MESSAGE_LOG_BEGIN_LEVEL ), 3000.0 ) );
        TimedDisplay::clear();
        Camera::clearQuakes();
        QuestManager::getDefault() -> clearActiveQuests();
        Cache::getFileAccessor() -> resetSpawns();
        restartLevel();
        Perspective::ChangeTo( PERSPECTIVE_LEVEL_ID );
        return true;
    }

    void setLoadingData( int chapterId, int levelId ) {
        _loadingChapter = chapterId;
        _loadingLevel = levelId;
        _loadingDataIsSet = true;
    }

    inline int getLoadedChapterId() {
        return _loadingChapter;
    }

    inline int getLoadedLevelId() {
        return _loadingLevel;
    }

    static void restartLevel();

    private:

    int _loadingChapter;
    int _loadingLevel;
    bool _loadingDataIsSet;

};


