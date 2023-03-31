#pragma once

#include "script/custom/ScriptWaitInstruction.h"
#include "script/custom/ScriptMessageManagerInstruction.h"
#include "script/custom/ScriptLoadInstruction.h"
#include "script/custom/ScriptGotoInstruction.h"
#include "script/custom/ScriptPlayerTriggerInstruction.h"
#include "script/custom/ScriptNextLevelInstruction.h"
#include "script/custom/ScriptSkipFrameInstruction.h"
#include "script/custom/ScriptRunAndWaitDialogInstruction.h"
#include "script/custom/ScriptSpawnEntityInstruction.h"
#include "script/custom/ScriptSetSpawnInstruction.h"
#include "script/custom/ScriptPlayerInvertedTriggerInstruction.h"
#include "script/custom/ScriptSetTaskInstruction.h"
#include "script/custom/ScriptSetWaypointIndicatorInstruction.h"
#include "script/custom/ScriptSetWaypointIndicatorEnabledInstruction.h"
#include "script/custom/ScriptSetQuestInstruction.h"
#include "script/custom/ScriptSpawnPickupInstruction.h"
#include "script/custom/ScriptGenerateExplosionInstruction.h"
#include "script/custom/ScriptWhileNotMapFieldStateInstruction.h"
#include "script/custom/ScriptSetMapFieldInstruction.h"
#include "script/custom/ScriptSetAccumValueInstruction.h"
#include "script/custom/ScriptLoadVarInstruction.h"
#include "script/custom/ScriptStoreVarInstruction.h"
#include "script/custom/ScriptPushAccumValueInstruction.h"
#include "script/custom/ScriptPopAccumValueInstruction.h"

class ScriptEngine {
    private:
    ScriptEngine() {}
    virtual ~ScriptEngine() {}
    public:

    static const int SCRIPT_WAIT_KEY = 0;
    static const int SCRIPT_MESSAGE_MANAGER_KEY = 1;
    static const int SCRIPT_LOAD_KEY = 2;
    static const int SCRIPT_GOTO_KEY = 3;
    static const int SCRIPT_PLAYER_TRIGGER_KEY = 4;
    static const int SCRIPT_NEXT_LEVEL = 5;
    static const int SCRIPT_SKIP_FRAME = 6;
    static const int SCRIPT_RUN_AND_WAIT_DIALOG = 7;
    static const int SCRIPT_SPAWN_ENTITY = 8;
    static const int SCRIPT_SET_SPAWN = 9;
    static const int SCRIPT_PLAYER_INVERTED_TRIGGER = 10;
    static const int SCRIPT_SET_TASK = 11;
    static const int SCRIPT_SET_WAYPOINT_INDICATOR = 12;
    static const int SCRIPT_SET_WAYPOINT_INDICATOR_ENABLED = 13;
    static const int SCRIPT_SET_QUEST = 14;
    static const int SCRIPT_SPAWN_PICKUP = 15;
    static const int SCRIPT_GENERATE_EXPLOSION = 16;
    static const int SCRIPT_WHILE_NOT_MAP_FIELD_STATE = 17;
    static const int SCRIPT_SET_MAP_FIELD = 18;
    static const int SCRIPT_SET_ACCUM_VALUE = 19;
    static const int SCRIPT_LOAD_VAR = 20;
    static const int SCRIPT_STORE_VAR = 21;
    static const int SCRIPT_PUSH_ACCUM_VALUE = 22;
    static const int SCRIPT_POP_ACCUM_VALUE = 23;

    /**
        EXAMPLE TO REGISTER NEW FUNCTIONALITY:
        ScriptInstructionFactory::setFactory( 0, new MyScriptInstructionFactory(), 2 );
        EXAMPLE TO REGISTER NEW KEYWORD:
        ScriptParser::setParseKeywordFactoryKey( "KEYWORD", 0 );
    */
    static void Init() {
        // factory setting
        ScriptInstructionFactory::setFactory( SCRIPT_WAIT_KEY, new ScriptWaitInstruction::Factory(), 1 );
        ScriptInstructionFactory::setFactory( SCRIPT_MESSAGE_MANAGER_KEY, new ScriptMessageManagerInstruction::Factory(), 2 );
        ScriptInstructionFactory::setFactory( SCRIPT_LOAD_KEY, new ScriptLoadInstruction::Factory(), 1 );
        ScriptInstructionFactory::setFactory( SCRIPT_GOTO_KEY, new ScriptGotoInstruction::Factory(), 1 );
        ScriptInstructionFactory::setFactory( SCRIPT_PLAYER_TRIGGER_KEY, new ScriptPlayerTriggerInstruction::Factory(), 3 );
        ScriptInstructionFactory::setFactory( SCRIPT_NEXT_LEVEL, new ScriptNextLevelInstruction::Factory(), 0 );
        ScriptInstructionFactory::setFactory( SCRIPT_SKIP_FRAME, new ScriptSkipFrameInstruction::Factory(), 0 );
        ScriptInstructionFactory::setFactory( SCRIPT_RUN_AND_WAIT_DIALOG, new ScriptRunAndWaitDialogInstruction::Factory(), 1 );
        ScriptInstructionFactory::setFactory( SCRIPT_SPAWN_ENTITY, new ScriptSpawnEntityInstruction::Factory(), 3 );
        ScriptInstructionFactory::setFactory( SCRIPT_SET_SPAWN, new ScriptSetSpawnInstruction::Factory(), 1 );
        ScriptInstructionFactory::setFactory( SCRIPT_PLAYER_INVERTED_TRIGGER, new ScriptPlayerInvertedTriggerInstruction::Factory(), 3 );
        ScriptInstructionFactory::setFactory( SCRIPT_SET_TASK, new ScriptSetTaskInstruction::Factory(), 1 );
        ScriptInstructionFactory::setFactory( SCRIPT_SET_WAYPOINT_INDICATOR, new ScriptSetWaypointIndicatorInstruction::Factory(), 2 );
        ScriptInstructionFactory::setFactory( SCRIPT_SET_WAYPOINT_INDICATOR_ENABLED, new ScriptSetWaypointIndicatorEnabledInstruction::Factory(), 1 );
        ScriptInstructionFactory::setFactory( SCRIPT_SET_QUEST, new ScriptSetQuestInstruction::Factory(), 1 );
        ScriptInstructionFactory::setFactory( SCRIPT_SPAWN_PICKUP, new ScriptSpawnPickupInstruction::Factory(), 3 );
        ScriptInstructionFactory::setFactory( SCRIPT_GENERATE_EXPLOSION, new ScriptGenerateExplosionInstruction::Factory(), 3 );
        ScriptInstructionFactory::setFactory( SCRIPT_WHILE_NOT_MAP_FIELD_STATE, new ScriptWhileNotMapFieldStateInstruction::Factory(), 3 );
        ScriptInstructionFactory::setFactory( SCRIPT_SET_MAP_FIELD, new ScriptSetMapFieldInstruction::Factory(), 3 );
        ScriptInstructionFactory::setFactory( SCRIPT_SET_ACCUM_VALUE, new ScriptSetAccumValueInstruction::Factory(), 1 );
        ScriptInstructionFactory::setFactory( SCRIPT_LOAD_VAR, new ScriptLoadVarInstruction::Factory(), 1 );
        ScriptInstructionFactory::setFactory( SCRIPT_STORE_VAR, new ScriptStoreVarInstruction::Factory(), 1 );
        ScriptInstructionFactory::setFactory( SCRIPT_PUSH_ACCUM_VALUE, new ScriptPushAccumValueInstruction::Factory(), 0 );
        ScriptInstructionFactory::setFactory( SCRIPT_POP_ACCUM_VALUE, new ScriptPopAccumValueInstruction::Factory(), 0 );
        // parser setting
        ScriptParser::setParseKeywordFactoryKey( "WAIT", SCRIPT_WAIT_KEY );
        ScriptParser::setParseKeywordFactoryKey( "MESSAGE", SCRIPT_MESSAGE_MANAGER_KEY );
        ScriptParser::setParseKeywordFactoryKey( "LOAD_SCRIPT", SCRIPT_LOAD_KEY );
        ScriptParser::setParseKeywordFactoryKey( "GOTO", SCRIPT_GOTO_KEY );
        ScriptParser::setParseKeywordFactoryKey( "PLAYER_TRIGGER", SCRIPT_PLAYER_TRIGGER_KEY );
        ScriptParser::setParseKeywordFactoryKey( "NEXT_LEVEL", SCRIPT_NEXT_LEVEL );
        ScriptParser::setParseKeywordFactoryKey( "SKIP_FRAME", SCRIPT_SKIP_FRAME );
        ScriptParser::setParseKeywordFactoryKey( "RUN_AND_WAIT_DIALOG", SCRIPT_RUN_AND_WAIT_DIALOG );
        ScriptParser::setParseKeywordFactoryKey( "SPAWN_ENTITY", SCRIPT_SPAWN_ENTITY );
        ScriptParser::setParseKeywordFactoryKey( "SET_SPAWN", SCRIPT_SET_SPAWN );
        ScriptParser::setParseKeywordFactoryKey( "PLAYER_INVERTED_TRIGGER", SCRIPT_PLAYER_INVERTED_TRIGGER );
        ScriptParser::setParseKeywordFactoryKey( "SET_TASK", SCRIPT_SET_TASK );
        ScriptParser::setParseKeywordFactoryKey( "SET_WAYPOINT_INDICATOR", SCRIPT_SET_WAYPOINT_INDICATOR );
        ScriptParser::setParseKeywordFactoryKey( "SET_WAYPOINT_INDICATOR_ENABLED", SCRIPT_SET_WAYPOINT_INDICATOR_ENABLED );
        ScriptParser::setParseKeywordFactoryKey( "SET_QUEST", SCRIPT_SET_QUEST );
        ScriptParser::setParseKeywordFactoryKey( "SPAWN_PICKUP", SCRIPT_SPAWN_PICKUP );
        ScriptParser::setParseKeywordFactoryKey( "GENERATE_EXPLOSION", SCRIPT_GENERATE_EXPLOSION );
        ScriptParser::setParseKeywordFactoryKey( "WHILE_NOT_MAP_FIELD_STATE", SCRIPT_WHILE_NOT_MAP_FIELD_STATE );
        ScriptParser::setParseKeywordFactoryKey( "SET_MAP_FIELD", SCRIPT_SET_MAP_FIELD );
        ScriptParser::setParseKeywordFactoryKey( "SET_ACCUM_VALUE", SCRIPT_STORE_VAR );
        ScriptParser::setParseKeywordFactoryKey( "LOAD_VAR", SCRIPT_LOAD_VAR );
        ScriptParser::setParseKeywordFactoryKey( "STORE_VAR", SCRIPT_STORE_VAR );
        ScriptParser::setParseKeywordFactoryKey( "PUSH_ACCUM_VALUE", SCRIPT_PUSH_ACCUM_VALUE );
        ScriptParser::setParseKeywordFactoryKey( "POP_ACCUM_VALUE", SCRIPT_POP_ACCUM_VALUE );
    }

};


