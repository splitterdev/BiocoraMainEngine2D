#pragma once

#include <set>

/**
    *** IMPORTANT ***
    Quests can NOT call themselves (e.g. loop-to-self quest).
    Loops containing two or more different quests can be also troublesome.
*/

class Quest {
    public:

    class UnlockCondition {
        public:
        UnlockCondition() {}
        virtual ~UnlockCondition() {}

        static UnlockCondition parse( string input, int lineNumber ) {
            UnlockCondition condition;
            StringUtils::trim( input );
            vector < string > orConditions = StringUtils::split( input, '|' );
            for ( vector < string >::iterator i = orConditions.begin(); i != orConditions.end(); i++ ) {
                map < int, bool > andConditionMap;
                vector < string > andConditions = StringUtils::split( *i, '&' );
                for ( vector < string >::iterator j = andConditions.begin(); j != andConditions.end(); j++ ) {
                    string andPartialCondition = string( *j );
                    StringUtils::trim( andPartialCondition );
                    bool invertCheck = false;
                    if ( andPartialCondition[ 0 ] == '!' ) {
                        invertCheck = true;
                        andPartialCondition = andPartialCondition.substr( 1 );
                        StringUtils::trim( andPartialCondition );
                    }
                    int questId;
                    if ( StringUtils::toInteger( andPartialCondition, &questId ) ) {
                        andConditionMap[ questId ] = invertCheck;
                    } else {
                        throw ParseException( string( "Unable to parse: \"" ) + andPartialCondition + string( "\", input: \"" ) + input + string( "\"" ), lineNumber );
                    }
                }
                condition.addAndCondition( andConditionMap );
            }
            return condition;
        }

        bool checkCondition( set < int > completedQuests ) {
            for ( list < map < int, bool > >::iterator i = _conditions.begin(); i != _conditions.end(); i++ ) {
                map < int, bool > andConditions = ( *i );
                bool andConditionPass = true;
                //printf( "[QUEST CONDITION] Validate next AND\n" );
                for ( map < int, bool >::iterator j = andConditions.begin(); j != andConditions.end(); j++ ) {
                    int questId = j -> first;
                    //printf( "[QUEST CONDITION]   Validate %d\n", questId );
                    if ( questId < 0 ) {
                        //printf( "[QUEST CONDITION]   Null (-1) quest, skipping...\n" );
                        continue;
                    }
                    bool invertCheck = j -> second;
                    //printf( "[QUEST CONDITION]   Invert check: %c\n", invertCheck ? 'Y' : 'N' );
                    set < int >::iterator k = completedQuests.find( questId );
                    if ( k != completedQuests.end() ) {
                        //printf( "[QUEST CONDITION]   Quest %d found as completed.\n", questId );
                        if ( invertCheck ) {
                            andConditionPass = false;
                        }
                    } else {
                        //printf( "[QUEST CONDITION]   Quest %d not found as completed.\n", questId );
                        if ( !invertCheck ) {
                            andConditionPass = false;
                        }
                    }
                    //printf( "[QUEST CONDITION]   Condition pass: %c\n", andConditionPass ? 'Y' : 'N' );
                }
                if ( andConditionPass ) {
                    //printf( "[QUEST CONDITION]   AND condition passed.\n" );
                    return true;
                }
            }
            //printf( "[QUEST CONDITION]   OR condition failed (no AND passed).\n" );
            return false;
        }

        void addAndCondition( map < int, bool > andConditionMap ) {
            _conditions.push_back( andConditionMap );
        }

        list < int > getPrecedingQuestIds() {
            list < int > preceding;
            for ( list < map < int, bool > >::iterator i = _conditions.begin(); i != _conditions.end(); i++ ) {
                map < int, bool > andConditions = ( *i );
                for ( map < int, bool >::iterator j = andConditions.begin(); j != andConditions.end(); j++ ) {
                    preceding.push_back( j -> first );
                }
            }
            return preceding;
        }

        private:
        /**
            First level ( list<> ) - OR-separated
            Second level ( map<> ) - AND-separated, mapped questId to NOT-operand (true == invertedCheck)
        */
        list < map < int, bool > > _conditions;
    };

    Quest( int questId ) {
        _questId = questId;
        _descriptionId = -1;
        _completionScriptId = -1;
        _completed = true;
    }
    virtual ~Quest() {}

    static map < int, Quest* > parseFile( const char* file ) {
        FILE* handle = fopen( file, "r" );
        map < int, Quest* > questsMap;
        if ( handle ) {
            char* line = ( char* ) malloc( 4096 );
            try {
                int lineNumber = 0;
                while ( !feof( handle ) ) {
                    lineNumber++;
                    if ( fgets( line, 4000, handle ) ) {
                        string lineStr = string( line );
                        StringUtils::trim( lineStr );
                        if ( lineStr.size() == 0 ) {
                            continue;
                        }
                        if ( lineStr[ 0 ] == '#' ) {
                            continue;
                        }
                        list < Quest* > questsFromLine = parseMulti( lineStr, lineNumber );
                        bool exceptionTrigger = false;
                        int duplicateQuestId = -1;
                        for ( list < Quest* >::iterator i = questsFromLine.begin(); i != questsFromLine.end(); i++ ) {
                            Quest* quest = ( *i );
                            map < int, Quest* >::iterator fnd = questsMap.find( quest -> _questId );
                            if ( fnd != questsMap.end() ) {
                                if ( !exceptionTrigger ) {
                                    duplicateQuestId = quest -> _questId;
                                }
                                exceptionTrigger = true;
                                delete quest;
                            } else {
                                questsMap[ quest -> _questId ] = quest;
                            }
                        }
                        if ( exceptionTrigger ) {
                            throw ParseException( string( "Duplicate quest identifier: " ) + StringUtils::toString( duplicateQuestId ), lineNumber );
                        }
                    }
                }
            } catch ( ParseException e ) {
                for ( map < int, Quest* >::iterator i = questsMap.begin(); i != questsMap.end(); i++ ) {
                    delete i -> second;
                }
                free( line );
                fclose( handle );
                throw e;
            }
            fclose( handle );
        }
        return questsMap;
    }

    static list < Quest* > parseMulti( string line, int lineNumber ) {
        list < Quest* > quests;
        vector < string > splits = StringUtils::split( line, ';' );
        for ( vector < string >::iterator i = splits.begin(); i != splits.end(); i++ ) {
            string questSingleLine = string( *i );
            StringUtils::trim( questSingleLine );
            if ( questSingleLine.size() == 0 ) {
                continue;
            }
            try {
                Quest* quest = parseSingle( questSingleLine, lineNumber );
                quests.push_back( quest );
            } catch ( ParseException e ) {
                for ( list < Quest* >::iterator i = quests.begin(); i != quests.end(); i++ ) {
                    delete ( *i );
                }
                throw e;
            }
        }
        return quests;
    }

    static Quest* parseSingle( string line, int lineNumber ) {
        int questId;
        int descriptionId;
        int completionScriptId;
        vector < string > splits = StringUtils::split( line, ',' );
        if ( splits.size() != 4 ) {
            throw ParseException( string( "Invalid quest description line: \"" ) + line + string( "\"" ), lineNumber );
        }
        vector < string > splitsKw = StringUtils::split( splits[ 0 ], ' ' );
        if ( splitsKw.size() != 2 ) {
            throw ParseException( string( "Invalid quest description keyword: \"" ) + splits[ 0 ] + string( "\"" ), lineNumber );
        }
        string literalKeyword = string( splitsKw[ 0 ] );
        StringUtils::trim( literalKeyword );
        if ( strcmp( literalKeyword.c_str(), "QUEST" ) ) {
            throw ParseException( string( "Quest description must start from literal \"QUEST\"" ), lineNumber );
        }
        string questIdString = string( splitsKw[ 1 ] );
        StringUtils::trim( questIdString );
        if ( !StringUtils::toInteger( questIdString.c_str(), &questId ) ) {
            throw ParseException( string( "Invalid quest identifier: \"" ) + questIdString + string( "\"" ), lineNumber );
        }
        string descriptionString = string( splits[ 1 ] );
        StringUtils::trim( descriptionString );
        if ( !StringUtils::toInteger( descriptionString.c_str(), &descriptionId ) ) {
            throw ParseException( string( "Invalid description identifier: \"" ) + descriptionString + string( "\"" ), lineNumber );
        }
        string completionScriptString = string( splits[ 2 ] );
        StringUtils::trim( completionScriptString );
        if ( !StringUtils::toInteger( completionScriptString.c_str(), &completionScriptId ) ) {
            throw ParseException( string( "Invalid script identifier: \"" ) + completionScriptString + string( "\"" ), lineNumber );
        }
        UnlockCondition condition = UnlockCondition::parse( splits[ 3 ], lineNumber );
        Quest* quest = new Quest( questId );
        quest -> _descriptionId = descriptionId;
        quest -> _completionScriptId = completionScriptId;
        quest -> _condition = condition;
        quest -> _completed = false;
        return quest;
    }

    Quest* copyNewQuest() {
        Quest* quest = new Quest( _questId );
        quest -> _descriptionId = _descriptionId;
        quest -> _completionScriptId = _completionScriptId;
        quest -> _condition = _condition;
        quest -> _completed = false;
        return quest;
    }

    inline int getId() {
        return _questId;
    }

    inline int getDescription() {
        return _descriptionId;
    }

    inline void setDescription( int descriptionId ) {
        _descriptionId = descriptionId;
    }

    inline int getCompletionScriptId() {
        return _completionScriptId;
    }

    inline void setCompletionScriptId( int completionScriptId ) {
        _completionScriptId = completionScriptId;
    }

    inline UnlockCondition getUnlockCondition() {
        return _condition;
    }

    inline void setUnlockCondition( UnlockCondition condition ) {
        _condition = condition;
    }

    inline bool isCompleted() {
        return _completed;
    }

    inline void setCompleted() {
        _completed = true;
    }

    private:
    int _questId;
    int _descriptionId;
    int _completionScriptId;
    UnlockCondition _condition;
    bool _completed;
};

