#pragma once

/**
    Note: valid only for one object type (Entity) - attribute IDs are overlapping between types, so don't use general InteractiveObject without instance check
*/
class EntityStatCalculator {
    public:
    EntityStatCalculator( int statId ) {
        _statId = statId;
    }
    virtual ~EntityStatCalculator() {}

    virtual double getAttributeValue( EntityObject* object ) = 0;

    static double calculateAttribute( int statId, EntityObject* object ) {
        if ( object == NULL ) {
            return NAN;
        }
        map < int, EntityStatCalculator* >::iterator i = _calculatorMap.find( statId );
        if ( i != _calculatorMap.end() ) {
            EntityStatCalculator* calculator = i -> second;
            if ( calculator ) {
                return calculator -> getAttributeValue( object );
            }
        }
        return object -> getParameter( statId );
    }

    static void calculateAllAttributesForObject( EntityObject* object ) {
        for ( map < int, EntityStatCalculator* >::iterator i = _calculatorMap.begin(); i != _calculatorMap.end(); i++ ) {
            double newValue = calculateAttribute( i -> first, object );
            object -> setParameter( i -> first, newValue );
        }
    }

    inline int getStatId() {
        return _statId;
    }

    static void setCalculator( EntityStatCalculator* calculator ) {
        if ( calculator ) {
            map < int, EntityStatCalculator* >::iterator i = _calculatorMap.find( calculator -> _statId );
            if ( i != _calculatorMap.end() ) {
                EntityStatCalculator* prevCalculator = i -> second;
                if ( prevCalculator ) {
                    delete prevCalculator;
                }
            }
            _calculatorMap[ calculator -> _statId ] = calculator;
        }
    }

    static void Init();

    private:
    int _statId;

    static map < int, EntityStatCalculator* > _calculatorMap;

};

map < int, EntityStatCalculator* > EntityStatCalculator::_calculatorMap;

class HealthEntityStatCalculator : public EntityStatCalculator {
    public:
    HealthEntityStatCalculator() : EntityStatCalculator( EntityType::ATTRIBUTE_HP_CURRENT_MAX ) {
    }
    virtual ~HealthEntityStatCalculator() {
    }
    virtual double getAttributeValue( EntityObject* object ) {
        return object -> getParameter( EntityType::ATTRIBUTE_BASE_HP )
            + object -> getParameter( EntityType::ATTRIBUTE_HP ) * object -> getParameter( EntityType::ATTRIBUTE_HP_STAT_MULTIPLIER )
            + object -> getParameter( EntityType::ATTRIBUTE_LEVEL ) * object -> getParameter( EntityType::ATTRIBUTE_HP_PER_LEVEL );
    }
};

class EnergyEntityStatCalculator : public EntityStatCalculator {
    public:
    EnergyEntityStatCalculator() : EntityStatCalculator( EntityType::ATTRIBUTE_ENERGY_MAX ) {
    }
    virtual ~EnergyEntityStatCalculator() {
    }
    virtual double getAttributeValue( EntityObject* object ) {
        return object -> getParameter( EntityType::ATTRIBUTE_BASE_ENERGY )
            + object -> getParameter( EntityType::ATTRIBUTE_ENERGY ) * object -> getParameter( EntityType::ATTRIBUTE_ENE_STAT_MULTIPLIER )
            + object -> getParameter( EntityType::ATTRIBUTE_LEVEL ) * object -> getParameter( EntityType::ATTRIBUTE_ENE_PER_LEVEL );
    }
};



void EntityStatCalculator::Init() {
        setCalculator( new HealthEntityStatCalculator() );
        setCalculator( new EnergyEntityStatCalculator() );
    }


