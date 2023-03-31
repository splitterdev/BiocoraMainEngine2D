#pragma once

class SoldierLogic : public Logic {
    public:
    SoldierLogic() : Logic() {
    }
    virtual ~SoldierLogic() {
    }

    static const int BEHAVIOR_MODE_SHOOT = 0;
    static const int BEHAVIOR_MODE_WAIT = 1;

    LogicCallResult onCall() {
        EntityObject* caller = getCaller();
        AttackHelper::weaponIdleTick( caller );
        EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
        if ( player ) {
            Vector2D playerPosition = player -> getPosition();
            Vector2D myPosition = caller -> getPosition();
            double attrArmBroken = caller -> getParameter( EntityType::ATTRIBUTE_ARMS_BROKEN );
            if ( attrArmBroken > 0.5 && !isnan( attrArmBroken ) ) {
                // PANIC
                double kickMul = 2.5;
                Vector2D target = caller -> getTargetPosition();
                double changeTargetTime = caller -> getParameter( EntityType::ATTRIBUTE_ARMS_BROKEN_PANIC_TIMER );
                if ( myPosition.dist2D( target ) < caller -> getSpeed() * kickMul || changeTargetTime < Cache::gameTime() || isnan( changeTargetTime ) ) {
                    PolarVector2D newPositionOffset = PolarVector2D();
                    newPositionOffset.r = MathUtils::random() * double( Map::BLOCK_SIZE ) * 4.5 + 2.0;
                    newPositionOffset.a = MathUtils::random() * M_PI * 2.0;
                    Vector2D targetOffset = Vector2D( newPositionOffset );
                    target.x = myPosition.x + targetOffset.x;
                    target.y = myPosition.y + targetOffset.y;
                    caller -> setTargetPosition( target );
                    caller -> setParameter( EntityType::ATTRIBUTE_ARMS_BROKEN_PANIC_TIMER, Cache::gameTime() + 300.0 );
                        //myPosition.dist2D( target ) / ( caller -> getSpeed() * Map::BLOCK_SIZE ) )
                }
                double bleedTimer = caller -> getParameter( EntityType::ATTRIBUTE_ARMS_BROKEN_DEATH_TIMER );
                if ( bleedTimer < Cache::gameTime() || isnan( bleedTimer ) ) {
                    TrashHelper::dumpTrash(
                        Cache::getGameStateHolder() -> getMap(),
                        myPosition.x,
                        myPosition.y,
                        Cache::getFileAccessor() -> getTexture( caller -> getBloodTexture() ),
                        MathUtils::random() * 360.0,
                        1,
                        true
                    );
                    double nextBleedTime = MathUtils::random() * 350.0 + 150.0;
                    //printf( "Logic: bleed timer. Next afted: %d[ms]\n", ( int ) nextBleedTime );
                    caller -> setParameter( EntityType::ATTRIBUTE_ARMS_BROKEN_DEATH_TIMER, Cache::gameTime() + nextBleedTime );
                    double bleedDamageMultiplier = MathUtils::random() * 0.04 + 0.03;
                    caller -> setParameter( EntityType::ATTRIBUTE_HP_CURRENT,
                        caller -> getParameter( EntityType::ATTRIBUTE_HP_CURRENT ) - caller -> getParameter( EntityType::ATTRIBUTE_HP_CURRENT_MAX ) * bleedDamageMultiplier
                    );
                    if ( caller -> getParameter( EntityType::ATTRIBUTE_HP_CURRENT ) <= 0.0 ) {
                        //printf( "Logic: die. Hp: %d\n", ( int ) caller -> getParameter( EntityType::ATTRIBUTE_HP_CURRENT ) );
                        /// TODO death texture & sound (thud)
                        int soundId = caller -> getArmsBrokenDeathSoundId();
                        int textureId = caller -> getArmsBrokenDeathTextureId();
                        Sound* deathSound = Cache::getFileAccessor() -> getSound( soundId );
                        if ( deathSound ) {
                            deathSound -> play();
                        }
                        target.x -= myPosition.x;
                        target.y -= myPosition.y;
                        PolarVector2D deathDumpDir = PolarVector2D( target );
                        deathDumpDir.r = Map::BLOCK_SIZE;
                        Vector2D deathDumpDirCart = Vector2D( deathDumpDir );
                        TrashHelper::dumpTrash(
                            Cache::getGameStateHolder() -> getMap(),
                            myPosition.x + deathDumpDirCart.x,
                            myPosition.y + deathDumpDirCart.y,
                            Cache::getFileAccessor() -> getTexture( textureId ),
                            deathDumpDir.a * 180.0 / ( 2.0 * M_PI ),
                            1,
                            true
                        );
                        return LogicCallResult::suicide();
                    }
                }
                MovementHelper::walkTo( caller, target, kickMul );
            } else {
                caller -> setTargetPosition( playerPosition );
                double distance = playerPosition.dist2D( myPosition );
                if ( distance < caller -> getSoldierMinDist() ) {
                    MovementHelper::walkTo( caller, playerPosition, - 1.0 );
                } else if ( distance > caller -> getSoldierMaxDist() ) {
                    MovementHelper::walkTo( caller, playerPosition, 1.0 );
                }
                // Try shoot
                int behaviorMode = caller -> getSoldierBehaviorMode();
                if ( behaviorMode == BEHAVIOR_MODE_SHOOT ) {
                    int bulletCount = caller -> getSoldierBehaviorBulletCounter();
                    int bulletCountPredicted = caller -> getSoldierBehaviorPredictedBulletCounter();
                    //printf( "Bullets: %d/%d\n", bulletCount, bulletCountPredicted );
                    if ( distance < caller -> getVisibilityDistance() ) {
                        if ( AttackHelper::tryShoot( caller, playerPosition ) ) {
                            bulletCount++;
                        }
                        if ( bulletCount >= bulletCountPredicted ) {
                            caller -> setSoldierBehaviorMode( BEHAVIOR_MODE_WAIT );
                            caller -> setNextHoldFireTime( Cache::gameTime() );
                        } else {
                            caller -> setSoldierBehaviorBulletCounter( bulletCount );
                        }
                    }
                } else if ( behaviorMode == BEHAVIOR_MODE_WAIT ) {
                    if ( Cache::gameTime() >= caller -> getSoldierBehaviorTimer() ) {
                        caller -> setSoldierBehaviorMode( BEHAVIOR_MODE_SHOOT );
                        caller -> setNextBulletCount();
                        caller -> setSoldierBehaviorBulletCounter( 0 );
                    }
                }
            }
        }
        return LogicCallResult::Empty();
    }

};

