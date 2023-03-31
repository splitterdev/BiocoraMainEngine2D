#pragma once

class SkillType : public InteractiveType {
    public:
    SkillType( int id ) : InteractiveType( id ) {
    }
    virtual ~SkillType() {
    }

    static const int ATTRIBUTE_PLACEMENT_TYPE = 0;
    static const int ATTRIBUTE_SKILL_TYPE = 1;
    static const int ATTRIBUTE_BASE_DAMAGE_MIN = 2;
    static const int ATTRIBUTE_BASE_DAMAGE_MAX = 3;
    static const int ATTRIBUTE_BASE_ENERGY_COST = 4;
    static const int ATTRIBUTE_BASE_ENERGY_USAGE = 5;
    static const int ATTRIBUTE_BASE_DAMAGE_LEVEL_MUL = 6;
    static const int ATTRIBUTE_BASE_DAMAGE_LEVEL_MUL_TYPE = 7;
    static const int ATTRIBUTE_BASE_ENERGY_COST_CURVE = 8;
    static const int ATTRIBUTE_BASE_ENERGY_USAGE_MUL = 9;
    static const int ATTRIBUTE_NAME = 10;
    static const int ATTRIBUTE_EXPLOSION_TYPE_ID = 11;
    static const int ATTRIBUTE_DURATION = 12;
    static const int ATTRIBUTE_TEXTURE_UI_ID = 13;

    inline int getPlacementType() {
        return getParameter( ATTRIBUTE_PLACEMENT_TYPE );
    }

    inline int getSkillType() {
        return getParameter( ATTRIBUTE_SKILL_TYPE );
    }

    inline double getDamageForSkillLevel( int level ) { // level - for later use
        /// TODO skill leveling?
        double minDamage = getParameter( ATTRIBUTE_BASE_DAMAGE_MIN );
        double maxDamage = getParameter( ATTRIBUTE_BASE_DAMAGE_MAX );
        double dmgMul = getParameter( ATTRIBUTE_BASE_DAMAGE_LEVEL_MUL );
        int dmgMulType = getParameter( ATTRIBUTE_BASE_DAMAGE_LEVEL_MUL_TYPE );
        if ( dmgMulType == 0 ) {
            dmgMul *= ( level - 1 );
        }
        if ( dmgMulType == 1 ) {
            dmgMul *= level * ( level - 1 );
        }
        minDamage += minDamage * dmgMul;
        maxDamage += maxDamage * dmgMul;
        return MathUtils::random() * ( maxDamage - minDamage ) + minDamage;
    }

    inline double getEnergyCost( int level ) {
        double baseCost = getParameter( ATTRIBUTE_BASE_ENERGY_COST );
        double baseCostCurve = getParameter( ATTRIBUTE_BASE_ENERGY_COST_CURVE );
        return pow( baseCostCurve, level - 1 ) * baseCost;
    }

    inline double getEnergyUsage( int level ) {
        double baseUsage = getParameter( ATTRIBUTE_BASE_ENERGY_USAGE );
        double baseUsageMul = getParameter( ATTRIBUTE_BASE_ENERGY_USAGE_MUL );
        return baseUsage + baseUsageMul * ( level - 1 );
    }

    inline int getNameId() {
        return getParameter( ATTRIBUTE_NAME );
    }

    inline int getExplosionTypeId() {
        return getParameter( ATTRIBUTE_EXPLOSION_TYPE_ID );
    }

    inline int getTextureId() {
        return getParameter( ATTRIBUTE_TEXTURE_UI_ID );
    }

    inline double getDuration() {
        return getParameter( ATTRIBUTE_DURATION );
    }

    virtual InteractiveObject* createObjectInstance( int assignedId ) {
        return NULL;
    }
};

