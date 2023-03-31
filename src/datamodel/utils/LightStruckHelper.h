#pragma once

class LightStruckHelper {
    private:
    LightStruckHelper() {}
    virtual ~LightStruckHelper() {}
    public:

    static list < int > getAllNearbyEntities( int fromEntity, double range ) {
        list < int > returnedList;
        EntityObject* root = Cache::getGameStateHolder() -> getEntity( fromEntity );
        if ( root ) {
            GameStateHolder::Container < EntityObject > entities = Cache::getGameStateHolder() -> getEntityContainer();
            while ( entities.hasMore() ) {
                EntityObject* object = entities.nextObject();
                if ( ( object != NULL ) && ( object != root ) && ( object -> distanceTo( root ) < range ) ) {
                    returnedList.push_back( object -> getId() );
                }
            }
        }
        return returnedList;
    }

    static list < Vector2D > electroLine( Vector2D initial, Vector2D target, Vector2D sectorDeviation, Vector2D sectorLengthDeviation, int sectorCount, bool exactTarget ) {
        if ( sectorCount == 0 ) {
            return list < Vector2D > ();
        }
        // mark
        list < Vector2D > breakPositions;
        breakPositions.push_back( initial );
        int sectorGenCount = sectorCount;
        if ( !exactTarget ) {
            sectorGenCount += 1;
        }
        Vector2D prevMidPoint = initial;
        for ( int i = 1; i < sectorGenCount; i++ ) {
            double percentageSectorPosition = double( i ) / double( sectorCount );
            Vector2D midPoint = initial.balance( target, percentageSectorPosition );
            midPoint.x -= initial.x;
            midPoint.y -= initial.y;
            PolarVector2D polarMidPoint = PolarVector2D( midPoint );
            double midPointRadialDeviation = sectorLengthDeviation.x + MathUtils::random() * ( sectorLengthDeviation.y - sectorLengthDeviation.x );
            double midPointAngularDeviation = sectorDeviation.x + MathUtils::random() * ( sectorDeviation.y - sectorDeviation.x );
            if ( exactTarget ) {
                polarMidPoint.r *= midPointRadialDeviation * ( 1.0 - percentageSectorPosition ) + percentageSectorPosition;
                polarMidPoint.a += midPointAngularDeviation * ( 1.0 - percentageSectorPosition );
            } else {
                polarMidPoint.r *= midPointRadialDeviation;
                polarMidPoint.a += midPointAngularDeviation;
            }
            midPoint = Vector2D( polarMidPoint );
            midPoint.x += initial.x;
            midPoint.y += initial.y;
            breakPositions.push_back( midPoint );
            prevMidPoint = midPoint;
        }
        if ( exactTarget ) {
            breakPositions.push_back( target );
        }
        return breakPositions;
    }

    static list < Vector2D > electroLineWithCollision( Vector2D initial, Vector2D target, Vector2D sectorDeviation, Vector2D sectorLengthDeviation, double sectorLength, double exactTargetDisperseRange ) {
        if ( sectorLength == 0 ) {
            return list < Vector2D > ();
        }
        double distToTarget = initial.dist2D( target ) / double( Map::BLOCK_SIZE );
        double targetDisperseRange = 1.0 - exactTargetDisperseRange;
        // mark
        list < Vector2D > breakPositions;
        breakPositions.push_back( initial );
        int sectorGenCount = distToTarget / sectorLength;
        Vector2D prevMidPoint = initial;
        for ( int i = 1; i <= sectorGenCount; i++ ) {
            double percentageSectorPosition = double( i ) / double( sectorGenCount );
            Vector2D midPoint = initial.balance( target, percentageSectorPosition );
            midPoint.x -= initial.x;
            midPoint.y -= initial.y;
            PolarVector2D polarMidPoint = PolarVector2D( midPoint );
            double midPointRadialDeviation = sectorLengthDeviation.x + MathUtils::random() * ( sectorLengthDeviation.y - sectorLengthDeviation.x );
            double midPointAngularDeviation = sectorDeviation.x + MathUtils::random() * ( sectorDeviation.y - sectorDeviation.x );
            //polarMidPoint.r *= midPointRadialDeviation * ( 1.0 - percentageSectorPosition ) + percentageSectorPosition;
            polarMidPoint.r *= percentageSectorPosition;
            polarMidPoint.r += midPointRadialDeviation * sectorLength * double( Map::BLOCK_SIZE );
            polarMidPoint.a += midPointAngularDeviation * ( 1.0 - percentageSectorPosition * targetDisperseRange );
            midPoint = Vector2D( polarMidPoint );
            midPoint.x += initial.x;
            midPoint.y += initial.y;
            Vector2D collisionDetectionPoint;
            if ( _checkCollision( prevMidPoint, midPoint, &collisionDetectionPoint, 10.0 ) ) {
                breakPositions.push_back( collisionDetectionPoint );
                break;
            }
            breakPositions.push_back( midPoint );
            prevMidPoint = midPoint;
        }
        return breakPositions;
    }

    inline static list < list < Vector2D > > multiElectroLine( Vector2D initial, Vector2D target, Vector2D sectorDeviation, Vector2D sectorLengthDeviation, Vector2D sectorCount, Vector2D lineCount, bool exactTarget ) {
        int limit = MathUtils::random( lineCount.x, lineCount.y );
        list < list < Vector2D > > genLines;
        for ( int i = 0; i < limit; i++ ) {
            list < Vector2D > genLine = electroLine( initial, target, sectorDeviation, sectorLengthDeviation, MathUtils::random( sectorCount.x, sectorCount.y ), exactTarget );
            genLines.push_back( genLine );
        }
        return genLines;
    }

    inline static list < list < Vector2D > > electroBurst( Vector2D initial, list < Vector2D > targets, Vector2D sectorDeviation, Vector2D sectorLengthDeviation, Vector2D sectorCount, Vector2D lineCount, bool exactTarget ) {
        list < list < Vector2D > > allGenLines;
        for ( list < Vector2D >::iterator i = targets.begin(); i != targets.end(); i++ ) {
            list < list < Vector2D > > genLines = multiElectroLine( initial, ( *i ), sectorDeviation, sectorLengthDeviation, sectorCount, lineCount, exactTarget );
            for ( list < list < Vector2D > >::iterator j = genLines.begin(); j != genLines.end(); j++ ) {
                allGenLines.push_back( *j );
            }
        }
        return allGenLines;
    }

    private:

    inline static bool _checkCollision( Vector2D initial, Vector2D target, Vector2D * collisionDetectedOutput, double checkSteps ) {
        for ( double k = 0.0; k <= 1.0; k += 1.0 / checkSteps ) {
            Vector2D nowPosition = initial.balance( target, k );
            Uint32 field;
            if ( Cache::getGameStateHolder() -> getMap() -> getField( nowPosition.x / Map::BLOCK_SIZE, nowPosition.y / Map::BLOCK_SIZE, field ) ) {
                BlockInfo info = Cache::getFileAccessor() -> getBlockInfo( field );
                if ( info.isSolid() && !info.isTransparentBullet() ) {
                    ( *collisionDetectedOutput ) = nowPosition;
                    return true;
                }
            }
        }
        return false;
    }

};

