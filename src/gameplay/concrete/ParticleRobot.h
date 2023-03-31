#pragma once

class ParticleRobot : public GamePlayRobot {
    public:
    ParticleRobot() : GamePlayRobot() {
    }
    virtual ~ParticleRobot() {}

    virtual int onTick() {
        GameStateHolder* gameStateHolder = Cache::getGameStateHolder();
        GameStateHolder::Container < ParticleObject > container = gameStateHolder -> getParticleContainer();
        Map* currentMap = gameStateHolder -> getMap();
        while ( container.hasMore() ) {
            ParticleObject* object = container.nextObject();
            int movementFlag = MovementHelper::move( object, currentMap );
            Vector2D objectPosition = object -> getPosition();
            if ( movementFlag & ParticleObject::FLAG_DEAD ) {
                TrashHelper::dumpTrash(
                    gameStateHolder -> getMap(),
                    objectPosition.x,
                    objectPosition.y,
                    Cache::getFileAccessor() -> getTexture( object -> getTextureId() ),
                    object -> getRotation(),
                    1,
                    true
                );
                container.removeObject();
                delete object;
            } else if ( movementFlag & ParticleObject::FLAG_BOUNCE ) {
                map < int, double > soundIds = object -> getCollisionSoundIds();
                int soundId = rand() % soundIds.size();
                bool playSound = true;
                for ( map < int, double >::iterator i = soundIds.begin(); i != soundIds.end(); i++ ) {
                    Sound* sound = Cache::getFileAccessor() -> getSound( i -> second );
                    if ( sound ) {
                        if ( sound -> isPlaying() ) {
                            playSound = false;
                        }
                    }
                }
                if ( playSound ) {
                    map < int, double >::iterator i = soundIds.begin();
                    for ( int j = 0; j < soundId; j++ ) {
                        i++;
                    }
                    Sound* sound = Cache::getFileAccessor() -> getSound( i -> second );
                    if ( sound ) {
                        sound -> play();
                    }
                }
                TrashHelper::dumpTrash(
                    gameStateHolder -> getMap(),
                    objectPosition.x,
                    objectPosition.y,
                    Cache::getFileAccessor() -> getTexture( object -> getCollisionTexture() ),
                    MathUtils::random() * 360.0,
                    1,
                    true
                );
                PolarVector2D movement = PolarVector2D( object -> getMovement() );
                movement.a += MathUtils::random() * 0.5 - 0.25;
                object -> setMovement( Vector2D( movement ) );
            }
        }
        return GameStateProcessor::EVENT_RESULT_NONE;
    }

};


