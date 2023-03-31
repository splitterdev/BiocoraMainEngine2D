#pragma once

class QuestManager {
    private:

    static QuestManager* _DEFAULT_INSTANCE;

    public:
    QuestManager() {
        map < int, Quest* > allQuests = Cache::getFileAccessor() -> getQuests();
        for ( map < int, Quest* >::iterator i = allQuests.begin(); i != allQuests.end(); i++ ) {
            Quest* quest = i -> second;
            list < int > usingQuests = _getUsingQuestList( quest, allQuests );
            _questUsage[ i -> first ] = usingQuests;
        }
    }

    static QuestManager* getDefault() {
        if ( !_DEFAULT_INSTANCE ) {
            _DEFAULT_INSTANCE = new QuestManager();
        }
        return _DEFAULT_INSTANCE;
    }

    virtual ~QuestManager() {
        clearActiveQuests();
    }

    bool openQuest( int questId ) {
        if ( _finishedQuests.find( questId ) != _finishedQuests.end() ) {
            return false;
        }
        Quest* quest = Cache::getFileAccessor() -> getQuest( questId );
        if ( quest ) {
            Quest* questCopy = quest -> copyNewQuest();
            _activeQuestMap[ questCopy -> getId() ] = questCopy;
            ScriptPool* pool = Cache::getGameStateHolder() -> getActiveScriptPool();
            if ( pool ) {
                ScriptThread* newThread = Cache::getFileAccessor() -> getScript( questCopy -> getCompletionScriptId() );
                if ( newThread ) {
                    pool -> addScriptThread( newThread );
                }
            }
            return true;
        }
        return false;
    }

    void processTick() {
        ScriptPool* activePool = Cache::getGameStateHolder() -> getActiveScriptPool();
        if ( activePool ) {
            map < int, Quest* > newQuests;
            for ( map < int, Quest* >::iterator i = _activeQuestMap.begin(); i != _activeQuestMap.end(); ) {
                if ( _isCompleted( i -> second, activePool ) ) {
                    //printf( "Quest %d is completed. Starting related...\n", i -> second -> getId() );
                    _finishedQuests.insert( i -> second -> getId() );
                    _startRelatedQuests( i -> second -> getId(), newQuests );
                    map < int, Quest* >::iterator j = i;
                    i++;
                    delete j -> second;
                    _activeQuestMap.erase( j );
                } else {
                    i++;
                }
            }
            for ( map < int, Quest* >::iterator i = newQuests.begin(); i != newQuests.end(); i++ ) {
                ScriptThread* newThread = Cache::getFileAccessor() -> getScript( i -> second -> getCompletionScriptId() );
                if ( newThread ) {
                    activePool -> addScriptThread( newThread );
                }
                _activeQuestMap[ i -> first ] = i -> second;
            }
            activePool -> clearRecentlyEndedThreads();
        }
    }

    set < int > getFinishedQuests() {
        return _finishedQuests;
    }

    list < Quest* > getActiveQuests() {
        list < Quest* > quests;
        for ( map < int, Quest* >::iterator i = _activeQuestMap.begin(); i != _activeQuestMap.end(); i++ ) {
            if ( i -> second ) {
                quests.push_back( i -> second );
            }
        }
        return quests;
    }

    void clearActiveQuests() {
        for ( map < int, Quest* >::iterator i = _activeQuestMap.begin(); i != _activeQuestMap.end(); i++ ) {
            if ( i -> second ) {
                delete i -> second;
            }
        }
        _activeQuestMap.clear();
    }

    private:

    list < int > _getUsingQuestList( Quest* quest, map < int, Quest* > allQuests ) {
        list < int > questList;
        for ( map < int, Quest* >::iterator i = allQuests.begin(); i != allQuests.end(); i++ ) {
            Quest* questCheck = i -> second;
            Quest::UnlockCondition condition = questCheck -> getUnlockCondition();
            list < int > preceding = condition.getPrecedingQuestIds();
            for ( list < int >::iterator j = preceding.begin(); j != preceding.end(); j++ ) {
                //printf( " > Precedor of %d : %d\n", questCheck -> getId(), *j );
                if ( ( *j ) == quest -> getId() ) {
                    //printf( "   > Adding as related to %d\n", quest -> getId() );
                    questList.push_back( questCheck -> getId() );
                    break;
                }
            }
        }
        return questList;
    }

    bool _isCompleted( Quest* quest, ScriptPool* activePool ) {
        if ( quest ) {
            int scriptId = quest -> getCompletionScriptId();
            list < int > recentlyEnded = activePool -> getRecentlyEndedThreads();
            for ( list < int >::iterator i = recentlyEnded.begin(); i != recentlyEnded.end(); i++ ) {
                if ( ( *i ) == scriptId ) {
                    return true;
                }
            }
        }
        return false;
    }

    void _startRelatedQuests( int questId, map < int, Quest* > & newQuests ) {
        //printf( "Finding related quests of %d...\n", questId );
        map < int, list < int > >::iterator i = _questUsage.find( questId );
        if ( i != _questUsage.end() ) {
            list < int > relatedQuests = i -> second;
            for ( list < int >::iterator j = relatedQuests.begin(); j != relatedQuests.end(); j++ ) {
                //printf( "Related quests of %d : %d\n", questId, *j );
                if ( _canStartNewQuest( *j ) ) {
                    Quest* newQuest = _startQuest( *j );
                    if ( newQuest ) {
                        //printf( "Added related quest of %d : %d\n", questId, *j );
                        newQuests[ ( *j ) ] = newQuest;
                    }
                }
            }
        }
    }

    bool _canStartNewQuest( int requestedQuestId ) {
        Quest* quest = Cache::getFileAccessor() -> getQuest( requestedQuestId );
        if ( quest ) {
            Quest::UnlockCondition condition = quest -> getUnlockCondition();
            return condition.checkCondition( _finishedQuests );
        }
        return false;
    }

    Quest* _startQuest( int questId ) {
        Quest* quest = Cache::getFileAccessor() -> getQuest( questId );
        if ( quest ) {
            return quest -> copyNewQuest();
        }
        return NULL;
    }

    map < int, Quest* > _activeQuestMap;
    map < int, list < int > > _questUsage;
    set < int > _finishedQuests; /// TODO pull from player config
};

QuestManager* QuestManager::_DEFAULT_INSTANCE = NULL;


