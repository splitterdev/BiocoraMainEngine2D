#pragma once

#include "effect2d/MenuBackgroundController.h"

class DisplayUnit {
    private:

    DisplayUnit() {}
    virtual ~DisplayUnit() {}

    public:

    static void display() {
        FileAccessor* accessor = Cache::getFileAccessor();
        GameStateHolder* gameStateHolder = Cache::getGameStateHolder();
        // Particles
        GameStateHolder::Container < ParticleObject > particleContainer = gameStateHolder -> getParticleContainer();
        while ( particleContainer.hasMore() ) {
            ParticleObject* particle = particleContainer.nextObject();
            Texture* texture = accessor -> getTexture( particle -> getTextureId() );
            Vector2D particlePosition = particle -> getPosition();
            double height = particle -> getHeight();
            Draw::sprite( particlePosition.x, particlePosition.y, texture, particle -> getRotation(), 0.5 + pow( 1.0 + height, 0.25 ) * 0.5 );
        }
        // Pickups
        double time = Cache::gameTime();
        GameStateHolder::Container < PickupObject > pickupContainer = gameStateHolder -> getPickupContainer();
        while ( pickupContainer.hasMore() ) {
            PickupObject* pickup = pickupContainer.nextObject();
            Texture* texture = accessor -> getTexture( pickup -> getTextureId() );
            Vector2D pickupPosition = pickup -> getPosition();
            double createTime = pickup -> getCreateTime();
            Draw::sprite( pickupPosition.x, pickupPosition.y, texture, 0.0, 1.0 + ( sin( ( time - createTime ) / 1000.0 * M_PI * 2.0 ) * 0.25 ) );
        }
        // Bullets
        GameStateHolder::Container < BulletObject > bulletContainer = gameStateHolder -> getBulletContainer();
        while ( bulletContainer.hasMore() ) {
            BulletObject* bullet = bulletContainer.nextObject();
            int bulletDisplayType = bullet -> getParameter( BulletType::ATTRIBUTE_TYPE );
            Vector2D bulletPosition = bullet -> getPosition();
            Vector2D bulletLastPosition = bullet -> getLastPosition();
            if ( bulletDisplayType == BulletType::DISPLAY_TYPE_LINE ) {
                list < Vector2D > trace = bullet -> getTrace();
                if ( trace.size() > 0 ) {
                    SDL_SetRenderDrawColor( Renderer::getRenderer(), bullet -> getColorR(), bullet -> getColorG(), bullet -> getColorB(), bullet -> getColorA() );
                    list < Vector2D >::iterator i = trace.begin();
                    Vector2D prevTrace = ( *i );
                    i++;
                    Vector2D currTrace;
                    while( i != trace.end() ) {
                        currTrace = ( *i );
                        SDL_RenderDrawLine( Renderer::getRenderer(), prevTrace.x - Camera::X, prevTrace.y - Camera::Y, currTrace.x - Camera::X, currTrace.y - Camera::Y );
                        prevTrace = currTrace;
                        i++;
                    }
                }
            } else if ( bulletDisplayType == BulletType::DISPLAY_TYPE_ELECTRO_JOINED ) {
                Vector2D sectorDev = Vector2D( -0.25, 0.25 );
                Vector2D sectorLenDev = Vector2D( 0.95, 1.05 );
                double sectorLength = MathUtils::random() * 0.5 + 1.5;
                for ( int i = 0; i < 5; i++ ) {
                    list < Vector2D > genLine = LightStruckHelper::electroLineWithCollision( bulletLastPosition, bulletPosition, sectorDev, sectorLenDev, sectorLength, 0.0 );
                    Draw::line( genLine, bullet -> getColorR(), bullet -> getColorG(), bullet -> getColorB(), bullet -> getColorA() );
                }
            } else if ( bulletDisplayType == BulletType::DISPLAY_TYPE_ELECTRO_OPEN ) {
                for ( int i = 0; i < 5; i++ ) {
                    Vector2D sectorDev = Vector2D( -0.325, 0.325 );
                    Vector2D sectorLenDev = Vector2D( 0.75, 1.25 );
                    double sectorLength = MathUtils::random() * 0.5 + 1.5;
                    list < Vector2D > genLine = LightStruckHelper::electroLineWithCollision( bulletLastPosition, bulletPosition, sectorDev, sectorLenDev, sectorLength, 0.5 );
                    Draw::line( genLine, bullet -> getColorR(), bullet -> getColorG(), bullet -> getColorB(), bullet -> getColorA() );
                }
            }
            if ( bullet -> getRemoved() ) {
                bulletContainer.removeObject();
                delete bullet;
            }
        }
        // Entities
        GameStateHolder::Container < EntityObject > entityContainer = gameStateHolder -> getEntityContainer();
        while ( entityContainer.hasMore() ) {
            EntityObject* entity = entityContainer.nextObject();
            Texture* texture = accessor -> getTexture( entity -> getTextureId() );
            Vector2D entityPosition = entity -> getPosition();
            Vector2D targetPosition = entity -> getTargetPosition();
            Draw::targetSprite( entityPosition.x, entityPosition.y, targetPosition.x, targetPosition.y, texture, 1.0 );
        }
        // Debug Info
        /* /// Spawn
        Vector2D playerPos = Cache::getGameStateHolder() -> getPlayer() -> getPosition();
        Cache::getGameStateHolder() -> getMap() -> getAnalyser() -> displayDebug( playerPos.x / Map::BLOCK_SIZE, playerPos.y / Map::BLOCK_SIZE, 15.0 );*/

        // Display Timed Textures (Splats)
        TimedDisplay::displayAll();
    }

    static void displayMap( Map* currentMap ) {
        if ( currentMap ) {
            SDL_Rect srcRect = Draw::getSDLRect( Camera::X, Camera::Y, WINDOW_WIDTH, WINDOW_HEIGHT );
            int dstX = 0;
            int dstY = 0;
            int dstWidth = WINDOW_WIDTH;
            int dstHeight = WINDOW_HEIGHT;
            if ( Camera::X + WINDOW_WIDTH >= currentMap -> getDumpTexWidth() ) {
                dstWidth -= Camera::X + WINDOW_WIDTH - currentMap -> getDumpTexWidth();
            }
            if ( Camera::X < 0 ) {
                dstX = - Camera::X;
                dstWidth -= dstX;
            }
            if ( Camera::Y + WINDOW_HEIGHT >= currentMap -> getDumpTexHeight() ) {
                dstHeight -= Camera::Y + WINDOW_HEIGHT - currentMap -> getDumpTexHeight();
            }
            if ( Camera::Y < 0 ) {
                dstY = - Camera::Y;
                dstHeight -= dstY;
            }
            SDL_Rect dstRect = Draw::getSDLRect( dstX, dstY, dstWidth, dstHeight );
            SDL_RenderCopy( Renderer::getRenderer(), currentMap -> getDisplayTexture(), &srcRect, &dstRect );
        }
    }

    static void displayMenuBackground() {
        MenuBackgroundController::getDefault() -> onTick();
    }

};



