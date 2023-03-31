#pragma once

class EntityType : public InteractiveType {
    public:
    EntityType( int id ) : InteractiveType( id ) {
    }
    virtual ~EntityType() {
    }

    virtual InteractiveObject* createObjectInstance( int assignedId );

    static const int ATTRIBUTE_LOGIC_ID = 0;
    static const int ATTRIBUTE_TEXTURE_ID = 1;
    static const int ATTRIBUTE_STRING_NAME = 100;
    static const int ATTRIBUTE_HP = 101;
    static const int ATTRIBUTE_HP_CURRENT = 102;
    static const int ATTRIBUTE_DAMAGE = 103;
    static const int ATTRIBUTE_SPEED = 104;
    static const int ATTRIBUTE_SPEED_SPRINT = 105;
    static const int ATTRIBUTE_EXP = 106;
    static const int ATTRIBUTE_NEXP = 107;
    static const int ATTRIBUTE_LEVEL = 108;
    static const int ATTRIBUTE_UNASSIGNED_STAT_POINTS = 109;
    static const int ATTRIBUTE_GAINED_EXP = 110;
    static const int ATTRIBUTE_STATIC_ENERGY = 111;
    static const int ATTRIBUTE_HEALTH_BUFFER_UNIT_ENABLED = 112;
    static const int ATTRIBUTE_HEALTH_BUFFER_UNIT_CAPACITY = 113;
    static const int ATTRIBUTE_HEALTH_BUFFER_UNIT_CONSUMPTION_RATIO = 114;
    static const int ATTRIBUTE_HEALTH_BUFFER_UNIT_CONSUMPTION_SPEED = 115;
    static const int ATTRIBUTE_HEALTH_BUFFER_UNIT_LEVEL = 116;

    static const int ATTRIBUTE_COLLISION_RADIUS = 201;
    static const int ATTRIBUTE_SOLDIER_MIN_DIST = 202;
    static const int ATTRIBUTE_SOLDIER_MAX_DIST = 203;
    static const int ATTRIBUTE_DAMAGE_SOUND = 204;
    static const int ATTRIBUTE_DEATH_SOUND = 205;
    static const int DEATH_PARTICLE_COUNT_MIN = 206;
    static const int DEATH_PARTICLE_COUNT_MAX = 207;
    static const int ATTRIBUTE_MV_PARTICLE_IDS = 208;
    static const int ATTRIBUTE_MV_CORPSE_IDS = 209;
    static const int ATTRIBUTE_MV_BLOOD_IDS = 210;
    static const int ATTRIBUTE_MV_BLOOD_SPLAT_IDS = 211;
    static const int ATTRIBUTE_SHOOT_POSITION_X = 212;
    static const int ATTRIBUTE_SHOOT_POSITION_Y = 213;
    static const int ATTRIBUTE_INVENTORY_WEAPON_ID = 214;
    static const int ATTRIBUTE_SOLDIER_FIRE_COUNT_MIN = 215;
    static const int ATTRIBUTE_SOLDIER_FIRE_COUNT_MAX = 216;
    static const int ATTRIBUTE_SOLDIER_HOLD_FIRE_TIME_MIN = 217;
    static const int ATTRIBUTE_SOLDIER_HOLD_FIRE_TIME_MAX = 218;
    static const int ATTRIBUTE_MV_PICKUP_IDS = 219;
    static const int ATTRIBUTE_MV_PICKUP_ID_MINS = 220;
    static const int ATTRIBUTE_MV_PICKUP_ID_MAXS = 221;
    static const int ATTRIBUTE_HP_CURRENT_MAX = 222;
    static const int ATTRIBUTE_VISIBILITY_DISTANCE = 223;
    static const int ATTRIBUTE_EXPLODE_AT_DEATH = 224;
    static const int ATTRIBUTE_AUTO_BURST_THRESHOLD = 225;
    static const int ATTRIBUTE_AUTO_BURST_THRESHOLD_MAX = 226;
    static const int ATTRIBUTE_MELEE_WEAPON_ID = 227;
    static const int ATTRIBUTE_ENERGY = 228;
    static const int ATTRIBUTE_BASE_HP = 229;
    static const int ATTRIBUTE_HP_STAT_MULTIPLIER = 230;
    static const int ATTRIBUTE_HP_PER_LEVEL = 231;
    static const int ATTRIBUTE_BASE_ENERGY = 232;
    static const int ATTRIBUTE_ENE_STAT_MULTIPLIER = 233;
    static const int ATTRIBUTE_ENE_PER_LEVEL = 234;

    static const int ATTRIBUTE_POSITION_X = 1001;
    static const int ATTRIBUTE_POSITION_Y = 1002;
    static const int ATTRIBUTE_POSITION_TARGET_X = 1003;
    static const int ATTRIBUTE_POSITION_TARGET_Y = 1004;
    static const int ATTRIBUTE_WEAPON_SHOOT_NEXT_TIME = 1005;
    static const int ATTRIBUTE_EQUIPPED_WEAPON_ID = 1006;
    static const int ATTRIBUTE_SOLDIER_BEHAVIOR_MODE = 1007;
    static const int ATTRIBUTE_SOLDIER_BEHAVIOR_TIMER = 1008;
    static const int ATTRIBUTE_SOLDIER_BEHAVIOR_BULLET_COUNTER = 1009;
    static const int ATTRIBUTE_SOLDIER_BEHAVIOR_PREDICTED_BULLET_COUNTER = 1010;
    static const int ATTRIBUTE_EQUIPPED_ARMOR_ID = 1011;
    static const int ATTRIBUTE_INVENTORY_OPEN = 1012;
    static const int ATTRIBUTE_INVENTORY_PAGE_NUMBER = 1013;
    static const int ATTRIBUTE_ENERGY_MAX = 1014;
    static const int ATTRIBUTE_ENERGY_CURRENT = 1015;
    static const int ATTRIBUTE_HEAT_MAX = 1016;
    static const int ATTRIBUTE_HEAT_CURRENT = 1017;
    static const int ATTRIBUTE_ENERGY_RESTORE_LEVEL = 1018;
    static const int ATTRIBUTE_ARMS_BROKEN = 1019;
    static const int ATTRIBUTE_ARMS_BROKEN_MODEL_ID = 1020;
    static const int ATTRIBUTE_MV_ARMS_BROKEN_SOUND_ID = 1021;
    static const int ATTRIBUTE_MV_ARMS_BROKEN_DEBRIS_ID = 1022;
    static const int ATTRIBUTE_MV_ARMS_BROKEN_DEBRIS_MIN = 1023;
    static const int ATTRIBUTE_MV_ARMS_BROKEN_DEBRIS_MAX = 1024;
    static const int ATTRIBUTE_ARMS_BROKEN_CHANCE = 1025;
    static const int ATTRIBUTE_ARMS_BROKEN_DAMAGE_THRESHOLD = 1026;
    static const int ATTRIBUTE_ARMS_BROKEN_DEATH_TIMER = 1027;
    static const int ATTRIBUTE_ARMS_BROKEN_PANIC_TIMER = 1028;
    static const int ATTRIBUTE_LAST_MEAN_DAMAGE = 1029;
    static const int ATTRIBUTE_ACTIVE_SKILL = 1030;
    static const int ATTRIBUTE_SKILL_LEVEL = 1031;
    static const int ATTRIBUTE_LEVEL_PROGRESS = 1032;
    static const int ATTRIBUTE_INVENTORY_STORAGE = 1033;
    static const int ATTRIBUTE_ARMS_BROKEN_DIE_SOUND_ID = 1034;
    static const int ATTRIBUTE_ARMS_BROKEN_DIE_TEXTURE_ID = 1035;

    int getLevelProgressForChapter( int chapterId ) {
        map < int, double > progressLevels;
        if ( getMultiValuedParameter( EntityType::ATTRIBUTE_LEVEL_PROGRESS, progressLevels ) ) {
            map < int, double >::iterator i = progressLevels.find( chapterId );
            if ( i != progressLevels.end() ) {
                double level = i -> second;
                if ( !isnan( level ) ) {
                    return int( level );
                }
            }
        }
        return -1;
    }

};


