#pragma once

class AttackHelper {
    private:
    AttackHelper() {}
    virtual ~AttackHelper() {}
    public:

    static const int AUTOBURST_EXPLOSION_ID = 2;

    static bool canShoot( EntityObject* object, double delay ) {
        WeaponObject* weapon = InventoryHelper::currentWeaponOf( object );
        if ( weapon -> getClipSize() > 0 ) {
            if ( !isnan( weapon -> getReloadingCurrentTime() ) || ( weapon -> getCurrentClip() <= 0 ) ) {
                return false;
            }
        }
        /*if ( ( InventoryHelper::getAmmoState( object, weapon -> getBulletId() ) <= 0 ) && ( weapon -> getClipSize() <= 0 ) ) {
            return false;
        }*/
        double t = object -> getParameter( EntityType::ATTRIBUTE_WEAPON_SHOOT_NEXT_TIME );
        double tCurrent = Cache::gameTime();
        //printf( "T: %g / %g\n", t, tCurrent );
        if ( tCurrent >= t ) {
            object -> setParameter( EntityType::ATTRIBUTE_WEAPON_SHOOT_NEXT_TIME, tCurrent + delay );
            return true;
        }
        return false;
    }

    static bool tryShoot( EntityObject* object, Vector2D bulletTarget ) {
        WeaponObject* currentWeapon = InventoryHelper::currentWeaponOf( object );
        if ( currentWeapon ) {
            if ( canShoot( object, currentWeapon -> getShootDelay() * 1000.0 ) ) {
                bool isPlayer = ( object == Cache::getGameStateHolder() -> getPlayer() );
                if ( isPlayer ) {
                    double energyMultiplier = 1.0 + log10( 1.0 + object -> getHeatLevel() );
                    if ( object -> payEnergy( currentWeapon -> getEnergyConsumption() * energyMultiplier ) ) {
                        shoot( Cache::getGameStateHolder(), object -> getId(), bulletTarget );
                        return true;
                    }
                } else {
                    shoot( Cache::getGameStateHolder(), object -> getId(), bulletTarget );
                    return true;
                }
            }
        }
        return false;
    }

    static void shoot( GameStateHolder* holder, int entityId, Vector2D target ) {
        EntityObject* entity = holder -> getEntity( entityId );
        if ( entity ) {
            WeaponObject* weapon = InventoryHelper::currentWeaponOf( entity );
            if ( weapon ) {
                BulletType* bulletType = Cache::getFileAccessor() -> getBulletType( weapon -> getBulletId() );
                Vector2D position = entity -> getPosition();
                Texture* entityTexture = Cache::getFileAccessor() -> getTexture( entity -> getTextureId() );
                if ( entityTexture ) {
                    Vector2D shootPosition = entity -> getShootPosition();
                    shootPosition.x -= ( entityTexture -> getWidth() >> 1 );
                    shootPosition.y -= ( entityTexture -> getHeight() >> 1 );
                    PolarVector2D shootOffset = PolarVector2D( shootPosition );
                    Vector2D dirC = position;
                    dirC.x -= target.x;
                    dirC.y -= target.y;
                    PolarVector2D dir = PolarVector2D( dirC );
                    shootOffset.a += dir.a;
                    Vector2D shootOffsetC = Vector2D( shootOffset );
                    position.x += shootOffsetC.x;
                    position.y += shootOffsetC.y;
                }
                double distanceToTarget = position.dist2D( target ) / double( Map::BLOCK_SIZE );
                if ( bulletType ) {
                    int buck = weapon -> getBuckSize();
                    Vector2D baseScatter = weapon -> getPrecissionScatteredVector( distanceToTarget * ( 1.0 + Cursor::getAffection() * 3.0 ) );
                    for ( int i = 0; i < buck; i++ ) {
                        BulletObject* bullet = bulletType -> createObject < BulletObject > ( holder -> nextBulletId() );
                        bullet -> setMotherEntity( entityId );
                        bullet -> setPosition( position );
                        Vector2D scatter = weapon -> getSpreadScatteredVector( distanceToTarget );
                        scatter.x += target.x + baseScatter.x;
                        scatter.y += target.y + baseScatter.y;
                        bullet -> setTarget( scatter );
                        bullet -> setVelocity( weapon -> getVelocity() * ( MathUtils::random() * 0.3 + 0.85 ) );
                        bullet -> setDamage( weapon -> getDamageRnd() );
                        bullet -> setDeathPush( weapon -> getDeathPush() );
                        bullet -> updateLastPosition();
                        holder -> add( bullet );
                    }
                    int clip = weapon -> getCurrentClip();
                    clip--;
                    if ( clip <= 0 ) {
                        weapon -> setReloadingCurrentTime( Cache::gameTime() );
                    }
                    weapon -> setCurrentClip( clip );
                    Sound* shootSound = Cache::getFileAccessor() -> getSound( weapon -> getShootSoundId() );
                    if ( shootSound ) {
                        shootSound -> play();
                    }
                    // Create quake
                    weapon -> createShootQuake( Cache::gameTime() );
                    // Create fire effect
                    int fireTextureId = weapon -> getFireTextureId();
                    TimedDisplay::createRegular( fireTextureId, position, MathUtils::random() * 360.0, MathUtils::random() * 0.5 + 0.75, 60 );
                    // Cursor affection
                    if ( entity == holder -> getPlayer() ) {
                        Cursor::addAffection( weapon -> getCursorAffection() );
                    }
                    // Heat management
                    if ( entity == holder -> getPlayer() ) {
                        double heatValue = weapon -> getHeatMultiplier() * ( pow( 1.0 + weapon -> getEnergyConsumption(), 0.5 ) - 1.0 );
                        entity -> addHeat( heatValue );
                        double totalHeatLevel = entity -> getHeatLevel();
                        double autoBurstThreshold = entity -> getAutoBurstThreshold();
                        double autoBurstThresholdMax = entity -> getAutoBurstThresholdMax();
                        if ( totalHeatLevel >= autoBurstThreshold ) {
                            double burstOverheat = totalHeatLevel - autoBurstThreshold;
                            double burstOverheatDiff = autoBurstThresholdMax - autoBurstThreshold;
                            //printf( "totalHeatLevel = %lf, autoBurstThreshold = %lf, autoBurstThresholdMax = %lf\n", totalHeatLevel, autoBurstThreshold, autoBurstThresholdMax );
                            if ( MathUtils::random() < pow( burstOverheat / burstOverheatDiff, 1.5 ) ) {
                                MessageManager::get() -> addMessage( MessageManager::LEVEL_ERROR, LangStrings::getString( LangStringsID::MESSAGE_LOG_AUTOBURST ) );
                                double energyLoss = MathUtils::random() * 0.05 + 0.05;
                                double energyUnitsDissipated = min( entity -> getParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT ) * energyLoss, 500.0 );
                                entity -> setParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT, entity -> getParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT ) - energyUnitsDissipated );
                                double heatBurstDissipated = ( MathUtils::random() * 0.3 + 1.2 ) * ( energyLoss * burstOverheatDiff );
                                entity -> addHeat( - heatBurstDissipated );
                                createExplosion( AUTOBURST_EXPLOSION_ID, position );
                            }
                        }
                    }
                }
            }
        }
    }

    static bool performDamage( EntityObject* object, double dmgValue, Vector2D deathPush ) {
        double entityHp = object -> damage( dmgValue );
        object -> setParameter( EntityType::ATTRIBUTE_LAST_MEAN_DAMAGE, object -> getParameter( EntityType::ATTRIBUTE_LAST_MEAN_DAMAGE ) + dmgValue );
        Vector2D entityPosition = object -> getPosition();
        GameStateHolder* gameStateHolder = Cache::getGameStateHolder();
        // Create splat
        int splatTextureId = object -> getBloodSplatTexture();
        Vector2D splatPosition = entityPosition;
        PolarVector2D splatOffset;
        splatOffset.r = MathUtils::random() * object -> getCollisionRadius();
        splatOffset.a = MathUtils::random() * M_PI * 2.0;
        Vector2D splatOffsetC = Vector2D( splatOffset );
        splatPosition.x += splatOffsetC.x;
        splatPosition.y += splatOffsetC.y;
        TimedDisplay::createRegular( splatTextureId, splatPosition, MathUtils::random() * 360.0, MathUtils::random() * 0.4 + 0.8, MathUtils::random() * 100 + 50 );
        // Check if dead
        if ( entityHp < 0.0 ) {
            Sound* sound = Cache::getFileAccessor() -> getSound( object -> getDeathSoundId() );
            if ( sound ) {
                sound -> play();
            }
            // Create particles
            map < int, double > particleIds;
            object -> getParticleIds( particleIds );
            for ( map < int, double >::iterator i = particleIds.begin(); i != particleIds.end(); i++ ) {
                int particleFindId = i -> first;
                int particleId = i -> second;
                int particleCount = object -> getDeathParticleCount( particleFindId );
                for ( int j = 0; j < particleCount; j++ ) {
                    ParticleObject* particle = ParticleCreator::newParticle( particleId, entityPosition, deathPush, 16.0, 60.0 );
                    if ( particle ) {
                        gameStateHolder -> add( particle );
                    }
                }
            }
            // Create corpse
            TrashHelper::dumpTrash(
                gameStateHolder -> getMap(),
                entityPosition.x,
                entityPosition.y,
                Cache::getFileAccessor() -> getTexture( object -> getDeathTexture() ),
                MathUtils::random() * 360.0,
                1,
                true
            );
            // Create pickups
            map < int, int > pickupCounters = object -> getDropMap();
            for ( map < int, int >::iterator i = pickupCounters.begin(); i != pickupCounters.end(); i++ ) {
                int pickupId = i -> first;
                int pickupCount = i -> second;
                for ( int j = 0; j < pickupCount; j++ ) {
                    PolarVector2D positionOffset;
                    positionOffset.r = MathUtils::random() * object -> getCollisionRadius();
                    positionOffset.a = MathUtils::random() * M_PI * 2.0;
                    Vector2D randomizedPosition = Vector2D( positionOffset );
                    randomizedPosition.x += entityPosition.x;
                    randomizedPosition.y += entityPosition.y;
                    PickupObject* pickup = PickupCreator::newPickup( pickupId, randomizedPosition );
                    if ( pickup ) {
                        gameStateHolder -> add( pickup );
                    }
                }
            }
            // Create explosion at death
            double deathExplosionId = object -> getExplodeAtDeath();
            if ( !isnan( deathExplosionId ) ) {
                createExplosion( deathExplosionId, entityPosition );
            }
            if ( object == Cache::getGameStateHolder() -> getPlayer() ) {
                MessageManager::get() -> addMessage( MessageManager::LEVEL_FATAL, LangStrings::getString( LangStringsID::MESSAGE_LOG_DEATH ) );
            }
            return true;
        } else {
            // damage - still alive
            // check for arms break
            double chance = object -> getParameter( EntityType::ATTRIBUTE_ARMS_BROKEN_CHANCE );
            double breakHpThreshold = object -> getParameter( EntityType::ATTRIBUTE_ARMS_BROKEN_DAMAGE_THRESHOLD );
            double attArmsBroken = object -> getParameter( EntityType::ATTRIBUTE_ARMS_BROKEN );
            bool armsBroken = !( isnan( attArmsBroken ) || attArmsBroken < 0.5 );
            if ( ( MathUtils::random() < chance ) && ( object -> getParameter( EntityType::ATTRIBUTE_LAST_MEAN_DAMAGE ) >= breakHpThreshold * object -> getCurrentMaxHp() ) && ( object != Cache::getGameStateHolder() -> getPlayer() ) ) {
                armsBroken = object -> breakArms();
                if ( armsBroken ) {
                    int soundId = object -> getArmBrokenSoundId();
                    Sound* sound = Cache::getFileAccessor() -> getSound( soundId );
                    if ( sound ) {
                        sound -> play();
                    }
                    map < int, double > particleIds;
                    object -> getArmParticleIds( particleIds );
                    for ( map < int, double >::iterator i = particleIds.begin(); i != particleIds.end(); i++ ) {
                        int particleFindId = i -> first;
                        int particleId = i -> second;
                        int particleCount = object -> getArmParticleCount( particleFindId );
                        for ( int j = 0; j < particleCount; j++ ) {
                            ParticleObject* particle = ParticleCreator::newParticle( particleId, entityPosition, deathPush, 16.0, 60.0 );
                            if ( particle ) {
                                gameStateHolder -> add( particle );
                            }
                        }
                    }
                }
            }
            if ( !armsBroken ) {
                // casual damage
                Sound* sound = Cache::getFileAccessor() -> getSound( object -> getDamageSoundId() );
                if ( sound ) {
                    sound -> play();
                }
            }
            TrashHelper::dumpTrash(
                gameStateHolder -> getMap(),
                entityPosition.x,
                entityPosition.y,
                Cache::getFileAccessor() -> getTexture( object -> getBloodTexture() ),
                MathUtils::random() * 360.0,
                1,
                true
            );
            if ( object == Cache::getGameStateHolder() -> getPlayer() ) {
                double hpLevel = object -> getCurrentHp() / object -> getCurrentMaxHp();
                if ( hpLevel < 0.25 ) {
                    if ( MathUtils::random() < 0.3 ) {
                        MessageManager::get() -> addMessage( MessageManager::LEVEL_ERROR, LangStrings::getString( LangStringsID::MESSAGE_LOG_DAMAGE_LOW_HP ) );
                    }
                } else if ( hpLevel < 0.66 ) {
                    if ( MathUtils::random() < 0.25 ) {
                        MessageManager::get() -> addMessage( MessageManager::LEVEL_WARN, LangStrings::getString( LangStringsID::MESSAGE_LOG_DAMAGE_MEDIUM_HP ) );
                    }
                } else {
                    if ( MathUtils::random() < 0.1 ) {
                        MessageManager::get() -> addMessage( MessageManager::LEVEL_NOTICE, LangStrings::getString( LangStringsID::MESSAGE_LOG_DAMAGE_HIGH_HP ) );
                    }
                }
            }
        }
        return false;
    }

    static EntityObject* getNearestEntityToPosition( Vector2D position, EntityObject* autoSkip, double range ) {
        GameStateHolder::Container < EntityObject > entities = Cache::getGameStateHolder() -> getEntityContainer();
        EntityObject* nearest = NULL;
        double nearestDist = range;
        while ( entities.hasMore() ) {
            EntityObject* object = entities.nextObject();
            if ( object && ( object != autoSkip ) ) {
                double dist = Vector2D( object -> getPosition() ).dist2D( position );
                if ( dist <= nearestDist ) {
                    nearest = object;
                    nearestDist = dist;
                }
            }
        }
        return nearest;
    }

    private:

    class _ExplosionEventDelegate : public EventDelegate {
        public:
        _ExplosionEventDelegate( int explosionTypeIdI, Vector2D positionI, double customDamageI, EntityObject* launcherI ) : EventDelegate() {
            explosionTypeId = explosionTypeIdI;
            position = positionI;
            customDamage = customDamageI;
            launcher = launcherI;
            customTimeMs = NAN;
            removeTimeMs = NAN;
            nextAttackTime = 0.0;
            attackTimeInterval = 200.0;
            isStarted = false;
        }
        ~_ExplosionEventDelegate() {
        }
        int explosionTypeId;
        Vector2D position;
        double customDamage;
        EntityObject* launcher;
        double customTimeMs;
        double removeTimeMs;
        double nextAttackTime;
        double attackTimeInterval;
        bool isStarted;
    };

    static const int _EVENT_QUEUE_ID = 1;
    static EventDelegateQueue* _EDQ;

    public:

    static void createExplosion( int explosionTypeId, Vector2D position ) {
        EventDelegate* delegate = new _ExplosionEventDelegate( explosionTypeId, position, NAN, NULL );
        if ( !_EDQ -> addEvent( _EVENT_QUEUE_ID, delegate ) ) {
            delete delegate;
        }
    }

    static void createExplosion( int explosionTypeId, Vector2D position, double customDamage ) {
        EventDelegate* delegate = new _ExplosionEventDelegate( explosionTypeId, position, customDamage, NULL );
        if ( !_EDQ -> addEvent( _EVENT_QUEUE_ID, delegate ) ) {
            delete delegate;
        }
    }

    static void createExplosion( int explosionTypeId, Vector2D position, double customDamage, EntityObject* launcher ) {
        EventDelegate* delegate = new _ExplosionEventDelegate( explosionTypeId, position, customDamage, launcher );
        if ( !_EDQ -> addEvent( _EVENT_QUEUE_ID, delegate ) ) {
            delete delegate;
        }
    }

    static void createField( int explosionTypeId, Vector2D position, double customDamage, EntityObject* launcher, double customTimeMs ) {
        _ExplosionEventDelegate* exEvD = new _ExplosionEventDelegate( explosionTypeId, position, customDamage, launcher );
        exEvD -> customTimeMs = customTimeMs;
        if ( !_EDQ -> addEvent( _EVENT_QUEUE_ID, exEvD ) ) {
            delete exEvD;
        }
    }

    static void processExplosionTasks() {
        EventDelegate* delegate;
        list < EventDelegate* > edList;
        while ( ( delegate = _EDQ -> getEvent( _EVENT_QUEUE_ID ) ) != NULL ) {
            bool removeDelegate = true;
            _ExplosionEventDelegate* exEvD = dynamic_cast < _ExplosionEventDelegate* > ( delegate );
            if ( exEvD ) {
                bool doDamage = ( !exEvD -> isStarted );
                if ( !exEvD -> isStarted ) {
                    exEvD -> removeTimeMs = exEvD -> customTimeMs + Cache::gameTime();
                    exEvD -> nextAttackTime = Cache::gameTime();
                }
                if ( exEvD -> nextAttackTime <= Cache::gameTime() ) {
                    doDamage = true;
                    exEvD -> nextAttackTime += exEvD -> attackTimeInterval;
                }
                ExplosionType* type = Cache::getFileAccessor() -> getExplosionType( exEvD -> explosionTypeId );
                if ( type ) {
                    Vector2D position = exEvD -> position;
                    EntityObject* launcher = exEvD -> launcher;
                    double damage = type -> getDamage();
                    if ( !isnan( exEvD -> customDamage ) ) {
                        damage = exEvD -> customDamage;
                    }
                    if ( !isnan( exEvD -> customTimeMs ) ) {
                        damage *= ( exEvD -> attackTimeInterval / 1000.0 );
                    }
                    double range = type -> getRange();
                    double duration = type -> getDuration();
                    if ( !isnan( exEvD -> customTimeMs ) ) {
                        duration = exEvD -> customTimeMs;
                    }
                    double quakePower = type -> getQuakePower();
                    int loadType = type -> getLoadType();
                    if ( !exEvD -> isStarted ) {
                        Sound* sound = Cache::getFileAccessor() -> getSound( type -> getSoundId() );
                        if ( sound ) {
                            sound -> play();
                        }
                        Camera::createQuake( quakePower, duration, Cache::gameTime() );
                        if ( loadType == TimedDisplay::LOAD_TYPE_REGULAR ) {
                            int textureId = type -> getTextureId();
                            Texture* texture = Cache::getFileAccessor() -> getTexture( textureId );
                            double scale = 1.0;
                            if ( texture ) {
                                double pixelWidth = texture -> getWidth();
                                double pixelHeight = texture -> getHeight();
                                double pixelScaleOrig = ( pixelWidth + pixelHeight ) / 2.0;
                                if ( pixelScaleOrig > 0.0 ) {
                                    scale = range / pixelScaleOrig;
                                }
                            }
                            TimedDisplay::createRegular( textureId, position, NAN, scale, duration );
                        } else if ( loadType == TimedDisplay::LOAD_TYPE_BURST ) {
                            double rangeMin = type -> getTextureId( 0 );
                            double rangeMax = type -> getTextureId( 1 );
                            TimedDisplay::createBurst( position, rangeMin, rangeMax, 50, duration );
                        }
                    }
                    if ( doDamage && ( range > 0.0 ) && ( !isnan( damage ) ) && ( damage > 0.0 ) ) {
                        GameStateHolder::Container < EntityObject > entities = Cache::getGameStateHolder() -> getEntityContainer();
                        while ( entities.hasMore() ) {
                            EntityObject* entity = entities.nextObject();
                            if ( entity == launcher ) {
                                continue;
                            }
                            Vector2D entityPosition = entity -> getPosition();
                            double dist = position.dist2D( entityPosition );
                            if ( dist < range ) {
                                Vector2D collisionAt;
                                if ( !CollisionHelper::checkCollisionFromTo( position, entityPosition, Cache::getGameStateHolder() -> getMap(), collisionAt, Cache::getFileAccessor(), 1.0 ) ) {
                                    double dmgMul = pow( 1.0 - dist / range, 0.5 );
                                    double dmgReceived = damage * dmgMul;
                                    Vector2D dPos = entityPosition;
                                    dPos.x -= position.x;
                                    dPos.y -= position.y;
                                    PolarVector2D deathPushDir = PolarVector2D( dPos );
                                    deathPushDir.r = type -> getDeathPush() * dmgMul;
                                    //printf( "Receiving damage: %d / push %lg / dPos dist %lg\n", ( int ) dmgReceived, deathPushDir.r, dPos.dist2D() );
                                    if ( performDamage( entity, dmgReceived, Vector2D( deathPushDir ) ) ) {
                                        entities.removeObject();
                                        delete entity;
                                    }
                                }
                            }
                        }
                        int rangeBlocks = range / double( Map::BLOCK_SIZE );
                        for ( int i1 = - rangeBlocks; i1 <= rangeBlocks; i1++ ) {
                            for ( int i2 = - rangeBlocks; i2 <= rangeBlocks; i2++ ) {
                                Vector2D blockPosition = Vector2D( ( double( i1 ) + 0.5 ) * double( Map::BLOCK_SIZE ), ( double( i2 ) + 0.5 ) * double( Map::BLOCK_SIZE ) );
                                if ( blockPosition.dist2D() > range ) {
                                    continue;
                                }
                                double dmgMul = pow( 1.0 - blockPosition.dist2D() / range, 0.5 );
                                double dmgPropagated = damage * dmgMul;
                                PolarVector2D deathPushDir = PolarVector2D( blockPosition );
                                deathPushDir.r = type -> getDeathPush() * dmgMul;
                                blockPosition.x += position.x;
                                blockPosition.y += position.y;
                                BlockDeconstructionHelper::updateMapOnDamage( Cache::getGameStateHolder() -> getMap(), blockPosition, dmgPropagated, Vector2D( deathPushDir ) );
                            }
                        }
                    }
                }
                exEvD -> isStarted = true;
                if ( !isnan( exEvD -> customTimeMs ) ) {
                    if ( Cache::gameTime() < exEvD -> removeTimeMs ) {
                        // leave for next frame
                        removeDelegate = false;
                    }
                }
            }
            if ( !removeDelegate ) {
                edList.push_back( delegate );
            } else {
                delete delegate;
            }
        }
        for ( list < EventDelegate* >::iterator i = edList.begin(); i != edList.end(); i++ ) {
            if ( !_EDQ -> addEvent( _EVENT_QUEUE_ID, ( *i ) ) ) {
                delete ( *i );
            }
        }
    }

    static void weaponIdleTick( EntityObject* object ) {
        WeaponObject* melee = ( WeaponObject* ) object -> getMeleeWeapon();
        if ( melee ) {
            WeaponType* type = Cache::getFileAccessor() -> getWeaponType( melee -> getTypeId() );
            double dmg = object -> getParameter( EntityType::ATTRIBUTE_DAMAGE );
            melee -> setDamage(
                type -> getParameter( WeaponType::ATTRIBUTE_DAMAGE_MIN ) * dmg,
                type -> getParameter( WeaponType::ATTRIBUTE_DAMAGE_MAX ) * dmg
            );
        }
        WeaponObject* weapon = InventoryHelper::currentWeaponOf( object );
        if ( !weapon ) {
            return;
        }
        double reloadingTime = weapon -> getReloadingCurrentTime();
        if ( !isnan( reloadingTime ) ) {
            if ( reloadingTime <= Cache::gameTime() ) {
                weapon -> setReloadingCurrentTime( NAN );
                if ( weapon -> getInfinityAmmo() < 0.5 ) {
                    int ammoInInventory = InventoryHelper::getAmmoState( object, weapon -> getBulletId() );
                    int lackingAmmo = weapon -> getClipSize() - weapon -> getCurrentClip();
                    int refillCount = min( ammoInInventory, lackingAmmo );
                    InventoryHelper::addAmmo( object, weapon -> getBulletId(), - refillCount );
                    //printf( "Ammo[ %d ] left: %d, refill: %d\n", weapon -> getBulletId(), InventoryHelper::getAmmoState( object, weapon -> getBulletId() ), refillCount );
                    weapon -> setCurrentClip( weapon -> getCurrentClip() + refillCount );
                } else {
                    weapon -> setCurrentClip( weapon -> getClipSize() );
                }
            }
        }
    }

};

EventDelegateQueue* AttackHelper::_EDQ = new EventDelegateQueue();

void BlockDeconstructionHelper::_onBlockDestroy( int blockX, int blockY, MAP_FIELD_TYPE previousBlockId, BlockInfo previousBlockInfo, Vector2D deathPush ) {
        Vector2D position = Vector2D( ( double( blockX ) + 0.5 ) * Map::BLOCK_SIZE, ( double( blockY ) + 0.5 ) * Map::BLOCK_SIZE );
        // sound
        Sound* sound = Cache::getFileAccessor() -> getSound( previousBlockInfo.getDestroySoundId() );
        if ( sound ) {
            sound -> play();
        }
        // explosion
        if ( previousBlockInfo.getDestroyExplosionId() >= 0 ) {
            AttackHelper::createExplosion( previousBlockInfo.getDestroyExplosionId(), position );
        }
        // debris
        map < int, int > debrisMap = previousBlockInfo.getDebrisConfiguration();
        for ( map < int, int >::iterator i = debrisMap.begin(); i != debrisMap.end(); i++ ) {
            int debrisId = i -> first;
            int cnt = i -> second;
            for ( int j = 0; j < cnt; j++ ) {
                ParticleObject* particle = ParticleCreator::newParticle( debrisId, position, deathPush, 32.0, 60.0 );
                if ( particle ) {
                    Cache::getGameStateHolder() -> add( particle );
                }
            }
        }
    }

