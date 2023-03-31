#pragma once

class CollisionHelper {
    private:
    CollisionHelper() {}
    virtual ~CollisionHelper() {}
    public:

    static inline bool checkCollisionFromTo( Vector2D from, Vector2D to, Map* currentMap, Vector2D & collisionAt, FileAccessor* accessor, double motionStep ) {
        Vector2D dir = to;
        dir.x -= from.x;
        dir.y -= from.y;
        double dist = dir.dist2D();
        if ( dist > 0.0 ) {
            double rDist = motionStep / dist;
            dir.x *= rDist;
            dir.y *= rDist;
            for ( double stepDist = 0.0; stepDist <= dist; stepDist += motionStep ) {
                from.x += dir.x;
                from.y += dir.y;
                if ( checkCollision( from, currentMap, accessor ) ) {
                    collisionAt = from;
                    return true;
                }
            }
        }
        return false;
    }

    static inline bool checkCollision( Vector2D position, Map* currentMap, FileAccessor* accessor ) {
        MAP_FIELD_TYPE field;
        if ( currentMap -> getField( position.x / Map::BLOCK_SIZE, position.y / Map::BLOCK_SIZE, field ) ) {
            return ( accessor -> getBlockInfo( field ) ).isSolid();
        }
        return false;
    }

    static inline bool checkCollisionForBullet( Vector2D position, Map* currentMap, FileAccessor* accessor, BulletObject* bullet, Vector2D lastAccessibleBulletPosition ) {
        MAP_FIELD_TYPE field;
        if ( currentMap -> getField( position.x / Map::BLOCK_SIZE, position.y / Map::BLOCK_SIZE, field ) ) {
            BlockInfo info = accessor -> getBlockInfo( field );
            if ( !bullet -> isMaterial() ) {
                if ( info.isGlass() ) {
                    if ( !bullet -> isInTransparentMaterial() ) {
                        //printf( "Bullet in transparent.\n" );
                        bullet -> occlude( getOcclusionValue( lastAccessibleBulletPosition, bullet ) * 1.0 );
                        bullet -> setInTransparentMaterial( true );
                    }
                    return false;
                } else {
                    if ( bullet -> isInTransparentMaterial() ) {
                        //printf( "Bullet out transparent.\n" );
                        bullet -> occlude( getOcclusionValue( lastAccessibleBulletPosition, bullet ) * ( - 1.0 ) );
                        bullet -> setInTransparentMaterial( false );
                    }
                    return info.isSolid() && !info.isTransparentBullet();
                }
            }
            return info.isSolid() && !info.isTransparentBullet();
        }
        return false;
    }

    static inline double getOcclusionValue( Vector2D lastAccessibleBulletPosition, BulletObject* bullet ) {
        Vector2D position = bullet -> getPosition();
        int pX = position.x / double( Map::BLOCK_SIZE );
        int pY = position.y / double( Map::BLOCK_SIZE );
        pX -= ( int )( lastAccessibleBulletPosition.x / double( Map::BLOCK_SIZE ) );
        pY -= ( int )( lastAccessibleBulletPosition.y / double( Map::BLOCK_SIZE ) );
        PolarVector2D surfaceDir = PolarVector2D( Vector2D( pX, pY ) );
        PolarVector2D movementDir = PolarVector2D( bullet -> getMovement() );
        double a = surfaceDir.a - movementDir.a;
        double sinOutValue = 0.2 * sin( a ) / 0.75;
        return asin( sinOutValue );
    }

};


