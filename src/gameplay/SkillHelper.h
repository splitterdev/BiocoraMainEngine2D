#pragma once

class SkillHelper {
    private:
    SkillHelper() {}
    virtual ~SkillHelper() {}

    static bool _prevIndexNavigate( int & root, map < int, double > & m ) {
        int rootBase = root;
        map < int, double >::reverse_iterator i = m.rbegin();
        do {
            if ( i -> first == rootBase ) {
                break;
            }
            i++;
        } while ( i != m.rend() );
        if ( i == m.rend() ) {
            i = m.rbegin();
        }
        do {
            i++;
            if ( i == m.rend() ) {
                i = m.rbegin();
            }
            double level = i -> second;
            if ( !isnan( level ) && ( level >= 0.5 ) ) {
                root = i -> first;
                return true;
            }
        } while ( i -> first != rootBase );
        return false;
    }

    static bool _nextIndexNavigate( int & root, map < int, double > & m ) {
        int rootBase = root;
        map < int, double >::iterator i = m.find( root );
        if ( i == m.end() ) {
            i = m.begin();
        }
        do {
            i++;
            if ( i == m.end() ) {
                i = m.begin();
            }
            double level = i -> second;
            if ( !isnan( level ) && ( level >= 0.5 ) ) {
                root = i -> first;
                return true;
            }
        } while ( i -> first != rootBase );
        return false;
    }

    public:

    static const int SKILL_PLACEMENT_ENTITY_POSITION = 0;
    static const int SKILL_PLACEMENT_PRESERVE_TARGET_DIRECT = 1;

    static const int SKILL_TYPE_INSTANT = 0;
    static const int SKILL_TYPE_FIELD = 1;

    static bool useSkill( int skillId, EntityObject* caller, Vector2D target ) {
        if ( !caller ) {
            return false;
        }
        SkillType* skill = Cache::getFileAccessor() -> getSkillType( skillId );
        if ( !skill ) {
            return false;
        }
        int level = caller -> getSkillLevel( skillId );
        if ( level < 1 ) {
            return false;
        }
        // actual use
        double currentEnergyUsage = skill -> getEnergyUsage( level );
        if ( !caller -> payEnergy( currentEnergyUsage ) ) {
            return false;
        }
        double currentDamage = skill -> getDamageForSkillLevel( level );
        int placement = skill -> getPlacementType();
        if ( placement == SKILL_PLACEMENT_ENTITY_POSITION ) {
            target = caller -> getPosition();
        }
        int type = skill -> getSkillType();
        if ( type == SKILL_TYPE_INSTANT ) {
            AttackHelper::createExplosion( skill -> getExplosionTypeId(), target, currentDamage, caller );
        } else if ( type == SKILL_TYPE_FIELD ) {
            AttackHelper::createField( skill -> getExplosionTypeId(), target, currentDamage, caller, skill -> getDuration() );
        }
        return true;
    }

    static bool useCurrentSkill( EntityObject* caller, Vector2D target ) {
        if ( !caller ) {
            return false;
        }
        double skillId = caller -> getParameter( EntityType::ATTRIBUTE_ACTIVE_SKILL );
        if ( isnan( skillId ) ) {
            return false;
        }
        return useSkill( skillId, caller, target );
    }

    static void setNextActiveSkill( EntityObject* caller, int dir ) {
        map < int, double > skillLevels;
        if ( !caller ) {
            return;
        }
        double skillId = caller -> getParameter( EntityType::ATTRIBUTE_ACTIVE_SKILL );
        if ( isnan( skillId ) ) {
            return;
        }
        printf( "Skill select / dir %d\n", dir );
        if ( caller -> getMultiValuedParameter( EntityType::ATTRIBUTE_SKILL_LEVEL, skillLevels ) ) {
            int newIndex = skillId;
            bool navigate = false;
            if ( dir < 0 ) {
                navigate = _prevIndexNavigate( newIndex, skillLevels );
            }
            if ( dir > 0 ) {
                navigate = _nextIndexNavigate( newIndex, skillLevels );
            }
            if ( navigate ) {
                caller -> setParameter( EntityType::ATTRIBUTE_ACTIVE_SKILL, newIndex );
            }
        }
    }

};


