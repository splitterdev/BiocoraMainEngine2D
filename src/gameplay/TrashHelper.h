#pragma once

class TrashHelper {
    private:
    TrashHelper() {}
    virtual ~TrashHelper() {}

    public:

    static void dumpTrash( Map* currentMap, int x, int y, Texture* texture, double rotation, double opacity, bool drawOnFloor ) {
        if ( !texture ) {
            return;
        }
        int textureW = texture -> getWidth();
        int textureH = texture -> getHeight();
        int rTextureW = double( textureW ) * M_SQRT2;
        int rTextureH = double( textureH ) * M_SQRT2;
        SDL_Texture* texRotated = SDL_CreateTexture( Renderer::getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rTextureW, rTextureH );
        SDL_SetTextureBlendMode( texRotated, SDL_BLENDMODE_BLEND );
        SDL_SetRenderTarget( Renderer::getRenderer(), texRotated );
        SDL_SetRenderDrawBlendMode( Renderer::getRenderer(), SDL_BLENDMODE_NONE );
        SDL_SetRenderDrawColor( Renderer::getRenderer(), 0, 0, 0, 0 );
        SDL_RenderFillRect( Renderer::getRenderer(), NULL );
        SDL_SetRenderDrawBlendMode( Renderer::getRenderer(), SDL_BLENDMODE_BLEND );
        SDL_Rect dst;
        dst.w = textureW;
        dst.h = textureH;
        dst.x = ( rTextureW - textureW ) >> 1;
        dst.y = ( rTextureH - textureH ) >> 1;
        SDL_RenderCopyEx( Renderer::getRenderer(), texture -> getTexture(), NULL, &dst, rotation, NULL, SDL_FLIP_NONE );
        currentMap -> beginUpdate();
        SDL_SetTextureAlphaMod( texRotated, double( opacity * 255.0 ) );
        SDL_Rect srcRect;
        x -= rTextureW >> 1;
        y -= rTextureH >> 1;
        int rx = int( rTextureW / Map::BLOCK_SIZE ) + 1;
        int ry = int( rTextureH / Map::BLOCK_SIZE ) + 1;
        int diffX = x % Map::BLOCK_SIZE;
        int diffY = y % Map::BLOCK_SIZE;
        SDL_Rect dstRect;
        int beginBlockX = x / Map::BLOCK_SIZE;
        int beginBlockY = y / Map::BLOCK_SIZE;
        for ( int ix = beginBlockX; ix <= beginBlockX + rx; ix++ ) {
            for ( int iy = beginBlockY; iy <= beginBlockY + ry; iy++ ) {
                MAP_FIELD_TYPE field;
                if ( currentMap -> getField( ix, iy, field ) ) {
                    BlockInfo info = Cache::getFileAccessor() -> getBlockInfo( field );
                    bool solid = info.isSolid();
                    if ( solid == drawOnFloor ) { // if block is solid and we draw on floor only, skip; if block is transparent (non-solid) and we draw on walls, skip
                        continue;
                    }
                } else { // don't draw beyond map anyway
                    continue;
                }
                srcRect.x = ( ix - beginBlockX ) * Map::BLOCK_SIZE - diffX;
                srcRect.y = ( iy - beginBlockY ) * Map::BLOCK_SIZE - diffY;
                srcRect.w = Map::BLOCK_SIZE <= rTextureW ? Map::BLOCK_SIZE : rTextureW;
                srcRect.h = Map::BLOCK_SIZE <= rTextureH ? Map::BLOCK_SIZE : rTextureH;
                dstRect.x = ix * Map::BLOCK_SIZE;
                dstRect.y = iy * Map::BLOCK_SIZE;
                dstRect.w = srcRect.w;
                dstRect.h = srcRect.h;
                if ( srcRect.x + srcRect.w >= rTextureW ) {
                    dstRect.w -= srcRect.x + srcRect.w - rTextureW;
                }
                if ( srcRect.x < 0 ) {
                    dstRect.x += - srcRect.x;
                    dstRect.w = Map::BLOCK_SIZE + srcRect.x;
                }
                if ( srcRect.y + srcRect.h >= rTextureH ) {
                    dstRect.h -= srcRect.y + srcRect.h - rTextureH;
                }
                if ( srcRect.y < 0 ) {
                    dstRect.y += - srcRect.y;
                    dstRect.h = Map::BLOCK_SIZE + srcRect.y;
                }
                SDL_RenderCopy( Renderer::getRenderer(), texRotated, &srcRect, &dstRect );
            }
        }
        currentMap -> endUpdate();
        SDL_DestroyTexture( texRotated );
    }

};

