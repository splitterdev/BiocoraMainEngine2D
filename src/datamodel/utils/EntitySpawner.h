#pragma once

class EntitySpawner {
    private:
    EntitySpawner() {}
    virtual ~EntitySpawner() {}
    public:

    //static const int SPAWN_TRIES = 100;

    static EntityObject* spawn( int entityTypeId ) {
        Map* currentMap = Cache::getGameStateHolder() -> getMap();
        EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
        if ( currentMap && player ) {
            //int x;
            //int y;
            //MAP_FIELD_TYPE field;
            //const int RANGE_MAX = 30;
            Vector2D playerPosition = player -> getPosition();
            Vector2D spawnedAt;
            if ( currentMap -> getAnalyser() -> getNextSpawnPositionFor( playerPosition, spawnedAt ) ) {
                EntityType* type = Cache::getFileAccessor() -> getEntityType( entityTypeId );
                EntityObject* object = type -> createObject < EntityObject > ( Cache::getGameStateHolder() -> nextEntityId() );
                object -> setPosition( spawnedAt.x, spawnedAt.y );
                WeaponType* objectWeapon = Cache::getFileAccessor() -> getWeaponType( object -> getInventoryWeaponId() );
                InventoryHelper::createWeapon( object, objectWeapon );
                return object;
            }
            /*for ( int i = 0; i < SPAWN_TRIES; i++ ) {
                x = playerPosition.x / Map::BLOCK_SIZE + rand() % RANGE_MAX;
                y = playerPosition.y / Map::BLOCK_SIZE + rand() % RANGE_MAX;
                if ( currentMap -> getField( x, y, field ) ) {
                    if ( !( Cache::getFileAccessor() -> getBlockInfo( field ) ).isSolid() ) {
                        EntityType* type = Cache::getFileAccessor() -> getEntityType( entityTypeId );
                        EntityObject* object = type -> createObject < EntityObject > ( Cache::getGameStateHolder() -> nextEntityId() );
                        object -> setPosition( x * Map::BLOCK_SIZE + Map::BLOCK_SIZE / 2, y * Map::BLOCK_SIZE + Map::BLOCK_SIZE / 2 );
                        WeaponType* objectWeapon = Cache::getFileAccessor() -> getWeaponType( object -> getInventoryWeaponId() );
                        InventoryHelper::createWeapon( object, objectWeapon );
                        return object;
                    }
                }
            }*/
        }
        return NULL;
    }

    static EntityObject* spawnAt( int entityTypeId, double x, double y ) {
        Map* currentMap = Cache::getGameStateHolder() -> getMap();
        Uint32 field;
        if ( currentMap ) {
            if ( currentMap -> getField( x / Map::BLOCK_SIZE, y / Map::BLOCK_SIZE, field ) ) {
                if ( !( Cache::getFileAccessor() -> getBlockInfo( field ) ).isSolid() ) {
                    EntityType* type = Cache::getFileAccessor() -> getEntityType( entityTypeId );
                    EntityObject* object = type -> createObject < EntityObject > ( Cache::getGameStateHolder() -> nextEntityId() );
                    object -> setPosition( x, y );
                    WeaponType* objectWeapon = Cache::getFileAccessor() -> getWeaponType( object -> getInventoryWeaponId() );
                    InventoryHelper::createWeapon( object, objectWeapon );
                    return object;
                }
            }
        }
        return NULL;
    }

};


