#pragma once

class StartGameButton : public UiButton {
    public:
    StartGameButton( int chapterId, int levelId, ComponentProperties properties ) : UiButton( "", properties ) {
        string chapterName = _getChapterName( chapterId );
        string levelName = _getLevelName( chapterId, levelId );
        string text = chapterName + string( ": " ) + levelName;
        setText( text );
        _chapterId = chapterId;
        _levelId = levelId;
    }
    virtual ~StartGameButton() {
    }
    virtual bool clickAction() {
        LoadingPerspective* loading = dynamic_cast < LoadingPerspective* > ( Perspective::GetPerspective( PERSPECTIVE_LOADING_ID ) );
        if ( loading ) {
            loading -> setLoadingData( _chapterId, _levelId );
        }
        Perspective::ChangeTo( PERSPECTIVE_WARP_ID );
        return true;
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


