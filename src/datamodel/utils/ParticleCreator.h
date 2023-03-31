#pragma once

class ParticleCreator {
    private:
    ParticleCreator() {}
    virtual ~ParticleCreator() {}

    static inline void _randomizeVectorPosition( Vector2D & vec, double amount ) {
        PolarVector2D pvec = PolarVector2D();
        pvec.r = MathUtils::random() * amount;
        pvec.a = MathUtils::random() * M_PI * 2.0;
        Vector2D vecAdd = Vector2D( pvec );
        vec.x += vecAdd.x;
        vec.y += vecAdd.y;
    }

    static inline void _randomizeVectorAngle( Vector2D & vec, double amount ) {
        PolarVector2D pvec = PolarVector2D( vec );
        pvec.r *= MathUtils::random() * 0.3 + 0.85;
        pvec.a += ( MathUtils::random() * 2.0 - 1.0 ) * amount * M_PI / 180.0;
        vec = Vector2D( pvec );
    }

    public:
    static ParticleObject* newParticle( int particleType, Vector2D position, Vector2D movement, double positionRandomizer, double movementRandomizer ) {
        ParticleType* type = Cache::getFileAccessor() -> getParticleType( particleType );
        if ( type ) {
            ParticleObject* particle = type -> createObject < ParticleObject > ( Cache::getGameStateHolder() -> nextParticleId() );
            _randomizeVectorPosition( position, positionRandomizer );
            particle -> setPosition( position );
            _randomizeVectorAngle( movement, movementRandomizer );
            movement.scale( 1.0 / particle -> getMass() );
            particle -> setMovement( movement );
            particle -> setHeight( MathUtils::random() * 0.5 + 0.5 );
            particle -> setHeightDerivative( 0.0 );
            particle -> _newRotation();
            return particle;
        }
        return NULL;
    }

};


