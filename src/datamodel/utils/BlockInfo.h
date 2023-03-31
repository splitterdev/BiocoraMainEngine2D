#pragma once

class BlockInfo {
    public:
    BlockInfo() {
        _collision = false;
        _transparentView = false;
        _transparentBullet = false;
        _destructible = false;
        _destroyBlockId = -1;
        _destroyExplosionId = -1;
    }
    virtual ~BlockInfo() {}

    inline bool isSolid() {
        return _collision;
    }

    inline bool isTransparentView() {
        return _transparentView;
    }

    inline bool isTransparentBullet() {
        return _transparentBullet;
    }

    inline bool isDestructible() {
        return _destructible;
    }

    inline bool isGlass() {
        return _glass;
    }

    inline int getDestroyBlockId() {
        return _destroyBlockId;
    }

    inline int getDestroyExplosionId() {
        return _destroyExplosionId;
    }

    inline double getBlockHp() {
        return _blockHp;
    }

    inline int getDestroySoundId() {
        if ( _sounds.empty() ) {
            return -1;
        }
        int index = MathUtils::random( 0, _sounds.size() );
        return _sounds[ index ];
    }

    map < int, int > getDebrisConfiguration() {
        map < int, int > debris;
        for ( int i = 0; i < ( int ) _debrisIds.size(); i++ ) {
            int key = _debrisIds[ i ];
            int value = MathUtils::random( _debrisMinCounts[ i ], _debrisMaxCounts[ i ] );
            debris[ key ] = value;
        }
        return debris;
    }

    inline void setSolid( bool solid ) {
        _collision = solid;
    }

    inline void setTransparentView( bool transparentView ) {
        _transparentView = transparentView;
    }

    inline void setTransparentBullet( bool transparentBullet ) {
        _transparentBullet = transparentBullet;
    }

    inline void setGlass( bool glass ) {
        _glass = glass;
    }

    inline void setDestructible( bool destructible, int destroyBlockId, int destroyExplosionId, double blockHp ) {
        _destructible = destructible;
        _destroyBlockId = destroyBlockId;
        _blockHp = blockHp;
        _destroyExplosionId = destroyExplosionId;
    }

    void addDestroySoundId( int soundId ) {
        _sounds.push_back( soundId );
    }

    void addDebrisCount( int debrisId, int minCount, int maxCount ) {
        _debrisIds.push_back( debrisId );
        _debrisMinCounts.push_back( minCount );
        _debrisMaxCounts.push_back( maxCount );
    }

    private:

    bool _collision;
    bool _transparentView;
    bool _glass;
    bool _transparentBullet;
    bool _destructible;
    int _destroyBlockId;
    int _destroyExplosionId;
    double _blockHp;
    vector < int > _sounds;
    vector < int > _debrisIds;
    vector < int > _debrisMinCounts;
    vector < int > _debrisMaxCounts;

};

