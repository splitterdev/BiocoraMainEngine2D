#pragma once

class MenuBackgroundController {
    public:

    static const int PLANE_TEXTURE_ID_BACK = 100950;
    static const int PLANE_TEXTURE_ID_FOG = 100951;
    static const int PLANE_TEXTURE_ID_FRONT = 100952;

    static const double WIND_SPEED = - 0.045;

    MenuBackgroundController() {
        _tw = WINDOW_WIDTH * 1.4;
        _th = WINDOW_HEIGHT * 1.4;
        _tex = SDL_CreateTexture( Renderer::getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, _tw, _th );
    }
    virtual ~MenuBackgroundController() {
        SDL_DestroyTexture( _tex );
    }

    static MenuBackgroundController* getDefault() {
        if ( !_DEF_INSTANCE ) {
            _DEF_INSTANCE = new MenuBackgroundController();
            _DEF_INSTANCE -> addStaticPlane( PLANE_TEXTURE_ID_BACK, 0.25 );
            _DEF_INSTANCE -> addFog( PLANE_TEXTURE_ID_FOG, 8 );
            _DEF_INSTANCE -> addStaticPlane( PLANE_TEXTURE_ID_FRONT, 2.25 );
            _DEF_INSTANCE -> addFog( PLANE_TEXTURE_ID_FOG, 16 );
        }
        return _DEF_INSTANCE;
    }

    void addStaticPlane( int textureId, double zMul ) {
        _Plane p = _Plane( textureId, _Plane::TYPE_STATIC );
        p.zMul = zMul;
        _planes.push_back( p );
    }

    void addFog( int textureId, int cnt ) {
        for ( int i = 0; i < cnt; i++ ) {
            Texture* tex = Cache::getFileAccessor() -> getTexture( textureId );
            if ( !tex ) {
                return;
            }
            double tw = tex -> getWidth();
            double xMin = - tw;
            double xMax = WINDOW_WIDTH + tw;
            _Plane p = _Plane( textureId, _Plane::TYPE_FOG );
            p.rotation = MathUtils::random() * 360.0;
            p.x = MathUtils::random() * ( xMax - xMin ) + xMin;
            p.y = ( MathUtils::random() * 0.2 + 0.75 ) * double( WINDOW_HEIGHT );
            p.scale = MathUtils::random() * 2.0 + 3.5;
            p.rotationSpeed = ( MathUtils::random() - 0.5 ) * 0.25;
            p.movementSpeed = MathUtils::random() * 1.0 + 0.5;
            _planes.push_back( p );
        }
    }

    void onTick() {
        const double GEN_SLOWDOWN = 1.25;
        const double STD_ANIM_FPS = 60.0 * GEN_SLOWDOWN;
        SDL_SetRenderTarget( Renderer::getRenderer(), _tex );
        SDL_RenderClear( Renderer::getRenderer() );
        double t = double( SDL_GetTicks() ) / 1000.0;
        double piMod = ( M_PI * 2.0 );
        for ( int i = 0; i < ( int ) _planes.size(); i++ ) {
            _Plane p = _planes[ i ];
            Texture* tex = Cache::getFileAccessor() -> getTexture( p.textureId );
            if ( !tex ) {
                continue;
            }
            if ( p.type == _Plane::TYPE_STATIC ) {
                double timeMul = 1.0 / STD_ANIM_FPS;
                double xMod = sin( t * piMod * timeMul ) * double( WINDOW_WIDTH ) * 0.1 * p.zMul;
                double yMod = fabs( sin( t * piMod * timeMul * 2.0 ) + 1.0 ) / 2.0 * double( WINDOW_HEIGHT ) * 0.15 - double( WINDOW_HEIGHT ) * 0.075;// * p.zMul;
                //double rotMod = sin( t * piMod * timeMul ) * 2.5;
                double tw = _tw;//tex -> getWidth();
                double th = _th;//tex -> getHeight();
                //double sMod = ( sin( t * M_PI * 2.0 * timeMul ) + 1.0 ) * 0.1 + 0.9;
                // - tw / 2.0 + WINDOW_WIDTH / 2.0
                // - th / 2.0 + WINDOW_HEIGHT / 2.0
                SDL_Rect dst = Draw::getSDLRect( p.x + xMod, p.y + yMod, tw, th );
                SDL_RenderCopyEx( Renderer::getRenderer(), tex -> getTexture(), NULL, &dst, p.rotation, NULL, SDL_FLIP_NONE );
                //Draw::spriteStatic( p.x + xMod, p.y + yMod, tex, p.rotation + rotMod, p.scale + sMod );
            } else if ( p.type == _Plane::TYPE_FOG ) {
                //double timeMul = 1.0 / 40.0;
                //double yMod = fabs( sin( t * piMod * timeMul * 2.0 ) + 1.0 ) / 2.0 * double( WINDOW_HEIGHT ) * 0.1;
                double tw = tex -> getWidth() * p.scale;
                double th = tex -> getHeight() * p.scale;
                double xMin = - tw;
                double xMax = WINDOW_WIDTH + tw;
                p.x += p.movementSpeed * double( WINDOW_WIDTH ) * WIND_SPEED / STD_ANIM_FPS;
                if ( p.x <= xMin ) {
                    p.x += ( xMax - xMin );
                } else if ( p.x >= xMax ) {
                    p.x -= ( xMax - xMin );
                }
                p.rotation += p.rotationSpeed;
                SDL_Rect dst = Draw::getSDLRect( p.x, p.y, tw, th );
                SDL_RenderCopyEx( Renderer::getRenderer(), tex -> getTexture(), NULL, &dst, p.rotation, NULL, SDL_FLIP_NONE );
                _planes[ i ] = p;
            }
        }
        SDL_SetRenderTarget( Renderer::getRenderer(), Renderer::getMainTexture() );
        double timeMul = 1.0 / STD_ANIM_FPS;
        double xMod = sin( t * piMod * timeMul ) * double( WINDOW_WIDTH ) * 0.1;
        double yMod = fabs( sin( t * piMod * timeMul * 2.0 ) + 1.0 ) / 2.0 * double( WINDOW_HEIGHT ) * 0.1;
        double rotMod = sin( t * piMod * timeMul ) * 2.5;
        double tw = _tw;//WINDOW_WIDTH * 1.25; // tex -> getWidth();
        double th = _th;//WINDOW_HEIGHT * 1.25; // tex -> getHeight();
        //double sMod = ( sin( t * M_PI * 2.0 * timeMul ) + 1.0 ) * 0.1 + 0.9;
        SDL_Rect dst = Draw::getSDLRect( xMod - tw / 2.0 + WINDOW_WIDTH / 2.0, yMod - th / 2.0 + WINDOW_HEIGHT / 2.0, tw, th );
        SDL_RenderCopyEx( Renderer::getRenderer(), _tex, NULL, &dst, rotMod, NULL, SDL_FLIP_NONE );
    }

    private:

    class _Plane {
        public:
        static const int TYPE_STATIC = 0;
        static const int TYPE_FOG = 1;
        _Plane( int iTextureId, int iType ) {
            textureId = iTextureId;
            type = iType;
            rotation = 0.0;
            rotationSpeed = 0.0;
            movementSpeed = 0.0;
            x = 0.0;
            y = 0.0;
            scale = 1.0;
            zMul = 0.0;
        }
        virtual ~_Plane() {}
        int textureId;
        int type;
        double rotation;
        double rotationSpeed;
        double movementSpeed;
        double x;
        double y;
        double scale;
        double zMul;
    };

    static MenuBackgroundController* _DEF_INSTANCE;

    vector < _Plane > _planes;
    SDL_Texture* _tex;
    int _tw;
    int _th;

};

MenuBackgroundController* MenuBackgroundController::_DEF_INSTANCE = NULL;

