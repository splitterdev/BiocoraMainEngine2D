#pragma once

const int SOUND_ID_HIT_EMPTY = 902;

class BulletRobot : public GamePlayRobot {
    public:
    BulletRobot() : GamePlayRobot() {
    }
    virtual ~BulletRobot() {}

    virtual int onTick() {
        GameStateHolder* gameStateHolder = Cache::getGameStateHolder();
        GameStateHolder::Container < BulletObject > container = gameStateHolder -> getBulletContainer();
        GameStateHolder::Container < EntityObject > entities = gameStateHolder -> getEntityContainer();
        GameStateHolder::Container < EntityObject > entityFinder = gameStateHolder -> getEntityContainer();
        Map* currentMap = gameStateHolder -> getMap();
        SectorCollision* sectorCollision = SectorCollision::create( currentMap -> getWidth(), currentMap -> getHeight(), Map::BLOCK_SIZE );
        while ( entities.hasMore() ) {
            EntityObject* entity = entities.nextObject();
            int id = entity -> getId();
            Vector2D position = entity -> getPosition();
            double radius = entity -> getCollisionRadius();
            sectorCollision -> addObjectToSectorRange( id, entity, position, radius );
        }
        while ( container.hasMore() ) {
            BulletObject* bullet = container.nextObject();
            bullet -> updateLastPosition();
            Vector2D lastAccessibleBulletPosition = bullet -> getPosition();
            Vector2D bulletPosition = bullet -> getPosition();
            Vector2D bulletTarget = bullet -> getTarget();
            double velocity = bullet -> getVelocity();
            static const double motionStep = BulletObject::CHECKER_STEP;
            bool removeBullet = false;
            bool autoAim = ( bullet -> isAutoAim() && ( bullet -> getMotherEntity() == gameStateHolder -> getPlayerId() ) );
            Vector2D autoTarget;
            bool wallIgnore = bullet -> canIgnoreWalls();
            if ( autoAim ) {
                autoAim = AutoAimCursor::getAutoAimTarget( &autoTarget );
            }
            Vector2D deathPush = bullet -> getMovement();
            PolarVector2D deathPushDir = PolarVector2D( deathPush );
            deathPushDir.r = 1.0;
            deathPushDir.r *= ( 4.0 + MathUtils::random() * 7.5 ) * bullet -> getDeathPush();
            deathPushDir.a += MathUtils::random() * 3.0 - 1.5;
            deathPush = Vector2D( deathPushDir );
            for ( double stepDist = 0.0; stepDist <= velocity; stepDist += motionStep ) {
                bullet -> move( motionStep / velocity, autoAim, autoTarget );
                lastAccessibleBulletPosition = bulletPosition;
                bulletPosition = bullet -> getPosition();
                map < int, InteractiveObject* > collideSector = sectorCollision -> getObjectsFromSectorRange( bulletPosition );
                for ( map < int, InteractiveObject* >::iterator e = collideSector.begin(); e != collideSector.end(); e++ ) {
                    EntityObject* entity = ( EntityObject* ) e -> second;
                    int motherId = bullet -> getMotherEntity();
                    entityFinder.startFrom( motherId );
                    EntityObject* mother = entityFinder.nextObject();
                    if ( ( mother != entity ) && ( entities.hasObject( e -> first ) ) ) {
                        Vector2D entityPosition = entity -> getPosition();
                        double entityCollisionRadius = entity -> getCollisionRadius();
                        if ( bulletPosition.dist2D( entityPosition ) < entityCollisionRadius ) {
                            if ( AttackHelper::performDamage( entity, bullet -> getDamage(), deathPush ) ) {
                                // Remove from container
                                entities.startFrom( e -> first );
                                entities.nextObject();
                                entities.removeObject();
                                if ( mother -> addExperience( entity -> getGainedExp() ) ) {
                                    if ( mother == Cache::getGameStateHolder() -> getPlayer() ) {
                                        MessageManager::get() -> addMessage(
                                            MessageManager::LEVEL_NOTICE,
                                            LangStrings::getString( LangStringsID::MESSAGE_LOG_LEVEL_ADVANCE )
                                                + string( ": " )
                                                + StringUtils::toString( int( mother -> getLevel() ) )
                                        );
                                    }
                                }
                                delete entity;
                            }
                            removeBullet = true;
                            break;
                        }
                    }
                }
                if ( removeBullet ) {
                    break;
                }
                Vector2D bulletPosition = bullet -> getPosition();
                if ( bulletPosition.dist2D( bulletTarget ) < motionStep && !autoAim ) {
                    // avoid checking bullet with autoAim, as such one has its own target
                    if ( bullet -> getExplosionOnDeathId() < 0 ) {
                        Sound* sound = Cache::getFileAccessor() -> getSound( SOUND_ID_HIT_EMPTY );
                        if ( sound ) {
                            sound -> play();
                        }
                    }
                    TrashHelper::dumpTrash(
                        gameStateHolder -> getMap(),
                        bulletPosition.x,
                        bulletPosition.y,
                        Cache::getFileAccessor() -> getTexture( bullet -> getCorpseTextureId() ),
                        MathUtils::random() * 360,
                        0.4,
                        true
                    );
                    BlockDeconstructionHelper::updateMapOnDamage( currentMap, bulletPosition, bullet -> getDamage(), deathPush );
                    removeBullet = true;
                    break;
                }
                if ( !wallIgnore && CollisionHelper::checkCollisionForBullet( bulletPosition, currentMap, Cache::getFileAccessor(), bullet, lastAccessibleBulletPosition ) ) {
                    if ( bullet -> getExplosionOnDeathId() < 0 ) {
                        Sound* sound = Cache::getFileAccessor() -> getSound( SOUND_ID_HIT_EMPTY );
                        if ( sound ) {
                            sound -> play();
                        }
                    }
                    TrashHelper::dumpTrash(
                        gameStateHolder -> getMap(),
                        bulletPosition.x,
                        bulletPosition.y,
                        Cache::getFileAccessor() -> getTexture( bullet -> getCorpseTextureId() ),
                        MathUtils::random() * 360,
                        0.4,
                        false
                    );
                    BlockDeconstructionHelper::updateMapOnDamage( currentMap, bulletPosition, bullet -> getDamage(), deathPush );
                    removeBullet = true;
                    break;
                }
                bullet -> addTravelDistance( bulletPosition.dist2D( lastAccessibleBulletPosition ) );
                if ( bullet -> isRangeExceeded() ) {
                    removeBullet = true;
                    break;
                }
                bullet -> addTrace( bulletPosition );
            }
            if ( removeBullet ) {
                if ( !bullet -> isRangeExceeded() ) {
                    int explosionOnDeathId = bullet -> getExplosionOnDeathId();
                    if ( explosionOnDeathId >= 0 ) {
                        AttackHelper::createExplosion( explosionOnDeathId, lastAccessibleBulletPosition );
                    }
                }
                bullet -> setRemoved();
            }
        }
        delete sectorCollision;
        AttackHelper::processExplosionTasks();
        return GameStateProcessor::EVENT_RESULT_NONE;
    }

};


