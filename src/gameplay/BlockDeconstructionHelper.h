#pragma once

class BlockDeconstructionHelper {
    private:
    BlockDeconstructionHelper() {}
    virtual ~BlockDeconstructionHelper() {}

    inline static void _onBlockDestroy( int blockX, int blockY, MAP_FIELD_TYPE previousBlockId, BlockInfo previousBlockInfo, Vector2D deathPush ); // dependency - in AttackHelper

    public:

    inline static bool updateMapOnDamage( Map* mapInstance, Vector2D position, double damage, Vector2D deathPush ) {
        int blockX = position.x / Map::BLOCK_SIZE;
        int blockY = position.y / Map::BLOCK_SIZE;
        return updateMapOnDamage( mapInstance, blockX, blockY, damage, deathPush );
    }

    inline static bool updateMapOnDamage( Map* mapInstance, int blockX, int blockY, double damage, Vector2D deathPush ) {
        double currentHp = mapInstance -> getBlockHp( blockX, blockY );
        if ( isnan( currentHp ) ) {
            // not a valid block
            return false;
        }
        MAP_FIELD_TYPE blockId;
        if ( !mapInstance -> getField( blockX, blockY, blockId ) ) {
            // not a valid block
            return false;
        }
        currentHp -= damage;
        if ( currentHp < 0.0 ) {
            BlockInfo currentInfo = Cache::getFileAccessor() -> getBlockInfo( blockId );
            mapInstance -> setField( blockX, blockY, ( MAP_FIELD_TYPE ) currentInfo.getDestroyBlockId() );
            Texture* updateTexture = Cache::getFileAccessor() -> getTexture( currentInfo.getDestroyBlockId() );
            if ( updateTexture ) {
                mapInstance -> beginUpdate();
                mapInstance -> updateDisplayField( blockX, blockY, updateTexture -> getTexture() );
                mapInstance -> endUpdate();
            }
            BlockInfo nextBlockInfo = Cache::getFileAccessor() -> getBlockInfo( currentInfo.getDestroyBlockId() );
            mapInstance -> initBlockHp( blockX, blockY, nextBlockInfo );
            _onBlockDestroy( blockX, blockY, blockId, currentInfo, deathPush );
            return true;
        }
        mapInstance -> setBlockHp( blockX, blockY, currentHp );
        return false;
    }
};

