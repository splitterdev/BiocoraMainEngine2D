#pragma once

class BulletObject : public InteractiveObject {
    public:
    BulletObject( int id, int typeId ) : InteractiveObject( id, typeId ) {
        _removed = false;
    }
    virtual ~BulletObject() {
    }
    virtual void setCalculatedAttributes() {
        setMovement( Vector2D() );
    }

    inline int getMotherEntity() {
        return getParameter( BulletType::ATTRIBUTE_MOTHER_ENTITY );
    }

    inline void setMotherEntity( int entityId ) {
        setParameter( BulletType::ATTRIBUTE_MOTHER_ENTITY, entityId );
    }

    inline string getTypeName() {
        double nameId = getParameter( BulletType::ATTRIBUTE_NAME );
        if ( !isnan( nameId ) ) {
            return LangStrings::getString( ( int ) nameId );
        }
        return "???";
    }

    inline int getTextureId() {
        return getParameter( BulletType::ATTRIBUTE_TEXTURE_ID );
    }

    inline int getInventoryTextureId() {
        return getParameter( BulletType::ATTRIBUTE_INVENTORY_TEXTURE_ID );
    }

    inline double getMaxCapacity() {
        return getParameter( BulletType::ATTRIBUTE_MAX_CAPACITY );
    }

    inline double getCost() {
        return getParameter( BulletType::ATTRIBUTE_COST );
    }

    inline double getBoxSize() {
        return getParameter( BulletType::ATTRIBUTE_BOX_SIZE );
    }

    inline double getDamage() {
        return getParameter( BulletType::ATTRIBUTE_DAMAGE );
    }

    inline void setDamage( double damage ) {
        setParameter( BulletType::ATTRIBUTE_DAMAGE, damage );
    }

    inline double getVelocity() {
        return getParameter( BulletType::ATTRIBUTE_VELOCITY );
    }

    inline void setVelocity( double velocity ) {
        setParameter( BulletType::ATTRIBUTE_VELOCITY, velocity );
    }

    inline Vector2D getTarget() {
        return Vector2D(
            getParameter( BulletType::ATTRIBUTE_TARGET_X ),
            getParameter( BulletType::ATTRIBUTE_TARGET_Y )
        );
    }

    inline void setTarget( Vector2D target ) {
        setParameter( BulletType::ATTRIBUTE_TARGET_X, target.x );
        setParameter( BulletType::ATTRIBUTE_TARGET_Y, target.y );
    }

    inline Vector2D getPosition() {
        return Vector2D(
            getParameter( BulletType::ATTRIBUTE_POSITION_X ),
            getParameter( BulletType::ATTRIBUTE_POSITION_Y )
        );
    }

    inline void setPosition( Vector2D position ) {
        setParameter( BulletType::ATTRIBUTE_POSITION_X, position.x );
        setParameter( BulletType::ATTRIBUTE_POSITION_Y, position.y );
    }

    inline Vector2D getLastPosition() {
        return Vector2D(
            getParameter( BulletType::ATTRIBUTE_LAST_POSITION_X ),
            getParameter( BulletType::ATTRIBUTE_LAST_POSITION_Y )
        );
    }

    inline void updateLastPosition() {
        setParameter( BulletType::ATTRIBUTE_LAST_POSITION_X, getParameter( BulletType::ATTRIBUTE_POSITION_X ) );
        setParameter( BulletType::ATTRIBUTE_LAST_POSITION_Y, getParameter( BulletType::ATTRIBUTE_POSITION_Y ) );
    }

    inline Vector2D getMovement() {
        return Vector2D(
            getParameter( BulletType::ATTRIBUTE_MOVEMENT_X ),
            getParameter( BulletType::ATTRIBUTE_MOVEMENT_Y )
        );
    }

    inline void setMovement( Vector2D movement ) {
        setParameter( BulletType::ATTRIBUTE_MOVEMENT_X, movement.x );
        setParameter( BulletType::ATTRIBUTE_MOVEMENT_Y, movement.y );
    }

    inline bool canIgnoreWalls() {
        return ( getParameter( BulletType::ATTRIBUTE_IGNORE_WALLS ) > 0.5 );
    }

    inline bool isMaterial() {
        return ( getParameter( BulletType::ATTRIBUTE_IS_MATERIAL ) > 0.5 );
    }

    inline bool isInTransparentMaterial() {
        double pValue = getParameter( BulletType::ATTRIBUTE_IS_IN_TRANSPARENT_MATERIAL );
        return !isnan( pValue ) && pValue > 0.5;
    }

    inline void setInTransparentMaterial( bool value ) {
        setParameter( BulletType::ATTRIBUTE_IS_IN_TRANSPARENT_MATERIAL, value ? 1.0 : 0.0 );
    }

    static const double AUTO_AIM_INERTIA = 0.01;

    inline void move( double speedMultiplier, bool autoAim, Vector2D autoTarget ) {
        Vector2D position = getPosition();
        Vector2D movement = getMovement();
        double velocity = getVelocity();
        if ( autoAim ) {
            if ( movement.dist2D() < 1E-12 ) {
                Vector2D target = getTarget();
                movement.x = target.x - position.x;
                movement.y = target.y - position.y;
                double distanceToTarget = movement.dist2D();
                if ( distanceToTarget > 0.0 ) {
                    double scaleMovementFactor = speedMultiplier * velocity / distanceToTarget;
                    movement.x *= scaleMovementFactor;
                    movement.y *= scaleMovementFactor;
                }
            }
            Vector2D targetMovement;
            targetMovement.x = autoTarget.x - position.x;
            targetMovement.y = autoTarget.y - position.y;
            double distanceToTarget = targetMovement.dist2D();
            if ( distanceToTarget > 0.0 ) {
                double scaleMovementFactor = speedMultiplier * velocity / distanceToTarget;
                targetMovement.x *= scaleMovementFactor;
                targetMovement.y *= scaleMovementFactor;
            }
            movement = movement.balance( targetMovement, AUTO_AIM_INERTIA );
        } else {
            Vector2D target = getTarget();
            movement.x = target.x - position.x;
            movement.y = target.y - position.y;
            double distanceToTarget = movement.dist2D();
            if ( distanceToTarget > 0.0 ) {
                double scaleMovementFactor = speedMultiplier * velocity / distanceToTarget;
                movement.x *= scaleMovementFactor;
                movement.y *= scaleMovementFactor;
            }
        }
        position.x += movement.x;
        position.y += movement.y;
        setPosition( position );
        setMovement( movement );
    }

    void occlude( double occlusionValue ) {
        Vector2D position = getPosition();
        Vector2D target = getTarget();
        //printf( "BEFORE Occlude = %lg / target %g, %g\n", occlusionValue, target.x, target.y );
        target.x -= position.x;
        target.y -= position.y;
        PolarVector2D targetDir = PolarVector2D( target );
        targetDir.a += occlusionValue;
        target = Vector2D( targetDir );
        target.x += position.x;
        target.y += position.y;
        setTarget( target );
        target = getTarget();
        //printf( "AFTER Occlude = %lg / target %g, %g\n", occlusionValue, target.x, target.y );
    }

    inline int getCorpseTextureId() {
        map < int, double > values;
        if ( getMultiValuedParameter( BulletType::ATTRIBUTE_CORPSE_TEXTURE_ID, values ) ) {
            return MathUtils::randomValueFromMap( values );
        }
        return -1;
    }

    inline double getDeathPush() {
        return getParameter( BulletType::ATTRIBUTE_DEATH_PUSH );
    }

    inline void setDeathPush( double deathPush ) {
        setParameter( BulletType::ATTRIBUTE_DEATH_PUSH, deathPush );
    }

    inline double getColorR() {
        return getParameter( BulletType::ATTRIBUTE_COLOR_R );
    }

    inline double getColorG() {
        return getParameter( BulletType::ATTRIBUTE_COLOR_G );
    }

    inline double getColorB() {
        return getParameter( BulletType::ATTRIBUTE_COLOR_B );
    }

    inline double getColorA() {
        return getParameter( BulletType::ATTRIBUTE_COLOR_A );
    }

    inline int getExplosionOnDeathId() {
        return getParameter( BulletType::ATTRIBUTE_EXPLOSION_ON_DEATH_ID );
    }

    inline void setRemoved() {
        _removed = true;
    }

    inline bool getRemoved() {
        return _removed;
    }

    inline list < Vector2D > getTrace() {
        return _trace;
    }

    static const double CHECKER_STEP = 5.0;

    inline void addTrace( Vector2D trace ) {
        _trace.push_back( trace );
        if ( _trace.size() >= getParameter( BulletType::ATTRIBUTE_VELOCITY ) / CHECKER_STEP * 2.0 ) {
            _trace.pop_front();
        }
    }

    inline bool isAutoAim() {
        return ( getParameter( BulletType::ATTRIBUTE_AUTO_AIM ) > 0.5 );
    }

    void addTravelDistance( double travelDistance ) {
        double tDistNow = getParameter( BulletType::ATTRIBUTE_TRAVEL_DISTANCE );
        if ( isnan( tDistNow ) ) {
            tDistNow = 0.0;
        }
        setParameter( BulletType::ATTRIBUTE_TRAVEL_DISTANCE, tDistNow + travelDistance );
    }

    bool isRangeExceeded() {
        double range = getParameter( BulletType::ATTRIBUTE_RANGE );
        if ( range < 0.0 ) {
            return false;
        }
        range *= double( Map::BLOCK_SIZE );
        double distTravel = getParameter( BulletType::ATTRIBUTE_TRAVEL_DISTANCE );
        if ( isnan( distTravel ) ) {
            distTravel = 0.0;
        }
        return distTravel >= range;
    }

    private:

    bool _removed;

    list < Vector2D > _trace;

};

InteractiveObject* BulletType::createObjectInstance( int assignedId ) {
        return new BulletObject( assignedId, getId() );
    }
