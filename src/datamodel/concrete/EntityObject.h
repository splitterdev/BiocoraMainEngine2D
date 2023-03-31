#pragma once

#include "EntityType.h"
#include "../utils/Inventory.h"

class EntityObject : public InteractiveObject {
    public:
    EntityObject( int id, int typeId ) : InteractiveObject( id, typeId ) {
        _inventory = new Inventory();
        _meleeWeapon = NULL;
    }
    virtual ~EntityObject() {
        delete _inventory;
        if ( _meleeWeapon ) {
            delete _meleeWeapon;
        }
    }
    virtual void setCalculatedAttributes() {
        setParameter( EntityType::ATTRIBUTE_HP_CURRENT_MAX, getParameter( EntityType::ATTRIBUTE_HP ) );
        setParameter( EntityType::ATTRIBUTE_HP_CURRENT, getParameter( EntityType::ATTRIBUTE_HP_CURRENT_MAX ) );
        setParameter( EntityType::ATTRIBUTE_WEAPON_SHOOT_NEXT_TIME, 0.0 );
        setParameter( EntityType::ATTRIBUTE_EQUIPPED_WEAPON_ID, 0 );
        setParameter( EntityType::ATTRIBUTE_EQUIPPED_ARMOR_ID, 0 );
        setParameter( EntityType::ATTRIBUTE_INVENTORY_OPEN, 0 );
        setParameter( EntityType::ATTRIBUTE_INVENTORY_PAGE_NUMBER, 0 );
        setParameter( EntityType::ATTRIBUTE_ENERGY_MAX, getParameter( EntityType::ATTRIBUTE_ENERGY ) );
        setParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT, getParameter( EntityType::ATTRIBUTE_ENERGY_MAX ) );
        setParameter( EntityType::ATTRIBUTE_HEAT_CURRENT, 0 );
        setParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_LEVEL , getParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_CAPACITY ) );
        setParameter( EntityType::ATTRIBUTE_LAST_MEAN_DAMAGE, 0 );
        setSoldierBehaviorBulletCounter( 0 );
        setSoldierBehaviorMode( 0 );
    }

    // Constant info

    inline string getName() {
        return getStringParameter( EntityType::ATTRIBUTE_STRING_NAME );
    }

    inline int getLogicId() {
        return getParameter( EntityType::ATTRIBUTE_LOGIC_ID );
    }

    inline int getTextureId() {
        return getParameter( EntityType::ATTRIBUTE_TEXTURE_ID );
    }

    inline double getCollisionRadius() {
        return getParameter( EntityType::ATTRIBUTE_COLLISION_RADIUS ) * Map::BLOCK_SIZE;
    }

    inline double getSoldierMinDist() {
        return getParameter( EntityType::ATTRIBUTE_SOLDIER_MIN_DIST ) * Map::BLOCK_SIZE;
    }

    inline double getSoldierMaxDist() {
        return getParameter( EntityType::ATTRIBUTE_SOLDIER_MAX_DIST ) * Map::BLOCK_SIZE;
    }

    inline int getDamageSoundId() {
        map < int, double > values;
        if ( getMultiValuedParameter( EntityType::ATTRIBUTE_DAMAGE_SOUND, values ) ) {
            return _randomFromMap( values );
        }
        return -1;
    }

    inline int getArmBrokenSoundId() {
        map < int, double > values;
        if ( getMultiValuedParameter( EntityType::ATTRIBUTE_MV_ARMS_BROKEN_SOUND_ID, values ) ) {
            return _randomFromMap( values );
        }
        return -1;
    }

    inline int getDeathSoundId() {
        map < int, double > values;
        if ( getMultiValuedParameter( EntityType::ATTRIBUTE_DEATH_SOUND, values ) ) {
            return _randomFromMap( values );
        }
        return -1;
    }

    inline double getDeathParticleCountMin( int particleId ) {
        double value;
        if ( getValueFromMultiValued( EntityType::DEATH_PARTICLE_COUNT_MIN, particleId, value ) ) {
            return value;
        }
        return NAN;
    }

    inline double getDeathParticleCountMax( int particleId ) {
        double value;
        if ( getValueFromMultiValued( EntityType::DEATH_PARTICLE_COUNT_MAX, particleId, value ) ) {
            return value;
        }
        return NAN;
    }

    inline double getDeathParticleCount( int particleId ) {
        double cMin = getDeathParticleCountMin( particleId );
        double cMax = getDeathParticleCountMax( particleId );
        if ( cMax >= cMin ) {
            return cMin + rand() % int( 1.0 + cMax - cMin );
        }
        return cMin;
    }

    inline bool getParticleIds( map < int, double > & ret ) {
        return getMultiValuedParameter( EntityType::ATTRIBUTE_MV_PARTICLE_IDS, ret );
    }

    inline double getArmParticleCountMin( int particleId ) {
        double value;
        if ( getValueFromMultiValued( EntityType::ATTRIBUTE_MV_ARMS_BROKEN_DEBRIS_MIN, particleId, value ) ) {
            return value;
        }
        return NAN;
    }

    inline double getArmParticleCountMax( int particleId ) {
        double value;
        if ( getValueFromMultiValued( EntityType::ATTRIBUTE_MV_ARMS_BROKEN_DEBRIS_MAX, particleId, value ) ) {
            return value;
        }
        return NAN;
    }

    inline double getArmParticleCount( int particleId ) {
        double cMin = getDeathParticleCountMin( particleId );
        double cMax = getDeathParticleCountMax( particleId );
        if ( cMax >= cMin ) {
            return cMin + rand() % int( 1.0 + cMax - cMin );
        }
        return cMin;
    }

    inline bool getArmParticleIds( map < int, double > & ret ) {
        return getMultiValuedParameter( EntityType::ATTRIBUTE_MV_ARMS_BROKEN_DEBRIS_ID, ret );
    }

    inline int getDeathTexture() {
        map < int, double > values;
        if ( getMultiValuedParameter( EntityType::ATTRIBUTE_MV_CORPSE_IDS, values ) ) {
            return _randomFromMap( values );
        }
        return -1;
    }

    inline int getBloodTexture() {
        map < int, double > values;
        if ( getMultiValuedParameter( EntityType::ATTRIBUTE_MV_BLOOD_IDS, values ) ) {
            return _randomFromMap( values );
        }
        return -1;
    }

    inline int getBloodSplatTexture() {
        map < int, double > values;
        if ( getMultiValuedParameter( EntityType::ATTRIBUTE_MV_BLOOD_SPLAT_IDS, values ) ) {
            return _randomFromMap( values );
        }
        return -1;
    }

    inline Vector2D getShootPosition() {
        return Vector2D(
            getParameter( EntityType::ATTRIBUTE_SHOOT_POSITION_X ),
            getParameter( EntityType::ATTRIBUTE_SHOOT_POSITION_Y )
        );
    }

    inline int getInventoryWeaponId() {
        return getParameter( EntityType::ATTRIBUTE_INVENTORY_WEAPON_ID );
    }

    inline void setNextHoldFireTime( double currentTime ) {
        double mN = getParameter( EntityType::ATTRIBUTE_SOLDIER_HOLD_FIRE_TIME_MIN );
        double mX = getParameter( EntityType::ATTRIBUTE_SOLDIER_HOLD_FIRE_TIME_MAX );
        double value = currentTime + ( MathUtils::random() * ( mX - mN ) + mN ) * 1000.0;
        setParameter( EntityType::ATTRIBUTE_SOLDIER_BEHAVIOR_TIMER, value );
    }

    inline void setNextBulletCount() {
        double mN = getParameter( EntityType::ATTRIBUTE_SOLDIER_FIRE_COUNT_MIN );
        double mX = getParameter( EntityType::ATTRIBUTE_SOLDIER_FIRE_COUNT_MAX );
        int value = int( MathUtils::random() * ( mX - mN ) + mN );
        setSoldierBehaviorPredictedBulletCounter( value );
    }

    inline int getSoldierBehaviorBulletCounter() {
        return getParameter( EntityType::ATTRIBUTE_SOLDIER_BEHAVIOR_BULLET_COUNTER );
    }

    inline int getSoldierBehaviorPredictedBulletCounter() {
        return getParameter( EntityType::ATTRIBUTE_SOLDIER_BEHAVIOR_PREDICTED_BULLET_COUNTER );
    }

    inline int getSoldierBehaviorMode() {
        return getParameter( EntityType::ATTRIBUTE_SOLDIER_BEHAVIOR_MODE );
    }

    inline double getSoldierBehaviorTimer() {
        return getParameter( EntityType::ATTRIBUTE_SOLDIER_BEHAVIOR_TIMER );
    }

    inline void setSoldierBehaviorBulletCounter( int count ) {
        setParameter( EntityType::ATTRIBUTE_SOLDIER_BEHAVIOR_BULLET_COUNTER, count );
    }

    inline void setSoldierBehaviorPredictedBulletCounter( int count ) {
        setParameter( EntityType::ATTRIBUTE_SOLDIER_BEHAVIOR_PREDICTED_BULLET_COUNTER, count );
    }

    inline void setSoldierBehaviorMode( int mode ) {
        setParameter( EntityType::ATTRIBUTE_SOLDIER_BEHAVIOR_MODE, mode );
    }

    inline double getVisibilityDistance() {
        return getParameter( EntityType::ATTRIBUTE_VISIBILITY_DISTANCE ) * double( Map::BLOCK_SIZE );
    }

    inline double getExplodeAtDeath() {
        return getParameter( EntityType::ATTRIBUTE_EXPLODE_AT_DEATH );
    }

    map < int, int > getDropMap() {
        map < int, double > pickupIds;
        map < int, int > returnMap;
        if ( getMultiValuedParameter( EntityType::ATTRIBUTE_MV_PICKUP_IDS, pickupIds ) ) {
            for ( map < int, double >::iterator i = pickupIds.begin(); i != pickupIds.end(); i++ ) {
                int pickupId = i -> first;
                if ( MathUtils::random() < i -> second ) {
                    int pickupCount = 0;
                    map < int, int >::iterator iP = returnMap.find( pickupId );
                    if ( iP != returnMap.end() ) {
                        pickupCount = iP -> second;
                    }
                    pickupCount += getPickupRandomCount( pickupId );
                    returnMap[ pickupId ] = pickupCount;
                }
            }
        }
        return returnMap;
    }

    int getPickupRandomCount( int pickupId ) {
        int dropMinCount = getDropMinCountFor( pickupId );
        int dropMaxCount = getDropMaxCountFor( pickupId );
        return ( MathUtils::random() * ( dropMaxCount - dropMinCount ) ) + dropMinCount;
    }

    int getDropMinCountFor( int pickupId ) {
        map < int, double > values;
        if ( getMultiValuedParameter( EntityType::ATTRIBUTE_MV_PICKUP_ID_MINS, values ) ) {
            return _fromMap( values, pickupId );
        }
        return 1;
    }

    int getDropMaxCountFor( int pickupId ) {
        map < int, double > values;
        if ( getMultiValuedParameter( EntityType::ATTRIBUTE_MV_PICKUP_ID_MAXS, values ) ) {
            return _fromMap( values, pickupId );
        }
        return 1;
    }

    // Stat parameters

    bool addExperience( double expValue ) {
        double nextLevelExp = getExperienceRequired();
        if ( nextLevelExp < 1E-12 ) {
            return false;
        }
        double currentExp = getExperience();
        currentExp += expValue;
        double currentLevel = getLevel();
        double statUnassignedPoints = getUnassignedStatPoints();
        bool advanced = false;
        while ( currentExp >= nextLevelExp ) {
            currentExp -= nextLevelExp;
            nextLevelExp *= 1.2;
            currentLevel += 1.0;
            statUnassignedPoints += 5.0;
            advanced = true;
        }
        setLevel( currentLevel );
        setExperience( currentExp );
        setExperienceRequired( nextLevelExp );
        setUnassignedStatPoints( statUnassignedPoints );
        if ( advanced ) {
            setCurrentHp( getCurrentMaxHp() );
        }
        printf( "Player | Level %d (%d / %d) | Remaining %d points\n", int( currentLevel ), int( currentExp ), int( nextLevelExp ), int( statUnassignedPoints ) );
        return advanced;
    }

    inline double getExperience() {
        return getParameter( EntityType::ATTRIBUTE_EXP );
    }

    inline void setExperience( double value ) {
        setParameter( EntityType::ATTRIBUTE_EXP, value );
    }

    inline double getExperienceRequired() {
        return getParameter( EntityType::ATTRIBUTE_NEXP );
    }

    inline void setExperienceRequired( double value ) {
        setParameter( EntityType::ATTRIBUTE_NEXP, value );
    }

    inline double getLevel() {
        return getParameter( EntityType::ATTRIBUTE_LEVEL );
    }

    inline void setLevel( double value ) {
        setParameter( EntityType::ATTRIBUTE_LEVEL, value );
    }

    inline double getUnassignedStatPoints() {
        return getParameter( EntityType::ATTRIBUTE_UNASSIGNED_STAT_POINTS );
    }

    inline void setUnassignedStatPoints( double value ) {
        setParameter( EntityType::ATTRIBUTE_UNASSIGNED_STAT_POINTS, value );
    }

    inline double getGainedExp() {
        return getParameter( EntityType::ATTRIBUTE_GAINED_EXP );
    }

    inline double getMotionSpeed( bool isSprint ) {
        if ( isSprint ) {
            return Map::BLOCK_SIZE * getParameter( EntityType::ATTRIBUTE_SPEED_SPRINT ) / 60.0;
        }
        return getSpeed();
    }

    inline double getSpeed() {
        return Map::BLOCK_SIZE * getParameter( EntityType::ATTRIBUTE_SPEED ) / 60.0;
    }

    inline double getEnergy() {
        return getParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT );
    }

    inline double getEnergyPercentageLevel() {
        return getParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT ) / getParameter( EntityType::ATTRIBUTE_ENERGY_MAX );
    }

    inline double getHeatLevel() {
        return getParameter( EntityType::ATTRIBUTE_HEAT_CURRENT );
    }

    // Utility variables

    inline Vector2D getPosition() {
        return Vector2D(
            getParameter( EntityType::ATTRIBUTE_POSITION_X ),
            getParameter( EntityType::ATTRIBUTE_POSITION_Y )
        );
    }

    inline void setPosition( double x, double y ) {
        setParameter( EntityType::ATTRIBUTE_POSITION_X, x );
        setParameter( EntityType::ATTRIBUTE_POSITION_Y, y );
    }

    inline void setPosition( Vector2D position ) {
        setPosition( position.x, position.y );
    }

    inline Vector2D getTargetPosition() {
        return Vector2D(
            getParameter( EntityType::ATTRIBUTE_POSITION_TARGET_X ),
            getParameter( EntityType::ATTRIBUTE_POSITION_TARGET_Y )
        );
    }

    inline void setTargetPosition( double x, double y ) {
        setParameter( EntityType::ATTRIBUTE_POSITION_TARGET_X, x );
        setParameter( EntityType::ATTRIBUTE_POSITION_TARGET_Y, y );
    }

    inline void setTargetPosition( Vector2D position ) {
        setTargetPosition( position.x, position.y );
    }

    inline double getCurrentHp() {
        return getParameter( EntityType::ATTRIBUTE_HP_CURRENT );
    }

    inline double getCurrentMaxHp() {
        return getParameter( EntityType::ATTRIBUTE_HP_CURRENT_MAX );
    }

    inline void setCurrentHp( double currentHp ) {
        setParameter( EntityType::ATTRIBUTE_HP_CURRENT, currentHp );
    }

    inline int getEquippedWeaponId() {
        return getParameter( EntityType::ATTRIBUTE_EQUIPPED_WEAPON_ID );
    }

    inline void setEquippedWeaponId( int weaponId ) {
        setParameter( EntityType::ATTRIBUTE_EQUIPPED_WEAPON_ID, weaponId );
    }

    inline int getEquippedArmorId() {
        return getParameter( EntityType::ATTRIBUTE_EQUIPPED_ARMOR_ID );
    }

    inline void setEquippedArmorId( int armorId ) {
        setParameter( EntityType::ATTRIBUTE_EQUIPPED_ARMOR_ID, armorId );
    }

    inline bool inventoryEnabled() {
        double invOpen = getParameter( EntityType::ATTRIBUTE_INVENTORY_OPEN );
        return ( !isnan( invOpen ) && ( invOpen > 0.5 ) );
    }

    inline double getAutoBurstThreshold() {
        return getParameter( EntityType::ATTRIBUTE_AUTO_BURST_THRESHOLD );
    }

    inline double getAutoBurstThresholdMax() {
        return getParameter( EntityType::ATTRIBUTE_AUTO_BURST_THRESHOLD_MAX );
    }

    inline double getStaticEnergy() {
        return getParameter( EntityType::ATTRIBUTE_STATIC_ENERGY );
    }

    inline int getSkillLevel( int skillId ) {
        map < int, double > skillLevels;
        if ( getMultiValuedParameter( EntityType::ATTRIBUTE_SKILL_LEVEL, skillLevels ) ) {
            map < int, double >::iterator i = skillLevels.find( skillId );
            if ( i != skillLevels.end() ) {
                double level = i -> second;
                if ( !isnan( level ) ) {
                    return int( level );
                }
            }
        }
        return 0;
    }

    inline int getArmsBrokenDeathTextureId() {
        return getParameter( EntityType::ATTRIBUTE_ARMS_BROKEN_DIE_TEXTURE_ID );
    }

    inline int getArmsBrokenDeathSoundId() {
        return getParameter( EntityType::ATTRIBUTE_ARMS_BROKEN_DIE_SOUND_ID );
    }

    // Operations

    double damage( double dmgValue ) {
        double hp = getCurrentHp();
        hp -= dmgValue;
        setCurrentHp( hp );
        return hp;
    }

    inline double distanceTo( EntityObject* object ) {
        Vector2D pos = getPosition();
        Vector2D otherPos = object -> getPosition();
        return pos.dist2D( otherPos );
    }

    inline bool payEnergy( double amount ) {
        double currEnergy = getParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT );
        if ( ( amount > currEnergy ) || ( currEnergy < -1E-12 ) ) {
            return false;
        }
        setParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT, currEnergy - amount );
        return true;
    }

    inline void restoreEnergy( double multiplier ) {
        double currEnergy = getParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT );
        double restoredEnergy = getParameter( EntityType::ATTRIBUTE_ENERGY_RESTORE_LEVEL ) * multiplier;
        setParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT, min( currEnergy + restoredEnergy, getParameter( EntityType::ATTRIBUTE_ENERGY_MAX ) ) );
    }

    inline void dissipateHeat( double multiplier ) {
        static const double DISSIPATION_HEAT_MUL = 0.001;
        static const double DISSIPATION_HEAT_MAX = 0.05;
        double currentHeat = getParameter( EntityType::ATTRIBUTE_HEAT_CURRENT );
        double heatDissipated = min( DISSIPATION_HEAT_MAX, currentHeat * DISSIPATION_HEAT_MUL );
        setParameter( EntityType::ATTRIBUTE_HEAT_CURRENT, currentHeat - max( 0.0, heatDissipated * multiplier ) );
    }

    inline void addHeat( double heat ) {
        setParameter( EntityType::ATTRIBUTE_HEAT_CURRENT, getParameter( EntityType::ATTRIBUTE_HEAT_CURRENT ) + heat );
    }

    inline Inventory* getInventory() {
        return _inventory;
    }

    inline void addStaticEnergy( double amount ) {
        setParameter( EntityType::ATTRIBUTE_STATIC_ENERGY, getParameter( EntityType::ATTRIBUTE_STATIC_ENERGY ) + amount );
    }

    inline bool payStaticEnergy( double amount ) {
        double staticEnergy = getParameter( EntityType::ATTRIBUTE_STATIC_ENERGY );
        if ( staticEnergy >= amount ) {
            setParameter( EntityType::ATTRIBUTE_STATIC_ENERGY, staticEnergy - amount );
            return true;
        }
        return false;
    }

    inline void toggleHealthBuffer() {
        double enabled = getParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_ENABLED );
        if ( enabled > 0.5 ) {
            setParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_ENABLED, 0.0 );
        } else {
            setParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_ENABLED, 1.0 );
        }
    }

    inline bool isHealthBufferEnabled() {
        return getParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_ENABLED ) > 0.5;
    }

    void refillHealthBuffer( double multiplier ) {
        if ( isHealthBufferEnabled() ) {
            // check and adjust
            double minRefillHBU = 0.25;
            double energyLevel = getParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT );
            double energyPercentageLevel = energyLevel / getParameter( EntityType::ATTRIBUTE_ENERGY_MAX );
            if ( energyPercentageLevel < minRefillHBU ) {
                return; // cannot refill, energy under safe threshold
            }
            double energyConsumption = getParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_CONSUMPTION_SPEED ) * multiplier;
            if ( energyConsumption > energyLevel ) {
                energyConsumption = energyLevel;
            }
            double hbuEnergyLevel = getParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_LEVEL );
            double hbuEnergyCapacity = getParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_CAPACITY );
            double hbuRequiredEnergy = hbuEnergyCapacity - hbuEnergyLevel;
            double energyConsumptionRatio = getParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_CONSUMPTION_RATIO );
            double hbuRestoredEnergyLevel = energyConsumption * energyConsumptionRatio;
            if ( hbuRestoredEnergyLevel > hbuRequiredEnergy ) {
                hbuRestoredEnergyLevel = hbuRequiredEnergy;
                energyConsumption = hbuRestoredEnergyLevel / energyConsumptionRatio;
            }
            // refill
            setParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT, energyLevel - energyConsumption );
            setParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_LEVEL, hbuEnergyLevel + hbuRestoredEnergyLevel );
        }
    }

    int tryUseHealthBuffer() {
        double maxHp = getParameter( EntityType::ATTRIBUTE_HP_CURRENT_MAX );
        double currentHp = getParameter( EntityType::ATTRIBUTE_HP_CURRENT );
        double requiredHp = maxHp - currentHp;
        double hbuEnergyLevel = getParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_LEVEL );
        if ( hbuEnergyLevel < requiredHp ) {
            requiredHp = hbuEnergyLevel;
        }
        if ( hbuEnergyLevel > 1E-09 ) {
            setParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_LEVEL, hbuEnergyLevel - requiredHp );
            setParameter( EntityType::ATTRIBUTE_HP_CURRENT, currentHp + requiredHp );
            return requiredHp;
        }
        return -1;
    }

    int convertEnergyStaticToDynamic( double fillRate ) {
        double energyLevel = getParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT );
        double energyRequired = getParameter( EntityType::ATTRIBUTE_ENERGY_MAX ) - energyLevel;
        double limit = fillRate * getParameter( EntityType::ATTRIBUTE_ENERGY_MAX );
        if ( energyRequired > limit ) {
            energyRequired = limit;
        }
        if ( energyRequired > 1E-09 ) {
            double staticEnergyLevel = getParameter( EntityType::ATTRIBUTE_STATIC_ENERGY );
            if ( staticEnergyLevel < 1E-09 ) {
                return -1;
            }
            if ( staticEnergyLevel < energyRequired ) {
                energyRequired = staticEnergyLevel;
            }
            setParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT, energyLevel + energyRequired );
            setParameter( EntityType::ATTRIBUTE_STATIC_ENERGY, staticEnergyLevel - energyRequired );
            return 1;
        }
        return 0;
    }

    bool breakArms() {
        double attArmsBroken = getParameter( EntityType::ATTRIBUTE_ARMS_BROKEN );
        if ( attArmsBroken < 0.5 || isnan( attArmsBroken ) ) {
            setParameter( EntityType::ATTRIBUTE_ARMS_BROKEN, 1.0 );
            setParameter( EntityType::ATTRIBUTE_TEXTURE_ID, getParameter( EntityType::ATTRIBUTE_ARMS_BROKEN_MODEL_ID ) );
            setTargetPosition( getPosition() );
            return true;
        }
        return false;
    }

    void setNextLevelProgress( int chapterId, int levelId ) {
        map < int, double > progressLevels;
        if ( !getMultiValuedParameter( EntityType::ATTRIBUTE_LEVEL_PROGRESS, progressLevels ) ) {
            progressLevels[ chapterId ] = levelId;
            setMultiValuedParameter( EntityType::ATTRIBUTE_LEVEL_PROGRESS, progressLevels );
        } else {
            map < int, double >::iterator i = progressLevels.find( chapterId );
            if ( i != progressLevels.end() ) {
                double level = i -> second;
                if ( !isnan( level ) && levelId > int( level ) ) {
                    progressLevels[ chapterId ] = levelId;
                    setMultiValuedParameter( EntityType::ATTRIBUTE_LEVEL_PROGRESS, progressLevels );
                }
            }
        }
    }

    inline InteractiveObject* getMeleeWeapon() {
        return _meleeWeapon;
    }

    inline void setMeleeWeapon( InteractiveObject* weapon ) {
        if ( _meleeWeapon ) {
            delete _meleeWeapon;
        }
        _meleeWeapon = weapon;
    }

    private:

    inline static double _randomFromMap( map < int, double > & m ) {
        map < int, double >::iterator i = m.begin();
        if ( i == m.end() ) {
            return NAN;
        }
        int r = rand() % m.size();
        for ( int j = 0; j < r; j++ ) {
            i++;
        }
        return i -> second;
    }

    inline static double _fromMap( map < int, double > & m, int keyId ) {
        map < int, double >::iterator i = m.find( keyId );
        if ( i != m.end() ) {
            return i -> second;
        }
        return NAN;
    }

    Inventory* _inventory;
    InteractiveObject* _meleeWeapon;

};

InteractiveObject* EntityType::createObjectInstance( int assignedId ) {
        return new EntityObject( assignedId, getId() );
    }

