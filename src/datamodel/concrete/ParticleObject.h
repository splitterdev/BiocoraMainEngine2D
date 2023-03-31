#pragma once

class ParticleCreator;

class ParticleObject : public InteractiveObject {
    friend class ParticleCreator;
    public:
    ParticleObject( int id, int typeId ) : InteractiveObject( id, typeId ) {
    }
    virtual ~ParticleObject() {
    }
    virtual void setCalculatedAttributes() {
    }

    inline int getTextureId() {
        return getParameter( ParticleType::ATTRIBUTE_TEXTURE_ID );
    }

    inline void setTextureId( int textureId ) {
        setParameter( ParticleType::ATTRIBUTE_TEXTURE_ID, textureId );
    }

    inline Vector2D getPosition() {
        return Vector2D(
            getParameter( ParticleType::ATTRIBUTE_POSITION_X ),
            getParameter( ParticleType::ATTRIBUTE_POSITION_Y )
        );
    }

    inline void setPosition( double x, double y ) {
        setParameter( ParticleType::ATTRIBUTE_POSITION_X, x );
        setParameter( ParticleType::ATTRIBUTE_POSITION_Y, y );
    }

    inline void setPosition( Vector2D position ) {
        setParameter( ParticleType::ATTRIBUTE_POSITION_X, position.x );
        setParameter( ParticleType::ATTRIBUTE_POSITION_Y, position.y );
    }

    inline Vector2D getMovement() {
        return Vector2D(
            getParameter( ParticleType::ATTRIBUTE_MOVEMENT_X ),
            getParameter( ParticleType::ATTRIBUTE_MOVEMENT_Y )
        );
    }

    inline void setMovement( double x, double y ) {
        setParameter( ParticleType::ATTRIBUTE_MOVEMENT_X, x );
        setParameter( ParticleType::ATTRIBUTE_MOVEMENT_Y, y );
    }

    inline void setMovement( Vector2D movement ) {
        setParameter( ParticleType::ATTRIBUTE_MOVEMENT_X, movement.x );
        setParameter( ParticleType::ATTRIBUTE_MOVEMENT_Y, movement.y );
    }

    inline double getRotation() {
        return getParameter( ParticleType::ATTRIBUTE_ROTATION );
    }

    inline void setRotation( double value ) {
        setParameter( ParticleType::ATTRIBUTE_ROTATION, value );
    }

    inline double getMass() {
        return getParameter( ParticleType::ATTRIBUTE_MASS );
    }

    inline void setMass( double value ) {
        setParameter( ParticleType::ATTRIBUTE_MASS, value );
    }

    inline void addRotation( double value ) {
        setParameter( ParticleType::ATTRIBUTE_ROTATION, getRotation() + value );
    }

    inline double getRotationSpeed() {
        return getParameter( ParticleType::ATTRIBUTE_ROTATION_SPEED );
    }

    inline void setRotationSpeed( double value ) {
        setParameter( ParticleType::ATTRIBUTE_ROTATION_SPEED, value );
    }

    inline double getHeight() {
        return getParameter( ParticleType::ATTRIBUTE_HEIGHT );
    }

    inline void setHeight( double value ) {
        setParameter( ParticleType::ATTRIBUTE_HEIGHT, value );
    }

    inline void addHeight( double value ) {
        setParameter( ParticleType::ATTRIBUTE_HEIGHT, getHeight() + value );
    }

    inline double getHeightDerivative() {
        return getParameter( ParticleType::ATTRIBUTE_HEIGHT_DERV );
    }

    inline void setHeightDerivative( double value ) {
        setParameter( ParticleType::ATTRIBUTE_HEIGHT_DERV, value );
    }

    inline void addHeightDerivative( double value ) {
        setParameter( ParticleType::ATTRIBUTE_HEIGHT_DERV, getHeightDerivative() + value );
    }

    inline double getCollisionTexture() {
        map < int, double > values;
        if ( getMultiValuedParameter( ParticleType::ATTRIBUTE_COLLISION_TEXTURE_ID, values ) ) {
            return MathUtils::randomValueFromMap( values );
        }
        return NAN;
    }

    inline map < int, double > getCollisionSoundIds() {
        map < int, double > values;
        getMultiValuedParameter( ParticleType::ATTRIBUTE_COLLISION_SOUND_ID, values );
        return values;
    }

    static const int FLAG_BOUNCE = 0x00000001;
    static const int FLAG_DEAD = 0x00000002;

    static const double HEIGHT_DERV_GRAVITY = 0.001;
    static const double PARTICLE_BOUNCE_FACTOR = 0.85;
    static const double PARTICLE_INERTIA = 0.99;
    static const double PARTICLE_MOVEMENT_STOP_RADIUS = 0.5;

    private:

    inline void _newRotation() {
        setRotation( MathUtils::random() * 360.0 );
        setRotationSpeed( MathUtils::random() * 1.0 - 0.5 );
    }

};

InteractiveObject* ParticleType::createObjectInstance( int assignedId ) {
        return new ParticleObject( assignedId, getId() );
    }
