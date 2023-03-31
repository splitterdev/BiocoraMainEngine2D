#pragma once

/// TODO remove in prod!
const int WEAPON_SPAMMER_ARRAY_SIZE = 20;
int WEAPON_SPAMMER_ARRAY[ WEAPON_SPAMMER_ARRAY_SIZE ] = {
    1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1010,
    1011, 1012, 1013, 1014, 1015, 1016, 1017, 1018, 1019, 1020
};

class GameStateLoader {
    private:
    GameStateLoader() {}
    ~GameStateLoader() {}
    public:

    static const double DEFAULT_SPAWN_RANGE_MIN = 16.0;
    static const double DEFAULT_SPAWN_RANGE_MAX = 18.0;

    static void loadLevel( int levelId ) {
        GameStateHolder* holder = Cache::getGameStateHolder();
        if ( holder ) {
            printf( "RM holder... " );
            delete holder;
            printf( "DONE.\n" );
        }
        holder = new GameStateHolder();
        Cache::setGameStateHolder( holder );
        holder -> setMap( Cache::getFileAccessor() -> getMap( levelId ) );
        try {
            Map* currentMap = holder -> getMap();
            if ( currentMap ) {
                SpawnPositionAnalyser::create( currentMap, DEFAULT_SPAWN_RANGE_MIN, DEFAULT_SPAWN_RANGE_MAX, Cache::getFileAccessor() );
            }
            // player
            int characterId = PlayerManager::getProfileId();//PlayerManager::createCharacter( 1, "data/player/character/Testing.txt" );
            if ( characterId < 0 ) {
                characterId = 0;
            }
            EntityType* character = Cache::getFileAccessor() -> getCharacter( characterId );
            //printf( "Character: %p (/ID = %d)\n", character, characterId );
            if ( character ) {
                EntityObject* characterObject = PlayerManager::copyFromCharacter( characterId, holder -> nextEntityId() );
                holder -> add( characterObject );
                holder -> setPlayer( characterObject -> getId() );
                if ( currentMap ) {
                    characterObject -> setPosition( currentMap -> getParameter( Map::PARAM_MAP_START_X ), currentMap -> getParameter( Map::PARAM_MAP_START_Y ) );
                }
                _setMeleeWeapon( characterObject );
                /*for ( int i = 0; i < WEAPON_SPAMMER_ARRAY_SIZE; i++ ) {
                    int weaponTypeId = WEAPON_SPAMMER_ARRAY[ i ];
                    WeaponType* weaponType = Cache::getFileAccessor() -> getWeaponType( weaponTypeId );
                    int weaponId = InventoryHelper::createWeapon( characterObject, weaponType );
                    if ( weaponId >= 0 ) {
                        printf( "Weapon %d created (%d).\n", weaponTypeId, weaponId );
                        WeaponObject* weapon = InventoryHelper::getWeaponOf( characterObject, weaponId );
                        if ( weapon ) {
                            InventoryHelper::addAmmo( characterObject, weapon -> getBulletId(), 120 );
                            printf( "Ammo added.\n" );
                        }
                        characterObject -> setEquippedWeaponId( weaponId );
                    }
                }*/
                //InventoryHelper::restoreInventoryData( characterObject );
            }
        } catch ( Exception e ) {
            e.log();
        }
    }

    private:

    static void _setMeleeWeapon( EntityObject* playerObject ) {
        WeaponType* type = Cache::getFileAccessor() -> getWeaponType( playerObject -> getParameter( EntityType::ATTRIBUTE_MELEE_WEAPON_ID ) );
        if ( type ) {
            WeaponObject* object = type -> createObject < WeaponObject > ( -1 );
            playerObject -> setMeleeWeapon( object );
        }
    }

};


