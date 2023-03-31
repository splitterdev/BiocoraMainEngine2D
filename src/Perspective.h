#pragma once

const int FORM_MAIN_MENU = 1;
const int FORM_PLAY_MENU = 2;
const int FORM_NEW_GAME_MENU = 3;
const int FORM_SELECT_LEVEL_MENU = 4;
const int FORM_PROFILES_MENU = 5;
const int FORM_NEW_PROFILE_MENU = 6;
const int FORM_SELECT_PROFILE_MENU = 7;
const int FORM_DELETE_PROFILE_MENU = 8;
const int FORM_CONFIRM_DELETE_PROFILE_MENU = 9;
const int FORM_SETTINGS_MENU = 10;
const int FORM_STATS_MENU = 11;
const int FORM_INVENTORY_MENU = 12;
const int FORM_BUY_ITEMS_STORE_MENU = 13;
const int FORM_OPTIONS_MENU = 14;
const int FORM_CALIBRATION_MENU = 15;
const int FORM_SELECT_CHAPTER_MENU = 16;
const int FORM_EXIT_CONFIRMATION_MENU = 1000;
const int INGAME_FORM_INVENTORY = 1001;

#include "perspective/Perspective.h"

#include "ui/Cursor.h"
#include "ui/MenuCursor.h"
#include "gameplay/AttackHelper.h"
#include "gameplay/SkillHelper.h"
#include "ui/AutoAimCursor.h"

#include "gameplay/CustomLogic.h"
#include "gameplay/GamePlayRobots.h"
#include "gameplay/GameStateProcessorSchemaBuilder.h"
#include "gameplay/ScriptEngine.h"

