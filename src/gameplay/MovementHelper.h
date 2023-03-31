#pragma once

class MovementHelper {
    private:
    MovementHelper() {}
    virtual ~MovementHelper() {}
    public:

    static void walkTo( EntityObject* object, Vector2D position, double forwardSpeed ) {
        Vector2D myPosition = object -> getPosition();
        Vector2D movement = Vector2D(
            ( position.x - myPosition.x ),
            ( position.y - myPosition.y )
        );
        double movementDistance = movement.dist2D();
        double speed = object -> getSpeed() * Cache::getTimeLastDifferenceMultiplier();
        if ( movementDistance > 0.0 ) {
            movement.x = movement.x / movementDistance * speed * forwardSpeed;
            movement.y = movement.y / movementDistance * speed * forwardSpeed;
        }
        myPosition = calcPositionMapAndEntityCollision( object, movement.x, movement.y );
        object -> setPosition( myPosition );
    }

    static Vector2D calcPositionMapAndEntityCollision( EntityObject* object, double motionAxisX, double motionAxisY ) {
        Vector2D position = object -> getPosition();
        Vector2D movement = Vector2D( motionAxisX, motionAxisY );
        // entities
	    GameStateHolder* gameStateHolder = Cache::getGameStateHolder();
        GameStateHolder::Container < EntityObject > container = gameStateHolder -> getEntityContainer();
        double myRadius = object -> getCollisionRadius();
        while ( container.hasMore() ) {
            EntityObject* entity = container.nextObject();
            if ( entity != object ) {
                Vector2D entityPosition = entity -> getPosition();
                double otherRadius = entity -> getCollisionRadius();
                double sumRadius = myRadius + otherRadius;
                double entityDistance = position.dist2D( entityPosition );
                if ( entityDistance < sumRadius ) {
                    Vector2D movementE;
                    movementE.x = ( position.x - entityPosition.x );
                    movementE.y = ( position.y - entityPosition.y );
                    double movementDist = movementE.dist2D();
                    if ( movementDist > 0.0 ) {
                        movementE.x /= movementDist;
                        movementE.y /= movementDist;
                        movementE.x *= 1.0 / ( 1.0 + ( movementDist ) ) * 100.0;
                        movementE.y *= 1.0 / ( 1.0 + ( movementDist ) ) * 100.0;
                        movement.x += movementE.x;
                        movement.y += movementE.y;
                    }
                }
            }
        }
        // map
	    Map* globalMap = Cache::getGameStateHolder() -> getMap();
	    MAP_FIELD_TYPE field;
	    Vector2D movementCheck = Vector2D(
            fabs( movement.x + myRadius ) * ( movement.x >= 0.0 ? 1.0 : - 1.0 ),
            fabs( movement.y + myRadius ) * ( movement.y >= 0.0 ? 1.0 : - 1.0 )
        );
	    if ( globalMap -> getField( ( position.x + movementCheck.x ) / Map::BLOCK_SIZE, position.y / Map::BLOCK_SIZE, field ) ) {
            if ( ( Cache::getFileAccessor() -> getBlockInfo( field ) ).isSolid() ) {
                movement.x = 0.0;
            }
	    }
	    if ( globalMap -> getField( position.x / Map::BLOCK_SIZE, ( position.y + movementCheck.y ) / Map::BLOCK_SIZE, field ) ) {
            if ( ( Cache::getFileAccessor() -> getBlockInfo( field ) ).isSolid() ) {
                movement.y = 0.0;
            }
	    }
	    // final
        position.x += movement.x;
        position.y += movement.y;
	    return position;
    }

    static int move( ParticleObject* object, Map* currentMap ) {
        Vector2D movement = object -> getMovement();
        Vector2D position = object -> getPosition();
        double movementSpeed = movement.dist2D();
        movement.scale( Cache::getTimeLastDifferenceMultiplier() );
        bool bounced = false;
        if ( movementSpeed < ParticleObject::PARTICLE_MOVEMENT_STOP_RADIUS ) {
            return ParticleObject::FLAG_DEAD;
        }
        object -> addRotation( object -> getRotationSpeed() * movementSpeed );
        object -> addHeightDerivative( - ParticleObject::HEIGHT_DERV_GRAVITY * Cache::getTimeLastDifferenceMultiplier() );
        object -> addHeight( object -> getHeightDerivative() );
        if ( object -> getHeight() < 0.0 ) {
            object -> setHeight( - object -> getHeight() );
            object -> setHeightDerivative( fabs( object -> getHeightDerivative() ) * ParticleObject::PARTICLE_BOUNCE_FACTOR );
            movement.x *= ParticleObject::PARTICLE_BOUNCE_FACTOR;
            movement.y *= ParticleObject::PARTICLE_BOUNCE_FACTOR;
            bounced = true;
        }
        movement.x *= pow( ParticleObject::PARTICLE_INERTIA, Cache::getTimeLastDifferenceMultiplier() );
        movement.y *= pow( ParticleObject::PARTICLE_INERTIA, Cache::getTimeLastDifferenceMultiplier() );
        MAP_FIELD_TYPE field;
        if ( currentMap -> getField( ( position.x + movement.x ) / Map::BLOCK_SIZE, position.y / Map::BLOCK_SIZE, field ) ) {
            BlockInfo info = Cache::getFileAccessor() -> getBlockInfo( field );
            if ( info.isSolid() && !info.isTransparentBullet() ) {
                movement.x = ( - movement.x );
                bounced = true;
            }
        }
        if ( currentMap -> getField( position.x / Map::BLOCK_SIZE, ( position.y + movement.y ) / Map::BLOCK_SIZE, field ) ) {
            BlockInfo info = Cache::getFileAccessor() -> getBlockInfo( field );
            if ( info.isSolid() && !info.isTransparentBullet() ) {
                movement.y = ( - movement.y );
                bounced = true;
            }
        }
        if ( bounced ) {
            _bounceRotation( object, movementSpeed );
        }
        position.x += movement.x;
        position.y += movement.y;
        object -> setPosition( position );
        movement.scale( 1.0 / Cache::getTimeLastDifferenceMultiplier() );
        object -> setMovement( movement );
        return ( bounced ? ParticleObject::FLAG_BOUNCE : 0 );
    }

    private:

    static inline void _bounceRotation( ParticleObject* object, double changeRotationMultiplier ) {
        double f1 = 10.0 * changeRotationMultiplier;
        double f2 = 5.0 * changeRotationMultiplier;
        object -> addRotation( MathUtils::random() * f1 - f1 / 2.0 );
        object -> setRotationSpeed( MathUtils::random() * f2 - f2 / 2.0 );
    }

};

