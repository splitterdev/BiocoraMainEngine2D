#pragma once

class WeaponObject : public InteractiveObject {
    public:
    WeaponObject( int id, int typeId ) : InteractiveObject( id, typeId ) {
    }
    virtual ~WeaponObject() {
    }
    virtual void setCalculatedAttributes() {
        setCurrentClip( 0 );
        setParameter( WeaponType::ATTRIBUTE_NEXT_SHOOT_AVAILABLE, 0 );
        setParameter( WeaponType::ATTRIBUTE_RELOAD_TIME_CURRENT, 0 );
        setParameter( WeaponType::ATTRIBUTE_UPGRADE_LEVEL, 0 );
    }

    inline int getTextureId() {
        return getParameter( WeaponType::ATTRIBUTE_TEXTURE_ID );
    }

    inline void setTextureId( int textureId ) {
        setParameter( WeaponType::ATTRIBUTE_TEXTURE_ID, textureId );
    }

    inline int getCurrentClip() {
        return getParameter( WeaponType::ATTRIBUTE_CLIP_CURRENT );
    }

    inline void setCurrentClip( int clipSize ) {
        setParameter( WeaponType::ATTRIBUTE_CLIP_CURRENT, clipSize );
    }

    inline int getClipSize() {
        return getParameter( WeaponType::ATTRIBUTE_CLIP_SIZE );
    }

    inline int getInfinityAmmo() {
        return getParameter( WeaponType::ATTRIBUTE_INFINITY_AMMO );
    }

    inline double getCursorAffection() {
        return getParameter( WeaponType::ATTRIBUTE_CURSOR_AFFECTION );
    }

    /// INFO

    inline int getBulletId() {
        return getParameter( WeaponType::ATTRIBUTE_BULLET_ID );
    }

    inline int getBuckSize() {
        return getParameter( WeaponType::ATTRIBUTE_BUCK );
    }

    inline int getVelocity() {
        return getParameter( WeaponType::ATTRIBUTE_BULLET_SPEED );
    }

    inline int getDamageRnd() {
        double dmgMin = getParameter( WeaponType::ATTRIBUTE_DAMAGE_MIN );
        double dmgMax = getParameter( WeaponType::ATTRIBUTE_DAMAGE_MAX );
        return MathUtils::random() * ( dmgMax - dmgMin ) + dmgMin;
    }

    inline int getShootSoundId() {
        return getParameter( WeaponType::ATTRIBUTE_SHOOT_SOUND_ID );
    }

    static const double PRECISION_CALC_RADIUS_DEFAULT = 30.0;

    inline Vector2D getPrecissionScatteredVector( double distanceToTarget ) {
        PolarVector2D pvec;
        pvec.r = MathUtils::random() * getParameter( WeaponType::ATTRIBUTE_PRECISION ) * double( Map::BLOCK_SIZE ) * ( distanceToTarget / PRECISION_CALC_RADIUS_DEFAULT );
        pvec.a = MathUtils::random() * M_PI * 2.0;
        return Vector2D( pvec );
    }

    inline Vector2D getSpreadScatteredVector( double distanceToTarget ) {
        double spread = getParameter( WeaponType::ATTRIBUTE_BULLET_SPREAD );
        if ( isnan( spread ) || spread < 1E-08 ) {
            return Vector2D();
        }
        PolarVector2D pvec;
        pvec.r = MathUtils::random() * spread * double( Map::BLOCK_SIZE ) * ( distanceToTarget / PRECISION_CALC_RADIUS_DEFAULT );
        pvec.a = MathUtils::random() * M_PI * 2.0;
        return Vector2D( pvec );
    }

    inline void createShootQuake( double msCurrentTime ) {
        double quakePower = getParameter( WeaponType::ATTRIBUTE_SHOOT_QUAKE_POWER );
        double quakeDuration = getParameter( WeaponType::ATTRIBUTE_SHOOT_QUAKE_DURATION );
        if ( !isnan( quakePower ) && !isnan( quakeDuration ) ) {
            Camera::createQuake( quakePower, quakeDuration, msCurrentTime );
        }
    }

    inline double getDeathPush() {
        return getParameter( WeaponType::ATTRIBUTE_DEATH_PUSH );
    }

    inline int getFireTextureId() {
        return getParameter( WeaponType::ATTRIBUTE_FIRE_TEXTURE_ID );
    }

    inline double getShootDelay() {
        return getParameter( WeaponType::ATTRIBUTE_DELAY );
    }

    inline double getReloadingCurrentTime() {
        return getParameter( WeaponType::ATTRIBUTE_RELOAD_TIME_CURRENT );
    }

    inline void setReloadingCurrentTime( double currentTime ) {
        setParameter( WeaponType::ATTRIBUTE_RELOAD_TIME_STARTED, currentTime );
        setParameter( WeaponType::ATTRIBUTE_RELOAD_TIME_CURRENT, currentTime + getParameter( WeaponType::ATTRIBUTE_RELOAD_TIME ) * 1000.0 );
    }

    inline double getPercentageReloadingTime( double currentTime ) {
        double initialTime = getParameter( WeaponType::ATTRIBUTE_RELOAD_TIME_STARTED );
        currentTime -= initialTime;
        double reloadingTime = getReloadingCurrentTime() - initialTime;
        return 1.0 - currentTime / reloadingTime;
    }

    inline string getTypeName() {
        double nameId = getParameter( WeaponType::ATTRIBUTE_NAME );
        if ( !isnan( nameId ) ) {
            return LangStrings::getString( ( int ) nameId );
        }
        return "???";
    }

    inline double getEnergyConsumption() {
        double eC = getParameter( WeaponType::ATTRIBUTE_ENERGY_CONSUMPTION );
        if ( isnan( eC ) ) {
            return 0.0;
        }
        return eC;
    }

    inline double getHeatMultiplier() {
        double hM = getParameter( WeaponType::ATTRIBUTE_HEAT_MULTIPLIER );
        if ( isnan( hM ) ) {
            return 1.0;
        }
        return hM;
    }

    bool upgrade() {
        double chance = pow( 1.0 / ( 2.0 + getParameter( WeaponType::ATTRIBUTE_UPGRADE_LEVEL ) ), 0.5 );
        if ( MathUtils::random() < chance ) {
            setParameter( WeaponType::ATTRIBUTE_UPGRADE_LEVEL, getParameter( WeaponType::ATTRIBUTE_UPGRADE_LEVEL ) + 1 );
            _upgradeAttributes();
            return true;
        }
        return false;
    }

    void upgradeNoFail( int levels ) {
        for ( int i = 0; i < levels; i++ ) {
            setParameter( WeaponType::ATTRIBUTE_UPGRADE_LEVEL, getParameter( WeaponType::ATTRIBUTE_UPGRADE_LEVEL ) + 1 );
            _upgradeAttributes();
        }
    }

    void setDamage( double minDamage, double maxDamage ) {
        setParameter( WeaponType::ATTRIBUTE_DAMAGE_MIN, minDamage );
        setParameter( WeaponType::ATTRIBUTE_DAMAGE_MAX, maxDamage );
    }

    private:

    void _upgradeAttributes() {
        /// TODO
    }

};

InteractiveObject* WeaponType::createObjectInstance( int assignedId ) {
        return new WeaponObject( assignedId, getId() );
    }

